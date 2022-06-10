#include "pch.h"
#include <EventAPI.h>
#include <LoggerAPI.h>

using namespace LL;
Logger logger("NoWitherDestroy");

void PluginMain()
{
	logger.info("Wither destroy blocked, author: redbeanw.");
}

void PluginInit()
{
	registerPlugin("NoWitherDestroy", "Block wither destroy.", Version{ 1,0,4,Version::Release }, { {"Author","Redbeanw"} });
	Event::ServerStartedEvent::subscribe([](Event::ServerStartedEvent ev) -> bool {
		PluginMain();
		return true;
	});
}

THook(bool, "?explode@Level@@UEAAXAEAVBlockSource@@PEAVActor@@AEBVVec3@@M_N3M3@Z",
	void* _this, BlockSource* a2, Actor* a3, Vec3* a4, float a5, bool a6, bool a7, float a8, bool a9)
{
	if (a3)
	{
		auto type = SymCall("?EntityTypeToString@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@W4ActorType@@W4ActorTypeNamespaceRules@@@Z",
			std::string, int, int)(a3->getEntityTypeId(), 1);
		if (type == "minecraft:wither_skull_dangerous"
			|| type == "minecraft:wither_skull"
			|| type == "minecraft:wither")
		{
			return false;
		}
	}
	return original(_this, a2, a3, a4, a5, a6, a7, a8, a9);
}

THook(void, "?_destroyBlocks@WitherBoss@@AEAAXAEAVLevel@@AEBVAABB@@AEAVBlockSource@@H@Z",
	void* _this, Level* a2, AABB* a3, BlockSource* a4, int a5)
{
	; // do nothing
}