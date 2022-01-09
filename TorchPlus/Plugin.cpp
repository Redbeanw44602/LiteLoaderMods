#include "pch.h"
#include <EventAPI.h>
#include <LoggerAPI.h>

using namespace std;
using namespace LL;

Logger logger("TorchPlus");
Version PLUGIN_VERSION { 1,6,1,Version::Release };

void PluginMain()
{
    logger.info("Enhanced torches, ver " + PLUGIN_VERSION.toString() + ", author: redbeanw.");
}

void PluginInit()
{
    LL::registerPlugin("TorchPlus", "Enhance the torches.", PLUGIN_VERSION, { {"Author","RedbeanW"}, {"Github","https://github.com/Redbeanw44602/TorchPlus.git"} });
    Event::ServerStartedEvent::subscribe([](Event::ServerStartedEvent ev) -> bool {
        PluginMain();
        return true;
    });
}