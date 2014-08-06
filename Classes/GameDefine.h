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

	//snake occupied
	eType_Snake,

	//there is food on the block
	eType_Food,

	//there is a score apple
	eType_Apple,

	//slow down star
	eType_Star,

	//speed up ball
	eType_Ball,

	//transfer door
	eType_Door,

	//the block is blocked
	eType_Blocked,

	//dead area, lightning by UFO
	eType_Dead,

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