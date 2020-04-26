/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ScreamerDistortionAudioProcessor::ScreamerDistortionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
oversampler(getTotalNumInputChannels(), 2, dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR, true)
#endif
{
}

ScreamerDistortionAudioProcessor::~ScreamerDistortionAudioProcessor()
{
}

//==============================================================================
const String ScreamerDistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ScreamerDistortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ScreamerDistortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ScreamerDistortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ScreamerDistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ScreamerDistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ScreamerDistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ScreamerDistortionAudioProcessor::setCurrentProgram (int index)
{
}

const String ScreamerDistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void ScreamerDistortionAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ScreamerDistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    if (sampleRate != localSampleRate){
        localSampleRate = sampleRate*4;
        oversampler.initProcessing(samplesPerBlock);
        
        C2 = 1/(2*localSampleRate*51e-12);
        R2 = 51e+3;
        double R3 = 250000.0;
        double adaptG = (1/C2) + (1/(R3+R2));
        initializeSbig(adaptG);
        double g_n[3];
        g_n[0] = 1/C2;
        g_n[1] = 1/(R3+R2);
        g_n[2] = adaptG;
        double sum_elements = g_n[0] + g_n[1] + g_n[2];
        double alpha[3];
        alpha[0] = g_n[0]*(2/sum_elements);
        alpha[1] = g_n[1]*(2/sum_elements);
        alpha[2] = g_n[2]*(2/sum_elements);
        double** eye = MatrixEng::identity(3);
        S_P2 = new double*[3];
        for (int i = 0; i < 3; i++) {
            S_P2[i] = new double[3];
            for (int j = 0; j<3; j++) {
                S_P2[i][j] = -eye[i][j] + alpha[j];
            }
        }
        R_nonlin = 1/adaptG;
        c1_state = 0;
        c2_state = 0;
        c3_state = 0;
        this -> S1 = SeriesJunctionWDF(1.0, 1/(2*localSampleRate*1e-6), 3);
        this -> S2 = SeriesJunctionWDF(S1.getAdaptRes(), 220, 3);
        this -> S3 = SeriesJunctionWDF(1/(2*localSampleRate*0.047e-6), 4.7e+3, 3);
        this -> P1 = ParallelJunctionWDF(S2.getAdaptRes(), 10e+3, 3);
    }
}

void ScreamerDistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    c1_state = 0;
    c2_state = 0;
    c3_state = 0;
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ScreamerDistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void ScreamerDistortionAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    int bufferLength = buffer.getNumSamples()*4; //oversampling factor 4

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    dsp::AudioBlock<float> block(buffer);
    dsp::AudioBlock<float> highSampleRateBlock = oversampler.processSamplesUp(block);
    double a[5];
    double sct1[3];
    //double scatter1;
    double sct2[3];
    //double scatter2;
    double sct3[3];
    dsp::Matrix<double> S_row3(1,3);
    double** S_S1;
    double** S_S2;
    double** S_P1;
    double** S_S3;
    double b[5];
    double b_sct1[3];
    dsp::Matrix<double> S_row4(1,5);
    
    float* bufferData = highSampleRateBlock.getChannelPointer(0);
    
    for (int i = 0; i<bufferLength; i++) {
        
        float temp = bufferData[i];
        
        // forward scan
        
        sct1[0] = temp;
        sct1[1] = c1_state;
        sct1[2] = 0;
        S_S1 = S1.getS();
        double a_out_S1 = S_S1[2][0]*sct1[0] + S_S1[2][1]*sct1[1] + S_S1[2][2]*sct1[2];
        sct1[0] = a_out_S1;
        sct1[1] = 0;
        sct1[2] = 0;
        S_S2 = S2.getS();
        double a_out_S2 = S_S2[2][0]*sct1[0];
        sct1[0] = a_out_S2;
        S_P1 = P1.getS();
        a[0] = S_P1[2][0]*sct1[0];
        sct2[0] = c3_state;
        sct2[1] = 0;
        sct2[2] = 0;
        S_S3 = S3.getS();
        a[1] = S_S3[2][0]*sct2[0];
        sct3[0] = c2_state;
        sct3[1] = 0;
        sct3[2] = 0;
        a[2] = S_P2[2][0]*sct3[0];
        a[4] = 0;
        
        // diode equation
        double sct_nonlin = Sbig[3][0]*a[0] + Sbig[3][1]*a[1] + Sbig[3][2]*a[2] + Sbig[3][3]*a[3] + Sbig[3][4]*a[4];
        a[3] = solve_NR_1N914(v_n, sct_nonlin, 1/R_nonlin);
        
        // backward scan
        b[0] = Sbig[0][0]*a[0] + Sbig[0][1]*a[1] + Sbig[0][2]*a[2] + Sbig[0][3]*a[3] + Sbig[0][4]*a[4];
        b[1] = Sbig[1][0]*a[0] + Sbig[1][1]*a[1] + Sbig[1][2]*a[2] + Sbig[1][3]*a[3] + Sbig[1][4]*a[4];
        b[2] = Sbig[2][0]*a[0] + Sbig[2][1]*a[1] + Sbig[2][2]*a[2] + Sbig[2][3]*a[3] + Sbig[2][4]*a[4];
        b[4] = Sbig[4][0]*a[0] + Sbig[4][1]*a[1] + Sbig[4][2]*a[2] + Sbig[4][3]*a[3] + Sbig[4][4]*a[4];
        
        float out = 0.5*(a[4] + b[4]);
        bufferData[i] = out;
        b_sct1[0] = a_out_S2;
        b_sct1[1] = 0;
        b_sct1[2] = S_P1[0][0]*b_sct1[0] + S_P1[0][2]*b[0];
        b_sct1[0] = a_out_S1;
        b_sct1[2] = S_S2[0][0]*a_out_S1 + S_S2[0][2]*b_sct1[2];
        b_sct1[0] = temp;
        b_sct1[1] = c1_state;
        c1_state = S_S1[1][0]*b_sct1[0] + S_S1[1][1]*b_sct1[1] + S_S1[1][2]*b_sct1[2];
        c2_state = S_P2[0][0]*c2_state + S_P2[0][2]*b[2];
        c3_state = S_S3[0][0]*c3_state + S_S3[0][2]*b[1];
    }
    oversampler.processSamplesDown(block);
    if (totalNumOutputChannels == 2)
    {
        buffer.copyFrom(1, 0, buffer.getReadPointer(0), buffer.getNumSamples());
    }
}

//==============================================================================
bool ScreamerDistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ScreamerDistortionAudioProcessor::createEditor()
{
    return new ScreamerDistortionAudioProcessorEditor (*this);
}

//==============================================================================
void ScreamerDistortionAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ScreamerDistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void ScreamerDistortionAudioProcessor::initializeSbig(double G3){
    
    Sbig = new double*[5];
    for (int i = 0; i<5; i++) {
        Sbig[i] = new double[5];
        for (int j=0; j<5; j++) {
            Sbig[i][j] = 0.0;
            
        }
    }
    Sbig[0][0] = 1;
    Sbig[1][0] = 2;
    Sbig[1][1] = -1;
    Sbig[2][3] = 1;
    Sbig[3][2] = 1;
    Sbig[4][2] = 1;
    Sbig[4][3] = 1;
    Sbig[4][4] = -1;
    double value1 = 887804898779493.25;
    double value2 = 4172683024263618275.0;
    double value3 = 9444732965739290427392.0;
    double value4 = 3551219595117973.0;
    double value5 = 75557863725914323419136.0;
    double value6 = 33381464194108946200.0;
    double den =(G3*(value2*localSampleRate + value3));
    double x1 = (value1*localSampleRate)/den;
    double x2 = (0.25*(value4*localSampleRate + value5*G3 + value6*G3*localSampleRate))/den;
    Sbig[2][0] = x1;
    Sbig[3][0] = x1;
    Sbig[4][0] = x2;
    Sbig[2][1] = -x1;
    Sbig[3][1] = -x1;
    Sbig[4][1] = -x1;
}

void ScreamerDistortionAudioProcessor::recomputeSbig(double G3){
    
    double value1 = 887804898779493.25;
    double value2 = 4172683024263618275.0;
    double value3 = 9444732965739290427392.0;
    double value4 = 3551219595117973.0;
    double value5 = 75557863725914323419136.0;
    double value6 = 33381464194108946200.0;
    double den =(G3*(value2*localSampleRate + value3));
    double x1 = (value1*localSampleRate)/den;
    double x2 = (0.25*(value4*localSampleRate + value5*G3 + value6*G3*localSampleRate))/den;
    Sbig[2][0] = x1;
    Sbig[3][0] = x1;
    Sbig[4][0] = x2;
    Sbig[2][1] = -x1;
    Sbig[3][1] = -x1;
    Sbig[4][1] = -x1;
}

void ScreamerDistortionAudioProcessor::recomputeScatteringValues(double R3){
    double adaptG = (1/C2) + (1/(R3+R2));
    double g_n[3];
    g_n[0] = 1/C2;
    g_n[1] = 1/(R3+R2);
    g_n[2] = adaptG;
    double sum_elements = g_n[0] + g_n[1] + g_n[2];
    double alpha[3];
    alpha[0] = g_n[0]*(2/sum_elements);
    alpha[1] = g_n[1]*(2/sum_elements);
    alpha[2] = g_n[2]*(2/sum_elements);
    double** eye = MatrixEng::identity(3);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j<3; j++) {
            S_P2[i][j] = -eye[i][j] + alpha[j];
        }
    }
    R_nonlin = 1/adaptG;
    c2_state = 0;
    recomputeSbig(adaptG);
}

double ScreamerDistortionAudioProcessor::hdiff_diode_1N914(double Z, double v){
    double out = (2*Is/(mu*Vt))*cosh(v/(mu*Vt)) + 1/Z;
    return out;
}

double ScreamerDistortionAudioProcessor::h_diode_1N914(double Z, double v, double a){
    double out = 2*Is*sinh(v/(mu*Vt)) - ((a-v)/Z);
    return out;
}

double ScreamerDistortionAudioProcessor::solve_NR_1N914(double v_guess, double a, double G){
    
    double Z = 1/G;
    double v = v_guess - (h_diode_1N914(Z, v_guess, a)/hdiff_diode_1N914(Z, v_guess));
    double val_lim = abs(v-v_guess);
    double lim = pow(10, 8);
    
    while (val_lim > lim) {
        v_guess = v;
        v = v_guess - (h_diode_1N914(Z, v_guess, a)/hdiff_diode_1N914(Z, v_guess));
        val_lim = abs(v-v_guess);
    }
    v_n = v;
    double b_n = 2*v_n - a;
    return b_n;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ScreamerDistortionAudioProcessor();
}
