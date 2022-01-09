// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头

#include <iostream>

#include <Global.h>
#include <LLAPI.h>

#include <MC/Player.hpp>
#include <MC/Level.hpp>
#include <MC/HashedString.hpp>
#include <MC/BlockSource.hpp>
#include <MC/Block.hpp>
#include <MC/ItemStack.hpp>
#include <MC/Container.hpp>

#include <MC/VanillaBlocks.hpp>
#include <MC/BinaryStream.hpp>
#include <MC/MinecraftPackets.hpp>
#include <MC/Dimension.hpp>
#include <MC/Packet.hpp>

/*
THook(__int64, "?setBlockLight@SubChunkRelighter@@QEAAXAEBVPos@@UBrightness@@111@Z",
    void* self, BlockPos* a1, char* a2, char* a3, char* a4, char* a5)
{
    a1 = Pos    位置
    a2 = 14     若取消亮度，传值
    a3 = 0      若设定亮度，传值
    a4 = 0      Unknown
    a5 = 0      Unknown
}

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
*/

#endif //PCH_H