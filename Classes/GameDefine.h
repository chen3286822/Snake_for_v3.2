#ifndef __GAMEDEFINE_H__
#define __GAMEDEFINE_H__

enum eDirection
{
	eDir_None,
	eDir_Up,
	eDir_Down,
	eDir_Left,
	eDir_Right,
};

enum eID
{
	eID_SnakeMap,
	eID_Snake,
	eID_Food,
};

enum eType
{
	//wrong type, should not be set
	eType_None,

	//there is food on the block
	eType_Food,

	//there is a score item
	eType_Score,

	//the block is blocked, maybe it's a wall or a body rect
	eType_Stop,

	//the block is empty, the snake can crawl over
	eType_Empty
};

#define MAPWIDTH 30
#define MAPHEIGHT 20

#define SnakeHeadModel "models/box.c3b"
#define SnakeBodyModel "models/box.c3b"
#define SnakeTailModel "models/box.c3b"
#define FoodModel "models/box.c3b"

#endif