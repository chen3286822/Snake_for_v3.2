#include "Snake.h"
#include "VisibleRect.h"

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
	return true;
}

void BodyRect::setDirection(eDirection direction)
{
	m_eDirection = direction;
}

void BodyRect::setMapIndex(cocos2d::Vec2 index)
{
	if (index.x < 0 || index.x >= MAPWIDTH || index.y < 0 || index.y >= MAPHEIGHT)
		return;
	m_mapIndex = index;
}

bool Snake::init()
{
	if (!Node::init())
	{
		return false;
	}

	m_lpBody.clear();

	auto visualRect = VisibleRect::getVisibleRect();
	auto gridLength = VisibleRect::getGridLength();

	//init the snake model
	std::string fileName = "models/box.c3b";
	
	m_pHead = BodyRect::create(fileName);
	this->addChild(m_pHead, 1);
	m_lpBody.push_back(m_pHead);
	m_pHead->setPosition(VisibleRect::center() + Vec2(gridLength / 2, gridLength/2));
	m_pHead->setMapIndex((m_pHead->getPosition() - visualRect.origin )/ gridLength);

	auto body = BodyRect::create(fileName);
	this->addChild(body, 1);
	m_lpBody.push_back(body);
	body->setPosition(m_pHead->getPosition() + Vec2(gridLength,0));
	body->setMapIndex(m_pHead->getMapIndex() + Vec2(1, 0));

	m_pTail = BodyRect::create(fileName);
	this->addChild(m_pTail, 1);
	m_lpBody.push_back(m_pTail);
	m_pTail->setPosition(body->getPosition() + Vec2(gridLength, 0));
	m_pTail->setMapIndex(body->getMapIndex() + Vec2(1, 0));

	//init snake direction, content size
	for (auto bodyRect : m_lpBody)
	{
		bodyRect->ignoreAnchorPointForPosition(true);
		bodyRect->setContentSize(Size(VisibleRect::getGridLength(), VisibleRect::getGridLength()));
		bodyRect->setDirection(eDir_Left);
	}
	m_eLastDir = eDir_Left;

	crawl();
	return true;
}

void Snake::setDirection(eDirection dir)
{
	if (m_pHead)
	{
		auto curDir = m_pHead->getDirection();
		if ((curDir == eDir_Up && dir == eDir_Down) || (curDir == eDir_Down && dir == eDir_Up) ||
			(curDir == eDir_Right && dir == eDir_Left) || (curDir == eDir_Left && dir == eDir_Right))
			return;
		m_eNextDirection = dir;
	}
}

void Snake::setNextAction(BodyAction action, BodyRect* bodyRect, eDirection previousDir)
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

	auto rotateArc = [&](BodyRect* first, eDirection previousDir) -> Vec3
	{
		auto fDir = first->getDirection();
		if (fDir == eDir_Up)
		{
			if (previousDir == eDir_Left)
			{
				first->setAnchorPoint(Vec2(0, 0));
				return Vec3(0, 0, -90);
			}
			else if (previousDir == eDir_Right)
			{
				first->setAnchorPoint(Vec2(1, 0));
				return Vec3(0, 0, 90);
			}
		}
		else if (fDir == eDir_Down)
		{
			if (previousDir == eDir_Left)
			{
				first->setAnchorPoint(Vec2(0.5, -0.5));
				return Vec3(0, 0, -90);
			}
			else if (previousDir == eDir_Right)
			{
				first->setAnchorPoint(Vec2(1, 1));
				return Vec3(0, 0, -90);
			}
		}
		else if (fDir == eDir_Left)
		{
			if (previousDir == eDir_Up)
			{
				first->setAnchorPoint(Vec2(1, 1));
				return Vec3(0, 0, 90);
			}
			else if (previousDir == eDir_Down)
			{
				first->setAnchorPoint(Vec2(0.5, -0.5));
				return Vec3(0, 0, -90);
			}
		}
		else if (fDir == eDir_Right)
		{
			if (previousDir == eDir_Up)
			{
				first->setAnchorPoint(Vec2(0, 1));
				return Vec3(0, 0, -90);
			}
			else if (previousDir == eDir_Down)
			{
				first->setAnchorPoint(Vec2(0, 0));
				return Vec3(0, 0, 90);
			}
		}

		return Vec3();
	};

	if (action == BodyAction::eBodyAction_Move)
	{
		auto moveOffset = Vec2();
		auto moveAction = MoveBy::create(5.0f, moveDistance(bodyRect->getDirection(), moveOffset));
		auto doneAction = CallFunc::create(CC_CALLBACK_0(Snake::setNextDirection, this, bodyRect, bodyRect->getMapIndex() + moveOffset));
		auto sequenceAction = Sequence::create(moveAction, doneAction, NULL);
		bodyRect->runAction(sequenceAction);
		bodyRect->setMoving(true);
	}
	else if (action == BodyAction::eBodyAction_Rotate)
	{
		auto arc = rotateArc(bodyRect, previousDir);
		auto rotateAction = RotateBy::create(5.0f, arc);
		auto doneAction = CallFunc::create(CC_CALLBACK_0(Snake::setNextDirection, this, bodyRect));
		auto sequenceAction = Sequence::create(rotateAction, doneAction, NULL);
		bodyRect->runAction(sequenceAction);
		bodyRect->setMoving(true);
	}
}

void Snake::crawl()
{
	std::list<BodyRect*>::iterator next = m_lpBody.begin();
	for (auto it = m_lpBody.begin(); it != m_lpBody.end(); ++it)
	{
		//compare with the next direction to decide the next action
		next++;
		if (next != m_lpBody.end())
		{
			//they are the same, so body rect just move across the original direction
			if ((*next)->getDirection() == (*it)->getDirection())
			{
				setNextAction(BodyAction::eBodyAction_Move, *it);
			}
			//the body rect will turn over
			else
			{
				setNextAction(BodyAction::eBodyAction_Rotate, *it, (*next)->getDirection());
			}
		}
		//the tail should compare with the m_eLastDir
		else
		{
			if (m_eLastDir == (*it)->getDirection())
			{
				setNextAction(BodyAction::eBodyAction_Move, *it);
			}
			else
			{
				setNextAction(BodyAction::eBodyAction_Rotate, *it, m_eLastDir);
			}
		}
	}
}

void Snake::setNextDirection(BodyRect* bodyRect, cocos2d::Vec2 newMapIndex)
{
	if (bodyRect)
	{
		bodyRect->setMoving(false);
		//reset the position
		bodyRect->setPosition(newMapIndex*VisibleRect::getGridLength() + VisibleRect::getVisibleRect().origin);
		//reset the anchor
		bodyRect->setAnchorPoint(Vec2(0,0));
	}

	for (auto body : m_lpBody)
	{
		if (body->isMoving())
			return;
	}


	//save the last direction
	std::list<BodyRect*>::reverse_iterator previous = m_lpBody.rbegin();
	if (previous != m_lpBody.rend())
		m_eLastDir = (*previous)->getDirection();

	//set every body rect's direction with the previous one's
	for (auto it = m_lpBody.rbegin(); it != m_lpBody.rend();++it)
	{
		previous++;
		if (previous != m_lpBody.rend())
		{
			(*it)->setDirection((*previous)->getDirection());
		}
	}

	//the head rect remains the same if m_eNextDirection not set
	if (m_eNextDirection != eDir_None)
	{
		m_pHead->setDirection(m_eNextDirection);
	}

	//call the crawl again
	crawl();
}