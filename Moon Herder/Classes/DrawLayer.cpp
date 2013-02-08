//
//  DrawLayer.cpp
//  MoonHerder
//
//  Created by Roger Engelbert on 2/1/13.
//
//

#include "DrawLayer.h"

DrawLayer::DrawLayer() {
	_screenSize = CCDirector::sharedDirector()->getWinSize();
	glLineWidth(LINE_THICKNESS * CC_CONTENT_SCALE_FACTOR());
    
    _manager = GameManager::sharedGameManager();
    //_blinking = false;
    _blinkTimer = 0;
    _blinkCount = 0;
    _blinkStateTime = 0;
    _blinkStateEnergy = 0;
    _startPoint = ccp(0,0);
}

DrawLayer * DrawLayer::create(){
	DrawLayer * layer = new DrawLayer();
    
	if (layer) {
		layer->autorelease();
        return layer;
	}
	CC_SAFE_DELETE(layer);
	return NULL;
}

void DrawLayer::setBlinking (bool var) {

    if (var) {
        _blinkCount = 0;
        _blinkStateTime = 0;
    }
}

void DrawLayer::draw(){
	
	CCNode::draw();
	
	
	glLineWidth(LINE_THICKNESS * CC_CONTENT_SCALE_FACTOR());
	//draw lines
	CCArray * lines = _manager->getLines();
	int numLines = lines->count();
    Line * line;
    for (int i = numLines - 1; i >= 0; i--) {
        line = (Line *) lines->objectAtIndex(i);
        drawLine(line);
    }
    
    _blinkTimer++;
    
    if (_blinkTimer > 10) {
        if (_blinkStateTime == 0) {
            _blinkStateTime = 1;
        } else {
            _blinkStateTime = 0;
        }
        if (_blinkStateEnergy == 0) {
            _blinkStateEnergy = 1;
        } else {
            _blinkStateEnergy = 0;
        }
        _blinkTimer = 0;
        _blinkCount++;
        if (_blinkCount > 5) {
            _blinkStateTime = 0;
            _blinkCount = 6;
        }
    }
	//draw temp line if any
    if (!pointEqualsZero(_startPoint) &&
        !pointEqualsZero(_touchPoint)) {
        ccDrawColor4F(0.0, 0.0, 0.0, 1.0);
        
        ccDrawCircle(_startPoint, 5, CC_DEGREES_TO_RADIANS(360), 10, false);
        
        ccDrawCircle(_touchPoint, 5, CC_DEGREES_TO_RADIANS(360), 10, false);
            
        ccDrawLine(_startPoint, _touchPoint);
        
    }
	
	glLineWidth(BAR_THICKNESS);

	//draw time bar
	drawBar(HORIZONTAL);
	
	//draw power bar
	drawBar(VERTICAL);
	
	
}
void DrawLayer::drawBar (int orientation ){

	float barX;
	float barY;
	
	if (orientation == HORIZONTAL) {
        //draw time bar
        if (_manager->getTime() == 0.0) return;
        
        float totalWidth = _screenSize.width * 0.8f;
        barX = _screenSize.width * 0.1f;
        barY = _screenSize.height * 0.03f;
        
        if (_manager->getTime() != 1) {
        	ccDrawColor4F(0.0, 0.0, 0.0, 1.0);
        	ccDrawLine(ccp(barX,barY),
        			   ccp(barX + totalWidth, barY));
        }
        
        ccDrawColor4F(0.0, 1.0, 1.0, 1.0);
        ccDrawLine(ccp(barX, barY), ccp(barX + totalWidth * _manager->getTime(), barY));
        
    } else {
        //draw energy bar
        float totalHeight = _screenSize.height * 0.8f;
        barX = _screenSize.width * 0.96f;
        barY = _screenSize.height * 0.1f;
        
        if (_manager->getLineEnergy() != 1) {
            if (_blinkStateTime == 0) {
                ccDrawColor4F(0.0, 0.0, 0.0, 1.0);
            } else {
                ccDrawColor4F(1.0, 0.0, 0.0, 1.0);
            }
            ccDrawLine(ccp(barX, barY), ccp(barX, barY + totalHeight));
        }
        
        if (_manager->getLineEnergy() <= 0) return;
        
        if (_manager->getBoosting()) {
            if (_blinkStateEnergy == 0) {
                ccDrawColor4F(1.0, 0.9, 0.0, 1.0);
            } else {
                ccDrawColor4F(1.0, 0.0, 1.0, 1.0);
            }
        } else {
            ccDrawColor4F(1.0, 0.9, 0.0, 1.0);
        }
        
        ccDrawLine(ccp(barX, barY), ccp(barX, barY + totalHeight * _manager->getLineEnergy()));
    }
}
void DrawLayer::drawLine(Line * line){
	if (line->getBlinkState() == 0) {
        ccDrawColor4F(1.0, 0.0, 1.0, 1.0);
    } else {
        ccDrawColor4F(1.0, 1.0, 1.0, 1.0);
    }
    
    //draw curved line
    if (line->getCurveState() != 0 && !pointEqualsZero(line->getCurve())) {
        
        drawQuadBezier( line->getStart(),
        				line->getCurve(),
        				line->getEnd(),
        				10);
        
    //draw straight line
    } else {
        ccDrawLine(line->getStart(), line->getEnd());
    }
}


void DrawLayer::drawQuadBezier(CCPoint origin, CCPoint control, CCPoint destination,  int segments) {
	
    
    float t = 0.0f;
    float x_;
    float y_;
    
    CCPoint previous = origin;
    for (int i = 0; i < segments + 1; i++) {
        
        x_ = powf(1 - t, 2) * origin.x + 2.0f * (1 - t) * t * control.x + t * t * destination.x;
        y_ = powf(1 - t, 2) * origin.y + 2.0f * (1 - t) * t * control.y + t * t * destination.y;
        
        if (i != 0)

        ccDrawCircle(previous, 2.2, M_PI, 6, false);
        
        ccDrawLine(previous, ccp (x_, y_) );
        previous = ccp(x_, y_);
        
        t += (float) 1 / segments;
    }
}
