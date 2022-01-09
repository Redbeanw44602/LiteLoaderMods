#pragma once

#ifndef EASYPKT_H
#define EASYPKT_H

using namespace std;
class EasyPkt : public Packet { // EasyPkt by WangYneos.
public:
    string_view _data;
    int _pktid;
    EasyPkt(string_view sv, int pktid)
        : _data(sv)
        , _pktid(int(pktid)) {
        incompressible = true;
    }
    inline virtual ~EasyPkt() {
    }
    enum MinecraftPacketIds getId() const {
        return (enum MinecraftPacketIds)_pktid;
    }
    virtual std::string getName() const {
        return "EasyPkt";
    }
    virtual void write(BinaryStream& bs) const {
        bs.getRaw().append(_data);
    }
    virtual void dummyread() {
    }
    virtual bool disallowBatching() const {
        return false;
    }

private:
    virtual enum StreamReadResult _read(ReadOnlyBinaryStream&) {
        return (enum StreamReadResult)1;
    }
};

#endif