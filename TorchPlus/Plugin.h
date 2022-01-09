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
    static unordered_map<Player*, BlockPos> sendedBlocks;
    static unordered_map<Player*, bool> openedPlayers;

    inline bool isTurningOn(Player* pl) {
        return openedPlayers.count(pl) && openedPlayers[pl];
    }

    inline void turnOff(Player* pl) {
        if (!isTurningOn(pl))
            return;
        openedPlayers[pl] = false;
        auto bs = &pl->getRegion();
        auto& bp = sendedBlocks[pl];
        sendNetBlock(bs, bp, bs->getBlock(bp).getRuntimeId());
    }

    inline void turnOn(Player* pl, int lightLv) {
        BlockSource* bs = pl->getBlockSource();
        auto tmp_pos = pl->getBlockPos();
        BlockPos pos = { tmp_pos.x,tmp_pos.y + 1,tmp_pos.z };

        bool isOpened = isTurningOn(pl);
        if (isOpened && pos.operator==(sendedBlocks[pl]))
            return;


        if (bs->getBlock(pos).getName().getString() != "minecraft:air")
            return;

        sendNetBlock(bs, pos, VanillaBlocks::mLightBlock->getRuntimeId() - 15 + lightLv);
        if (isOpened)
            turnOff(pl);
        sendedBlocks[pl] = pos;
        openedPlayers[pl] = true;
    }

    inline bool clear(Player* pl) {
        turnOff(pl);
        sendedBlocks.erase(pl);
        openedPlayers.erase(pl);
    }

}

#endif