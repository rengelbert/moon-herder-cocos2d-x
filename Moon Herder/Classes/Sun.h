//
//  GameSprite.h
//  MoonHerder
//
//  Created by Roger Engelbert on 2/1/13.
//
//

#ifndef __MoonHerder__Sun__
#define __MoonHerder__Sun__

#include "GameSprite.h"

class Moon;

class Sun : public GameSprite {

public:

	CC_SYNTHESIZE(bool, _hasRisen, HasRisen);
	CC_SYNTHESIZE(bool, _hasGrown, HasGrown);
	
	Sun();
	static Sun* create();
	
	bool checkCollisionWithMoon (Moon * moon);
	void highNoon (void);
	virtual void update (float dt);
	virtual void reset ();
	
	
private:
	CCSpriteFrame *_frame1;
    CCSpriteFrame *_frame2;
	float _friction;
    int _sunPosY;
	
	CCSize _screenSize;
};

#endif /* defined(__MoonHerder__Sun__) */

