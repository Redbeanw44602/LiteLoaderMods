#pragma once

#ifndef PLUGIN_H
#define PLUGIN_H
using namespace std;
#include "PacketHelper.h"

typedef long long UniqueID;

namespace Config {

    static bool enable = true;
    static bool enableItemActor = true;
    static unsigned int VERSION = 100;

    static unordered_map<string, unsigned int> items = {
        {"minecraft:beacon",15},            // �ű�
        {"minecraft:campfire",15},          // Ӫ��
        {"minecraft:soul_campfire",10},     // ���Ӫ��
        {"minecraft:glowstone",15},         // ӫʯ
        {"minecraft:lit_pumpkin",15},       // �Ϲϵ�
        {"minecraft:sealantern",15},        // ������
        {"minecraft:lantern",15},           // ����
        {"minecraft:soul_lantern",10},      // ������
        {"minecraft:shroomlight",15},       // ������
        {"minecraft:end_rod",14},           // ĩ����
        {"minecraft:torch",14},             // ���
        {"minecraft:soul_torch",10},        // �����
        {"minecraft:underwater_torch",14},  // ˮ�»��
        {"minecraft:colored_torch_rg",14},  // ��ɫ���[��/��]
        {"minecraft:colored_torch_bp",14},  // ��ɫ���[��/��]
        {"minecraft:glow_berries",14},      // ���⽬��
        {"minecraft:crying_obsidian",10},   // �����ĺ���ʯ
        {"minecraft:enchanting_table",7},   // ��ħ̨
        {"minecraft:ender_chest",7},        // ĩӰ��
        {"minecraft:redstone_torch",7},     // ��ʯ���
        {"minecraft:sculk_catalyst",6},     // Sculk Catalyst
        {"minecraft:large_amethyst_bud",4}, // �����Ͼ�ѿ
        {"minecraft:amethyst_cluster",5},   // ��ˮ����
        {"minecraft:lava_bucket",15}        // ����Ͱ
    };

    static vector<string> OffHandItems = {
        "minecraft:campfire",
        "minecraft:soul_campfire",
        "minecraft:lit_pumpkin",
        "minecraft:sealantern",
        "minecraft:lantern",
        "minecraft:soul_lantern",
        "minecraft:torch",
        "minecraft:soul_torch",
        "minecraft:underwater_torch",
        "minecraft:colored_torch_rg",
        "minecraft:colored_torch_bp",
        "minecraft:redstone_torch",
        "minecraft:ender_chest",
        "minecraft:glow_berries",
        "minecraft:large_amethyst_bud",
        "minecraft:amethyst_cluster",
        "minecraft:lava_bucket"
    };

    inline bool from_json(json& cfg) {
        cfg.at("enabled").get_to(enable);
        cfg.at("items").get_to(items);
        cfg.at("offhand").get_to(OffHandItems);
        return true;
    };

    inline string to_json() {
        json cfg = {
            {"version",VERSION},
            {"enabled",enable},
            {"items",items},
            {"offhand",OffHandItems}
        };
        return cfg.dump(4);
    }

    inline bool isLightSource(string name) {
        return items.count(name);
    }

    inline bool isOffhandItem(string name) {
        return count(OffHandItems.begin(), OffHandItems.end(), name);
    }

    inline int getBrightness(const ItemStack* it) {
        if (it->isNull())
            return 0;
        auto name = it->getTypeName();
        return isLightSource(name) ? Config::items[name] : 0;
    }

};

namespace LightMgr {

    struct LightInfo {
        bool mLighting = false;
        unsigned int mLevel = 0;
        BlockPos mPos = BlockPos::ZERO;
        int mDimId = -1;
    };

    static unordered_map<UniqueID, LightInfo> RecordedInfo;
    
    inline bool isVaild(ActorUniqueID id) {
        return RecordedInfo.count(id);
    }

    inline void init(ActorUniqueID id) {
        LightInfo li;
        RecordedInfo[id] = li;
    }

    inline bool isTurningOn(ActorUniqueID id) {
        return isVaild(id) && RecordedInfo[id].mLighting;
    }

    inline void turnOff(ActorUniqueID id) {
        if (!isTurningOn(id))
            return;
        RecordedInfo[id].mLighting = false;
        auto pos = RecordedInfo[id].mPos;
        auto dim = Global<Level>->getDimension(RecordedInfo[id].mDimId);
        auto region = &dim->getBlockSourceFromMainChunkSource();
        PacketHelper::UpdateBlockPacket(dim, pos, region->getBlock(pos).getRuntimeId());
    }

    inline void turnOn(ActorUniqueID id, BlockSource* region, BlockPos bp, int lightLv) {
        if (!isVaild(id))
            init(id);
        auto& Info = RecordedInfo[id];
        bool isOpened = isTurningOn(id);
        bool isSamePos = bp.operator==(Info.mPos);
        bool isSameLight = lightLv == Info.mLevel;
        if (isOpened && isSamePos && isSameLight)
            return;

        auto& name = region->getBlock(bp).getName().getString();
        if (name == "minecraft:lava" || name == "minecraft:water")
            return;

        auto dimId = region->getDimensionId();
        PacketHelper::UpdateBlockPacket(dimId, bp, VanillaBlocks::mLightBlock->getRuntimeId() - 15 + lightLv);
        if (!isSamePos && (isOpened || !isSameLight))
            turnOff(id);

        Info.mLighting = true;
        Info.mDimId = dimId;
        Info.mPos = bp;
        Info.mLevel = lightLv;
        
    }

    inline void clear(ActorUniqueID id) {
        turnOff(id);
        RecordedInfo.erase(id);
    }

}

#endif