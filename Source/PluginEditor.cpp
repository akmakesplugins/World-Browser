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

        .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
        .withResourceProvider([=](const juce::String& path) -> std::optional<WBC::Resource>
            {
                juce::String relativePath = path;

                if (relativePath == "/" || relativePath.isEmpty())
                    relativePath = "index.html";
                else
                    relativePath = relativePath.trimCharactersAtStart("/");

                juce::File indexHtml = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
                    .getParentDirectory()
                    .getChildFile("dist")
                    .getChildFile(relativePath);

                if (indexHtml.existsAsFile())
                {
                    juce::MemoryBlock data;
                    indexHtml.loadFileAsData(data);

                    // Convert MemoryBlock std::vector<std::byte>
                    std::vector<std::byte> bytes;
                    bytes.resize(data.getSize());
                    std::memcpy(bytes.data(), data.getData(), data.getSize());

                    // choose MIME type
                    juce::String ext = indexHtml.getFileExtension().toLowerCase();
                    juce::String mimeType = "application/octet-stream";

                    if (ext == ".html") mimeType = "text/html";
                    else if (ext == ".js" || ext == ".mjs") mimeType = "application/javascript";
                    else if (ext == ".css") mimeType = "text/css";
                    else if (ext == ".json") mimeType = "application/json";
                    else if (ext == ".png") mimeType = "image/png";
                    else if (ext == ".jpg" || ext == ".jpeg") mimeType = "image/jpeg";
                    else if (ext == ".gif") mimeType = "image/gif";
                    else if (ext == ".svg") mimeType = "image/svg+xml";
                    else if (ext == ".ico") mimeType = "image/x-icon";
                    else if (ext == ".woff") mimeType = "font/woff";
                    else if (ext == ".woff2") mimeType = "font/woff2";
                    else if (ext == ".ttf") mimeType = "font/ttf";
                    else if (ext == ".eot") mimeType = "application/vnd.ms-fontobject";
                    else if (ext == ".mp4") mimeType = "video/mp4";
                    else if (ext == ".webm") mimeType = "video/webm";
                    else if (ext == ".ogg") mimeType = "audio/ogg";
                    else if (ext == ".mp3") mimeType = "audio/mpeg";

                    return WBC::Resource{ std::move(bytes), mimeType };
                }
                return std::nullopt;
            })
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
    webView->goToURL(WBC::getResourceProviderRoot());

    /*
    auto resourcesDir = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
        .getParentDirectory()
        .getChildFile("dist");

    auto indexFile = resourcesDir.getChildFile("index.html");

    auto fileUrl = "file:///" + indexFile.getFullPathName().replace("\\", "/");
    webView->goToURL(fileUrl);
    
    //webView->goToURL("https://appassets/index.html");
    */
}

WorldSamplerAudioProcessorEditor::~WorldSamplerAudioProcessorEditor() {}

void WorldSamplerAudioProcessorEditor::resized()
{
    if (webView) webView->setBounds(getLocalBounds());
}

