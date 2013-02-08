//
//  Line.h
//  MoonHerder
//
//  Created by Roger Engelbert on 2/1/13.
//
//

#ifndef __GameConfig__
#define __GameConfig__

#include "cocos2d.h"
#include "GameConfig.h"
#include "SimpleAudioEngine.h"


using namespace cocos2d;
using namespace CocosDenshion;

enum {
    kGameStatePlay,
    kGameStateNewLevel,
    kGameStateGameOver,
    kGameStateMenu
};

enum {
    kBackground,
    kMiddleground,
    kForeground
};

enum {
	STAR_SIZE_1 = 1,
	STAR_SIZE_2,
	STAR_SIZE_3,
	STAR_SIZE_4
};

enum {
	HORIZONTAL,
	VERTICAL
};


#define DT_RATE 10
#define TILE 16
#define GRAVITY 0.2f
#define TERMINAL_VELOCITY 30.0
#define LINE_TIME_ACTIVE 36
#define LINE_TIME_CURVING 0.85
#define LINE_BLINK_INTERVAL 1.0
#define LINE_CURVE_INTERVAL 1.5
#define LINE_CURVE_AMOUNT 40
#define LINE_BOUNCE 0.2
#define LINE_THICKNESS 6.0
#define BAR_THICKNESS 2.0
#define STARS_IN_POOL 450


#endif /* defined(__GameConfig__) */
