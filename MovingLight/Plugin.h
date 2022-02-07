#pragma once

#ifndef PLUGIN_H
#define PLUGIN_H
using namespace std;
#include "PacketHelper.h"

namespace Config {

    static bool enable = true;
    static unsigned int VERSION = 100;

    static unordered_map<string, unsigned int> items = {
        {"minecraft:beacon",15},            // 信标
        {"minecraft:campfire",15},          // 营火
        {"minecraft:soul_campfire",10},     // 灵魂营火
        {"minecraft:glowstone",15},         // 荧石
        {"minecraft:lit_pumpkin",15},       // 南瓜灯
        {"minecraft:sealantern",15},        // 海晶灯
        {"minecraft:lantern",15},           // 灯笼
        {"minecraft:soul_lantern",10},      // 灵魂灯笼
        {"minecraft:shroomlight",15},       // 菌光体
        {"minecraft:end_rod",14},           // 末地烛
        {"minecraft:torch",14},             // 火把
        {"minecraft:soul_torch",10},        // 灵魂火把
        {"minecraft:underwater_torch",14},  // 水下火把
        {"minecraft:colored_torch_rg",14},  // 彩色火把[红/绿]
        {"minecraft:colored_torch_bp",14},  // 彩色火把[蓝/紫]
        {"minecraft:glow_berries",14},      // 发光浆果
        {"minecraft:crying_obsidian",10},   // 哭泣的黑曜石
        {"minecraft:enchanting_table",7},   // 附魔台
        {"minecraft:ender_chest",7},        // 末影箱
        {"minecraft:redstone_torch",7},     // 红石火把
        {"minecraft:sculk_catalyst",6},     // Sculk Catalyst
        {"minecraft:large_amethyst_bud",4}, // 大型紫晶芽
        {"minecraft:amethyst_cluster",5},   // 紫水晶簇
        {"minecraft:lava_bucket",15}        // 熔岩桶
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