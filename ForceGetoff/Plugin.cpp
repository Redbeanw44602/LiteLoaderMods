#include "pch.h"
#include <EventAPI.h>
#include <LoggerAPI.h>

using namespace LL;
Logger logger("ForceGetoff");

bool onAttack(Event::PlayerAttackEvent event)
{
	auto target = event.mTarget;
	if (!target->isPlayer() && target->isRiding())
	{
		auto pos = target->getPos();
		target->teleport({ pos.x + 2,pos.y,pos.z + 2 }, target->getDimensionId());
		return false;
	}
	return true;
}

void PluginMain()
{
	logger.info("Now you can make entity get off from boat/horse...");
	Event::PlayerAttackEvent::subscribe(onAttack);
}

void PluginInit()
{
	registerPlugin("ForceGetoff", "Make entity get off from boat/horse...", Version{ 1,0,5,Version::Release }, { {"Author","Redbeanw"} });
	Event::ServerStartedEvent::subscribe([](Event::ServerStartedEvent ev) -> bool {
		PluginMain();
		return true;
	});
}