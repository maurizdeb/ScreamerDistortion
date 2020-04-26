/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ScreamerDistortionAudioProcessorEditor::ScreamerDistortionAudioProcessorEditor (ScreamerDistortionAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (200, 400);
    
    gainSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    gainSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 25);
    
    gainSlider.setRange(0, 500000, 1);
    gainSlider.setValue(0);
    addAndMakeVisible(gainSlider);
    addAndMakeVisible(gainLabel);
    gainLabel.setText("Drive", dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, true);
    
    gainSlider.addListener(this);
}

ScreamerDistortionAudioProcessorEditor::~ScreamerDistortionAudioProcessorEditor()
{
}

//==============================================================================
void ScreamerDistortionAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    
    
}

void ScreamerDistortionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    gainSlider.setBounds(getLocalBounds());
}

void ScreamerDistortionAudioProcessorEditor::sliderValueChanged(Slider* slider){
    
    if(slider == &gainSlider){
        processor.recomputeScatteringValues(gainSlider.getValue());
    }
}
