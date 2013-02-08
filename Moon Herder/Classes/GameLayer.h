#ifndef __GameLayer_SCENE_H__
#define __GameLayer_SCENE_H__

#include "GameManager.h"
#include "DrawLayer.h"

class GameLayer : public cocos2d::CCLayer
{
public:
    
    ~GameLayer();
    GameLayer();
    
    static GameLayer * create();
    static cocos2d::CCScene* scene();
    
    virtual void ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    virtual void ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    virtual void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    void update (float dt);

private:
	
	CCSpriteBatchNode * _gameBatchNode;
	CCSprite * _moonPerch;
	CCSprite * _bgDark;
	CCSprite * _bgLight;
    
    CCSprite * _labelMeridian;
    CCSprite * _labelGameOver;
    CCSprite * _labelBestScore;
    CCSprite * _labelPoundKey;
    CCSprite * _btnTryAgain;
    CCSprite * _btnStart;
    CCSprite * _btnMenu;
	
    GameManager * _manager;
	Moon * _moon;
	Sun * _sun;
	DrawLayer *_drawLayer;

    CCParticleSystem * _boostHitParticles;
    CCParticleSystem * _lineHitParticles;
    CCParticleSystem * _groundHitParticles;
    CCParticleSystem * _starHitParticles;
    
    CCLabelBMFont * _scoreLabel;
    CCLabelBMFont * _levelLabel;

	CCSize _screenSize;
	CCPoint _moonStartPoint;
        
	bool _starsCollected;
	bool _sunRise;
    bool _running;
    
    int _gameState;
    int _numStars;
    int _starsUpdateIndex;
    int _starsUpdateRange;
    int _starsUpdateInterval;
    float _starsUpdateTimer;
    
	void initGame();
	void createGameScreen();
	void newLevel();
	void gameOver();
    void reset (void);
	void clearStarAt(CCPoint point);
	
	void addStars(void);
    
    inline bool pointEqualsPoint (CCPoint point1, CCPoint point2) {
        return point1.x == point2.x && point1.y == point2.y;
    }
    
    inline void clearLabels (void) {
        _labelMeridian->setVisible(false);
        _labelGameOver->setVisible(false);
        _labelBestScore->setVisible(false);
        _labelPoundKey->setVisible(false);
        _btnTryAgain->setVisible(false);
        _btnStart->setVisible(false);
        _btnMenu->setVisible(false);
        _scoreLabel->setVisible(false);
        _levelLabel->setVisible(false);
    }
    
};

#endif // __GameLayer_SCENE_H__

