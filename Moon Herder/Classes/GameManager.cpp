//
//  GameManager.cpp
//  MoonHerder
//
//  Created by Roger Engelbert on 2/1/13.
//
//

#include "GameManager.h"


static GameManager * _instance = NULL;


GameManager::~GameManager() {
    
	CC_SAFE_RELEASE(_lines);
	CC_SAFE_RELEASE(_starsPool);
	CC_SAFE_RELEASE(_linesPool);
}

GameManager::GameManager() {
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("sprite_sheet.plist");
	
	_starsPool = CCArray::createWithCapacity(STARS_IN_POOL);
	_starsPool->retain();
	_linesPool = CCArray::createWithCapacity(50);
	_linesPool->retain();
	
	_lines = CCArray::createWithCapacity(50);
	_lines->retain();
	
	_linesPoolIndex = 0;
	   
    //create stars pool
    for (int i = 0; i < STARS_IN_POOL; i++) {
        _starsPool->addObject(Star::create());
    }
    
    //create lines pool
    for (int i = 0; i < 50; i++) {
        _linesPool->addObject(Line::create());
    }
    
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    
    //create grid cells
    _cols = screenSize.width / TILE;
    _rows = screenSize.height * 0.9f / TILE;
    
    //create grid
    int c;
    int r;
    
    //for (r = 0; r < _rows; r++) {
    for (r = _rows - 1; r >= 0; r--) {
        for (c = 0; c < _cols; c++) {
            _gridCells.push_back(ccp(c,r));
        }
    }
    
    random_shuffle(_gridCells.begin(), _gridCells.end());
    
    _initialLineDecrement = 0.07;
	_lineDecrementRatio = 0.009;
		
	_initialStars = 3;
	_starsRatio = 3;
	
	_initialTimeDecrement = 0.006f;
	_timeDecrementRatio = 0.0005f;
	
	_initialBoostNumber = -2;
	_boostNumberIncrement = 1;
	
	//test code to clear stored data
	//CCUserDefault::sharedUserDefault()->setIntegerForKey("bestScore", 0);
	
	
	//load las best score
   if (CCUserDefault::sharedUserDefault()->getIntegerForKey("bestScore") == 0){
		_bestScore = 0;
		CCUserDefault::sharedUserDefault()->setIntegerForKey("bestScore", 0);
		CCUserDefault::sharedUserDefault()->flush();
	
	} else {
		_bestScore = CCUserDefault::sharedUserDefault()->getIntegerForKey("bestScore");
	}
	 
	reset();
}
	
GameManager * GameManager::sharedGameManager() {
	 if (!_instance) {
        _instance = new GameManager();
    }
    return _instance;
}

void GameManager::reset (void) {
	//reset to level 1 values (base values)
    _level = 1;
    _totalCollectedStars = 0;
	updateGameData();
    
}

void GameManager::setBoosting(bool var) {
    
    _boosting = var;
    
    if (var) {
        _boostSpeed++;
    } else {
        _boostSpeed = 0;
    }
}

void GameManager::setBestScore (int var) {
    _bestScore = var;
    CCUserDefault::sharedUserDefault()->setIntegerForKey("bestScore", _bestScore);
    CCUserDefault::sharedUserDefault()->flush();
}

void GameManager::update(float dt) {
    
    _time -= _timeDecrement * dt;
    if (_time < 0) _time = 0;
    
    //boosting
    if (_boosting) {
        _lineEnergy += 0.04 * dt * _boostSpeed;
        if (_lineEnergy > 1) _lineEnergy = 1;
        
    }
}


void GameManager::updateGameData (void) {
	//increase line decrement value, total stars, boosts, and time decrement

    _lineDecrement = _initialLineDecrement + (_level-1) * _lineDecrementRatio;
    _totalStars = _initialStars + (_level - 1) * _starsRatio;
	
    if (_totalStars > STARS_IN_POOL) {
		_totalStars = STARS_IN_POOL;
	}
    if (_lineDecrement > 0.5f) _lineDecrement = 0.5f;
	
    _collectedStars = 0;
    _timeDecrement = _initialTimeDecrement + (_level - 1) * _timeDecrementRatio;
    _boostNumber = _initialBoostNumber + (_level - 1) *_boostNumberIncrement;
    if (_boostNumber > 5) _boostNumber = 5;
    
    random_shuffle(_gridCells.begin(), _gridCells.end());
    
    int count = _starsPool->count();
    Star * star;
    for (int i = 0; i < count; i++) {
        star = (Star *) _starsPool->objectAtIndex(i);
        star->setVisible(false);
        if (star->getParent()) {
            star->removeFromParentAndCleanup(false);
        }
    }
    
    _lines->removeAllObjects();
    
    _lineEnergy = 1;
    _time = 1;
    _boostSpeed = 0;
    _boosting = false;
}	
	

	
