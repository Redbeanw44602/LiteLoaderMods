#include "pch.h"
#include "Plugin.h"

THook(void, "?normalTick@Player@@UEAAXXZ",
    Player* pl)
{
    original(pl);
    if (!Config::enable)
        return;
    int light = max(Config::getBrightness(&pl->getSelectedItem()), Config::getBrightness(&pl->getOffhandSlot()));
    if (light != 0)
        LightMgr::turnOn(pl, light);
    else
        LightMgr::turnOff(pl);
}

THook(void, "?sendBlockDestructionStarted@BlockEventCoordinator@@QEAAXAEAVPlayer@@AEBVBlockPos@@@Z",
    void* self, Player* pl, BlockPos* bp)
{
    original(self, pl, bp);
    if (!Config::enable)
        return;
    auto mainhand = &pl->getSelectedItem();
    if (mainhand->isNull() || !Config::isOffhandItem(mainhand->getTypeName()))
        return;
    auto newHand = mainhand->clone_s();
    if (Config::isLightSource(newHand->getTypeName()) && pl->getOffhandSlot().isNull())
    {
        auto& cont = pl->getInventory();
        auto nowSlot = pl->getSelectedItemSlot();
        cont.removeItem(nowSlot, 64);
        pl->setOffhandSlot(*newHand);
        pl->sendInventory(true);
    }
}