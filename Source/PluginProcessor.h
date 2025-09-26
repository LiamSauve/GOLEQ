#pragma once

#include <atomic>
#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>

#include "ILife.h"
#include "Constants.h"

class GOLEQAudioProcessor : public juce::AudioProcessor
{
public:
  GOLEQAudioProcessor();
  ~GOLEQAudioProcessor() override;

  // JUCE Lifecycle
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif
  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  // Logging
  void InitialiseLogging(bool init);
  void Log(const juce::String& s);

  // Editor
  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  // Identity
  const juce::String getName() const override;

  // MIDI
  bool   acceptsMidi() const override;
  bool   producesMidi() const override;
  bool   isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  // Program
  int   getNumPrograms() override;
  int   getCurrentProgram() override;
  void  setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void  changeProgramName(int index, const juce::String& newName) override;

  // State
  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  // Public API
  void SetLifeInterface(const ILife* iLifePtr);
  int  ReadAudioForFFT(float* dst, int maxSamples);
  void PushMonoToFIFO(const float* L, const float* R, int numSamples);

  std::atomic<float>       _rmsAtomic{ 0.0f };
private:
  // Processing
  void ApplyEffect(juce::AudioBuffer<float>& buffer);

  // Dependencies
  const ILife* _life = nullptr;

  // Logging
  std::atomic<int> GOLEQAudioProcessor::_instanceCount{ 0 };
  std::unique_ptr<juce::FileLogger> GOLEQAudioProcessor::_logger;

  // Analysis
  juce::AbstractFifo       _analysisFifo{ Constants::AnalysisFifoCapacitySamples };
  juce::AudioBuffer<float> _analysisFifoMonoStorage{ 1, Constants::AnalysisFifoCapacitySamples };

  // processBlock Helpers
  struct InputPointers { const float* L; const float* R; };
  InputPointers GetInputPointers(const juce::AudioBuffer<float>& buffer) const;
  void UpdateRMS(const float* L, const float* R, int numSamples);
  void MirrorRightToLeftIfStereo(juce::AudioBuffer<float>& buffer) const;
  void ClearExtraOutputs(juce::AudioBuffer<float>& buffer) const;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GOLEQAudioProcessor)
};
