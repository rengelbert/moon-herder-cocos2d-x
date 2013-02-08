#include "GameLayer.h"
#include "MenuLayer.h"

GameLayer::~GameLayer () {
}

GameLayer::GameLayer() {
}


GameLayer * GameLayer::create() {
	GameLayer * layer = new GameLayer();
    
	if (layer) {
		layer->autorelease();
		layer->initGame();
        return layer;
	}
	CC_SAFE_DELETE(layer);
	return NULL;
}

void GameLayer::initGame () {
	
    _screenSize = CCDirector::sharedDirector()->getWinSize();
	_moonStartPoint = ccp(60, _screenSize.height - 60);

    _manager = GameManager::sharedGameManager();
    _manager->reset();
    
    _starsCollected = false;
    _sunRise = false;
    _running = false;
    
    
	createGameScreen();
	
	reset();
    _running = true;
    
    setTouchEnabled(true);
    scheduleUpdate();
    
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
    SimpleAudioEngine::sharedEngine()->playBackgroundMusic("bg.mp3", true);
}

void GameLayer::reset() {
    
    //reset sun
    _sun->setPosition(ccp(_screenSize.width * 0.5f, -_sun->getRadius()));
    _sun->setNextPosition(ccp(_screenSize.width * 0.5f, -_sun->getRadius()));
    
    _sun->setVisible(false);
    _sun->reset();
    
	//reset manager
    //_manager->reset();
    
	//reset moon and perch
    _moon->setPosition(_moonStartPoint);
    _moon->setNextPosition(_moonStartPoint);
    _moon->reset();
    _moonPerch->setOpacity(50);
    
	
	//clear all visible stars
    //clearAllStars();
	_starsCollected = false;

    _drawLayer->setStartPoint(ccp(0,0));
    _drawLayer->setTouchPoint(ccp(0,0));
    
    _bgDark->setOpacity (255);
    _bgLight->setOpacity(0);
    _bgDark->setVisible(true);
    _bgLight->setVisible(false);
    
    _starsUpdateIndex = 0;
    _starsUpdateRange = 10;
    _starsUpdateInterval = 5;
    _starsUpdateTimer = 0.0;
    
    addStars();
    _gameState = kGameStatePlay;
    
}

void GameLayer::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
    
    if (_gameState != kGameStatePlay) return;
    
    CCTouch *touch = (CCTouch *) touches->anyObject();
    
    if (touch) {
	    
	    CCPoint tap = touch->getLocation();
        _drawLayer->setStartPoint(tap);
    }
}


void GameLayer::ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
    
    if (_gameState != kGameStatePlay) return;
    
    CCTouch *touch = (CCTouch *) touches->anyObject();
    
    
    if (touch) {
	    
	    CCPoint tap = touch->getLocation();
        _drawLayer->setTouchPoint(tap);
    }
}

void GameLayer::ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
    
    CCTouch *touch = (CCTouch *) touches->anyObject();
    
    if (touch) {
	    
	    CCPoint tap = touch->getLocation();
        
        
        if (_gameState == kGameStatePlay) {
            
            int line_count = _manager->getLines()->count();
            
            //don't bother creating new lines if 10 on stage already, or if no power
            if (line_count > 10 || _manager->getLineEnergy() <= 0) {
                _drawLayer->setStartPoint(ccp(0,0));
                _drawLayer->setTouchPoint(ccp(0,0));
                return;
            }
            //don't bother with line if points aren't set on screen
            //and if start and end points are the same
            if (!pointEqualsPoint(_drawLayer->getStartPoint(), ccp(0,0)) &&
                !pointEqualsPoint(_drawLayer->getStartPoint(), tap)) {
                
                Line * line = _manager->lineFromPool();
                line->setValues(_drawLayer->getStartPoint(), tap);
                
                //check length of the line, if too short, get rid of it
                if (line->getLength() < 40) {
                    _drawLayer->setStartPoint(ccp(0,0));
                    _drawLayer->setTouchPoint(ccp(0,0));
                    return;
                }
                _manager->getLines()->addObject(line);
                if (_manager->getBoosting()) {
                    _manager->setBoosting(false);
                    _moon->turnOnOff(_manager->getLineEnergy() > 0);
                }
                _drawLayer->setBlinking(true);
                
                 //calculate the energy cost of the created line
                float cost = line->getLength() * _manager->getLineDecrement();
                 
                 //if more than one line on screen, then add that to the cost
                if (line_count > 1) cost *= line_count;
                
                 _manager->setLineEnergy(
                    _manager->getLineEnergy() - 0.005 * cost
                );
            }
            _drawLayer->setStartPoint(ccp(0,0));
            _drawLayer->setTouchPoint(ccp(0,0));
            
        } else if  (_gameState == kGameStateNewLevel) {
            //if showing New Level message, with TOUCH END, start new level
            
            CCRect bounds = _btnStart->boundingBox();
            if (bounds.containsPoint(tap)) {
                clearLabels();
                
                reset();
            
            }
            
                //if showing Game Over message, with TOUCH END, track buttons
        } else if (_gameState == kGameStateGameOver){
            CCRect try_bounds = _btnTryAgain->boundingBox();
            CCRect menu_bounds = _btnMenu->boundingBox();
            
            if (try_bounds.containsPoint(tap)) {
                SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
                SimpleAudioEngine::sharedEngine()->playBackgroundMusic("bg.mp3", true);
                
                clearLabels();
                _manager->reset();
                reset();
                
            } else if (menu_bounds.containsPoint(tap)) {
                //clearLabels();
                //show menu
                CCScene* newScene = CCTransitionFlipY::create(0.5f, MenuLayer::scene(), kOrientationDownOver);
                CCDirector::sharedDirector()->replaceScene(newScene);
            }
        }
    }
}


void GameLayer::update (float dt) {
    
    if (_gameState == kGameStatePlay) {
        
		//if running game logic
		if (_running) {
            
            
            _manager->update(dt);
            dt *= DT_RATE;
            
			//update elements
            _moon->update(dt);
            if (_sun->isVisible()) {
                _sun->update(dt);
                if (_sun->checkCollisionWithMoon(_moon)) {
                    SimpleAudioEngine::sharedEngine()->playEffect("sun_hit.wav");
                }
            }
            
			
			//check collision with lines, update, draw
			Line * line;
            int len = _manager->getLines()->count();
            bool collisionDetected = false;
			
			for (int i = len-1; i >= 0; i--) {
                
				line = (Line *) _manager->getLines()->objectAtIndex(i);
                
				if (!collisionDetected && line->getActive()) {
					if (line->collidesWithMoon(_moon)) {
						collisionDetected = true;
                        SimpleAudioEngine::sharedEngine()->playEffect("line_hit.wav");
                        _lineHitParticles->setPosition(line->getCollisionPoint());
                        _lineHitParticles->resetSystem();
					}
				}
                
				if (line->getTrashme()) {
                    _manager->getLines()->removeObjectAtIndex(i);
				} else {
                    line->update(dt);
                }
			}
            _moon->place();
            
            //if moon off screen to the top, make screen darker as moons gets farther and farther away
            if (_moon->getPositionY() > _screenSize.height) {
                if (!_sun->isVisible()) {
                    float opacity = fabs((255 * (_moon->getPositionY() - _screenSize.height))/_screenSize.height);
                    if (opacity > 200) opacity = 200;
                    if (!_sun->isVisible()) _bgDark->setOpacity ( 255 - opacity );
                }
            } else {
                if (!_sun->isVisible()) {
                    if (_bgDark->getOpacity() != 255) _bgDark->setOpacity ( 255 );
                }
            }
        
            
			//track collision with MOON and STAR (usign grid logic)
            float range = _moon->getRadius();
            float posX = _moon->getPositionX();
            float posY = _moon->getPositionY();
            
			//I decided to check 9 cells for precision sake
            clearStarAt(ccp(posX, posY));
            clearStarAt(ccp(posX, posY + range));
            clearStarAt(ccp(posX, posY - range));
            clearStarAt(ccp(posX + range, posY));
            clearStarAt(ccp(posX + range, posY + range));
            clearStarAt(ccp(posX + range, posY - range));
            clearStarAt(ccp(posX - range, posY));
            clearStarAt(ccp(posX - range, posY - range));
            clearStarAt(ccp(posX - range, posY + range));
            
            
            //update bars
            
            
            
			//check timer
			if (_manager->getTime()  <= 0.65f && !_sun->isVisible()) {
				SimpleAudioEngine::sharedEngine()->playEffect("sun_rise.wav");
                _sun->setVisible(true);
				_sun->setHasRisen(false);
			} else if (_manager->getTime() <= 0.25f && _sun->isVisible() && !_sun->getHasGrown()) {
                SimpleAudioEngine::sharedEngine()->playEffect("sun_grow.wav");
                _sun->highNoon();
            } else if (_manager->getTime() <= 0.0f) {
                //if you want game over once time runs out.
                //game;
            }
            
            if (_sun->isVisible()) {
                if (!_bgLight->isVisible()) {
                    _bgLight->setVisible(true);
                }
                //when sun is added to screen, fade out dark bg
                if (_bgLight->getOpacity() + 5  < 255) {
                    _bgLight->setOpacity(_bgLight->getOpacity() + 5 );
                    _bgDark->setOpacity (_bgDark->getOpacity() - 5);
                } else {
                    _bgDark->setVisible(false);
                    _bgDark->setOpacity(255);
                   _bgLight->setOpacity(255);
                }
                _sun->place();
			}
            
            //check power
            if (_manager->getLineEnergy() <= 0) {
                if (!_moon->getIsOff()) {
                    _moon->turnOnOff(false);
                }
            }
			
			//track collision between Moon and Moon's perch
			if (_starsCollected) {
                if (pow (_moonStartPoint.x - _moon->getPositionX(), 2) +
                    pow (_moonStartPoint.y - _moon->getPositionY(), 2) < _moon->getSquaredRadius()) {
					_moon->setPosition(_moonStartPoint);
                    _moon->setNextPosition(_moonStartPoint);
                    _moon->setActive(false);
					newLevel();					
				}
			}
            
            if (_moon->getPositionY() < _moon->getRadius() && _moon->getActive()) {
                _groundHitParticles->setPosition(_moon->getPosition());
                _groundHitParticles->resetSystem();
                SimpleAudioEngine::sharedEngine()->playEffect("ground_hit.wav");
                _moon->setActive(false);
                gameOver();
            }
            
            //make stars blink
            if (!_sun->isVisible()) {
                _starsUpdateTimer += dt;
                int stars_count = _numStars;
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
                        if (star->isVisible() && !star->getIsBoost()) star->update(dt);
                    }
                    
                }
            }
            
		}
	}
}


void GameLayer::createGameScreen () {
    
    _bgLight = CCSprite::create("bg_light.jpg");
    _bgLight->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.5f));
    this->addChild(_bgLight, kBackground);
    _bgLight->setVisible(false);
    _bgLight->setOpacity(0);

    _bgDark = CCSprite::create("bg_dark.jpg");
    _bgDark->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.5f));
    this->addChild(_bgDark, kBackground);
    
    _gameBatchNode = CCSpriteBatchNode::create("sprite_sheet.png", 800);
    this->addChild(_gameBatchNode, kMiddleground);
    
    _moonPerch = CCSprite::createWithSpriteFrameName("moon_perch.png");
    _moonPerch->setPosition(_moonStartPoint);
    _moonPerch->setOpacity(50);
    _gameBatchNode->addChild(_moonPerch, kBackground);
    
    CCSprite * ground = CCSprite::createWithSpriteFrameName("ground.png");
    ground->setAnchorPoint(ccp(0,0));
    _gameBatchNode->addChild(ground, kForeground);
    
    _moon = Moon::create();
    _moon->setPosition(_moonStartPoint);
    _moon->setNextPosition(_moonStartPoint);
    _gameBatchNode->addChild(_moon, kMiddleground);
    
    _sun = Sun::create();
    _sun->setPosition(ccp(_screenSize.width * 0.5, -_sun->getRadius()));
    _sun->setNextPosition(ccp(_screenSize.width * 0.5, -_sun->getRadius()));
    _sun->setVisible(false);
    _gameBatchNode->addChild(_sun, kMiddleground);
    
    _scoreLabel = CCLabelBMFont::create("", "font_score.fnt", _screenSize.width * 0.8f);
    _scoreLabel->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.48f));
    this->addChild(_scoreLabel, kForeground);
    _scoreLabel->setVisible(false);
    
    _levelLabel = CCLabelBMFont::create("12", "font_level.fnt", _screenSize.width * 0.5f);
    _levelLabel->setAnchorPoint(ccp(0,0));
    _levelLabel->setPosition(ccp(_screenSize.width * 0.46f, _screenSize.height * 0.61f));
    this->addChild(_levelLabel, kForeground);
    _levelLabel->setVisible(false);
    
    _boostHitParticles = CCParticleSystemQuad::create("boost_hit.plist");
    _lineHitParticles = CCParticleSystemQuad::create("line_burst.plist");
    _groundHitParticles = CCParticleSystemQuad::create("off_screen.plist");
    _starHitParticles  = CCParticleSystemQuad::create("star_burst.plist");
    
    _boostHitParticles->stopSystem();
    _lineHitParticles->stopSystem();
    _groundHitParticles->stopSystem();
    _starHitParticles->stopSystem();
    
    this->addChild(_boostHitParticles);
    this->addChild(_lineHitParticles);
    this->addChild(_groundHitParticles);
    this->addChild(_starHitParticles);
    
    _drawLayer = DrawLayer::create();
    this->addChild(_drawLayer, kForeground);
   
    
    //create game over state
    _labelGameOver = CCSprite::createWithSpriteFrameName("label_game_over.png");
    _labelGameOver->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.65f));
    _labelGameOver->setVisible(false);
    _gameBatchNode->addChild(_labelGameOver, kForeground);
    
     
    _labelBestScore = CCSprite::createWithSpriteFrameName("label_best_score.png");
    _labelBestScore->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.42f));
    _labelBestScore->setVisible(false);
    _gameBatchNode->addChild(_labelBestScore, kForeground);
    
    _btnTryAgain = CCSprite::createWithSpriteFrameName("label_try_again.png");
    _btnTryAgain->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.26f));
    _btnTryAgain->setVisible(false);
    _gameBatchNode->addChild(_btnTryAgain, kForeground);
    
    _btnMenu = CCSprite::createWithSpriteFrameName("label_menu.png");
    _btnMenu->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.15f));
    _btnMenu->setVisible(false);
    _gameBatchNode->addChild(_btnMenu, kForeground);
     

    //create new level state
    _labelPoundKey = CCSprite::createWithSpriteFrameName("label_number.png");
    _labelPoundKey->setPosition(ccp(_screenSize.width * 0.42f, _screenSize.height * 0.65f));
    _labelPoundKey->setVisible(false);
    _gameBatchNode->addChild(_labelPoundKey, kForeground);
    
    _labelMeridian = CCSprite::createWithSpriteFrameName("label_meridian.png");
    _labelMeridian->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.55f));
    _labelMeridian->setVisible(false);
    _gameBatchNode->addChild(_labelMeridian, kForeground);
    
    _btnStart = CCSprite::createWithSpriteFrameName("label_start.png");
    _btnStart->setPosition(ccp(_screenSize.width * 0.5f, _screenSize.height * 0.2f));
    _btnStart->setVisible(false);
    _gameBatchNode->addChild(_btnStart, kForeground);
    
         
}

void GameLayer::newLevel () {
    
    _gameState = kGameStateNewLevel;
    
    _manager->getLines()->removeAllObjects();
    _manager->setLevel(_manager->getLevel() + 1);
    
    
    _labelPoundKey->setVisible(true);
    _labelMeridian->setVisible(true);
    _btnStart->setVisible(true);

    CCString * level = CCString::createWithFormat("%i", _manager->getLevel());
    _levelLabel->setString(level->getCString());
    _levelLabel->setVisible(true);
    
    int totalScore = _manager->getTotalCollectedStars();
    
    if (totalScore > 1) {
        if (totalScore > _manager->getBestScore() && totalScore > 1) {
            _manager->setBestScore(totalScore);
            _labelBestScore->setVisible(true);
        }
        
        CCString * score = CCString::createWithFormat("score: %i stars", totalScore);
        
        _scoreLabel->setString(score->getCString());
        _scoreLabel->setVisible(true);
    }
}

void GameLayer::gameOver () {
    
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
    
    _gameState = kGameStateGameOver;
    _manager->getLines()->removeAllObjects();

    _labelGameOver->setVisible(true);
    _btnTryAgain->setVisible(true);
    _btnMenu->setVisible(true);
    
    int totalScore = _manager->getTotalCollectedStars();
    
    if (totalScore > 1) {
        CCString * score = CCString::createWithFormat("score: %i stars", totalScore);
        
        _scoreLabel->setString(score->getCString());
        _scoreLabel->setVisible(true);
        
        if (totalScore > _manager->getBestScore()) {
            _manager->setBestScore(totalScore);
            _labelBestScore->setVisible(true);
        }
    }

}


void GameLayer::clearStarAt(CCPoint point) {
    int col = (int) point.x / TILE;
	int row  = (int) (_screenSize.height - point.y) / TILE;
    
	if (row < 0 || col < 0 || row >= _manager->getRows() || col >= _manager->getColumns() ||
        row * _manager->getColumns() + col >= STARS_IN_POOL) {
		return;
	}
    
	//identify cell in array
	Star * star = _manager->starFromPool(row * _manager->getColumns() + col);
    //CCLOG ("COL: %i  ROW: %i", col, row);
    
    if (star->isVisible()) {
        
        float diffx = _moon->getPositionX() - star->getPositionX();
        float diffy = _moon->getPositionY() - star->getPositionY();

        
        if ((diffx * diffx + diffy * diffy) <= _moon->getSquaredRadius()) {
            
            int starsCollected = _manager->getCollectedStars();
            int totalStarsCollected = _manager->getTotalCollectedStars();
            
            starsCollected++;
            totalStarsCollected++;
            
            _manager->setCollectedStars(starsCollected);
            _manager->setTotalCollectedStars(totalStarsCollected);
            
            star->setVisible(false);
            
            
            int totalStars = _manager->getTotalStars();
            
            //did we hit a boost?
            if (star->getIsBoost()) {
                
                _manager->setBoosting(true);
                
                if (starsCollected != totalStars) {
                    _boostHitParticles->setPosition(star->getPosition());
                    _boostHitParticles->resetSystem();
                }
            }
            
            //if last star on screen, show particles, show Moon Perch...
            if (starsCollected == totalStars) {
                SimpleAudioEngine::sharedEngine()->playEffect("last_star_hit.wav");
                _starHitParticles->setPosition(star->getPosition());
                _starHitParticles->resetSystem();
                _starsCollected = true;

                if (_sun->isVisible()) {
                    _moonPerch->setOpacity(100);
                } else {
                    _moonPerch->setOpacity(200);
                }
            } else {
                if (star->getIsBoost()) {
                    SimpleAudioEngine::sharedEngine()->playEffect("boost_hit.wav");
                } else {
                    SimpleAudioEngine::sharedEngine()->playEffect("star_hit.wav");
                }
            }
            
        }
	}
}

void GameLayer::addStars(void) {
    
    Star * star;
    //number of stars and boosts to add to this level
    _numStars = _manager->getTotalStars();
    int numBoosts = _manager->getBoostNumber();
    

    int cnt = 0;
    int i = 0;

    int index;
    CCPoint starPosition;
    CCPoint position;
    
    
    while (cnt < _numStars) {
        
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
        
        if (fabs(position.x  - _moonStartPoint.x) < _moon->getRadius() * 2 &&
            fabs(position.y - _moonStartPoint.y) < _moon->getRadius() * 2) {
            continue;
        }
         
        
        //grab star from pool
        star = (Star *) _manager->starFromPool(index);
        if (star->getParent()) star->removeFromParentAndCleanup(false);
        if (star->getOpacity() != 255) star->setOpacity(255);
        
        //add boosts first, if any
        if ( cnt >= _numStars - numBoosts) {
            star->setValues(position, true);
            
        } else {
            star->setValues(position, false);
        }
        
        _gameBatchNode->addChild(star, kMiddleground);
        star->setVisible(true);
        
        cnt++;
    }
}


CCScene* GameLayer::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // add layer as a child to scene
    scene->addChild(GameLayer::create());
    
    return scene;
}


