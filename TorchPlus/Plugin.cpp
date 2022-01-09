#include "pch.h"
#include <EventAPI.h>
#include <LoggerAPI.h>

using namespace std;
using namespace LL;

Logger logger("TorchPlus");
const Version PLUGIN_VERSION { 1,6,0,Version::Release };

namespace FindTorch {
    static unordered_map<string, int> Torches{
        {"minecraft:torch",14},
        {"minecraft:soul_torch",10},
        {"minecraft:redstone_torch",7}
    };
    
    inline bool isEnabled(string name) {
        return Torches.count(name);
    }

    inline int getBrightness(const ItemStack* it) {
        if (it->isNull())
            return 0;
        auto name = it->getTypeName();
        auto light = 0;
        if (isEnabled(name))
            light = Torches[name];
        return light;
    }
}

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
    wp.writeUnsignedVarInt(0); // layer 0
    EasyPkt pkt(wp.getRaw(), 21);
    // auto pkt = MinecraftPackets::createPacket(MinecraftPacketIds::UpdateBlock);
    // pkt->read(wp);
    dim->sendPacketForPosition({ bp.x, bp.y, bp.z }, *(Packet*)&pkt, nullptr);
    return true;
}

namespace LightMgr {
    static unordered_map<Player*, BlockPos> sendedBlocks;
    static unordered_map<Player*, bool> openedPlayers;

    inline bool isTurningOn(Player* pl) {
        return openedPlayers.count(pl) && openedPlayers[pl];
    }

    inline void turnOff(Player* pl) {
        if (!isTurningOn(pl))
            return;
        openedPlayers[pl] = false;
        auto bs = &pl->getRegion();
        auto& bp = sendedBlocks[pl];
        sendNetBlock(bs, bp, bs->getBlock(bp).getRuntimeId());
    }

    inline void turnOn(Player* pl, int lightLv) {
        BlockSource* bs = pl->getBlockSource();
        auto tmp_pos = pl->getBlockPos();
        BlockPos pos = { tmp_pos.x,tmp_pos.y + 1,tmp_pos.z };

        bool isOpened = isTurningOn(pl);
        if (isOpened && pos.operator==(sendedBlocks[pl]))
            return;


        if (bs->getBlock(pos).getName().getString() != "minecraft:air")
            return;

        sendNetBlock(bs, pos, VanillaBlocks::mLightBlock->getRuntimeId() - 15 + lightLv);
        if (isOpened)
            turnOff(pl);
        sendedBlocks[pl] = pos;
        openedPlayers[pl] = true;
    }

}

THook(void, "?normalTick@Player@@UEAAXXZ",
    Player* pl)
{
    original(pl);
    int light = max(FindTorch::getBrightness(&pl->getSelectedItem()), FindTorch::getBrightness(&pl->getOffhandSlot()));
    if (light != 0)
        LightMgr::turnOn(pl, light);
    else
        LightMgr::turnOff(pl);
}

THook(void, "?sendBlockDestructionStarted@BlockEventCoordinator@@QEAAXAEAVPlayer@@AEBVBlockPos@@@Z",
    void* self, Player* pl, BlockPos* bp)
{
    original(self, pl, bp);
    auto mainhand = &pl->getSelectedItem();
    if (mainhand->isNull())
        return;
    auto newHand = mainhand->clone_s();
    if (FindTorch::isEnabled(newHand->getTypeName()) && pl->getOffhandSlot().isNull())
    {
        auto& cont = pl->getInventory();
        auto nowSlot = pl->getSelectedItemSlot();
        cont.removeItem(nowSlot, 64);
        pl->setOffhandSlot(*newHand);
        pl->sendInventory(true);
    }
}

THook(void, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z",
    ServerNetworkHandler* self, ServerPlayer* sp, bool a3)
{
    LightMgr::turnOff((Player*)sp);
    original(self, sp, a3);
}

void PluginMain()
{
    logger.info("Enhanced torches, ver " + to_string(PLUGIN_VERSION.major) + "." + to_string(PLUGIN_VERSION.minor) + "." + to_string(PLUGIN_VERSION.revision) + ", author: redbeanw.");
}

void PluginInit()
{
    LL::registerPlugin("TorchPlus", "Enhance the torches.", PLUGIN_VERSION, { {"Author","RedbeanW"}, {"Github","https://github.com/Redbeanw44602/TorchPlus.git"} });
    Event::ServerStartedEvent::subscribe([](Event::ServerStartedEvent ev) -> bool {
        PluginMain();
        return true;
    });
}

/* Protect the light block.

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