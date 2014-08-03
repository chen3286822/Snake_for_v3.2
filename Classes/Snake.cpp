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
	m_pModel->setContentSize(Size(VisibleRect::getGridLength(), VisibleRect::getGridLength()));
	return true;
}

void BodyRect::setDirection(eDirection direction)
{
	m_eDirection = direction;
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

	auto body = BodyRect::create(fileName);
	this->addChild(body, 1);
	m_lpBody.push_back(body);
	body->setPosition(m_pHead->getPosition() + Vec2(gridLength,0));

	m_pTail = BodyRect::create(fileName);
	this->addChild(m_pTail, 1);
	m_lpBody.push_back(m_pTail);
	m_pTail->setPosition(body->getPosition() + Vec2(gridLength, 0));

	//init snake direction
	for (auto bodyRect : m_lpBody)
		bodyRect->setDirection(eDir_Left);
	m_eLastDir = eDir_Left;

	crawl();
	return true;
}

void Snake::crawl()
{
	auto moveDistance = [=](eDirection dir) -> Vec2
	{
		if (dir == eDir_Up)
			return Vec2(0, VisibleRect::getGridLength());
		else if (dir == eDir_Down)
			return Vec2(0, -VisibleRect::getGridLength());
		else if (dir == eDir_Left)
			return Vec2(-VisibleRect::getGridLength(), 0);
		else if (dir == eDir_Right)
			return Vec2(VisibleRect::getGridLength(), 0);

		return Vec2(0, 0);
	};

	auto rotateArc = [&](BodyRect* first, eDirection nextDir) -> Vec3
	{
		auto fDir = first->getDirection();
		if (fDir == eDir_Up)
		{
			if (nextDir == eDir_Left)
			{
				first->setAnchorPoint(Vec2(0, 0));
				return Vec3(0, 0, -90);
			}
			else if (nextDir == eDir_Right)
			{
				first->setAnchorPoint(Vec2(1, 0));
				return Vec3(0, 0, 90);
			}
		}
		else if (fDir == eDir_Down)
		{
			if (nextDir == eDir_Left)
			{
				first->setAnchorPoint(Vec2(0, 1));
				return Vec3(0, 0, 90);
			}
			else if (nextDir == eDir_Right)
			{
				first->setAnchorPoint(Vec2(1, 1));
				return Vec3(0, 0, -90);
			}
		}
		else if (fDir == eDir_Left)
		{
			if (nextDir == eDir_Up)
			{
				first->setAnchorPoint(Vec2(1, 1));
				return Vec3(0, 0, 90);
			}
			else if (nextDir == eDir_Down)
			{
				first->setAnchorPoint(Vec2(1, 0));
				return Vec3(0, 0, -90);
			}
		}
		else if (fDir == eDir_Right)
		{
			if (nextDir == eDir_Up)
			{
				first->setAnchorPoint(Vec2(0, 1));
				return Vec3(0, 0, -90);
			}
			else if (nextDir == eDir_Down)
			{
				first->setAnchorPoint(Vec2(0, 0));
				return Vec3(0, 0, 90);
			}
		}

		return Vec3();
	};

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
				auto moveAction = MoveTo::create(0.5f, (*it)->getPosition() + moveDistance((*it)->getDirection()));
				auto doneAction = CallFunc::create(CC_CALLBACK_0(Snake::setNextDirection, this, *it));
				auto sequenceAction = Sequence::create(moveAction, doneAction, NULL);
				(*it)->runAction(sequenceAction);
				(*it)->setMoving(true);
			}
			//the body rect will turn over
			else
			{
				auto arc = rotateArc(*it, (*next)->getDirection());
				auto rotateAction = RotateBy::create(0.5f, arc);
				auto doneAction = CallFunc::create(CC_CALLBACK_0(Snake::setNextDirection, this, *it));
				auto sequenceAction = Sequence::create(rotateAction, doneAction, NULL);
				(*it)->runAction(sequenceAction);
				(*it)->setMoving(true);
			}
		}
		//the tail should compare with the m_eLastDir
		else
		{
			if (m_eLastDir == (*it)->getDirection())
			{
				auto moveAction = MoveTo::create(0.5f, (*it)->getPosition() + moveDistance((*it)->getDirection()));
				auto doneAction = CallFunc::create(CC_CALLBACK_0(Snake::setNextDirection, this, *it));
				auto sequenceAction = Sequence::create(moveAction, doneAction, NULL);
				(*it)->runAction(sequenceAction);
				(*it)->setMoving(true);
			}
			else
			{
				auto arc = rotateArc(*it, m_eLastDir);
				auto rotateAction = RotateBy::create(0.5f, arc);
				auto doneAction = CallFunc::create(CC_CALLBACK_0(Snake::setNextDirection, this, *it));
				auto sequenceAction = Sequence::create(rotateAction, doneAction, NULL);
				(*it)->runAction(sequenceAction);
				(*it)->setMoving(true);
			}
		}
	}
}

void Snake::setNextDirection(BodyRect* bodyRect)
{
	if (bodyRect)
	{
		bodyRect->setMoving(false);
		//reset the anchor
		bodyRect->setAnchorPoint(Vec2(0.5,0.5));
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