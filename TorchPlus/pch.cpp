// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"

using namespace std;

unordered_map<Player*, BlockPos> lightMem;
unordered_map<Player*, bool> openMem;

void turnOnLight(Player* pl, int lightLevel)
{
    BlockPos b_pos = pl->getBlockPos();
    BlockPos pos = { b_pos.x,b_pos.y + 1,b_pos.z };
    BlockSource* bs = pl->getBlockSource();

    if (bs->getBlock(pos).getName().getString() != "minecraft:air")
        return;

    Global<Level>->setBlock(pos, bs->getDimensionId(), "minecraft:light_block", lightLevel);
    if (openMem.find(pl) != openMem.end() && openMem[pl]) // if opened.
    {
        if (pos.operator==(lightMem[pl]))
            return;
        turnOffLight(pl);
    }
    lightMem[pl] = pos;
    openMem[pl] = true;
}

void turnOffLight(Player* pl)
{
    if (openMem.find(pl) == openMem.end() || !openMem[pl]) // not opened.
        return;
    openMem[pl] = false;
    if (Global<Level>->getBlock(lightMem[pl], pl->getBlockSource())->getTypeName() == "minecraft:light_block")
        Global<Level>->setBlock(lightMem[pl], pl->getDimensionId(), "minecraft:air", 1);
}