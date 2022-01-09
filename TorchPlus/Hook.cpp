#include "pch.h"
#include "Plugin.h"

THook(void, "?normalTick@Player@@UEAAXXZ",
    Player* pl)
{
    original(pl);
    int light = max(FindTorch::getBrightness(&pl->getSelectedItem()), FindTorch::getBrightness(&pl->getOffhandSlot()));
    if (light != 0)
        LightMgr::turnOn(pl, light);
    else
        LightMgr::turnOff(pl);
}

THook(void, "?sendBlockDestructionStarted@BlockEventCoordinator@@QEAAXAEAVPlayer@@AEBVBlockPos@@@Z",
    void* self, Player* pl, BlockPos* bp)
{
    original(self, pl, bp);
    auto mainhand = &pl->getSelectedItem();
    if (mainhand->isNull())
        return;
    auto newHand = mainhand->clone_s();
    if (FindTorch::isEnabled(newHand->getTypeName()) && pl->getOffhandSlot().isNull())
    {
        auto& cont = pl->getInventory();
        auto nowSlot = pl->getSelectedItemSlot();
        cont.removeItem(nowSlot, 64);
        pl->setOffhandSlot(*newHand);
        pl->sendInventory(true);
    }
}

THook(void, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z",
    ServerNetworkHandler* self, ServerPlayer* sp, bool a3)
{
    LightMgr::clear((Player*)sp);
    original(self, sp, a3);
}