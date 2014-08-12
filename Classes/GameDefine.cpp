#include "GameDefine.h"
#include "cocos2d.h"

eDirection randomDirection()
{
	auto index = (int)(rand() % 4);
	cocos2d::log("%d", index);
	if (index == 0)
		return eDir_Up;
	else if (index == 1)
		return eDir_Right;
	else if (index == 2)
		return eDir_Down;
	else
		return eDir_Left;
}

float arcByDirection(eDirection dir)
{
	auto arc = 0.0f;
	if (dir == eDir_Right)
		arc = 90.0f;
	else if (dir == eDir_Down)
		arc = 180.0f;
	else if (dir == eDir_Left)
		arc = 270.0f;
	return arc;
}

eDirection oppositeDirection(eDirection dir)
{
	if (dir == eDir_Up)
		return eDir_Down;
	else if (dir == eDir_Down)
		return eDir_Up;
	else if (dir == eDir_Left)
		return eDir_Right;
	else if (dir == eDir_Right)
		return eDir_Left;
	return eDir_None;
}