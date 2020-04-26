/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SeriesJunctionWDF.h"
#include "ParallelJunctionWDF.h"

//==============================================================================
/**
*/
class ScreamerDistortionAudioProcessor  : public AudioProcessor
{
public:
    
    //==============================================================================
    ScreamerDistortionAudioProcessor();
    ~ScreamerDistortionAudioProcessor();

    dsp::Oversampling<float> oversampler;
    
    void initializeSbig(double R3);
    void recomputeSbig(double G3);
    void recomputeScatteringValues(double R3);
    double hdiff_diode_1N914(double Z, double v);
    double h_diode_1N914(double Z, double v, double a);
    double solve_NR_1N914(double v_guess, double a, double G);
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    
    float localSampleRate = 1.0;
    double Vt = 26e-3;
    double mu = 1.752;
    double Is = 2.52e-9;
    double v_n = 0;
    
    double** Sbig;
    double** S_P2;
    SeriesJunctionWDF S1;
    SeriesJunctionWDF S2;
    SeriesJunctionWDF S3;
    ParallelJunctionWDF P1;
    double R_nonlin;
    double R2;
    double C2;
    double c1_state;
    double c2_state;
    double c3_state;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScreamerDistortionAudioProcessor)
};
