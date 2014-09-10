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

void BodyRect::setRectPosition(cocos2d::Vec2 position)
{
	this->setPosition(position*VisibleRect::getGridLength() + VisibleRect::getVisibleRect().origin + VisibleRect::getHalfGridVec());
}

Vec2 BodyRect::moveDistance(eDirection dir)
{
	if (dir == eDir_Up)
		return Vec2(0, 1);
	else if (dir == eDir_Down)
		return Vec2(0, -1);
	else if (dir == eDir_Left)
		return Vec2(-1, 0);
	else if (dir == eDir_Right)
		return Vec2(1, 0);
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
	m_mStates.clear();

	scheduleUpdate();

	auto visualRect = VisibleRect::getVisibleRect();
	auto gridLength = VisibleRect::getGridLength();

	//init the snake model	
	m_pHead = BodyRect::create(SnakeHeadModel);
	this->addChild(m_pHead, 1);
	m_lpBody.push_back(m_pHead);
	m_pHead->setPosition(VisibleRect::center() + Vec2(gridLength / 2, gridLength / 2));
	m_pHead->setMapIndex((m_pHead->getPosition() - visualRect.origin - VisibleRect::getHalfGridVec()) / gridLength);

	auto body = BodyRect::create(SnakeBodyModel);
	this->addChild(body, 1);
	m_lpBody.push_back(body);
	body->setPosition(m_pHead->getPosition() + Vec2(0, -gridLength));
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
	m_fMoveSpeed = 64.0f;

	//set map block type
	for (auto bodyRect : m_lpBody)
	{
		auto index = bodyRect->getMapIndex();
		m_pSnakeMap->setGridType(index, eType_Snake);
	}
	return true;
}

void Snake::update(float delta)
{
	for (auto state : m_mStates)
	{
		if (state.first != eFiniteState_None)
		{
			if (state.second.m_fLeftTime > 0)
			{
				state.second.m_fLeftTime -= delta;
			}
			else
			{
				state.second.m_fLeftTime = 0;
				switch (state.first)
				{
				case eFiniteState_Speed:
					m_fMoveSpeed -= state.second.m_fVar;
					break;
				case eFiniteState_Score:
					m_fStateScoreRate -= state.second.m_fVar;
					break;
				}
			}
		}
	}
	//remove all the invalid states
	for (std::map<int, FiniteState>::iterator mit = m_mStates.begin(); mit != m_mStates.end();)
	{
		if (mit->second.m_fLeftTime == 0)
			m_mStates.erase(mit);
		else
			mit++;
	}
}

void Snake::addState(FiniteState state)
{
	if (state.m_nType == eFiniteState_None || state.m_fLeftTime <= 0)
		return;

	std::map<int, FiniteState>::iterator mit = m_mStates.find(state.m_nType);
	if (mit != m_mStates.end())
	{
		// reset the time
		if (state.m_fLeftTime > mit->second.m_fLeftTime)
			mit->second.m_fLeftTime = state.m_fLeftTime;
	}
	else
	{
		//insert the state
		m_mStates[state.m_nType] = state;
		//modify some values
		switch (state.m_nType)
		{
		case eFiniteState_Speed:
			m_fMoveSpeed += state.m_fVar;
			break;
		case eFiniteState_Score:
			m_fStateScoreRate += state.m_fVar;
			break;
		}
	}
}

void Snake::addState(eFiniteState state, float var, float time)
{
	FiniteState finiteState;
	finiteState.m_nType = state;
	finiteState.m_fVar = var;
	finiteState.m_fLeftTime = time;
	addState(finiteState);
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

void Snake::setSpeed(float speed)
{
	m_fMoveSpeed = speed;
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
	auto moveAction = MoveBy::create(VisibleRect::getGridLength() / m_fMoveSpeed, VisibleRect::getGridLength() * BodyRect::moveDistance(bodyRect->getCurDirection()));
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
	auto moveAction = MoveBy::create(VisibleRect::getGridLength() / m_fMoveSpeed, VisibleRect::getGridLength() * moveOffset);
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

void Snake::setFadeWalkAction(BodyRect* bodyRect)
{
	auto actionTime = VisibleRect::getGridLength() / m_fMoveSpeed / 2;
	//move cross the border
	auto moveAction = MoveBy::create(actionTime, VisibleRect::getGridLength() * BodyRect::moveDistance(bodyRect->getCurDirection()));
	//destination move start
	auto startPos = BodyRect::moveDistance(oppositeDirection(bodyRect->getTransferDirection())) + bodyRect->getDestinationIndex();
	auto resetPosAction = CallFunc::create(CC_CALLBACK_0(BodyRect::setRectPosition, bodyRect, startPos));

	auto doneAction = CallFunc::create(CC_CALLBACK_0(Snake::setNextDirection, this, bodyRect));
	auto sequenceAction = Sequence::create(moveAction, resetPosAction, moveAction, doneAction, NULL);
	bodyRect->runAction(sequenceAction);
	bodyRect->setMoving(true);
}

void Snake::setFadeRotateAction(BodyRect* bodyRect)
{
	auto actionTime = VisibleRect::getGridLength() / m_fMoveSpeed / 2;
	auto arc = BodyRect::rotateArc(bodyRect->getCurDirection(), bodyRect->getLastDirection());
	//rotate cross the border
	auto rotateAction = RotateBy::create(actionTime, arc);
	auto moveAction = MoveBy::create(actionTime, VisibleRect::getGridLength() * BodyRect::moveDistance(bodyRect->getCurDirection()));
	//destination move start
	auto startPos = BodyRect::moveDistance(oppositeDirection(bodyRect->getTransferDirection())) + bodyRect->getDestinationIndex();
	auto resetPosAction = CallFunc::create(CC_CALLBACK_0(BodyRect::setRectPosition, bodyRect, startPos));

	auto spawnAction = Spawn::create(rotateAction, moveAction, NULL);
	auto doneAction = CallFunc::create(CC_CALLBACK_0(Snake::setNextDirection, this, bodyRect));
	auto sequenceAction = Sequence::create(spawnAction, resetPosAction, moveAction, doneAction, NULL);
	bodyRect->runAction(sequenceAction);
	bodyRect->setMoving(true);
}

void Snake::setScaleWalkAction(BodyRect* bodyRect)
{
	auto actionTime = VisibleRect::getGridLength() / m_fMoveSpeed / 2;
	//move into the door
	auto moveAction = MoveBy::create(actionTime, VisibleRect::getGridLength() * BodyRect::moveDistance(bodyRect->getCurDirection()));
	auto scaleSmallAction = ScaleTo::create(actionTime, 0.1f);
	auto spawnAction = Spawn::create(moveAction, scaleSmallAction, nullptr);
	//rotate to the transfer direction and set position to the destination
	auto arc = arcByDirection(bodyRect->getTransferDirection());
	auto resetRotationAction = CallFunc::create(CC_CALLBACK_0(BodyRect::setRotation, bodyRect, arc));
	auto resetPosAction = CallFunc::create(CC_CALLBACK_0(BodyRect::setRectPosition, bodyRect, bodyRect->getDestinationIndex()));
	//scale back
	auto scaleBigAction = ScaleTo::create(actionTime, 1.0f);
	auto doneAction = CallFunc::create(CC_CALLBACK_0(Snake::setNextDirection, this, bodyRect));
	auto sequenceAction = Sequence::create(spawnAction, resetRotationAction, resetPosAction, scaleBigAction, doneAction, NULL);
	bodyRect->runAction(sequenceAction);
	bodyRect->setMoving(true);
}

void Snake::setScaleRotateAction(BodyRect* bodyRect)
{
	auto actionTime = VisibleRect::getGridLength() / m_fMoveSpeed / 2;
	auto arcRotate = BodyRect::rotateArc(bodyRect->getCurDirection(), bodyRect->getLastDirection());
	auto rotateAction = RotateBy::create(actionTime, arcRotate);
	auto moveAction = MoveBy::create(actionTime, VisibleRect::getGridLength() * BodyRect::moveDistance(bodyRect->getCurDirection()));
	auto scaleSmallAction = ScaleTo::create(actionTime, 0.1f);
	auto spawnAction = Spawn::create(moveAction, scaleSmallAction, rotateAction, nullptr);
	//rotate to the transfer direction and set position to the destination
	auto arc = arcByDirection(bodyRect->getTransferDirection());
	auto resetRotationAction = CallFunc::create(CC_CALLBACK_0(BodyRect::setRotation, bodyRect, arc));
	auto resetPosAction = CallFunc::create(CC_CALLBACK_0(BodyRect::setRectPosition, bodyRect, bodyRect->getDestinationIndex()));
	//scale back
	auto scaleBigAction = ScaleTo::create(actionTime, 1.0f);
	auto doneAction = CallFunc::create(CC_CALLBACK_0(Snake::setNextDirection, this, bodyRect));
	auto sequenceAction = Sequence::create(spawnAction, resetRotationAction, resetPosAction, scaleBigAction, doneAction, NULL);
	bodyRect->runAction(sequenceAction);
	bodyRect->setMoving(true);
}

void Snake::moveOneGrid(BodyRect* bodyRect)
{
	if (bodyRect->getCurDirection() == bodyRect->getLastDirection())
		setWalkAction(bodyRect);
	else
		setRotateAction(bodyRect);
}

void Snake::moveFadeOneGrid(BodyRect* bodyRect)
{
	if (bodyRect->getCurDirection() == bodyRect->getLastDirection())
		setFadeWalkAction(bodyRect);
	else
		setFadeRotateAction(bodyRect);
}

void Snake::moveScaleOneGrid(BodyRect* bodyRect)
{
	if (bodyRect->getCurDirection() == bodyRect->getLastDirection())
		setScaleWalkAction(bodyRect);
	else
		setScaleRotateAction(bodyRect);
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
			//when a body rect stays in a door, the direction should be transfer direction
			if (m_pSnakeMap->isInDoor((*it)->getMapIndex()))
			{
				//set the rect's direction and last direction to the transfer direction
				(*it)->setCurDirection((*it)->getTransferDirection());
				(*it)->setLastDirection((*it)->getTransferDirection());
			}
			else
			{
				(*it)->setLastDirection((*it)->getCurDirection());
				if (pPreviousRect != m_lpBody.rend())
				{
					//the current direction comes from the previous body rect
					(*it)->setCurDirection((*pPreviousRect)->getCurDirection());
				}
			}
		}
	}
	//if head stay in the door, then the direction cannot be set
	if (!(m_pSnakeMap->isInDoor(m_pHead->getMapIndex())))
	{
		m_pHead->setLastDirection(m_pHead->getCurDirection());
		//set the head's current direction if the m_eNextDirection is set
		if (m_eNextDirection != eDir_None)
			m_pHead->setCurDirection(m_eNextDirection);
	}
	else
	{
		//cover the m_eNextDirection
		m_eNextDirection = m_pHead->getCurDirection();
	}

	//all the body rects' current direction are set, not used yet
	for (auto bodyRect : m_lpBody)
		bodyRect->setUseDirection(false);

	//do the move action	
	for (auto bodyRect : m_lpBody)
	{
		//find every body rect's destination and move type
		m_pSnakeMap->setDestinationOfBodyRect(bodyRect);

		//if destination grid contains something special, it effects here
		effectDestination(bodyRect);

		//set move action
		auto bContinue = setAction(bodyRect);

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
	std::function<void()> moveToDes = CC_CALLBACK_0(Snake::moveOneGrid, this, bodyRect);
	if (bodyRect->getCrossing())
	{
		moveToDes = CC_CALLBACK_0(Snake::moveFadeOneGrid, this, bodyRect);
		bodyRect->setCrossing(false);
	}

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
		//rotate the rect to fit the snake
		auto dir = bodyRect->getCurDirection();
		m_pToAdd->setRotation(arcByDirection(dir));

		setAppearAction(m_pToAdd);

		//then the head will move to the destination.
		moveToDes();
	}
		return false;
	case eMoveType_Transfer:
	{
		moveScaleOneGrid(bodyRect);
	}
		break;
	case eMoveType_Dead:
	{
		m_pSnakeMap->die();
	}
		return false;
	}
	return true;
}

void Snake::effectDestination(BodyRect* bodyRect)
{
	//let the item effect
	auto itemFactory = m_pSnakeMap->getItemFactory();
	if (itemFactory)
	{
		auto item = itemFactory->getItem(bodyRect->getDestinationIndex());
		if (item && item->getValid())
			item->effect(this);

		switch (m_pSnakeMap->getGridType(bodyRect->getDestinationIndex()))
		{
		case eType_Food:
		{
			//eat the food
			itemFactory->eatFood();
		}
			break;
		case eType_Apple:
		{
			// remove the apple
			itemFactory->eatApple();
		}
			break;
		case eType_Star:
		{
			// remove the star
			itemFactory->eatStar();
		}
			break;
		case eType_Ball:
		{
			// remove the ball
			itemFactory->eatBall();
		}
			break;
		default:
			break;
		}
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
	else if (moveType == eMoveType_Transfer)
	{
		//just set the original index to empty, destination index do not set to snake, remain to eType_Door
		m_pSnakeMap->setGridType(bodyRect->getMapIndex(), eType_Empty);
	}
	else
	{
		//set the current grid and destination grid's grid type except the snake is dead
		m_pSnakeMap->setGridType(bodyRect->getDestinationIndex(), eType_Snake);
		//if the body rect stays in the door, the grid cannot be set to empty
		if (!(m_pSnakeMap->isInDoor(bodyRect->getMapIndex())))
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