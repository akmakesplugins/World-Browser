/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "httplib/HttpServer.h"

class WorldSamplerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    WorldSamplerAudioProcessorEditor (WorldSamplerAudioProcessor& p);
    ~WorldSamplerAudioProcessorEditor() override;

    void resized() override;

private:
    WorldSamplerAudioProcessor& processor;

    // WebBrowserComponent for the globe UI
    std::unique_ptr<juce::WebBrowserComponent> webView;

    std::unique_ptr<HttplibServer> httpServer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WorldSamplerAudioProcessorEditor)
};

