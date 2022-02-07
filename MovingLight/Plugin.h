#pragma once

#ifndef PLUGIN_H
#define PLUGIN_H
using namespace std;
#include "PacketHelper.h"

namespace Config {

    static bool enable = true;
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
        auto light = 0;
        if (isLightSource(name))
            light = Config::items[name];
        return light;
    }

};

namespace LightMgr {

    struct LightInfo {
        bool isLighting = false;
        BlockPos lightingPos = BlockPos::ZERO;
        int lightLevel = 0;
    };

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
        PacketHelper::UpdateBlockPacket(bs, bp, bs->getBlock(bp).getRuntimeId());
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

        PacketHelper::UpdateBlockPacket(bs, pos, VanillaBlocks::mLightBlock->getRuntimeId() - 15 + lightLv);
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