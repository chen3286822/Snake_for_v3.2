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

//get a random direction
eDirection randomDirection();
//get the rotate arc according to the direction
float arcByDirection(eDirection dir);
//get the opposited direction
eDirection oppositeDirection(eDirection dir);

enum eID
{
	eID_SnakeMap,
	eID_Snake,
	eID_Food,
	eID_Door1,
	eID_Door2,
	eID_ItemFactory,
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

#define MAPWIDTH 30
#define MAPHEIGHT 20

#define SnakeHeadModel "models/box.c3b"
#define SnakeBodyModel "models/box.c3b"
#define SnakeTailModel "models/box.c3b"
#define FoodModel "models/box.c3b"
#define DoorModel "models/door.c3b"

#endif