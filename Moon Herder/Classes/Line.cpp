//
//  Line.cpp
//  MoonHerder
//
//  Created by Roger Engelbert on 2/1/13.
//
//

#include "Line.h"
#include "Moon.h"

Line * Line::create () {
	Line * line = new Line();
    
	if (line) {
		line->autorelease();
        return line;
	}
	CC_SAFE_DELETE(line);
	return NULL;
}

void Line::update (float dt){
	if (!_active || _trashme) return;
	
    //count active time
	_activeInterval += dt;
	if (_hit) {
		//animate curve if line is hit
		_curveInterval += dt;

		if (_curveInterval > LINE_CURVE_INTERVAL) {
			_curveInterval = 0;
			_curveState = (_curveState == 0) ? 1 : 0;
		}
		
	} else {
		//if not hit, and at 75% active time, start blinking the line
		if (_activeInterval > LINE_TIME_ACTIVE * 0.75f) {
           _blinking = true;
		}
	}
	
	//destroy line if past its active time
	if (_activeInterval > LINE_TIME_ACTIVE) {
		_active = false;
		_trashme = true;
		return;
	}
	
	if (_blinking) {
		_blinkInterval += dt;
		//blink
		if (_blinkInterval > LINE_BLINK_INTERVAL) {
            _blinkInterval = 0;
			_blinkState = (_blinkState == 0) ? 1 : 0;
		}
		
	}

}
void Line::setValues (CCPoint start, CCPoint end){
	if (start.x <= end.x) {
		_start = start;
		_end = end;
	} else {
		_start = end;
		_end = start;
	}
	
	_curve = ccp(0,0);
    
	_active = true;
	_trashme = false;
	_hit = false;
	
	//these are used as timers for blinking time, active time, curve animation
	_blinkInterval = 0;
	_activeInterval = 0;
	_curveInterval = 0;
	_blinking = false;
	_blinkState = 0;
	_curveState = 0;

	calculateLineData();
}
bool Line::collidesWithMoon (Moon * moon){
	//line can only collide once
	if (_hit)
	{
		return false;
	}
	
	//if within range of line segment
	float r = rangeWithMoon(moon);
	
	if (r < 0 || r > 1)  return false;
	
	CCPoint normal = getNormalForMoon(moon);
	float t = moonProjectionOfMoon(moon, normal);
	
	if (t > 0 && t < 1) {
        
        //get moon's vector
        float moonDiffX = moon->getNextPosition().x - moon->getPositionX();
		float moonDiffY = moon->getNextPosition().y - moon->getPositionY();
        
        if (moonDiffX * normal.x + moonDiffY * normal.y > 0) return false;
        
		//collision!!!!
		_hit = true;
		_blinking = true;
		if (_activeInterval < LINE_TIME_ACTIVE * LINE_TIME_CURVING) 
			_activeInterval = LINE_TIME_ACTIVE * LINE_TIME_CURVING;
		
		moon->setVector ( ccp(LINE_BOUNCE * _length * normal.x, LINE_BOUNCE * _length * normal.y) );

		_collisionPoint = ccp(moon->getPositionX() + t * moonDiffX,
							  moon->getPositionY() + t * moonDiffY);
		_curve = _collisionPoint;
		
		_curve.x -= LINE_CURVE_AMOUNT * normal.x;
		_curve.y -= LINE_CURVE_AMOUNT * normal.y;
    
		_curveState = 1;

		return true;
	}
	
	return false;
}

float Line::rangeWithMoon (Moon * moon){
	float moonToStartX = moon->getPositionX() - _start.x;
	float moonToStartY = moon->getPositionY() - _start.y;
				
	//get dot product of this line's vector and moonToStart vector
	float dot = moonToStartX * _vectorX + moonToStartY * _vectorY;
	//solve it to the segment
	return dot/_lenSquared;
}
CCPoint Line::getNormalForMoon (Moon * moon){
	float lineStartTomoonX = moon->getPositionX() - _start.x;
	float lineStartTomoonY = moon->getPositionY() - _start.y;
	
	//check dot product value to grab correct normal
	float leftNormal = lineStartTomoonX * _normalLeft.x + lineStartTomoonY * _normalLeft.y;
	CCPoint normal;
	
	if (leftNormal > 0) {
		normal = _normalLeft;
	} else {
		normal = _normalRight;
	}
	return normal;
}

float Line::moonProjectionOfMoon (Moon * moon, CCPoint normal){
	float lineStartTomoonNowX = moon->getPositionX() - _start.x;
	float lineStartTomoonNowY = moon->getPositionY() - _start.y;
	
	float lineStartTomoonNextX = moon->getNextPosition().x - _start.x;
	float lineStartTomoonNextY = moon->getNextPosition().y - _start.y;
	
	//check dot product value to grab correct normal
	float distanceToLineNow = lineStartTomoonNowX * normal.x + lineStartTomoonNowY * normal.y;
	float distanceToLineNext = lineStartTomoonNextX * normal.x + lineStartTomoonNextY * normal.y;
	
	return (moon->getRadius() - distanceToLineNow) / (distanceToLineNext - distanceToLineNow);
}

void Line::calculateLineData(void){
    _length = ccpDistance(_start,_end);
    //line vector
    _vectorX = _end.x - _start.x;
	_vectorY = _end.y - _start.y;
	
	//squared length
	_lenSquared = _vectorX * _vectorX + _vectorY * _vectorY;
	
	float normalX = _vectorY;
	float normalY = -_vectorX;
	
	float normalLength = sqrt(normalX * normalX + normalY * normalY);
	
	//normalized normals
	_normalLeft = ccp(normalX / normalLength, normalY / normalLength);
	_normalRight = ccp( -1*_normalLeft.x, -1*_normalLeft.y); 

}

