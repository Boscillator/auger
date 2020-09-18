#ifdef AUGER_DEBUG

#include <chrono>

#endif

#include <thread>

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "utilities.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
        : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                                 .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                                 .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
),
          _blockSizeAdapter(this, 2 * 480),
          _parameters(*this, nullptr, juce::Identifier("AugerPlugin"),
                      {
                              std::make_unique<juce::AudioParameterInt>("bitrate", "BitRate", 8000, 40000, 16000),
                              std::make_unique<juce::AudioParameterFloat>("drywet", "Dry/Wet", 0.0f, 1.0f, 1.0f),
                              std::make_unique<juce::AudioParameterChoice>("mode", "Mode", juce::StringArray(
                                      {"Natural", "Harsh", "Metallic"}), 0)
                      }) {
    _parameters.addParameterListener("bitrate", this);
    _parameters.addParameterListener("mode", this);
    _drywet = _parameters.getRawParameterValue("drywet");
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor() {
    unattachAllAttachements();
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms() {
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram() {
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram(int index) {
    juce::ignoreUnused(index);
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index) {
    juce::ignoreUnused(index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName(int index, const juce::String& newName) {
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    // Set the interlace buffer to the correct size
    _interlacedBuffer.resize(2 * samplesPerBlock);

    // Create the OPUS encoder
    int err;
    _encoder = opus_encoder_create(48000, 2, OPUS_APPLICATION_VOIP, &err);
    _decoder = opus_decoder_create(48000, 2, &err);

    // Set bitrate
    int bitrate = (int) *_parameters.getRawParameterValue("bitrate");
    opus_encoder_ctl(_encoder, OPUS_SET_BITRATE(bitrate));

    // Configure dry/wet mixer
    _dryWetMixer.setMixingRule(juce::dsp::DryWetMixingRule::balanced);
    juce::dsp::ProcessSpec spec{sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2};
    _dryWetMixer.prepare(spec);
    _dryWetMixer.setWetLatency(_blockSizeAdapter.getChunkSize());

    // Configure filter
    _lowpassFilterLeft.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, 8000, 0.1));
    _lowpassFilterRight.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, 8000, 0.1));

    // Initalize processing mode
    parameterChanged("mode", *_parameters.getRawParameterValue("mode"));

}

void AudioPluginAudioProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

//==============================================================================
void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages) {
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;

    // Set up dry/wet mixer
    _dryWetMixer.setWetMixProportion(*_drywet);
    _dryWetMixer.pushDrySamples(buffer);

    // Perform compression
    interlace(buffer.getNumSamples(), buffer.getReadPointer(0), buffer.getReadPointer(1), _interlacedBuffer.data());
    _blockSizeAdapter.process(_interlacedBuffer);
    deinterlace(buffer.getNumSamples(), buffer.getWritePointer(0), buffer.getWritePointer(1), _interlacedBuffer.data());

    // Filter samples to remove harshness
    if (_useFilter) {
        _lowpassFilterLeft.processSamples(buffer.getWritePointer(0), buffer.getNumSamples());
        _lowpassFilterRight.processSamples(buffer.getWritePointer(1), buffer.getNumSamples());
    }

    // Remix dry signal
    _dryWetMixer.mixWetSamples(buffer);
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor() {
    return new AudioPluginAudioProcessorEditor(*this);
//    return new juce::GenericAudioProcessorEditor(this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = _parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(_parameters.state.getType()))
            _parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
void AudioPluginAudioProcessor::processChunk(std::span<float> chunk) {
    // Encode and decode packet
    unsigned char data[PACKET_SIZE];
    size_t bytes = opus_encode_float(_encoder, chunk.data(), chunk.size() / 2, data, PACKET_SIZE);
    opus_decode_float(_decoder, data, bytes, chunk.data(), chunk.size() / 2, 0);
}

//==============================================================================
void AudioPluginAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue) {
    std::cout << parameterID << " updated to " << newValue << std::endl;
    if (_encoder == nullptr) return;
    else if (parameterID == "bitrate") {
        opus_encoder_ctl(_encoder, OPUS_SET_BITRATE((int) newValue));
    } else if (parameterID == "mode") {
        switch ((int) newValue) {
            case 0:
                updateFrameSize(480);
                _useFilter = true;
                break;
            case 1:
                updateFrameSize(480);
                _useFilter = false;
                break;
            case 2:
                updateFrameSize(120);
                _useFilter = false;
                break;
        }
    }
}

//==============================================================================
void AudioPluginAudioProcessor::attachSlider(const juce::String& parameterId, juce::Slider& slider) {
    _sliderAttachments.push_back(
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(_parameters, parameterId, slider));
}

void AudioPluginAudioProcessor::attachComboBox(const juce::String& parameterId, juce::ComboBox& comboBox) {
    _comboBoxAttachements.push_back(
            std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(_parameters, parameterId,
                                                                                     comboBox));
}

void AudioPluginAudioProcessor::unattachAllAttachements() {
    _sliderAttachments.clear();
    _comboBoxAttachements.clear();
}

void AudioPluginAudioProcessor::updateFrameSize(size_t size) {
    _blockSizeAdapter.setChunkSize(2 * size);
    _dryWetMixer.setWetLatency(size);
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new AudioPluginAudioProcessor();
}
