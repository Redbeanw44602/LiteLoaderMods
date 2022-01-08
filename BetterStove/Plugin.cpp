#include "pch.h"
#include <EventAPI.h>
#include <LoggerAPI.h>

using namespace LL;
using namespace std;

Logger logger("BetterStove");
static QWORD EID = 9223372036854775809;

void PluginMain()
{
	logger.info("loaded.");
}

void PluginInit()
{
	registerPlugin("BetterStove", "Improve stoves!", Version{ 1,0,0,Version::Release }, { {"Author","RedbeanW"} });
	Event::ServerStartedEvent::subscribe([](Event::ServerStartedEvent ev) -> bool {
		PluginMain();
		return true;
	});
}

THook(void, "?tick@FurnaceBlockActor@@UEAAXAEAVBlockSource@@@Z",
	FurnaceBlockActor* self, BlockSource* bs)
{
	original(self, bs);
	int remainFuel = self->getLitTime();
	int savedXp = self->getStoredXP();
	int cooked = self->getTickCount();
	int totalFuel = *((DWORD*)self + 107);
	int cookMax = 200;
	if (!remainFuel || !totalFuel)
		return;
	auto &bp = ((BlockActor*)self)->getPosition();
	for (auto pl : Level::getAllPlayers())
	{
		auto result = pl->sendAddEntityPacket(EID++, "minecraft:pig", Vec3{ (float)bp.x,(float)bp.y + 1,(float)bp.z }, Vec3{ 0,0,0 }, vector<FakeDataItem>{
			FakeDataItem(ActorDataIDs::ALWAYS_SHOW_NAMETAG, DataItemType::BYTE, (bool)1),
			FakeDataItem(ActorDataIDs::NAMETAG, DataItemType::STRING, u8"Testing.")
		});
		cout << result << "\n";
	}
}