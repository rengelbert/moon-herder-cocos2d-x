//
//  Line.h
//  MoonHerder
//
//  Created by Roger Engelbert on 2/1/13.
//
//

#ifndef __MoonHerder__Line__
#define __MoonHerder__Line__

#include "GameConfig.h"

class Moon;

class Line : public CCObject {
public:
	
	CC_SYNTHESIZE(bool, _active, Active);
	CC_SYNTHESIZE(bool, _trashme, Trashme);
	CC_SYNTHESIZE(bool, _hit, Hit);
	CC_SYNTHESIZE(float, _length, Length);
	CC_SYNTHESIZE(int, _blinkState, BlinkState);
	CC_SYNTHESIZE(int, _curveState, CurveState);
	CC_SYNTHESIZE(CCPoint, _start, Start);
	CC_SYNTHESIZE(CCPoint, _end, End);
	CC_SYNTHESIZE(CCPoint, _curve, Curve);
	CC_SYNTHESIZE(CCPoint, _collisionPoint, CollisionPoint);
	
	static Line * create();
	
	void update (float dt);
	void setValues (CCPoint start, CCPoint end);
	bool collidesWithMoon (Moon * moon);
	

private:

	float _blinkInterval;
    float _activeInterval;
    float _curveInterval;
    float _vectorX;
    float _vectorY;
    float _normalAngle;
    float _normalCos;
    float _normalSin;
    float _lenSquared;
    
    CCPoint _normalLeft;
    CCPoint _normalRight;
	
	bool _blinking;
	
	float rangeWithMoon (Moon * moon);
	CCPoint getNormalForMoon (Moon * moon);
	float moonProjectionOfMoon (Moon * moon, CCPoint normal);
	void calculateLineData(void);
};
	

#endif /* defined(__MoonHerder__Line__) */

