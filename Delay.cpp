// Karl Yerkes
// 2023-02-07
// MAT 240B ~ Audio Programming
//

#include <juce_audio_processors/juce_audio_processors.h>

template <typename T>
T mtof(T m) {
  return T(440) * pow(T(2), (m - T(69)) / T(12));
}

template <typename T>
T dbtoa(T db) {
  return pow(T(10), db / T(20));
}

class Delay : public juce::AudioProcessor {
  juce::AudioParameterFloat* gain;
  juce::AudioParameterFloat* delay;
  /// add parameters here ///////////////////////////////////////////////////

 public:
  Delay()
      : AudioProcessor(
            BusesProperties()
                .withInput("Input", juce::AudioChannelSet::stereo())
                .withOutput("Output", juce::AudioChannelSet::stereo())) {
    addParameter(gain = new juce::AudioParameterFloat(
                     {"gain", 1}, "Gain",
                     juce::NormalisableRange<float>(-65, -1, 0.01f), -65));
    addParameter(delay = new juce::AudioParameterFloat(
                     {"delay", 1}, "Delay",
                     juce::NormalisableRange<float>(0, 4, 0.01f), 0.7f));
    /// add parameters here /////////////////////////////////////////////

    // XXX getSampleRate() is not valid here
  }

  /// handling the actual audio! ////////////////////////////////////////////
  void processBlock(juce::AudioBuffer<float>& buffer,
                    juce::MidiBuffer&) override {
    buffer.clear(0, 0, buffer.getNumSamples());
    auto left = buffer.getWritePointer(0, 0);
    auto right = buffer.getWritePointer(1, 0);

    for (int i = 0; i < buffer.getNumSamples(); ++i) {
      left[i] = 0;
      right[i] = left[i];
    }
  }

  /// handle doubles ? //////////////////////////////////////////////////////
  // void processBlock(AudioBuffer<double>& buffer, MidiBuffer&) override {
  //   buffer.applyGain(dbtoa((float)*gain));
  // }

  /// start and shutdown callbacks///////////////////////////////////////////
  void prepareToPlay(double, int) override {
    // XXX when does this get called? seems to not get called in stand-alone
  }
  void releaseResources() override {}

  /// maintaining persistant state on suspend ///////////////////////////////
  void getStateInformation(juce::MemoryBlock& destData) override {
    juce::MemoryOutputStream(destData, true).writeFloat(*gain);
  }

  void setStateInformation(const void* data, int sizeInBytes) override {
    gain->setValueNotifyingHost(
        juce::MemoryInputStream(data, static_cast<size_t>(sizeInBytes), false)
            .readFloat());
  }

  /// general configuration /////////////////////////////////////////////////
  const juce::String getName() const override { return "Quasi Band Limited"; }
  double getTailLengthSeconds() const override { return 0; }
  bool acceptsMidi() const override { return true; }
  bool producesMidi() const override { return false; }

  /// for handling presets //////////////////////////////////////////////////
  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int) override {}
  const juce::String getProgramName(int) override { return "None"; }
  void changeProgramName(int, const juce::String&) override {}

  /// ?????? ////////////////////////////////////////////////////////////////
  bool isBusesLayoutSupported(const BusesLayout& layouts) const override {
    const auto& mainInLayout = layouts.getChannelSet(true, 0);
    const auto& mainOutLayout = layouts.getChannelSet(false, 0);

    return (mainInLayout == mainOutLayout && (!mainInLayout.isDisabled()));
  }

  /// automagic user interface //////////////////////////////////////////////
  juce::AudioProcessorEditor* createEditor() override {
    return new juce::GenericAudioProcessorEditor(*this);
  }
  bool hasEditor() const override { return true; }

 private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Delay)
};

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new Delay(); }
