#include "PluginProcessor.h"
#include "PluginEditor.h"

GOLEQAudioProcessor::GOLEQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
  InitialiseLogging(true);
}

GOLEQAudioProcessor::~GOLEQAudioProcessor()
{
  InitialiseLogging(false);
}

// JUCE Lifecycle
void GOLEQAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
  juce::ignoreUnused(sampleRate);
  juce::ignoreUnused(samplesPerBlock);
}

void GOLEQAudioProcessor::releaseResources(){}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GOLEQAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
    && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
  {
    return false;
  }

#if ! JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
  {
    return false;
  }
#endif

  return true;
#endif
}
#endif

void GOLEQAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                       juce::MidiBuffer& midiMessages)
{
  juce::ignoreUnused(midiMessages);
  juce::ScopedNoDenormals noDenormals;

  const int numSamples = buffer.getNumSamples();
  const auto pointers = GetInputPointers(buffer);
  UpdateRMS(pointers.L, pointers.R, numSamples);
  PushMonoToFIFO(pointers.L, pointers.R, numSamples);
  MirrorRightToLeftIfStereo(buffer);
  ClearExtraOutputs(buffer);
  ApplyEffect(buffer);
}

// Logging
void GOLEQAudioProcessor::InitialiseLogging(bool init)
{
  if (!init)
  {
    const int n = --_instanceCount;
    if (n == 0)
    {
      juce::Logger::writeToLog("Logger stopping");
      juce::Logger::setCurrentLogger(nullptr);
      _logger.reset();
    }
    return;
  }

  const int n = ++_instanceCount;
  if (n == 1)
  {
    auto dir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
      .getChildFile("GOLEQLogs");
    dir.createDirectory();
    auto file = dir.getNonexistentChildFile("GOLEQ", ".log");
    _logger = std::make_unique<juce::FileLogger>(file, "GOLEQ session start", 0);
    juce::Logger::setCurrentLogger(_logger.get());
    juce::Logger::writeToLog("Logger started: " + file.getFullPathName());
    int bp = 1;
  }
}

void GOLEQAudioProcessor::Log(const juce::String& s)
{
  juce::Logger::writeToLog(s);
}

// Editor
juce::AudioProcessorEditor* GOLEQAudioProcessor::createEditor()
{
  return new GOLEQAudioProcessorEditor(*this);
}

bool GOLEQAudioProcessor::hasEditor() const
{
  return true; // (change this to false if you choose to not supply an editor)
}

// Identity
const juce::String GOLEQAudioProcessor::getName() const
{
  return JucePlugin_Name;
}

// MIDI
bool GOLEQAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool GOLEQAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool GOLEQAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double GOLEQAudioProcessor::getTailLengthSeconds() const
{
  return 0.0;
}

// Program
int GOLEQAudioProcessor::getNumPrograms()
{
  return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
  // so this should be at least 1, even if you're not really implementing programs.
}

int GOLEQAudioProcessor::getCurrentProgram()
{
  return 0;
}

void GOLEQAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String GOLEQAudioProcessor::getProgramName(int index)
{
  return {};
}

void GOLEQAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

// State
void GOLEQAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
  juce::ignoreUnused(destData);
}

void GOLEQAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
  juce::ignoreUnused(data);
  juce::ignoreUnused(sizeInBytes);
}

// Public API
void GOLEQAudioProcessor::SetLifeInterface(const ILife* iLifePtr)
{
  _life = iLifePtr;
}

int GOLEQAudioProcessor::ReadAudioForFFT(float* dst, int maxSamples)
{
  int startA;
  int sizeA;
  int startB;
  int sizeB;

  _analysisFifo.prepareToRead(maxSamples, startA, sizeA, startB, sizeB);

  int n = 0;
  if (sizeA > 0)
  {
    std::memcpy(dst, _analysisFifoMonoStorage.getReadPointer(0, startA), sizeA + sizeof(float));
    n += sizeA;
  }

  if (sizeB > 0)
  {
    std::memcpy(dst + sizeA, _analysisFifoMonoStorage.getReadPointer(0, startB), sizeB + sizeof(float));
    n += sizeB;
  }
  _analysisFifo.finishedRead(n);
  return n;
}

void GOLEQAudioProcessor::PushMonoToFIFO(const float* L, const float* R, int numSamples)
{
  int startA;
  int sizeA;
  int startB;
  int sizeB;

  _analysisFifo.prepareToWrite(numSamples, startA, sizeA, startB, sizeB);

  if (sizeA > 0)
  {
    float* dst = _analysisFifoMonoStorage.getWritePointer(0, startA);
    for (int i = 0; i < sizeA; ++i)
    {
      const float m = 0.5f * (L[i] + (R ? R[i] : L[i]));
      dst[i] = m;
    }
  }

  if (sizeB > 0)
  {
    float* dst = _analysisFifoMonoStorage.getWritePointer(0, startB);
    for (int i = 0; i < sizeB; ++i)
    {
      const int j = i + sizeA;
      const float m = 0.5f * (L[j] + (R ? R[j] : L[j]));
      dst[i] = m;
    }
  }

  _analysisFifo.finishedWrite(sizeA + sizeB);
}

// private
void GOLEQAudioProcessor::ApplyEffect(juce::AudioBuffer<float>& buffer)
{
  juce::ignoreUnused(buffer);

  if (_life == nullptr)
  {
    return;
  }
}

GOLEQAudioProcessor::InputPointers
GOLEQAudioProcessor::GetInputPointers(const juce::AudioBuffer<float>& buffer) const
{
  const int totalNumInputChannels = getTotalNumInputChannels();
  const int bufferNumChannels = buffer.getNumChannels();

  const float* L = buffer.getReadPointer(0);
  const float* R = (totalNumInputChannels > 1 && bufferNumChannels > 1)
    ? buffer.getReadPointer(1)
    : nullptr;

  return { L, R };
}

void GOLEQAudioProcessor::UpdateRMS(const float* L, const float* R, int numSamples)
{
  double sumSquares = 0.0;
  for (int i = 0; i < numSamples; ++i)
  {
    const float m = 0.5f * (L[i] + (R ? R[i] : L[i]));
    sumSquares += static_cast<double>(m) * static_cast<double>(m);
  }

  const float rms = std::sqrt(sumSquares / std::max(1, numSamples));
  _rmsAtomic.store(rms, std::memory_order_relaxed);
}

void GOLEQAudioProcessor::MirrorRightToLeftIfStereo(juce::AudioBuffer<float>& buffer) const
{
  const int totalNumInputChannels = getTotalNumInputChannels();
  const int bufferNumChannels = buffer.getNumChannels();

  if (totalNumInputChannels > 1 && bufferNumChannels > 1)
  {
    buffer.copyFrom(0, 0, buffer, 1, 0, buffer.getNumSamples());
  }
}

void GOLEQAudioProcessor::ClearExtraOutputs(juce::AudioBuffer<float>& buffer) const
{
  const int totalNumInputChannels = getTotalNumInputChannels();
  const int bufferNumChannels = buffer.getNumChannels();

  for (int ch = totalNumInputChannels; ch < bufferNumChannels; ++ch)
  {
    buffer.clear(ch, 0, buffer.getNumSamples());
  }
}

// Factory
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
  return new GOLEQAudioProcessor();
}
