//
//  HelpLayer.cpp
//  MoonHerderCpp
//
//  Created by Roger Engelbert on 2/4/13.
//
//

#include "HelpLayer.h"
#include "GameLayer.h"


HelpLayer::~HelpLayer () {
}

HelpLayer::HelpLayer() {
}


HelpLayer * HelpLayer::create() {
	HelpLayer * layer = new HelpLayer();
    
	if (layer) {
		layer->autorelease();
		layer->initHelp();
        return layer;
	}
	CC_SAFE_DELETE(layer);
	return NULL;
}

void HelpLayer::initHelp () {
	
    _screenSize = CCDirector::sharedDirector()->getWinSize();
	_moonStartPoint = ccp(60, _screenSize.height - 60);
    
    _manager = GameManager::sharedGameManager();
    _manager->reset();
    
    _starsCollected = false;
    _sunRise = false;
    _running = false;
    _energyMsgShown = false;
    _timeMsgShown = false;
    _tutorialDone = false;
    _labelInterval = 8;
    
	createHelpScreen();
	
    _running = false;
    
    setTouchEnabled(true);
    scheduleUpdate();
    
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
    SimpleAudioEngine::sharedEngine()->playBackgroundMusic("bg.mp3", true);
}

void HelpLayer::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
    
    if (_gameState != kGameStatePlay || !_running) return;
    
    CCTouch *touch = (CCTouch *) touches->anyObject();
    
    if (touch) {
	    
	    CCPoint tap = touch->getLocation();
        _drawLayer->setStartPoint(tap);
    }
}


void HelpLayer::ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
    
    if (_gameState != kGameStatePlay || !_running) return;
    
    CCTouch *touch = (CCTouch *) touches->anyObject();
    
    
    if (touch) {
	    
	    CCPoint tap = touch->getLocation();
        
        _drawLayer->setTouchPoint(tap);
    }
}

void HelpLayer::ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
    
    CCTouch *touch = (CCTouch *) touches->anyObject();
    
    if (touch) {
	    
	    CCPoint tap = touch->getLocation();
        
        if (!_running) {
            if (!_tutorialDone) {
                
                _moon->reset();
                _moon->setPosition(_moonStartPoint);
                _moon->setNextPosition(_moonStartPoint);
                
                _tutorialLabel->setString("Collect all stars.\n\nIf you take too long,\nthe sun will rise.\nAnd then grow!");
                _labelTimer = 0;
                _running = true;
            } else {
                CCScene* newScene = CCTransitionFlipY::create(0.5f, GameLayer::scene(), kOrientationUpOver);
                CCDirector::sharedDirector()->replaceScene(newScene);
            }
            return;
        } else {
        
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
                
            }
        }
    }
}


void HelpLayer::update (float dt) {
    
    if (_gameState == kGameStatePlay) {
        
		//if running game logic
		if (_running) {
            
            
            _manager->update(dt);
            
            
            _labelTimer += dt;
            if (_labelTimer > _labelInterval) {
                _labelTimer = 0;
                _tutorialLabel->setString("");
            }
            
            
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
            
        
            if (_manager->getTime() < 0.65 && !_timeMsgShown) {
                _tutorialLabel->setString("Watch out for your time.");
                _labelTimer = 0;
                _timeMsgShown = true;
            }
            
            
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
                    _tutorialLabel->setString("That's it!\n\nTo your right is the power bar.\nAt the bottom your timer\n\n Tap screen to play.");
                    _tutorialDone = true;
                    _labelTimer= 0;
                    _running = false;
                    _manager->getLines()->removeAllObjects();
				}
			}
            
            if (_moon->getPositionY() < _moon->getRadius() && _moon->getActive()) {
                _groundHitParticles->setPosition(_moon->getPosition());
                _groundHitParticles->resetSystem();
                SimpleAudioEngine::sharedEngine()->playEffect("ground_hit.wav");
                _moon->setActive(false);
                //GAME OVER!!!!
                _running = false;
                if ((float)rand()/RAND_MAX > 0.5f) {
                    _tutorialLabel->setString("Oops! Try again. \n\n\n Tap screen to begin.");
                } else {
                    _tutorialLabel->setString("Oops! \n\nDraw lines to stop the moon\n from falling to the ground.\n\n\n Tap screen to try again.");
                }
                _manager->getLines()->removeAllObjects();
                _labelTimer= 0;
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


void HelpLayer::createHelpScreen () {
    
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
    
    _sun->setPosition(ccp(_screenSize.width * 0.5f, -_sun->getRadius()));
    _sun->setNextPosition(ccp(_screenSize.width * 0.5f, -_sun->getRadius()));
    
    _sun->setVisible(false);
    _sun->reset();
    
     
    //reset moon and perch
    _moon->setPosition(_moonStartPoint);
    _moon->setNextPosition(_moonStartPoint);
    _moon->reset();
    _moonPerch->setOpacity(50);
    
    
    _starsCollected = false;
    
    _drawLayer->setStartPoint(ccp(0,0));
    _drawLayer->setTouchPoint(ccp(0,0));
    
    _bgDark->setOpacity (255);
    _bgLight->setOpacity(0);
    
    _starsUpdateIndex = 0;
    _starsUpdateRange = 10;
    _starsUpdateInterval = 5;
    _starsUpdateTimer = 0.0;
    
    //add stars
    Star * star;
    //number of stars and boosts to add to this level
    _numStars = 5;
    int numBoosts = 1;
    
    
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

    _tutorialLabel = CCLabelTTF::create("Draw lines to stop the moon\n from falling to the ground.\n\n\n Tap screen to begin.", "TrebuchetMS-Bold", 16, CCSize(_screenSize.width * 0.8f, _screenSize.height * 0.4f), kCCTextAlignmentCenter);
    _tutorialLabel->setPosition(ccp (_screenSize.width * 0.5f, _screenSize.height * 0.2f) );
    this->addChild(_tutorialLabel, kForeground);
    _labelTimer= 0;
    
    _gameState = kGameStatePlay;
}


void HelpLayer::clearStarAt(CCPoint point) {
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
            
            
            
            //did we hit a boost?
            if (star->getIsBoost()) {
            
                _manager->setBoosting(true);
                
                if (starsCollected != _numStars) {
                    _boostHitParticles->setPosition(star->getPosition());
                    _boostHitParticles->resetSystem();
                }
            }
            
            //if last star on screen, show particles, show Moon Perch...
            if (starsCollected == _numStars) {
                
                SimpleAudioEngine::sharedEngine()->playEffect("last_star_hit.wav");
                _starHitParticles->setPosition(star->getPosition());
                _starHitParticles->resetSystem();
                _starsCollected = true;
                
                if (_sun->isVisible()) {
                    _moonPerch->setOpacity(150);
                } else {
                    _moonPerch->setOpacity(200);
                }
                _tutorialLabel->setString("Return the moon to its perch.");
                _labelTimer= 0;
            } else {
                if (star->getIsBoost()) {
                    SimpleAudioEngine::sharedEngine()->playEffect("boost_hit.wav");
                    _tutorialLabel->setString("This is a boost.\n It increases your energy\n until a new line is added.");
                    _labelTimer= 0;
                } else {
                    SimpleAudioEngine::sharedEngine()->playEffect("star_hit.wav");
                }
                
                
            }
            
        }
	}
}


CCScene* HelpLayer::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // add layer as a child to scene
    scene->addChild(HelpLayer::create());
    
    return scene;
}



