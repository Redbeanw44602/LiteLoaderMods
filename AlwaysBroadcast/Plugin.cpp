#include "pch.h"
#include <EventAPI.h>
#include <LoggerAPI.h>

using namespace LL;

Logger logger("AlwaysBroadcast");

void PluginMain()
{
	logger.info("The bug is fixed.");
}

void PluginInit()
{
	registerPlugin("AlwaysBroadcast", "Fix an broadcast bug in bds.", Version{ 1,0,3,Version::Release }, { {"Author","RedbeanW"} });
	Event::ServerStartedEvent::subscribe([](Event::ServerStartedEvent ev) -> bool {
		PluginMain();
		return true;
	});
}

class LevelData;
THook(bool, "?getLANBroadcast@LevelData@@QEBA_NXZ",
	LevelData* _this)
{
	return true;
}

THook(bool, "?getLANBroadcastIntent@LevelData@@QEBA_NXZ",
	LevelData* _this)
{
	return true;
}