/*
  ==============================================================================

    HttpServer.h
    Created: 26 Sep 2025 5:28:09pm
    Author:  Pitch Innovations

  ==============================================================================
*/

#pragma once
/*
  ==============================================================================

    HTTPServer.h
    Created: 4 Aug 2025 12:48:45pm
    Author:  Pitch Innovations

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
using namespace juce;

#pragma once
#include "httplib.h"
#include <thread>
#include <atomic>

class HttplibServer
{
public:
    HttplibServer(const std::string& mountPath, int port = 8080)
        : mountPath(mountPath)
        , port(port)
        , running(false) 
    {
    }

    ~HttplibServer() { stop(); }

    void start()
    {
        if (running.load())
            return;

        running = true;
        server.set_mount_point("/", mountPath.c_str());

        serverThread = std::thread([this]() {
            server.listen("127.0.0.1", port);
            });
    }

    void stop()
    {
        if (!running.load())
            return;

        server.stop();
        if (serverThread.joinable())
            serverThread.join();

        running = false;
    }

    int getPort() const { return port; }

private:
    httplib::Server server;
    std::string mountPath;
    int port;
    std::thread serverThread;
    std::atomic<bool> running;
};
