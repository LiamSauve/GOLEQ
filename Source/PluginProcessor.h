#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <juce_dsp/juce_dsp.h>
#include "ILife.h"

class GOLEQAudioProcessor  : public juce::AudioProcessor
{
public:
  static constexpr int kFftOrder                    = 11;              // 2048-point FFT (if you want it here)
  static constexpr int kFftSize                     = 1 << kFftOrder;
  static constexpr int kAnalysisFifoCapacitySamples = kFftSize * 8;    // 8 windows of headroom

  GOLEQAudioProcessor();
  ~GOLEQAudioProcessor() override;

  void prepareToPlay (double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

  #ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
  #endif

  void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram (int index) override;
  const juce::String getProgramName (int index) override;
  void changeProgramName (int index, const juce::String& newName) override;

  void getStateInformation (juce::MemoryBlock& destData) override;
  void setStateInformation (const void* data, int sizeInBytes) override;

  void SetLifeInterface(const ILife* iLifePtr);

  int ReadAudioForFFT(float* dst, int maxSamples);
  void PushMonoToFIFO(const float* L, const float* R, int numSamples);

private:
  void ApplyEffect(juce::AudioBuffer<float>& buffer);
  const ILife* _life = nullptr; // Read-only, non-owning
  
  juce::AbstractFifo _analysisFifo{ kAnalysisFifoCapacitySamples };
  juce::AudioBuffer<float> _analysisFifoMonoStorage{ 1, kAnalysisFifoCapacitySamples };
  std::atomic<float> _rmsAtomic{0.0f};

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GOLEQAudioProcessor)
};
