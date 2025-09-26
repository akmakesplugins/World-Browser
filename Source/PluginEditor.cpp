/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "httplib/httplib.h"
#include <thread>
#include <atomic>

WorldSamplerAudioProcessorEditor::WorldSamplerAudioProcessorEditor (WorldSamplerAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    
    //SERVER STARTUP
    auto indexHtml = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
        .getParentDirectory()
        .getChildFile("dist");

    httpServer = std::make_unique<HttplibServer>(indexHtml.getFullPathName().toStdString(), 8080);
    httpServer->start();

    //WEB BROWSER COMPONENT
    using WBC = juce::WebBrowserComponent;
    auto options = WBC::Options()
        .withNativeIntegrationEnabled(true)
        .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
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

    webView->goToURL("http://127.0.0.1:8080/");
}

WorldSamplerAudioProcessorEditor::~WorldSamplerAudioProcessorEditor() 
{
    if (httpServer)
        httpServer->stop();
}

void WorldSamplerAudioProcessorEditor::resized()
{
    if (webView) webView->setBounds(getLocalBounds());
}

