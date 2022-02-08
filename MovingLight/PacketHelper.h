#ifndef PACKETHELPER_H
#define PACKETHELPER_H

namespace PacketHelper {

    /*
    * UpdateBlock:
        Flags:
            NONE = 0
            NEIGHBORS = 1
            NETWORK = 2
            ALL = 3
            NOGRAPHIC = 4
            PRIORITY = 8
            ALL_PRIORITY = 11
        DataLayer:
            NONE = 0
            PENETRATE = 1
            NOUPDATE = 2
    */
    inline bool UpdateBlockPacket(BlockSource* bs, BlockPos bp, const unsigned int runtimeId, unsigned int layer = 1) {
        auto dim = Global<Level>->getDimension(bs->getDimensionId());
        BinaryStream wp;
        wp.writeVarInt(bp.x);
        wp.writeUnsignedVarInt(bp.y);
        wp.writeVarInt(bp.z);
        wp.writeUnsignedVarInt(runtimeId);
        wp.writeUnsignedVarInt(3);  // flag
        wp.writeUnsignedVarInt(layer);  // layer
        shared_ptr<Packet> pkt = MinecraftPackets::createPacket(MinecraftPacketIds::UpdateBlock);
        pkt->read(wp);
        dim->sendPacketForPosition({ bp.x, bp.y, bp.z }, *pkt, nullptr);
        return true;
    };
};

#endif // !PACKETHELPER_H
