#pragma once

#ifndef PLUGIN_H
#define PLUGIN_H

using namespace std;

bool sendNetBlock(BlockSource* bs, BlockPos bp, const unsigned int runtimeId);

namespace FindTorch {
    static unordered_map<string, int> Torches{
        {"minecraft:torch",14},
        {"minecraft:soul_torch",10},
        {"minecraft:redstone_torch",7}
    };

    inline bool isEnabled(string name) {
        return Torches.count(name);
    }

    inline int getBrightness(const ItemStack* it) {
        if (it->isNull())
            return 0;
        auto name = it->getTypeName();
        auto light = 0;
        if (isEnabled(name))
            light = Torches[name];
        return light;
    }
}

struct LightInfo {
    bool isLighting = false;
    BlockPos lightingPos = BlockPos::ZERO;
    int lightLevel = 0;
};

namespace LightMgr {
    static unordered_map<Player*, LightInfo> RecordedInfo;
    
    inline bool isTurningOn(Player* pl) {
        return RecordedInfo.count(pl) && RecordedInfo[pl].isLighting;
    }

    inline void turnOff(Player* pl) {
        if (!isTurningOn(pl))
            return;
        RecordedInfo[pl].isLighting = false;
        BlockSource* bs = &pl->getRegion();
        BlockPos bp = RecordedInfo[pl].lightingPos;
        sendNetBlock(bs, bp, bs->getBlock(bp).getRuntimeId());
    }

    inline void turnOn(Player* pl, int lightLv) {
        BlockSource* bs = pl->getBlockSource();
        BlockPos bp = pl->getBlockPos();
        BlockPos pos = { bp.x,bp.y + 1,bp.z };

        auto &Info = RecordedInfo[pl];
        bool isOpened = isTurningOn(pl);
        bool isSamePos = pos.operator==(Info.lightingPos);
        bool isSameLight = lightLv == Info.lightLevel;
        if (isOpened && isSamePos && isSameLight)
            return;

        if (bs->getBlock(pos).getName().getString() != "minecraft:air")
            return;

        sendNetBlock(bs, pos, VanillaBlocks::mLightBlock->getRuntimeId() - 15 + lightLv);
        if (!isSamePos && (isOpened || !isSameLight))
            turnOff(pl);

        Info.isLighting = true;
        Info.lightingPos = pos;
        Info.lightLevel = lightLv;
        
    }

    inline void init(Player* pl) {
        LightInfo li;
        RecordedInfo[pl] = li;
    }

    inline void clear(Player* pl) {
        turnOff(pl);
        RecordedInfo.erase(pl);
    }

}

#endif