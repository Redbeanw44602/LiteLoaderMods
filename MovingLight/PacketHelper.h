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

    inline void UpdateBlockPacket(Dimension* dim, BlockPos bp, const unsigned int runtimeId, unsigned int layer = 1) {
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
    };

    inline void UpdateBlockPacket(int dimId, BlockPos bp, const unsigned int runtimeId, unsigned int layer = 1) {
        auto dim = Global<Level>->getDimension(dimId);
        if (!dim)
            return;
        UpdateBlockPacket(dim, bp, runtimeId);
    };

};

#endif // !PACKETHELPER_H
