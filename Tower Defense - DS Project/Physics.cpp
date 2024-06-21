#include "Physics.h"
#include "Components.h"

Vec2 GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
	auto& bb_player = a->getComponent<CBoundingBox>();
	auto& player = a->getComponent<CTransform>();
	auto& bb_tile = b->getComponent<CBoundingBox>();
	auto& tile = b->getComponent<CTransform>();
	
	Vec2 delta = { abs(player.pos.x - tile.pos.x), abs(player.pos.y - tile.pos.y) };
	auto ox = bb_player.halfSize.x + bb_tile.halfSize.x - delta.x;
	auto oy = bb_player.halfSize.y + bb_tile.halfSize.y - delta.y;

	return Vec2(ox, oy);
}

Vec2 GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
	auto& bb_player = a->getComponent<CBoundingBox>();
	auto& player = a->getComponent<CTransform>();
	auto& bb_tile = b->getComponent<CBoundingBox>();
	auto& tile = b->getComponent<CTransform>();

	Vec2 prevDelta = { abs(player.prevPos.x - tile.pos.x),abs(player.prevPos.y - tile.pos.y) };
	auto prevOx = bb_player.halfSize.x + bb_tile.halfSize.x - prevDelta.x;
	auto prevOy = bb_player.halfSize.y + bb_tile.halfSize.y - prevDelta.y;

	return Vec2(prevOx,prevOy);
}