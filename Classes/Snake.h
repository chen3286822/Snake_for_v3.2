#ifndef __SNAKE_H__
#define __SNAKE_H__

#include "cocos2d.h"
#include "GameDefine.h"

class BodyRect : public cocos2d::Node
{
public:
	static BodyRect* create(const std::string &modelPath);
	virtual bool initWithFile(const std::string &path);

	//current direction
	CC_SYNTHESIZE(eDirection, m_eCurDirection, CurDirection);

	//last move direction
	CC_SYNTHESIZE(eDirection, m_eLastDirection, LastDirection);

	//the new direction after transfer a door or cross a border
	CC_SYNTHESIZE(eDirection, m_eTransferDirection, TransferDirection);

	//move destination
	CC_SYNTHESIZE(cocos2d::Vec2, m_DestinationIndex, DestinationIndex);

	//is moving
	CC_SYNTHESIZE(bool, m_bMoving, Moving);

	//move type from current position to the destination
	CC_SYNTHESIZE(eMoveType, m_eMoveType, MoveType);

	//is cross the border
	CC_SYNTHESIZE(bool, m_bCrossing, Crossing);

	//the current direction is set, but not moved because of the head eat food, so next time the rect should not set the direction again
	CC_SYNTHESIZE(bool, m_bUseDirection, UseDirection);

	//the index in the snake map
	void setMapIndex(cocos2d::Vec2 index);
	cocos2d::Vec2 getMapIndex(){ return m_mapIndex; }

	//set the rect's position, just for the action call, avoid the std::bind's compile error: cannot bind overload function
	void setRectPosition(cocos2d::Vec2 position);

	//some helpful function
	static cocos2d::Vec2 moveDistance(eDirection dir);
	static cocos2d::Vec3 rotateArc(eDirection curDir, eDirection lastDir);
private:
	cocos2d::Sprite3D* m_pModel{ nullptr };
	cocos2d::Vec2 m_mapIndex;
};

class SnakeMapLayer;
class Snake : public cocos2d::Node
{
public:
	static Snake* create(SnakeMapLayer* snakeMap);
	bool initWithMap(SnakeMapLayer* snakeMap);

	//pause all its body rects' actions and its own
	void pauseAll();

	//resume all its body rects' actions and its own
	void resumeAll();

	//set the snake's crawling direction
	void setDirection(eDirection dir);

	//the snake will add a body rect just behind the head
	void grow();

	//make the snake crawls
	void crawl();

	//the number of body rect, include head and tail
	int getLength();

private:
	SnakeMapLayer* m_pSnakeMap{ nullptr };
	BodyRect* m_pHead{ nullptr };
	BodyRect* m_pTail{ nullptr };
	std::list<BodyRect*> m_lpBody;

	//save a new body rect to be added to the list
	BodyRect* m_pToAdd{ nullptr };

	//control the speed of snake
	float m_fMoveSpeed;

	//save previous step the tail's direction
	eDirection m_eLastDir{ eDir_None };	

	//next direction set by player
	eDirection m_eNextDirection{ eDir_None };	

	//according to the m_eNextDirection value is set or not, decide next direction
	void setNextDirection(BodyRect* bodyRect);

	void setWalkAction(BodyRect* bodyRect);
	void setRotateAction(BodyRect* bodyRect);
	void setAppearAction(BodyRect* bodyRect);
	//sprite 3D do not support fade action yet...
	void setFadeWalkAction(BodyRect* bodyRect);
	void setFadeRotateAction(BodyRect* bodyRect);
	void setScaleWalkAction(BodyRect* bodyRect);
	void setScaleRotateAction(BodyRect* bodyRect);
	void moveOneGrid(BodyRect* bodyRect);
	void moveFadeOneGrid(BodyRect* bodyRect);
	void moveScaleOneGrid(BodyRect* bodyRect);

	//set move action by eMoveType, if return false, rest rects' actions will be stopped
	bool setAction(BodyRect* bodyRect);

	//make the destination items effect, like play effect, make the snake speed up
	void effectDestination(BodyRect* bodyRect);

	//reset the map grid type
	void resetGridType(BodyRect* bodyRect);
};

#endif