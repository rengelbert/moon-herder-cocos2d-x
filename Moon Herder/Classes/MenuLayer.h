//
//  MenuLayer.h
//  MoonHerder
//
//  Created by Roger Engelbert on 2/1/13.
//
//

#ifndef __MoonHerder__MenuLayer__
#define __MoonHerder__MenuLayer__


#include "GameManager.h"
#include "DrawLayer.h"

class MenuLayer : public cocos2d::CCLayer
{
public:
    
    ~MenuLayer();
    MenuLayer();
    
    static MenuLayer * create();
    static cocos2d::CCScene* scene();
    
    //virtual void ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    //virtual void ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    virtual void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    void update (float dt);
    
private:
	
	CCSpriteBatchNode * _gameBatchNode;
    
    CCSprite * _btnPlay;
    CCSprite * _btnHelp;
	
    GameManager * _manager;
	
	CCSize _screenSize;
    
    int _starsUpdateIndex;
    int _starsUpdateRange;
    int _starsUpdateInterval;
    float _starsUpdateTimer;
    
	void init();
	void createScreen();
	
};


#endif /* defined(__MoonHerder__MenuLayer__) */
