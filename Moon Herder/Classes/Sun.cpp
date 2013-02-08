//
//  GameSprite.cpp
//  MoonHerder
//
//  Created by Roger Engelbert on 2/1/13.
//
//

#include "Sun.h"
#include "Moon.h"

Sun::Sun():GameSprite(50){
	//two suns, one small, one large
	_frame1 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("sun_1.png");
	_frame2 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("sun_2.png");
	
	_screenSize = CCDirector::sharedDirector()->getWinSize();
		
	_hasRisen = false;
	_hasGrown = false;
	_friction = 0.98f;
	_sunPosY = _screenSize.height - 120;
		
}

Sun* Sun::create() {
	Sun * sprite = new Sun();
    
	if (sprite && sprite->initWithSpriteFrameName("sun_1.png")) {
		sprite->autorelease();
        return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return NULL;
}

bool Sun::checkCollisionWithMoon (Moon * moon){
	if (!_hasRisen) return false;
			
	float diffx = moon->getNextPosition().x - _nextPosition.x;
	float diffy = moon->getNextPosition().y - _nextPosition.y;
	float distance = (diffx * diffx + diffy * diffy);

	//simple collision check
	if (distance < pow(moon->getRadius() + _radius, 2)) {
		float angle = atan2(diffy,diffx);

		moon->setVector(ccp(10 * cos(angle),  10 * sin(angle)));

		_vector.x = -1 * moon->getVector().x;
		_vector.y = -1 * moon->getVector().y;
        
        return true;
	}
    
    return false;
}

void Sun::highNoon (void){
	//show big sun
	_radius = 64;
    _squaredRadius = _radius * _radius;
    this->setDisplayFrame (
		_frame2
	);
	_friction = 0.9f;
	
	_hasGrown = true;
}

void Sun::update (float dt) {
	    
    //move sun up to final positon; don't check for collision yet
	if (!_hasRisen) {
				
		_vector.y += dt * 0.5;
		
        if (_nextPosition.y >= _sunPosY) {
			_vector.y = 0;
			_hasRisen = true;
		} else {
			_nextPosition.y = getPositionY() + _vector.y;
		}
		
		
	} else {
		//sun is ready, 
        if (_nextPosition.y < 80) _vector.y +=  0.2;
        
		_nextPosition.x = getPositionX() + _vector.x * dt;
		_nextPosition.y = getPositionY() + _vector.y * dt;
	
		
		//check collision with sides
		if (_nextPosition.x < _radius) {
			_nextPosition.x = _radius ;
			_vector.x *= -1;
		}
		
		if (_nextPosition.x > _screenSize.width - _radius) {
			_nextPosition.x = _screenSize.width - _radius ;
			_vector.x *= -1;
		}
    
        if (_nextPosition.y > _screenSize.height) {
			_nextPosition.y = _screenSize.height;
            _vector.y *= -1;
		}
		
		//rotate sun based either o VX or dt 
        if (fabs(_vector.x) > dt) {
            setRotation ( getRotation() + _vector.x * 0.5f);
        } else {
            setRotation ( getRotation() + dt);
        }
		
		_vector.x *= _friction;
		_vector.y *= _friction;
	}
}

void Sun::reset () {
	_radius = 50;
    _squaredRadius = _radius * _radius;
   this->setDisplayFrame (
		_frame1
	);
    
    _friction = 0.98f;
	_hasGrown = false;
}

