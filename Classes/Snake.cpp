#include "Snake.h"
#include "VisibleRect.h"
#include "MapLayer.h"

USING_NS_CC;

BodyRect* BodyRect::create(const std::string &modelPath)
{
	if (modelPath.length() < 4)
		CCASSERT(false, "improper name specified when creating model");

	auto bodyRect = new BodyRect();
	if (bodyRect && bodyRect->initWithFile(modelPath))
	{
		bodyRect->autorelease();
		return bodyRect;
	}
	CC_SAFE_DELETE(bodyRect);
	return nullptr;
}

bool BodyRect::initWithFile(const std::string &path)
{
	m_pModel = Sprite3D::create(path);
	this->addChild(m_pModel, 1);

	m_eCurDirection = eDir_None;
	m_eLastDirection = eDir_None;
	m_bMoving = false;
	m_eMoveType = eMoveType_None;
	return true;
}

void BodyRect::setMapIndex(cocos2d::Vec2 index)
{
	if (index.x < 0 || index.x >= MAPWIDTH || index.y < 0 || index.y >= MAPHEIGHT)
		return;
	m_mapIndex = index;
}

Snake* Snake::create(SnakeMapLayer* snakeMap)
{
	if (!snakeMap)
		CCASSERT(false, "the map should not be null!");

	auto bodyRect = new Snake();
	if (bodyRect && bodyRect->initWithMap(snakeMap))
	{
		bodyRect->autorelease();
		return bodyRect;
	}
	CC_SAFE_DELETE(bodyRect);
	return nullptr;
}

bool Snake::initWithMap(SnakeMapLayer* snakeMap)
{
	if (!Node::init())
	{
		return false;
	}

	//the m_pSnakeMap should never be null or invalid if the snake exist
	m_pSnakeMap = snakeMap;
	m_lpBody.clear();

	auto visualRect = VisibleRect::getVisibleRect();
	auto gridLength = VisibleRect::getGridLength();

	//init the snake model	
	m_pHead = BodyRect::create(SnakeHeadModel);
	this->addChild(m_pHead, 1);
	m_lpBody.push_back(m_pHead);
	m_pHead->setPosition(VisibleRect::center() + Vec2(gridLength / 2, gridLength/2));
	m_pHead->setMapIndex((m_pHead->getPosition() - visualRect.origin - VisibleRect::getHalfGridVec()) / gridLength);

	auto body = BodyRect::create(SnakeBodyModel);
	this->addChild(body, 1);
	m_lpBody.push_back(body);
	body->setPosition(m_pHead->getPosition() + Vec2(0,-gridLength));
	body->setMapIndex(m_pHead->getMapIndex() + Vec2(0, -1));

	m_pTail = BodyRect::create(SnakeTailModel);
	this->addChild(m_pTail, 1);
	m_lpBody.push_back(m_pTail);
	m_pTail->setPosition(body->getPosition() + Vec2(0, -gridLength));
	m_pTail->setMapIndex(body->getMapIndex() + Vec2(0, -1));

	//init snake direction
	for (auto bodyRect : m_lpBody)
	{
		bodyRect->setCurDirection(eDir_Up);
	}
	m_eLastDir = eDir_Up;

	//init snake speed
	m_fMoveSpeed = 32.0f;

	return true;
}

void Snake::onEnter()
{
	Node::onEnter();

	//set map block type
	for (auto bodyRect : m_lpBody)
	{
		auto index = bodyRect->getMapIndex();
		m_pSnakeMap->setGridType(index, eType_Snake);
	}
}

void Snake::pauseAll()
{
	for (auto bodyRect : m_lpBody)
	{
		bodyRect->pause();
	}
	pause();
}

void Snake::resumeAll()
{
	for (auto bodyRect : m_lpBody)
	{
		bodyRect->resume();
	}
	resume();
}

void Snake::setDirection(eDirection dir)
{
	if (m_pHead)
	{
		auto curDir = m_pHead->getCurDirection();
		if ((curDir == eDir_Up && dir == eDir_Down) || (curDir == eDir_Down && dir == eDir_Up) ||
			(curDir == eDir_Right && dir == eDir_Left) || (curDir == eDir_Left && dir == eDir_Right))
			return;
		m_eNextDirection = dir;
	}
}

int Snake::getLength()
{
	return m_lpBody.size();
}

void Snake::setMoveAction(BodyRect* bodyRect)
{
	auto moveDistance = [=](eDirection dir, Vec2& indexOffset) -> Vec2
	{
		if (dir == eDir_Up)
		{
			indexOffset = Vec2(0, 1);
			return Vec2(0, VisibleRect::getGridLength());
		}
		else if (dir == eDir_Down)
		{
			indexOffset = Vec2(0, -1);
			return Vec2(0, -VisibleRect::getGridLength());
		}
		else if (dir == eDir_Left)
		{
			indexOffset = Vec2(-1, 0);
			return Vec2(-VisibleRect::getGridLength(), 0);
		}
		else if (dir == eDir_Right)
		{
			indexOffset = Vec2(1, 0);
			return Vec2(VisibleRect::getGridLength(), 0);
		}
		indexOffset = Vec2(0, 0);
		return Vec2(0, 0);
	};

	auto moveOffset = Vec2();
	auto moveAction = MoveBy::create(VisibleRect::getGridLength()/m_fMoveSpeed, moveDistance(bodyRect->getCurDirection(), moveOffset));
	auto doneAction = CallFunc::create(CC_CALLBACK_0(Snake::setNextDirection, this, bodyRect, bodyRect->getMapIndex() + moveOffset));
	auto sequenceAction = Sequence::create(moveAction, doneAction, NULL);
	bodyRect->runAction(sequenceAction);
	bodyRect->setMoving(true);
}

void Snake::setRotateAction(BodyRect* bodyRect, eDirection previousDir)
{
	auto rotateArc = [=](BodyRect* first, eDirection previousDir, Vec2& indexOffset) -> Vec3
	{
		auto fDir = first->getCurDirection();
		if (fDir == eDir_Up)
		{
			if (previousDir == eDir_Left)
			{
				indexOffset = Vec2(0, 1);
				return Vec3(0, 0, 90);
			}
			else if (previousDir == eDir_Right)
			{
				indexOffset = Vec2(0, 1);
				return Vec3(0, 0, -90);
			}
		}
		else if (fDir == eDir_Down)
		{
			if (previousDir == eDir_Left)
			{
				indexOffset = Vec2(0, -1);
				return Vec3(0, 0, -90);
			}
			else if (previousDir == eDir_Right)
			{
				indexOffset = Vec2(0, -1);
				return Vec3(0, 0, 90);
			}
		}
		else if (fDir == eDir_Left)
		{
			if (previousDir == eDir_Up)
			{
				indexOffset = Vec2(-1, 0);
				return Vec3(0, 0, -90);
			}
			else if (previousDir == eDir_Down)
			{
				indexOffset = Vec2(-1, 0);
				return Vec3(0, 0, 90);
			}
		}
		else if (fDir == eDir_Right)
		{
			if (previousDir == eDir_Up)
			{
				indexOffset = Vec2(1, 0);
				return Vec3(0, 0, 90);
			}
			else if (previousDir == eDir_Down)
			{
				indexOffset = Vec2(1, 0);
				return Vec3(0, 0, -90);
			}
		}

		return Vec3();
	};

	auto moveOffset = Vec2();
	auto arc = rotateArc(bodyRect, previousDir, moveOffset);
	auto rotateAction = RotateBy::create(VisibleRect::getGridLength() / m_fMoveSpeed, arc);
	auto moveAction = MoveBy::create(VisibleRect::getGridLength() / m_fMoveSpeed, moveOffset*VisibleRect::getGridLength());
	auto spawnAction = Spawn::create(rotateAction, moveAction, NULL);
	auto doneAction = CallFunc::create(CC_CALLBACK_0(Snake::setNextDirection, this, bodyRect, bodyRect->getMapIndex() + moveOffset));
	auto sequenceAction = Sequence::create(spawnAction, doneAction, NULL);
	bodyRect->runAction(sequenceAction);
	bodyRect->setMoving(true);
}

void Snake::crawl()
{
	//set body rects' current direction and last direction
	auto pPreviousRect = m_lpBody.rbegin();
	for (auto it = m_lpBody.rbegin(); it != m_lpBody.rend(); ++it)
	{
		(*it)->setLastDirection((*it)->getCurDirection());
		pPreviousRect++;
		if (pPreviousRect != m_lpBody.rend())
		{
			//the current direction comes from the previous body rect
			(*it)->setCurDirection((*pPreviousRect)->getCurDirection());
		}
	}
	m_pHead->setLastDirection(m_pHead->getCurDirection());
	//set the head's current direction if the m_eNextDirection is set
	if (m_eNextDirection != eDir_None)
		m_pHead->setCurDirection(m_eNextDirection);

	//do the move action	
	for (auto bodyRect : m_lpBody)
	{
		//find every body rect's destination and move type
		m_pSnakeMap->setDestinationOfBodyRect(bodyRect);

		//set move action
		switch (bodyRect->getMoveType())
		{
		case eMoveType_None:
			break;
		}

		//if destination grid contains something special, it effects here
		switch (m_pSnakeMap->getGridType(bodyRect->getDestinationIndex()))
		{
		case eType_Food:
			break;
		default:
			break;
		}

		//set the current grid and destination grid's grid type except the snake is dead
		m_pSnakeMap->setGridType(bodyRect->getDestinationIndex(), eType_Snake);
		m_pSnakeMap->setGridType(bodyRect->getMapIndex(), eType_Empty);
	}

	/*
	std::list<BodyRect*>::iterator next = m_lpBody.begin();
	for (auto it = m_lpBody.begin(); it != m_lpBody.end(); ++it)
	{
		//compare with the next direction to decide the next action
		next++;
		if (next != m_lpBody.end())
		{
			//they are the same, so body rect just move across the original direction
			if ((*next)->getCurDirection() == (*it)->getCurDirection())
			{
				setMoveAction(*it);
			}
			//the body rect will turn over
			else
			{
				setRotateAction(*it, (*next)->getCurDirection());
			}
		}
		//the tail should compare with the m_eLastDir
		else
		{
			if (m_eLastDir == (*it)->getCurDirection())
			{
				setMoveAction(*it);
			}
			else
			{
				setRotateAction(*it, m_eLastDir);
			}
		}
	}
	*/
}

void Snake::setNextDirection(BodyRect* bodyRect, cocos2d::Vec2 newMapIndex)
{
	if (bodyRect)
	{
		if (bodyRect == m_pTail)
			m_tailLastMapIndex = bodyRect->getMapIndex();

		bodyRect->setMoving(false);
		//reset the map index
		bodyRect->setMapIndex(newMapIndex);
		//reset the position
		bodyRect->setPosition(newMapIndex*VisibleRect::getGridLength() + VisibleRect::getVisibleRect().origin + VisibleRect::getHalfGridVec());
	}

	for (auto body : m_lpBody)
	{
		if (body->getMoving())
			return;
	}

	//reset map block type
	m_pSnakeMap->setGridType(m_pHead->getMapIndex(), eType_Snake);
	m_pSnakeMap->setGridType(m_tailLastMapIndex, eType_Empty);

	//save the last direction
	std::list<BodyRect*>::reverse_iterator previous = m_lpBody.rbegin();
	if (previous != m_lpBody.rend())
		m_eLastDir = (*previous)->getCurDirection();

	//set every body rect's direction with the previous one's
	for (auto it = m_lpBody.rbegin(); it != m_lpBody.rend();++it)
	{
		previous++;
		if (previous != m_lpBody.rend())
		{
			(*it)->setCurDirection((*previous)->getCurDirection());
		}
	}

	//the head rect remains the same if m_eNextDirection not set
	if (m_eNextDirection != eDir_None)
	{
		m_pHead->setCurDirection(m_eNextDirection);
	}

	//detect next grid
	//call the crawl again
	crawl();
}