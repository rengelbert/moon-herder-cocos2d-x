//
//  GameManager.h
//  MoonHerder
//
//  Created by Roger Engelbert on 2/1/13.
//
//

#ifndef __MoonHerder__GameManager__
#define __MoonHerder__GameManager__

#include "Star.h"
#include "Sun.h"
#include "Line.h"
#include "Moon.h"
#include <vector>

class DrawLayer;

using namespace cocos2d;

class GameManager : public CCObject {
private:
	CCArray * _starsPool;
	CCArray * _linesPool;
	
	int _linesPoolIndex;
	
	float _initialLineDecrement;
    float _lineDecrementRatio;
    
    int _initialStars;
    int _starsRatio;
    
    float _initialTimeDecrement;
    float _timeDecrementRatio;
    
    int _initialBoostNumber;
    int _boostNumberIncrement;
    
    //int _boostInterval;
    //float _boostTimer;
    int _boostSpeed;
    
    void updateGameData(void);
    
    std::vector<CCPoint> _gridCells;
    
public:
	
	~GameManager();
	GameManager();
	    
	CC_SYNTHESIZE(CCArray *, _lines, Lines);
	//CC_SYNTHESIZE(int, _gameMode, GameMode);
    CC_SYNTHESIZE_READONLY(int, _cols, Columns);
    CC_SYNTHESIZE_READONLY(int, _rows, Rows);
    CC_SYNTHESIZE_READONLY(bool, _boosting, Boosting);
    
    
    //game data
    CC_SYNTHESIZE(int, _score, Score);
	CC_SYNTHESIZE_READONLY(int, _level, Level);
    CC_SYNTHESIZE_READONLY(int, _bestScore, BestScore);
	CC_SYNTHESIZE_READONLY(int, _boostNumber, BoostNumber);
    CC_SYNTHESIZE(int, _collectedStars, CollectedStars);
	CC_SYNTHESIZE(int, _totalCollectedStars, TotalCollectedStars);
	CC_SYNTHESIZE_READONLY(int, _totalStars, TotalStars);
    CC_SYNTHESIZE_READONLY(float, _lineDecrement, LineDecrement);
	CC_SYNTHESIZE_READONLY(float, _timeDecrement, TimeDecrement);
	CC_SYNTHESIZE(float, _lineEnergy, LineEnergy);
	CC_SYNTHESIZE_READONLY(float, _time, Time);

	static GameManager * sharedGameManager();
    
    void setBoosting (bool var);
    void setBestScore (int var);
	
	inline Line * lineFromPool(void) {
        Line * line = (Line *) _linesPool->objectAtIndex(_linesPoolIndex);
        _linesPoolIndex++;
        if (_linesPoolIndex == _linesPool->count()) _linesPoolIndex = 0;
        return line;
    }
    
	inline Star * starFromPool(int index) {
        Star * star = (Star *) _starsPool->objectAtIndex(index);
        return star;
    }
    
    inline void setLevel (int var) {
        _level = var;
        updateGameData();
    }
        
    CCPoint inline starPosition(int i) {
        return _gridCells[i];
    }
	
	void reset (void);
	void update (float dt);

	

    
};

#endif /* defined(__MoonHerder__GameManager__) */
