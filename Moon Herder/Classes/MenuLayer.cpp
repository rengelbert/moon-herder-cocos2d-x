//
//  MenuLayer.cpp
//  MoonHerder
//
//  Created by Roger Engelbert on 2/1/13.
//
//

#include "MenuLayer.h"
#include "GameLayer.h"
#include "HelpLayer.h"

MenuLayer::~MenuLayer () {
}

MenuLayer::MenuLayer() {
}


MenuLayer * MenuLayer::create() {
	MenuLayer * layer = new MenuLayer();
    
	if (layer) {
		layer->autorelease();
		layer->init();
        return layer;
	}
	CC_SAFE_DELETE(layer);
	return NULL;
}

void MenuLayer::init () {
	
    _screenSize = CCDirector::sharedDirector()->getWinSize();
	_manager = GameManager::sharedGameManager();
    
    createScreen();
    
    _starsUpdateIndex = 0;
    _starsUpdateRange = 10;
    _starsUpdateInterval = 5;
    _starsUpdateTimer = 0.0;
	
    setTouchEnabled(true);
    scheduleUpdate();
    
}


void MenuLayer::update(float dt) {
    
    
    _starsUpdateTimer += dt;
    int stars_count = 150;
    if (_starsUpdateTimer > _starsUpdateInterval) {
        
        if (stars_count - _starsUpdateIndex < _starsUpdateRange) {
            _starsUpdateIndex = 0;
        } else if (_starsUpdateIndex + _starsUpdateRange > stars_count - 1) {
            _starsUpdateIndex += stars_count - _starsUpdateIndex - 1;
        } else {
            _starsUpdateIndex += _starsUpdateRange;
        }
        
        _starsUpdateTimer = 0;
        _starsUpdateInterval = ((float)rand() / RAND_MAX) * 5;
    }

    //update stars within update range
    Star * star;
    for (int i = _starsUpdateIndex; i < _starsUpdateIndex + _starsUpdateRange; i++) {
        if (i < stars_count) {
            CCPoint point = _manager->starPosition(i);
            int index = point.y * _manager->getColumns() + point.x;
            if (index >= STARS_IN_POOL) index = STARS_IN_POOL - 1;
            
            //identify cell in array
            star = _manager->starFromPool(index);
            if (star->isVisible() && star->getParent()) star->update(dt * DT_RATE);

        }
        
    }
}

void MenuLayer::ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
    
    CCTouch *touch = (CCTouch *) touches->anyObject();
    
    if (touch) {
	    
	    CCPoint tap = touch->getLocation();
        CCRect boundsPlay = _btnPlay->boundingBox();
        CCRect boundsHelp = _btnHelp->boundingBox();
        
        if (boundsPlay.containsPoint(tap)) {
            CCScene* newScene = CCTransitionFlipY::create(0.5f, GameLayer::scene(), kOrientationUpOver);
            CCDirector::sharedDirector()->replaceScene(newScene);
        } else if (boundsHelp.containsPoint(tap)) {
            CCScene* newScene = CCTransitionFlipY::create(0.5f, HelpLayer::scene(), kOrientationUpOver);
            CCDirector::sharedDirector()->replaceScene(newScene);
        }
    }
    
    
}


void MenuLayer::createScreen() {
    
    CCSprite * bg = CCSprite::create("bg_dark.jpg");
    bg->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.5f));
    this->addChild(bg, kBackground);
    
    _gameBatchNode = CCSpriteBatchNode::create("sprite_sheet.png", 800);
    this->addChild(_gameBatchNode, kMiddleground);
    
    CCSprite * ground = CCSprite::createWithSpriteFrameName("ground.png");
    ground->setAnchorPoint(ccp(0,0));
    _gameBatchNode->addChild(ground, kForeground);
    
    //add moon
    CCSprite * moon = CCSprite::createWithSpriteFrameName("intro_moon.png");
    moon->setPosition(ccp(80, 400));
    _gameBatchNode->addChild(moon, kForeground);
    
    //add logo
    CCSprite * logo = CCSprite::createWithSpriteFrameName("label_logo.png");
    logo->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.55f));
    _gameBatchNode->addChild(logo, kForeground);
    

    //add play button
    _btnPlay = CCSprite::createWithSpriteFrameName("label_play.png");
    _btnPlay->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.3f));
    _gameBatchNode->addChild(_btnPlay, kForeground);
    
    _btnHelp = CCSprite::createWithSpriteFrameName("label_tutorial.png");
    _btnHelp->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.2f));
    _gameBatchNode->addChild(_btnHelp, kForeground);
    
    
    //add best score number
    int bestScore = _manager->getBestScore();
    
    if (bestScore > 1) {
        CCString * score = CCString::createWithFormat("Best Score: %i stars", bestScore);
        
        CCLabelBMFont * scoreLabel = CCLabelBMFont::create(score->getCString(),
                                    "font_score.fnt", _screenSize.width * 0.8f);
        scoreLabel->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.4f));
        this->addChild(scoreLabel, kForeground);
    }
    
    //add stars
    Star * star;
    //number of stars and boosts to add to this level
    int numStars = 150;
    
    int cnt = 0;
    int i = 0;
    
    int index;
    CCPoint starPosition;
    CCPoint position;
    
    
    while (cnt < numStars) {
        
        starPosition = _manager->starPosition(i);
        i++;
        
        //grab stars array index based on selected Grid Cell
        index = starPosition.y * _manager->getColumns() + starPosition.x;
        
        if (index >= STARS_IN_POOL) {
            continue;
        }
        
        //grab position from selected Grid Cell
        position.x = starPosition.x * TILE;
        position.y = _screenSize.height - starPosition.y * TILE;
        
        //don't use cells too close to moon perch
        
        if (fabs(position.x  - moon->getPositionX()) < moon->boundingBox().size.width * 0.4f &&
            fabs(position.y - moon->getPositionY()) < moon->boundingBox().size.width * 0.4f) {
            continue;
        }
        
        
        //grab star from pool
        star = (Star *) _manager->starFromPool(index);
        if (star->getParent()) star->removeFromParentAndCleanup(false);
        
        
        star->setValues(position, false);
        star->setOpacity(200);
        
        _gameBatchNode->addChild(star, kMiddleground);
        star->setVisible(true);
        
        cnt++;
    }
    
}


CCScene* MenuLayer::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // add layer as a child to scene
    scene->addChild(MenuLayer::create());
    
    return scene;
}