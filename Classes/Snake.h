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

	//move destination
	CC_SYNTHESIZE(cocos2d::Vec2, m_DestinationIndex, DestinationIndex);

	//is moving
	CC_SYNTHESIZE(bool, m_bMoving, Moving);

	//move type from current position to the destination
	CC_SYNTHESIZE(eMoveType, m_eMoveType, MoveType);

	//the index in the snake map
	void setMapIndex(cocos2d::Vec2 index);
	cocos2d::Vec2 getMapIndex(){ return m_mapIndex; }

private:
	cocos2d::Sprite3D* m_pModel{ nullptr };
	cocos2d::Vec2 m_mapIndex;
};

class SnakeMapLayer;
class Snake : public cocos2d::Node
{
public:
	static Snake* create(SnakeMapLayer* snakeMap);
	virtual bool initWithMap(SnakeMapLayer* snakeMap);
	virtual void onEnter();

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
	float m_fMoveSpeed;

	//save previous step the tail's direction
	eDirection m_eLastDir{ eDir_None };	

	//next direction set by player
	eDirection m_eNextDirection{ eDir_None };	

	//save tail's map index of last move
	cocos2d::Vec2 m_tailLastMapIndex;

	//according to the m_eNextDirection value is set or not, decide next direction
	void setNextDirection(BodyRect* bodyRect, cocos2d::Vec2 newMapIndex);

	void setMoveAction(BodyRect* bodyRect);
	void setRotateAction(BodyRect* bodyRect, eDirection previousDir = eDir_None);
};

#endif