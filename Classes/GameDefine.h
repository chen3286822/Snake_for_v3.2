#ifndef __GAMEDEFINE_H__
#define __GAMEDEFINE_H__

#include "VisibleRect.h"

enum eDirection
{
	eDir_None,
	eDir_Up,
	eDir_Down,
	eDir_Left,
	eDir_Right,
};

//get a random direction
eDirection randomDirection();
//get the rotate arc according to the direction
float arcByDirection(eDirection dir);
//get the opposited direction
eDirection oppositeDirection(eDirection dir);

enum eID
{
	eID_All = -1,
	eID_SnakeMap,
	eID_MainLayer,
	eID_SelectLevelLayer,
	eID_SelectLevelBg,
	eID_LevelView,
	eID_LevelViewItemAction1,
	eID_LevelViewItemAction2,
	eID_LevelViewItemAction3,
	eID_Snake,
	eID_Food,
	eID_Door1,
	eID_Door2,
	eID_Apple,
	eID_Star,
	eID_Ball,
	eID_ItemFactory,
	eID_ScoreLabel,
	//debug
	eID_SpeedLabel,
	eID_ScoreRateLabel,
};

//the grid type
enum eType
{
	//wrong type, should not be set
	eType_None,

	//snake occupied
	eType_Snake,

	//there is food on the grid
	eType_Food,

	//there is a score apple
	eType_Apple,

	//slow down star
	eType_Star,

	//speed up ball
	eType_Ball,

	//transfer door
	eType_Door,

	//the grid is blocked
	eType_Blocked,

	//dead area, lightning by UFO
	eType_Dead,

	//the grid is empty, the snake can crawl over
	eType_Empty
};

//the snake body rects' move type from current position to the destination
enum eMoveType
{
	//just move normally
	eMoveType_None,

	//transfer to the destination, because of a transfer door
	eMoveType_Transfer,

	//the snake will be dead
	eMoveType_Dead,

	//the snake eat food and grow
	eMoveType_Eat
};

enum eMoveAction
{
	eMoveAction_Straight,

	eMoveAction_Turn,
};

enum eFiniteState
{
	eFiniteState_None,
	eFiniteState_Speed,
	eFiniteState_Score,
};

struct FiniteState
{
	FiniteState()
	{
		m_nType = eFiniteState_None;
		m_fVar = 0;
		m_fLeftTime = 0;
	}
	eFiniteState m_nType;
	float m_fVar;
	float m_fLeftTime;
};

enum eResult
{ 
	// go on playing
	eResult_None,

	// complete the level
	eResult_Success,

	// failed
	eResult_Fail,
};

enum eVictoryType
{
	// not set
	eVictoryType_None,

	// get specific score before time out
	eVictoryType_TimeScore,

	// get specific score
	eVictoryType_Score,

	// grow to the specific length before time out
	eVictoryType_TimeLength,

	// grow to the specific length
	eVictoryType_Length,
};

struct VictoryCondition
{
	eVictoryType m_eType;
	float m_fTime;
	int m_nScore;
	int m_nLength;
	void clear()
	{
		m_eType = eVictoryType_None;
		m_fTime = 0;
		m_nLength = 0;
		m_nScore = 0;
	}
	VictoryCondition()
	{
		clear();
	}
};

#define MAPWIDTH 30
#define MAPHEIGHT 20

#define SnakeHeadModel "models/box.c3b"
#define SnakeBodyModel "models/box.c3b"
#define SnakeTailModel "models/box.c3b"
#define FoodModel "models/box.c3b"
#define DoorModel "models/door.c3b"
#define AppleModel "models/box2.c3b"
#define StarModel "models/box2.c3b"
#define BallModel "models/box2.c3b"
#define BlockModel "models/box2.c3b"

#endif