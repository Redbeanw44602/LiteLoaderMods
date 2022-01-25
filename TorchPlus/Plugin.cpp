#include "pch.h"
#include <EventAPI.h>
#include <LoggerAPI.h>
#include "Plugin.h"

using namespace std;
using namespace LL;

Logger logger("TorchPlus");
Version PLUGIN_VERSION { 1,6,3,Version::Release };

bool onPlayerJoin(Event::PlayerPreJoinEvent ev) {
    LightMgr::init(ev.mPlayer);
    return true;
}

bool onPlayerLeft(Event::PlayerLeftEvent ev) {
    LightMgr::clear(ev.mPlayer);
    return true;
}

void PluginMain()
{
    logger.info("Enhanced torches, ver " + PLUGIN_VERSION.toString() + ", author: redbeanw.");
}

void PluginInit()
{
    LL::registerPlugin("TorchPlus", "Enhance the torches.", PLUGIN_VERSION, { {"Author","RedbeanW"}, {"Github","https://github.com/Redbeanw44602/TorchPlus.git"} });
    Event::PlayerPreJoinEvent::subscribe(onPlayerJoin);
    Event::PlayerLeftEvent::subscribe(onPlayerLeft);
    Event::ServerStartedEvent::subscribe([](Event::ServerStartedEvent ev) -> bool {
        PluginMain();
        return true;
    });
}

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