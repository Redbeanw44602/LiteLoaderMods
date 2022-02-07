#ifndef PACKETHELPER_H
#define PACKETHELPER_H

namespace PacketHelper {

    // RuntimeId = 5458 -> 5473 , LightBlock 1-14;
    inline bool UpdateBlockPacket(BlockSource* bs, BlockPos bp, const unsigned int runtimeId) {
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
    };
};

#endif // !PACKETHELPER_H
