#include "pch.h"
#include <EventAPI.h>
#include <LoggerAPI.h>

using namespace LL;
using namespace std;

typedef long long UID;

Logger logger("ExplodeArrow");
map<UID,ExpAction> explist;

void PluginMain()
{
	logger.info("loaded.");
}

void PluginInit()
{
	registerPlugin("ExplodeArrow", "The exploding arrow!", Version{ 1,0,0,Version::Release }, { {"Author","RedbeanW"} });
	Event::ServerStartedEvent::subscribe([](Event::ServerStartedEvent ev) -> bool {
		PluginMain();
		return true;
	});
}


/*
Level::explode(
		(BlockSource)	region,
		(Actor)			source,
		(Vec3)			pos,
		(float)			radius,
		(bool)			fire,
		(bool)			break,
		(float)			maxResistance,
		(bool)			underWater?
	);
*/

THook(void, "?onProjectileHit@Block@@QEBAXAEAVBlockSource@@AEBVBlockPos@@AEBVActor@@@Z",
	Block* self, BlockSource* bs, BlockPos* bp, Actor* ac)
{
	original(self, bs, bp, ac);
	UID id = ac->getActorUniqueId().id;
	if (explist.find(id) != explist.end())
	{
		auto &Info = explist[id];
		Vec3 pos = bp->toVec3();
		Global<Level>->explode(*bs, 0, { pos.x,pos.y + 1,pos.z }, Info.mRadius, Info.mFire, Info.mBreaking, 3.40282347e+38, true);
		explist.erase(id);
	}
}

THook(void, "?onHit@ProjectileComponent@@QEAAXAEAVActor@@AEBVHitResult@@@Z",
	void* self, Actor* source, HitResult* res)
{
	Actor* target = res->getEntity();
	if (!target || !source)
		return original(self, source, res);
	UID id = source->getActorUniqueId().id;
	if (explist.find(id) != explist.end())
	{
		auto& Info = explist[id];
		Vec3 pos = target->getPos();
		Global<Level>->explode(target->getRegion(), 0, { pos.x,pos.y + 1,pos.z }, Info.mRadius, Info.mFire, Info.mBreaking, 3.40282347e+38, true);
		explist.erase(id);
	}
	original(self, source, res);
}

THook(Actor*, "?spawnProjectile@Spawner@@QEAAPEAVActor@@AEAVBlockSource@@AEBUActorDefinitionIdentifier@@PEAV2@AEBVVec3@@3@Z",
	void* self, BlockSource* bs, void* a3, Actor* a4, Vec3* a5, Vec3* a6)
{
	// minecraft:flint_and_steel		打火石
	// minecraft:tnt					TNT
	// minecraft:gunpowder				火药
	// minecraft:fire_charge			火焰弹
	auto spawned = original(self, bs, a3, a4, a5, a6);
	if (!spawned || !a4->isPlayer())
		return spawned;
	auto pl = (Player*)a4;
	Container* container = &pl->getInventory();
	auto nowSlot = pl->getSelectedItemSlot();
	if (nowSlot == 0 || nowSlot == 8)
		return spawned;
	auto &beforeSlot = container->getItem(nowSlot + 1);
	auto &afterSlot = container->getItem(nowSlot - 1);
	if (beforeSlot.isNull() || afterSlot.isNull())
		return spawned;
	string beforeSlot_name = beforeSlot.getTypeName();
	string afterSlot_name = afterSlot.getTypeName();
	/* 组合1: 打火石与火药（r = 3 ） */
	auto a1 = beforeSlot_name == "minecraft:flint_and_steel" && afterSlot_name == "minecraft:gunpowder";
	auto a2 = beforeSlot_name == "minecraft:gunpowder" && afterSlot_name == "minecraft:flint_and_steel";
	if (a1 || a2)
	{
		if (a1)
		{
			((ItemStackBase*)&beforeSlot)->hurtAndBreak(1, a4);
			container->removeItem_s(nowSlot - 1, 1);
		}
		else
		{
			((ItemStackBase*)&afterSlot)->hurtAndBreak(1, a4);
			container->removeItem_s(nowSlot + 1, 1);
		}
		ExpAction ea;
		ea.mRadius = 3.2;
		explist[spawned->getActorUniqueId().id] = ea;
		return spawned;
	}
	/* 组合2: 打火石与TNT（r = 3 | breaking ） */
	auto b1 = beforeSlot_name == "minecraft:flint_and_steel" && afterSlot_name == "minecraft:tnt";
	auto b2 = beforeSlot_name == "minecraft:tnt" && afterSlot_name == "minecraft:flint_and_steel";
	if (b1 || b2)
	{
		if (b1)
		{
			((ItemStackBase*)&beforeSlot)->hurtAndBreak(1, a4);
			container->removeItem_s(nowSlot - 1, 1);
		}
		else
		{
			((ItemStackBase*)&afterSlot)->hurtAndBreak(1, a4);
			container->removeItem_s(nowSlot + 1, 1);
		}
		ExpAction ea;
		ea.mBreaking = true;
		ea.mRadius = 3.2;
		explist[spawned->getActorUniqueId().id] = ea;
		return spawned;
	}
	/* 组合3: 火焰弹与TNT（r = 3 | breaking | fire ） */
	auto c1 = beforeSlot_name == "minecraft:tnt" && afterSlot_name == "minecraft:fire_charge";
	auto c2 = beforeSlot_name == "minecraft:fire_charge" && afterSlot_name == "minecraft:tnt";
	if (c1 || c2)
	{
		container->removeItem_s(nowSlot + 1, 1);
		container->removeItem_s(nowSlot - 1, 1);
		ExpAction ea;
		ea.mBreaking = true;
		ea.mFire = true;
		ea.mRadius = 3.2;
		explist[spawned->getActorUniqueId().id] = ea;
		return spawned;
	}
	return spawned;
}