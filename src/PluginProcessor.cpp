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
                              std::make_unique<juce::AudioParameterInt>("bitrate", "BitRate", 8000, 40000, 16000)
                      }) {
    _parameters.addParameterListener("bitrate", this);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor() {
    unattachAllSliders();
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
    juce::ignoreUnused(samplesPerBlock);

    // Set the interlace buffer to the correct size
    _interlacedBuffer.resize(2 * samplesPerBlock);

    // Create the OPUS encoder
    int err;
    _encoder = opus_encoder_create(48000, 2, OPUS_APPLICATION_VOIP, &err);
    _decoder = opus_decoder_create(48000, 2, &err);

    // Set bitrate TODO: replace with parameters
    int bitrate = (int)*_parameters.getRawParameterValue("bitrate");
    DBG(bitrate);
    opus_encoder_ctl(_encoder, OPUS_SET_BITRATE(bitrate));

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

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages) {
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;

    interlace(buffer.getNumSamples(), buffer.getReadPointer(0), buffer.getReadPointer(1), _interlacedBuffer.data());
    _blockSizeAdapter.process(_interlacedBuffer);
    deinterlace(buffer.getNumSamples(), buffer.getWritePointer(0), buffer.getWritePointer(1), _interlacedBuffer.data());
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

void AudioPluginAudioProcessor::processChunk(std::span<float> chunk) {
    // Encode and decode packet
    unsigned char data[PACKET_SIZE];
    size_t bytes = opus_encode_float(_encoder, chunk.data(), chunk.size() / 2, data, PACKET_SIZE);
    opus_decode_float(_decoder, data, bytes, chunk.data(), chunk.size() / 2, 0);
}

void AudioPluginAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue) {
    if(_encoder == nullptr) return;
    if(parameterID == "bitrate") {
        opus_encoder_ctl(_encoder, OPUS_SET_BITRATE((int) newValue));
    }
}

//==============================================================================
void AudioPluginAudioProcessor::attachSlider(const juce::String& parameterId, juce::Slider& slider) {
    _sliderAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(_parameters, parameterId, slider));
}

void AudioPluginAudioProcessor::unattachAllSliders() {
    _sliderAttachments.clear();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new AudioPluginAudioProcessor();
}
