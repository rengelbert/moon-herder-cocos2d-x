//
//  GameSprite.h
//  MoonHerder
//
//  Created by Roger Engelbert on 2/1/13.
//
//

#ifndef __MoonHerder__Moon__
#define __MoonHerder__Moon__

#include "GameSprite.h"

class GameManager;

class Moon : public GameSprite {
public:
	
	CC_SYNTHESIZE(bool, _off, IsOff);
	 
	 
	Moon(float radius);
	static Moon* create();
	
	void turnOnOff (bool value);
	virtual void reset(void);
	virtual void update (float dt);

private:
	CCSize _screenSize;
    GameManager * _manager;
    
	float _blinkTimer;
	int _blinkInterval;
	int _blinkState;
	
	void limitSpeeds(void);
	
};

#endif /* defined(__MoonHerder__Moon__) */

