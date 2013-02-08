//
//  DrawLayer.h
//  MoonHerder
//
//  Created by Roger Engelbert on 2/1/13.
//
//

#ifndef __MoonHerder__DrawLayer__
#define __MoonHerder__DrawLayer__

#include "GameManager.h"

class DrawLayer : public CCNode {

public:

	CC_SYNTHESIZE(float, _length, Length);
	CC_SYNTHESIZE(CCPoint, _startPoint, StartPoint);
	CC_SYNTHESIZE(CCPoint, _touchPoint, TouchPoint);
	
	DrawLayer();
	static DrawLayer * create();
	virtual void draw();
    void setBlinking (bool var);
	
private:
	GameManager * _manager;
	CCSize _screenSize;
    int _blinkStateTime;
    int _blinkStateEnergy;
    bool _blinking;
    float _blinkTimer;
    int _blinkCount;
    
    
    
	void drawBar(int orientation);
	void drawLine(Line * line);
	void drawQuadBezier(CCPoint origin, CCPoint control, CCPoint destination,  int segments);
	bool pointEqualsZero (CCPoint point) {
		return (point.x == 0 && point.y == 0);
	}
};

#endif /* defined(__MoonHerder__DrawLayer__) */
