//
//  HelpLayer.h
//  MoonHerderCpp
//
//  Created by Roger Engelbert on 2/4/13.
//
//

#ifndef __MoonHerderCpp__HelpLayer__
#define __MoonHerderCpp__HelpLayer__

#include "GameManager.h"
#include "DrawLayer.h"

class HelpLayer : public cocos2d::CCLayer
{
public:
    
    ~HelpLayer();
    HelpLayer();
    
    static HelpLayer * create();
    static cocos2d::CCScene* scene();
    
    virtual void ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    virtual void ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    virtual void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    void update (float dt);
    
private:
	
	CCSpriteBatchNode * _gameBatchNode;
    CCLabelTTF * _tutorialLabel;
    
	CCSprite * _moonPerch;
	CCSprite * _bgDark;
	CCSprite * _bgLight;
    
    GameManager * _manager;
	Moon * _moon;
	Sun * _sun;
	DrawLayer *_drawLayer;
    CCParticleSystem * _boostHitParticles;
    CCParticleSystem * _lineHitParticles;
    CCParticleSystem * _groundHitParticles;
    CCParticleSystem * _starHitParticles;
    
    CCSize _screenSize;
	CCPoint _moonStartPoint;
    
	bool _starsCollected;
	bool _sunRise;
    bool _running;
    bool _energyMsgShown;
    bool _timeMsgShown;
    bool _tutorialDone;
    
    int _gameState;
    int _numStars;
    int _starsUpdateIndex;
    int _starsUpdateRange;
    int _starsUpdateInterval;
    float _starsUpdateTimer;
    float _labelTimer;
    int _labelInterval;
    
	void initHelp();
	void createHelpScreen();
	void clearStarAt(CCPoint point);
	   
    inline bool pointEqualsPoint (CCPoint point1, CCPoint point2) {
        return point1.x == point2.x && point1.y == point2.y;
    }
    
};

#endif /* defined(__MoonHerderCpp__HelpLayer__) */
