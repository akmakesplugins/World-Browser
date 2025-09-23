/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

WorldSamplerAudioProcessorEditor::WorldSamplerAudioProcessorEditor (WorldSamplerAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    using WBC = juce::WebBrowserComponent;

    auto options = WBC::Options()
        .withNativeIntegrationEnabled(true)
#if JUCE_WINDOWS
        .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
#endif
        .withEventListener("locationClicked", [this](juce::var args)
            {
                if (args.hasProperty("lat") && args.hasProperty("lon"))
                {
                    double lat = (double)args["lat"];
                    double lon = (double)args["lon"];
                    DBG("Received location from JS -> lat: " << lat << ", lon: " << lon);
                }
            });

    webView.reset(new WBC(options));
    addAndMakeVisible(*webView);

    setSize(800, 600);
    webView->goToURL("http://localhost:5173"); // dev server
}

WorldSamplerAudioProcessorEditor::~WorldSamplerAudioProcessorEditor() {}

void WorldSamplerAudioProcessorEditor::resized()
{
    if (webView) webView->setBounds(getLocalBounds());
}

