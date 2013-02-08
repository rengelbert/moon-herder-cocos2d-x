//
//  GameSprite.h
//  MoonHerder
//
//  Created by Roger Engelbert on 2/1/13.
//
//

#ifndef __MoonHerder__GameSprite__
#define __MoonHerder__GameSprite__

#include "GameConfig.h"

class GameSprite : public CCSprite {
public:
	
	GameSprite (float radius);
	
	CC_SYNTHESIZE(bool, _active, Active);
	CC_SYNTHESIZE(bool, _trashme, Trashme);
	CC_SYNTHESIZE(bool, _hit, Hit);
	CC_SYNTHESIZE(float, _speed, Speed);
	CC_SYNTHESIZE(float, _radius, Radius);
	CC_SYNTHESIZE(float, _squaredRadius, SquaredRadius);
	CC_SYNTHESIZE(CCPoint, _vector, Vector);
	CC_SYNTHESIZE(CCPoint, _nextPosition, NextPosition);
	
	static GameSprite* createWithFrameName(const char * pszSpriteFrameName, float radius);
	
	virtual void reset (void);
	virtual void destroy(void);
	virtual void update (float dt);
	virtual inline void place (void) {
		this->setPositionX(_nextPosition.x);
		this->setPositionY(_nextPosition.y);
	}
};

#endif /* defined(__MoonHerder__GameSprite__) */