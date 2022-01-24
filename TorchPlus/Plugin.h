#pragma once

#ifndef PLUGIN_H
#define PLUGIN_H

using namespace std;

// RuntimeId = 5458 -> 5473 , LightBlock 1-14;
bool sendNetBlock(BlockSource* bs, BlockPos bp, const unsigned int runtimeId)
{
    auto dim = Global<Level>->getDimension(bs->getDimensionId());
    BinaryStream wp;
    wp.writeVarInt(bp.x);
    wp.writeUnsignedVarInt(bp.y);
    wp.writeVarInt(bp.z);
    wp.writeUnsignedVarInt(runtimeId);
    wp.writeUnsignedVarInt(3);
    wp.writeUnsignedVarInt(0);
    shared_ptr<Packet> pkt = MinecraftPackets::createPacket(MinecraftPacketIds::UpdateBlock);
    pkt->read(wp);
    dim->sendPacketForPosition({ bp.x, bp.y, bp.z }, *pkt, nullptr);
    return true;
}

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

namespace LightMgr {
    static unordered_map<Player*, bool> IsLighting;
    static unordered_map<Player*, BlockPos> LightingPos;
    static unordered_map<Player*, int> LightingLevel;
    
    inline bool isTurningOn(Player* pl) {
        return IsLighting.count(pl) && IsLighting[pl];
    }

    inline void turnOff(Player* pl) {
        if (!isTurningOn(pl))
            return;
        IsLighting[pl] = false;
        BlockSource* bs = &pl->getRegion();
        BlockPos bp = LightingPos[pl];
        sendNetBlock(bs, bp, bs->getBlock(bp).getRuntimeId());
    }

    inline void turnOn(Player* pl, int lightLv) {
        BlockSource* bs = pl->getBlockSource();
        BlockPos bp = pl->getBlockPos();
        BlockPos pos = { bp.x,bp.y + 1,bp.z };

        bool isOpened = isTurningOn(pl);
        bool isSamePos = pos.operator==(LightingPos[pl]);
        bool isSameLight = lightLv == LightingLevel[pl];
        if (isOpened && isSamePos && isSameLight)
            return;

        if (bs->getBlock(pos).getName().getString() != "minecraft:air")
            return;

        sendNetBlock(bs, pos, VanillaBlocks::mLightBlock->getRuntimeId() - 15 + lightLv);
        if (!isSamePos && (isOpened || !isSameLight))
            turnOff(pl);

        IsLighting[pl] = true;
        LightingPos[pl] = pos;
        LightingLevel[pl] = lightLv;
        
    }

    inline bool clear(Player* pl) {
        turnOff(pl);
        IsLighting.erase(pl);
        LightingPos.erase(pl);
        LightingLevel.erase(pl);
    }

}

#endif