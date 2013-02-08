//
//  GameSprite.cpp
//  MoonHerder
//
//  Created by Roger Engelbert on 2/1/13.
//
//

#include "GameSprite.h"


GameSprite::GameSprite (float radius) {
	_radius = radius;
	_squaredRadius = _radius * _radius;
	_vector = ccp(0,0);
    _active = true;
}

GameSprite * GameSprite::createWithFrameName (const char * pszSpriteFrameName, float radius) {
	GameSprite * sprite = new GameSprite(radius);
    
	if (sprite && sprite->initWithSpriteFrameName(pszSpriteFrameName)) {
		sprite->autorelease();
        return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return NULL;
}

void GameSprite::reset (void) {
	_vector = ccp(0,0);
	_nextPosition = this->getPosition();
	_active = true;
}

void GameSprite::destroy (void) {}

void GameSprite::update (float dt) {
	_nextPosition = ccp(
		this->getPositionX() + _vector.x,
		this->getPositionY() + _vector.y
	);
}
