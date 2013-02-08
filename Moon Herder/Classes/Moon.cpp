//
//  GameSprite.cpp
//  MoonHerder
//
//  Created by Roger Engelbert on 2/1/13.
//
//

#include "Moon.h"
#include "GameManager.h"


Moon::Moon (float radius):GameSprite (radius) {

	_blinkInterval = 2;
	_blinkTimer = 0;
	_blinkState = 1;
	_off = false;
    _manager = GameManager::sharedGameManager();
	
	_screenSize = CCDirector::sharedDirector()->getWinSize();
}

Moon* Moon::create() {
	Moon * sprite = new Moon(16);
    
	if (sprite && sprite->initWithSpriteFrameName("moon.png")) {
		sprite->autorelease();
        return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return NULL;
}
	
void Moon::turnOnOff (bool value) {
	if (value) {
        _off = false;
        _blinkState = 1;
        this->setDisplayFrame (
    		CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("moon.png")
    	);
    } else {
        _off = false;
        _blinkState = 0;
        this->setDisplayFrame (
    		CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("moon_off.png")
    	);
    }
}

void Moon::limitSpeeds(void) {
	if (_vector.x > TERMINAL_VELOCITY) _vector.x = TERMINAL_VELOCITY;
	if (_vector.x < -TERMINAL_VELOCITY) _vector.x = -TERMINAL_VELOCITY;
	if (_vector.y > TERMINAL_VELOCITY) _vector.y = TERMINAL_VELOCITY;
	if (_vector.y < -TERMINAL_VELOCITY) _vector.y = -TERMINAL_VELOCITY;
}

void Moon::reset () {
	_off = false;
    _blinkTimer = 0;
    _blinkState = 1;
    this->setDisplayFrame (
    	CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("moon.png")
    );
    _active = true;
    _vector = ccp(0,0);
    
}

void Moon::update (float dt) {

    
    if (!_active) return;

	_vector.y -= GRAVITY;
	
	_nextPosition = ccp (
		getPositionX() + _vector.x * dt,
		getPositionY() + _vector.y * dt
	);
	
    limitSpeeds();
    
	//if hitting sides of screen
	if (_nextPosition.x < _radius) {
		_nextPosition.x = _radius ;
		_vector.x *= -1.0;
		
		//I play the same sound as when sun is hit		
		SimpleAudioEngine::sharedEngine()->playEffect("sun_hit.wav");
	}
	
	if (_nextPosition.x > _screenSize.width - _radius) {
		_nextPosition.x = _screenSize.width - _radius;
		_vector.x *= -1.0;
        SimpleAudioEngine::sharedEngine()->playEffect("sun_hit.wav");
	}
	
	//rotate moon based on VX
	this->setRotation (getRotation() + _vector.x * 0.08f);

    //moon blinks when power is boosting
    if (_manager->getBoosting()) {

        _blinkTimer += dt;
        if (_blinkTimer > _blinkInterval) {
            _blinkTimer = 0;
            
            if (_blinkState == 1) {
                _blinkState = 0;
                
                this->setDisplayFrame (
    				CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("moon_off.png")
   			 	);
            } else {
                _blinkState = 1;
                this->setDisplayFrame (
					CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("moon.png")
				);
            }
        }
    } else {
        if (_blinkState == 0 && _manager->getLineEnergy() > 0) {
            _blinkState = 1;
            this->setDisplayFrame (
               CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("moon.png")
               );
        }
    }
}
