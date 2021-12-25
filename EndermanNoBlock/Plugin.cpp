#include "pch.h"
#include <EventAPI.h>
#include <LoggerAPI.h>

using namespace LL;
Logger logger("EndermanNoBlock");

void PluginMain()
{
    logger.info("Ok, now enderman can't carry the block.");
}

void PluginInit()
{
    registerPlugin("EndermanNoBlock", "Forbid enderman carry block.", Version{ 1,0,3,Version::Release }, { {"Author","RedbeanW"} });
	Event::ServerStartedEvent::subscribe([](Event::ServerStartedEvent ev) -> bool {
		PluginMain();
		return true;
	});
}

THook(bool, "?canUse@EndermanTakeBlockGoal@@UEAA_NXZ",
    void* _this)
{
    return false;
}

THook(bool, "?canUse@EndermanLeaveBlockGoal@@UEAA_NXZ",
    void* _this)
{
    return false;
}

THook(Block*, "?getCarryingBlock@EnderMan@@QEAAAEBVBlock@@XZ",
    void* _this, Block* a1)
{
    return *(Block**)dlsym("?mAir@BedrockBlocks@@3PEBVBlock@@EB");;
}

THook(void, "?setCarryingBlock@EnderMan@@QEAAXAEBVBlock@@@Z",
    void* _this, Block* bl)
{
    ; // do nothing.
}