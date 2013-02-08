//
//  GameSprite.cpp
//  MoonHerder
//
//  Created by Roger Engelbert on 2/1/13.
//
//
#include "Star.h"
#include <vector>

using namespace std;

static int types[] = {1,2,3,4,2,3,4,1,3,4,2,1,3,4,4,2,2,4,3,3,3,1,1};
static vector<int> _types (types, types + sizeof(types) / sizeof(int));
static int TYPE_INDEX = 0;

Star::Star():GameSprite(4) {
	_screenSize = CCDirector::sharedDirector()->getWinSize();
}


Star* Star::create() {
	Star * sprite = new Star();
    
	if (sprite && sprite->initWithSpriteFrameName("star_1.png")) {
		sprite->autorelease();
        return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return NULL;
}


void Star::setValues (CCPoint position, bool boost) {

 	_boost = boost;
    int r;
    
	if (boost) {
		this->setDisplayFrame (
    		CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("boost.png")
    	);
        
	} else {
		int index = _types[TYPE_INDEX];

        CCString * frameName;
        frameName = CCString::createWithFormat("star_%i.png", (int) index);
        _frame1 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName->getCString());
        
        frameName = CCString::createWithFormat("star_%i_off.png", (int) index);
        _frame2 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName->getCString());
        
        _size = 2 + index * 2;
        
        r = floor(((float) rand() / RAND_MAX) * 10);
        if (r > 5) {
            this->setDisplayFrame (
    			_frame1
    		);
            _frame = 1;
        } else {
            this->setDisplayFrame (
    			_frame2
    		);
            _frame = 2;
        }
  
	}
	
	float random_range = arc4random() % 2;
    r = floor(((float) rand() / RAND_MAX) * 10);
    
	//offset the stars a bit, so as to get rid of the "Grid Cell" look
	setPositionX (position.x + random_range * r * 0.5f);
	setPositionY (position.y + random_range * r * 0.5f);
	
	//if too close to the sides, move stars a bit
	if (getPositionX() < 10) setPositionX(10);
	if (getPositionX() > _screenSize.width - 16)
        setPositionX (_screenSize.width - 16);
	
	//if too close to the top, same thing
    if (getPositionY() > _screenSize.height - 10)
        setPositionY (_screenSize.height - 10);
      
	
	TYPE_INDEX += 1;
	if (TYPE_INDEX == 22) TYPE_INDEX = 0;
	
	setVisible(true);
	
    _blinkTimer = 0.0;
    _blinkInterval = ((float)rand() / RAND_MAX) * 5 + 5;
}

void Star::update (float dt) {
    
    _blinkTimer += dt;
    //change image used, and change opacity when animated
    if (_blinkTimer > _blinkInterval && !_boost) {
        _blinkTimer = 0.0;
        
        if (_frame == 1 ) {
            _frame = 2;
            this->setDisplayFrame (
    			_frame2
    		);
        } else  {
            _frame = 1;
            this->setDisplayFrame (
    			_frame1
    		);
        }
        setOpacity( getOpacity() == 180 ?  255 : 180);
        setRotation (getRotation() + 20);
    }
}
