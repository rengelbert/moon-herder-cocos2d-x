//
//  GameSprite.h
//  MoonHerder
//
//  Created by Roger Engelbert on 2/1/13.
//
//

#ifndef __MoonHerder__Star__
#define __MoonHerder__Star__

#include "GameSprite.h"

class Star : public GameSprite {

public:

	CC_SYNTHESIZE(bool, _boost, IsBoost);
	 
	Star();
	static Star* create();
	
	void setValues (CCPoint position, bool boost);
	virtual void update (float dt);
	
	
private:
    
    
	CCSpriteFrame *_frame1;
    CCSpriteFrame *_frame2;
	
	float _blinkTimer;
    float _blinkInterval;
    float _size;
    
    int _frame;
	
	CCSize _screenSize;
};

#endif /* defined(__MoonHerder__Star__) */

