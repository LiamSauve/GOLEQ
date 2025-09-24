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
}

GOLEQAudioProcessor::~GOLEQAudioProcessor()
{
}

const juce::String GOLEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

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

int GOLEQAudioProcessor::getNumPrograms()
{
  return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
  // so this should be at least 1, even if you're not really implementing programs.
}

int GOLEQAudioProcessor::getCurrentProgram()
{
  return 0;
}

void GOLEQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GOLEQAudioProcessor::getProgramName (int index)
{
    return {};
}

void GOLEQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void GOLEQAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
  // Use this method as the place to do any pre-playback
  // initialisation that you need..

  //const int maxDelaySamples = static_cast<int>(sampleRate * 0.75); // 750ms max
  //_leftDelayBuffer.resize(maxDelaySamples, 0.0f);
  //_rightDelayBuffer.resize(maxDelaySamples, 0.0f);
  //
  //_leftWriteIndex = 0;
  //_rightWriteIndex = 0;

}

void GOLEQAudioProcessor::releaseResources()
{
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GOLEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GOLEQAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
  const int numSamples = buffer.getNumSamples();
  const float* L = buffer.getReadPointer(0);
  const float* R = (getTotalNumInputChannels() > 1) ? buffer.getReadPointer(1) : nullptr;

  //
  double s = 0.0;
  for (int i = 0; i < numSamples; ++i)
  {
    const float m = 0.5f * (L[i] + (R ? R[i] : L[i]));
    s += double(m) * double(m);
  }
  _rmsAtomic.store(std::sqrt(s / std::max(1, numSamples)), std::memory_order_relaxed);
  PushMonoToFIFO(L, R, numSamples);

  juce::ScopedNoDenormals noDenormals;

  const int totalNumInputChannels = getTotalNumInputChannels();
  const int totalNumOutputChannels = getTotalNumOutputChannels();

  // Copy right input channel to left channel
  if (totalNumInputChannels >= 2) // check we have at least two input channels
  {
    const float* rightIn = buffer.getReadPointer(1);
    float* leftInOut = buffer.getWritePointer(0);
    float* rightInOut = buffer.getWritePointer(1);

    for (int i = 0; i < numSamples; ++i)
    {
      leftInOut[i] = rightIn[i]; // duplicate right channel
      rightInOut[i] = rightIn[i]; // keep original right input
    }
  }

  // Clear any extra output channels beyond input channels
  for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
  {
    buffer.clear(i, 0, numSamples);
  }

  // Apply the delay effect
  ApplyEffect(buffer);
}

bool GOLEQAudioProcessor::hasEditor() const
{
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GOLEQAudioProcessor::createEditor()
{
  return new GOLEQAudioProcessorEditor(*this);
}

void GOLEQAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
}

void GOLEQAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
  // You should use this method to restore your parameters from this memory block,
  // whose contents will have been created by the getStateInformation() call.
}


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
    auto* dst = _analysisFifoMonoStorage.getWritePointer(0, startA);
    for (int i = 0; i < sizeA; ++i)
    {
      const float m = 0.5f * (L[i] + (R ? R[i] : L[i])); dst[i] = m;
    }
  }
  if (sizeB > 0)
  {
    auto* dst = _analysisFifoMonoStorage.getWritePointer(0, startB);
    for (int i = 0; i < sizeB; ++i)
    {
      const int j = i + sizeA; const float m = 0.5f * (L[j] + (R ? R[j] : L[j])); dst[i] = m;
    }
  }
  _analysisFifo.finishedWrite(sizeA + sizeB);
}

void GOLEQAudioProcessor::ApplyEffect(juce::AudioBuffer<float>& buffer)
{
  if (!_life)
  {
    return;
  }
}

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
  return new GOLEQAudioProcessor();
}
