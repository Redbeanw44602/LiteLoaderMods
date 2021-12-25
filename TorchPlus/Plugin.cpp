#include "pch.h"
#include <EventAPI.h>
#include <LoggerAPI.h>

using namespace std;
using namespace LL;
Logger logger("TorchPlus");

const Version PLUGIN_VERSION { 1,5,2,Version::Release };

unordered_map<string, int> EnItem{
	{"minecraft:torch",14},
	{"minecraft:soul_torch",10},
	{"minecraft:redstone_torch",7}
};

bool onStartDestroy(Event::PlayerStartDestroyBlockEvent event)
{
    auto pl = event.mPlayer;
	auto &mainhand = pl->getSelectedItem();
    if (mainhand.isNull())
        return true;
    auto newHand = mainhand.clone();
    auto &offhand = pl->getOffhandSlot();
    if (EnItem.find(newHand.getTypeName()) != EnItem.end() && offhand.isNull())
    {
        auto &cont = pl->getInventory();
        auto nowSlot = pl->getSelectedItemSlot();
        cont.removeItem(nowSlot, 64);
        pl->setOffhandSlot(newHand);
        pl->sendInventory(true);

    }
    return true;
}

bool onPlayerLeft(Event::PlayerLeftEvent event)
{
    turnOffLight((Player*)event.mPlayer);
    return true;
}

void PluginMain()
{
    logger.info("Enhanced torches, ver " + to_string(PLUGIN_VERSION.major) + "." + to_string(PLUGIN_VERSION.minor) + "." + to_string(PLUGIN_VERSION.revision) + ", author: redbeanw.");
    Event::PlayerStartDestroyBlockEvent::subscribe(onStartDestroy);
    Event::PlayerLeftEvent::subscribe(onPlayerLeft);
}

void PluginInit()
{
    LL::registerPlugin("TorchPlus", "Enhance the torches.", PLUGIN_VERSION, { {"Author","RedbeanW"}, {"Github","https://github.com/Redbeanw44602/TorchPlus.git"} });
    Event::ServerStartedEvent::subscribe([](Event::ServerStartedEvent ev) -> bool {
        PluginMain();
        return true;
    });
}

THook(void, "?normalTick@Player@@UEAAXXZ",
    Player* pl)
{
    original(pl);

    auto &mainhand = pl->getSelectedItem();
    auto &offhand = pl->getOffhandSlot();

    int light = 0;
    if (!mainhand.isNull())
    {
        string m_name = mainhand.getTypeName();
        if (EnItem.find(m_name) != EnItem.end())
            light = EnItem[m_name];
    }
    if (!offhand.isNull())
    {
        string o_name = offhand.getTypeName();
        if (EnItem.find(o_name) != EnItem.end())
            light = max(light, EnItem[o_name]);
    }

    if (light != 0)
        turnOnLight(pl, light);
    else
        turnOffLight(pl);
}

// Protect the light block.

THook(bool, "?_attachedBlockWalker@PistonBlockActor@@AEAA_NAEAVBlockSource@@AEBVBlockPos@@EE@Z",
    void* _this, BlockSource* a2, const BlockPos* a3, unsigned __int8 a4, char a5)
{
    if (a2->getBlock(*a3).getName().getString() == "minecraft:light_block")
        return false;
    return original(_this, a2, a3, a4, a5);
}

THook(float, "?getExplosionResistance@Block@@QEBAMPEAVActor@@@Z",
    Block* bl, Actor* ac)
{
    if (bl->getName().getString() == "minecraft:light_block")
        return 1.8e+07; // bedrock
    return original(bl, ac);
}