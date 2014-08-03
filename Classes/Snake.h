#ifndef __SNAKE_H__
#define __SNAKE_H__

#include "cocos2d.h"
#include "GameDefine.h"

class BodyRect : public cocos2d::Node
{
public:
	static BodyRect* create(const std::string &modelPath);
	virtual bool initWithFile(const std::string &path);

	void setDirection(eDirection direction);
	eDirection getDirection(){ return m_eDirection; }
	void setMoving(bool bMove){ m_bMoving = bMove; }
	bool isMoving(){return m_bMoving; }
private:
	cocos2d::Sprite3D* m_pModel{ nullptr };
	eDirection m_eDirection{ eDir_None };
	bool m_bMoving{ false };
};

class Snake : public cocos2d::Node
{
public:
	CREATE_FUNC(Snake);
	virtual bool init() override;

	void setDirection(eDirection dir);
	void grow();

private:
	BodyRect* m_pHead{ nullptr };
	BodyRect* m_pTail{ nullptr };
	std::list<BodyRect*> m_lpBody;
	eDirection m_eLastDir{ eDir_None };		//save previous step the tail's direction
	eDirection m_eNextDirection{ eDir_None };	//next direction set by player

	void crawl();
	void setNextDirection(BodyRect* bodyRect);	//according the m_eNextDirection value is set or not, decide next direction
};

#endif