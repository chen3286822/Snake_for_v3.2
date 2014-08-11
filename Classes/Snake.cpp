#include "Snake.h"
#include "VisibleRect.h"
#include "MapLayer.h"
#include "Item.h"

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
	m_bCrossing = false;
	m_bUseDirection = true;
	return true;
}

void BodyRect::setMapIndex(cocos2d::Vec2 index)
{
	if (index.x < 0 || index.x >= MAPWIDTH || index.y < 0 || index.y >= MAPHEIGHT)
		return;
	m_mapIndex = index;
}

Vec2 BodyRect::moveDistance(eDirection dir)
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
}

Vec3 BodyRect::rotateArc(eDirection curDir, eDirection lastDir)
{
	if (curDir == eDir_Up)
	{
		if (lastDir == eDir_Left)
			return Vec3(0, 0, 90);
		else if (lastDir == eDir_Right)
			return Vec3(0, 0, -90);
	}
	else if (curDir == eDir_Down)
	{
		if (lastDir == eDir_Left)
			return Vec3(0, 0, -90);
		else if (lastDir == eDir_Right)
			return Vec3(0, 0, 90);
	}
	else if (curDir == eDir_Left)
	{
		if (lastDir == eDir_Up)
			return Vec3(0, 0, -90);
		else if (lastDir == eDir_Down)
			return Vec3(0, 0, 90);
	}
	else if (curDir == eDir_Right)
	{
		if (lastDir == eDir_Up)
			return Vec3(0, 0, 90);
		else if (lastDir == eDir_Down)
			return Vec3(0, 0, -90);
	}

	return Vec3();
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

	//set map block type
	for (auto bodyRect : m_lpBody)
	{
		auto index = bodyRect->getMapIndex();
		m_pSnakeMap->setGridType(index, eType_Snake);
	}
	return true;
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

void Snake::setWalkAction(BodyRect* bodyRect)
{
	auto moveAction = MoveBy::create(VisibleRect::getGridLength()/m_fMoveSpeed, BodyRect::moveDistance(bodyRect->getCurDirection()));
	auto doneAction = CallFunc::create(CC_CALLBACK_0(Snake::setNextDirection, this, bodyRect));
	auto sequenceAction = Sequence::create(moveAction, doneAction, NULL);
	bodyRect->runAction(sequenceAction);
	bodyRect->setMoving(true);
}

void Snake::setRotateAction(BodyRect* bodyRect)
{
	auto moveOffset = BodyRect::moveDistance(bodyRect->getCurDirection());
	auto arc = BodyRect::rotateArc(bodyRect->getCurDirection(), bodyRect->getLastDirection());
	auto rotateAction = RotateBy::create(VisibleRect::getGridLength() / m_fMoveSpeed, arc);
	auto moveAction = MoveBy::create(VisibleRect::getGridLength() / m_fMoveSpeed, moveOffset);
	auto spawnAction = Spawn::create(rotateAction, moveAction, NULL);
	auto doneAction = CallFunc::create(CC_CALLBACK_0(Snake::setNextDirection, this, bodyRect));
	auto sequenceAction = Sequence::create(spawnAction, doneAction, NULL);
	bodyRect->runAction(sequenceAction);
	bodyRect->setMoving(true);
}

void Snake::setAppearAction(BodyRect* bodyRect)
{
	bodyRect->setScale(0.1f);
	auto scaleAction = ScaleTo::create(VisibleRect::getGridLength() / m_fMoveSpeed, 1);
	auto doneAction = CallFunc::create(CC_CALLBACK_0(Snake::setNextDirection, this, bodyRect));
	auto sequenceAction = Sequence::create(scaleAction, doneAction, NULL);
	bodyRect->runAction(sequenceAction);
	bodyRect->setMoving(true);
}

void Snake::setFlipWalkAction(BodyRect* bodyRect)
{

}

void Snake::setFlipRotateAction(BodyRect* bodyRect)
{

}

void Snake::moveOneGrid(BodyRect* bodyRect)
{
	if (bodyRect->getCurDirection() == bodyRect->getLastDirection())
		setWalkAction(bodyRect);
	else
		setRotateAction(bodyRect);
}

void Snake::moveFlipOneGrid(BodyRect* bodyRect)
{
	if (bodyRect->getCurDirection() == bodyRect->getLastDirection())
		setFlipWalkAction(bodyRect);
	else
		setFlipRotateAction(bodyRect);
}

void Snake::crawl()
{
	//set body rects' current direction and last direction
	auto pPreviousRect = m_lpBody.rbegin();
	for (auto it = m_lpBody.rbegin(); it != m_lpBody.rend(); ++it)
	{
		//only the direction is used, means the rect has moved, then the direction can be set again
		pPreviousRect++;
		if ((*it)->getUseDirection())
		{
			(*it)->setLastDirection((*it)->getCurDirection());		
			if (pPreviousRect != m_lpBody.rend())
			{
				//the current direction comes from the previous body rect
				(*it)->setCurDirection((*pPreviousRect)->getCurDirection());
			}
		}
	}
	m_pHead->setLastDirection(m_pHead->getCurDirection());
	//set the head's current direction if the m_eNextDirection is set
	if (m_eNextDirection != eDir_None)
		m_pHead->setCurDirection(m_eNextDirection);

	//all the body rects' current direction are set, not used yet
	for (auto bodyRect : m_lpBody)
		bodyRect->setUseDirection(false);

	//do the move action	
	for (auto bodyRect : m_lpBody)
	{
		//find every body rect's destination and move type
		m_pSnakeMap->setDestinationOfBodyRect(bodyRect);

		//set move action
		auto bContinue = setAction(bodyRect);

		//if destination grid contains something special, it effects here
		effectDestination(bodyRect);

		//reset the snake map grid type
		resetGridType(bodyRect);

		if (!bContinue)
			break;

		//the direction has been used
		bodyRect->setUseDirection(true);
	}

	if (m_pToAdd)
	{
		//insert the rect to the list behind the head
		auto headPos = m_lpBody.begin();
		++headPos;
		m_lpBody.insert(headPos, m_pToAdd);

		m_pToAdd = nullptr;
	}
}

bool Snake::setAction(BodyRect* bodyRect)
{
	//move and flip to the destination grid if the body rect is crossing the border
	std::function<void()> moveToDes = CC_CALLBACK_0(Snake::moveOneGrid,this,bodyRect);
	if (bodyRect->getCrossing())
		moveToDes = CC_CALLBACK_0(Snake::moveFlipOneGrid, this, bodyRect);

	switch (bodyRect->getMoveType())
	{
	case eMoveType_None:
	{
						   moveToDes();
	}
		break;
	case eMoveType_Eat:
	{
		//only the head rect will run here
		//a new rect will be 'born' in the head's position
		m_pToAdd = BodyRect::create(SnakeBodyModel);
		this->addChild(m_pToAdd, 1);
		m_pToAdd->setPosition(bodyRect->getPosition());
		m_pToAdd->setMapIndex(bodyRect->getMapIndex());
		m_pToAdd->setDestinationIndex(bodyRect->getMapIndex());
		m_pToAdd->setCurDirection(bodyRect->getCurDirection());
		setAppearAction(m_pToAdd);

		//then the head will move to the destination.
		moveToDes();
	}
		return false;
	}
	return true;
}

void Snake::effectDestination(BodyRect* bodyRect)
{
	switch (m_pSnakeMap->getGridType(bodyRect->getDestinationIndex()))
	{
	case eType_Food:
	{
		//eat the food
		auto itemFactory = dynamic_cast<ItemFactory*>(m_pSnakeMap->getChildByTag(eID_ItemFactory));
		if (itemFactory)
		{
			itemFactory->eatFood();
		}
	}
		break;
	default:
		break;
	}
}

void Snake::resetGridType(BodyRect* bodyRect)
{
	auto moveType = bodyRect->getMoveType();
	if (moveType == eMoveType_Eat)
	{
		//just set the destination index to eType_Snake
		m_pSnakeMap->setGridType(bodyRect->getDestinationIndex(), eType_Snake);
	}
	else
	{
		//set the current grid and destination grid's grid type except the snake is dead
		m_pSnakeMap->setGridType(bodyRect->getDestinationIndex(), eType_Snake);
		m_pSnakeMap->setGridType(bodyRect->getMapIndex(), eType_Empty);
	}
}

void Snake::setNextDirection(BodyRect* bodyRect)
{
	if (bodyRect)
	{
		bodyRect->setMoving(false);
		//reset the map index
		bodyRect->setMapIndex(bodyRect->getDestinationIndex());
		//reset the position
		bodyRect->setPosition(bodyRect->getMapIndex()*VisibleRect::getGridLength() + VisibleRect::getVisibleRect().origin + VisibleRect::getHalfGridVec());
	}

	for (auto body : m_lpBody)
	{
		if (body->getMoving())
			return;
	}

	//call the crawl again
	crawl();
}