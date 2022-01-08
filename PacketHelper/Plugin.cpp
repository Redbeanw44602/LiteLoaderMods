#include "pch.h"
#include <EventAPI.h>
#include <LoggerAPI.h>

using namespace LL;
Logger logger("PacketHelper");

void PluginMain()
{
	logger.info("loaded.");
}

void PluginInit()
{
	registerPlugin("PacketHelper", "BDS Network packet dumper.", Version{ 1,0,0,Version::Beta }, { {"Author","Redbeanw"} });
	Event::ServerStartedEvent::subscribe([](Event::ServerStartedEvent ev) -> bool {
		PluginMain();
		return true;
	});
}

THook(void**, "?getEncryptedPeerForUser@NetworkHandler@@QEAA?AV?$weak_ptr@VEncryptedNetworkPeer@@@std@@AEBVNetworkIdentifier@@@Z",
	void* self, void** ret, void* id) // Disable encrypt.
{
	ret[0] = ret[1] = 0;
	return ret;
}

TInstanceHook(NetworkPeer::DataStatus, "?receivePacket@Connection@NetworkHandler@@QEAA?AW4DataStatus@NetworkPeer@@AEAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEAV2@AEBV?$shared_ptr@V?$time_point@Usteady_clock@chrono@std@@V?$duration@_JU?$ratio@$00$0DLJKMKAA@@std@@@23@@chrono@std@@@6@@Z",
	NetworkHandler::Connection, std::string& data)
{
	auto status = original(this, data);

	if (status == NetworkPeer::DataStatus::OK) {
		auto stream = ReadOnlyBinaryStream(data, 0i64);
		auto pktId = stream.getUnsignedVarInt();
		Packet* pkt;
		SymCall(
			"?createPacket@MinecraftPackets@@SA?AV?$shared_ptr@VPacket@@@std@@W4MinecraftPacketIds@@@Z",
			void*, Packet**, int)(&pkt, pktId);
		logger.debug("Packet [I] MC({}) >> {}", pktId, pkt->getName());
	}

	return status;
}
TClasslessInstanceHook(void, "?_sendInternal@NetworkHandler@@AEAAXAEBVNetworkIdentifier@@AEBVPacket@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z",
	NetworkIdentifier const& id, Packet const& pkt, std::string& data)
{
	original(this, id, pkt, data);
	auto stream = ReadOnlyBinaryStream(data, 0i64);
	auto pktId = stream.getUnsignedVarInt();
	if (pktId == 111	// MoveActorDeltaPacket
		|| pktId == 39	// SetActorDataPacket
		|| pktId == 174	// SubChunkPacket
		|| pktId == 123	// LevelSoundEventPacket
		|| pktId == 40	// SetActorMotionPacket
		|| pktId == 14	// RemoveActorPacket
		|| pktId == 58	// LevelChunkPacket
		|| pktId == 121	// NetworkChunkPublisherUpdatePacket
		|| pktId == 136	// ClientCacheMissResponsePacket
		|| pktId == 172	// UpdateSubChunkBlocksPacket
		|| pktId == 10	// SetTimePacket
		)
		return;

	logger.info("Packet [O] MC({}) >> {}", pkt.getId(), pkt.getName());
}