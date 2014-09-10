#include "Item.h"
#include "GameDefine.h"
#include "MapLayer.h"
#include "Snake.h"
#include "VisibleRect.h"
#include "UserRecord.h"

USING_NS_CC;

bool Food::init()
{
	if (!Node::init())
	{
		return false;
	}

	//set  the food model
	m_pModel = Sprite3D::create(FoodModel);
	this->addChild(m_pModel, 1);

	return true;
}

void Food::effect(Snake* snake)
{

}

bool Door::init()
{
	if (!Node::init())
	{
		return false;
	}

	//set  the food model
	m_pModel = Sprite3D::create(DoorModel);
	this->addChild(m_pModel, 1);

	m_eTransferDirection = eDir_None;
	m_otherDoor = nullptr;

	return true;
}

void Door::effect(Snake* snake)
{

}

bool Apple::init()
{
	if (!Node::init())
	{
		return false;
	}

	m_pModel = Sprite3D::create(AppleModel);
	this->addChild(m_pModel, 1);

	m_fDuration = 0.0f;
	m_bValid = true;
	return true;
}

void Apple::effect(Snake* snake)
{
	if (snake)
	{
		UserRecord::getInstance()->addScore(m_nBasicScore * snake->getScoreRate());
		setValid(false);
	}
}

bool Star::init()
{
	if (!Node::init())
	{
		return false;
	}

	m_pModel = Sprite3D::create(StarModel);
	this->addChild(m_pModel, 1);

	m_fDuration = 0.0f;
	m_bValid = true;
	return true;
}

void Star::effect(Snake* snake)
{	
	if (snake)
	{
		// speed up the snake
		snake->setSpeed(snake->getSpeed() + 16.0f);
		// set the score rate
		snake->addState(eFiniteState_Score, 1.0f, 30.0f);
		setValid(false);
	}
}

bool Ball::init()
{
	if (!Node::init())
	{
		return false;
	}

	m_pModel = Sprite3D::create(BallModel);
	this->addChild(m_pModel, 1);

	m_fDuration = 0.0f;
	m_bValid = true;
	return true;
}

void Ball::effect(Snake* snake)
{
	if (snake)
	{
		// set the speed rate
		snake->addState(eFiniteState_Speed, -16.0f, 20.0f);
		setValid(false);
	}
}

ItemFactory* ItemFactory::create(SnakeMapLayer* snakeMap)
{
	if (!snakeMap)
		CCASSERT(false, "the map should not be null!");

	auto factory = new ItemFactory();
	if (factory && factory->initWithMap(snakeMap))
	{
		factory->autorelease();
		return factory;
	}
	CC_SAFE_DELETE(factory);
	return nullptr;
}

bool ItemFactory::initWithMap(SnakeMapLayer* snakeMap)
{
	if (!Node::init())
	{
		return false;
	}

	m_pSnakeMap = snakeMap;

	//random time from [5, 15] seconds, after the time, an apple appears
	m_fTimeToAddApple = rand() % 11 + 5.0f;
	//random time from [0, 10] seconds, after the time, a star appears
	m_fTimeToAddStar = rand() % 11;
	//random time from [2, 11] seconds, after the time, a ball appears
	m_fTimeToAddBall = rand() % 10 + 2.0f;
	return true;
}

Item* ItemFactory::getItem(cocos2d::Vec2 index)
{
	int x = (int)index.x;
	int y = (int)index.y;
	if (x < 0 || y < 0 || x >= MAPWIDTH || y >= MAPHEIGHT)
		return nullptr;

	if (m_pFood && m_pFood->getIndex() == index)
		return m_pFood;
	else if (m_pDoors.first && m_pDoors.first->getIndex() == index)
		return m_pDoors.first;
	else if (m_pDoors.second && m_pDoors.second->getIndex() == index)
		return m_pDoors.second;
	else if (m_pApple && m_pApple->getIndex() == index)
		return m_pApple;
	else if (m_pStar && m_pStar->getIndex() == index)
		return m_pStar;
	else if (m_pBall && m_pBall->getIndex() == index)
		return m_pBall;

	return nullptr;
}

int ItemFactory::getItemsNumber()
{
	int number = 0;
	if (m_pFood)
		number++;
	if (m_pDoors.first && m_pDoors.second)
		number += 2;
	if (m_pApple)
		number++;
	if (m_pStar)
		number++;
	if (m_pBall)
		number++;

	return number;
}

void ItemFactory::produce(float dt)
{
	//produce food when necessary
	addFood();

	//add the transfer doors
	addDoor();

	//add the score apple
	addApple(dt);

	//add the speed star
	addStar(dt);

	//add the slow down ball
	addBall(dt);

	//check and remove expired item
	removeExpiredItem(dt);
}

void ItemFactory::removeExpiredItem(float dt)
{
	//remove all the item that is out of time
	if (m_pApple)
	{
		if (m_pApple->getDuration() <= 0 && !m_bRemovingApple)
			eatApple();
		else
		{
			m_pApple->setDuration(m_pApple->getDuration() - dt);
		}
	}
	if (m_pStar)
	{
		if (m_pStar->getDuration() <= 0 && !m_bRemovingStar)
			eatStar();
		else
			m_pStar->setDuration(m_pStar->getDuration() - dt);
	}
	if (m_pBall)
	{
		if (m_pBall->getDuration() <= 0 && !m_bRemovingBall)
			eatBall();
		else
			m_pBall->setDuration(m_pBall->getDuration() - dt);
	}
}

void ItemFactory::addFood()
{
	//check if the food still exist
	if (m_pFood)
		return;

	//check the empty grid
	Snake* snake = dynamic_cast<Snake*>(getParent()->getChildByTag(eID_Snake));
	if (!snake)
		return;
	int left = m_pSnakeMap->getMovableNumbers() - snake->getLength();

	//no empty grids left, you win
	if (left <= 0)
		return;

	//set the food index
	int index = (int)(rand() % left + 1);
	auto mapIndex = m_pSnakeMap->getEmptyGridIndex(index);

	//create food model
	m_pFood = Food::create();
	this->addChild(m_pFood, 1, eID_Food);
	m_pFood->setPosition(VisibleRect::getVisibleRect().origin + VisibleRect::getHalfGridVec() + VisibleRect::getGridLength()*mapIndex);
	m_pFood->setIndex(mapIndex);

	//set the grid type
	m_pSnakeMap->setGridType(mapIndex, eType_Food);
}

void ItemFactory::eatFood()
{
	//here we do not need to reset the map grid type, because it will be set later in Snake::resetGridType
	this->removeChildByTag(eID_Food);
	m_pFood = nullptr;
}

void ItemFactory::addDoor()
{
	//check if the doors exist
	if (m_pDoors.first != nullptr || m_pDoors.second != nullptr)
		return;

	//check the empty grid
	Snake* snake = m_pSnakeMap->getSnake();
	if (!snake)
		return;
	int left = m_pSnakeMap->getMovableNumbers() - snake->getLength();

	//no empty grids left
	if (left <= 0)
		return;

	//set the door index
	int index = (int)(rand() % left + 1);
	auto mapIndex = m_pSnakeMap->getEmptyGridIndex(index);

	//create door model
	m_pDoors.first = Door::create();
	this->addChild(m_pDoors.first, 1, eID_Door1);
	m_pDoors.first->setPosition(VisibleRect::getVisibleRect().origin + VisibleRect::getHalfGridVec() + VisibleRect::getGridLength()*mapIndex);
	m_pDoors.first->setIndex(mapIndex);

	//set the door transfer direction
	//auto transferDir = randomDirection();
	auto transferDir = eDir_Up;
	m_pDoors.first->setTransferDirection(transferDir);
	//actual door direction is opposite to transfer direction
	m_pDoors.first->setRotation(arcByDirection(oppositeDirection(transferDir)));

	//set the grid type
	m_pSnakeMap->setGridType(mapIndex, eType_Door);

	//create the second door
	index = (int)(rand() % left + 1);
	mapIndex = m_pSnakeMap->getEmptyGridIndex(index);
	m_pDoors.second = Door::create();
	this->addChild(m_pDoors.second, 1, eID_Door2);
	m_pDoors.second->setPosition(VisibleRect::getVisibleRect().origin + VisibleRect::getHalfGridVec() + VisibleRect::getGridLength()*mapIndex);
	m_pDoors.second->setIndex(mapIndex);

	//transferDir = randomDirection();
	transferDir = eDir_Right;
	m_pDoors.second->setTransferDirection(transferDir);
	m_pDoors.second->setRotation(arcByDirection(oppositeDirection(transferDir)));

	m_pSnakeMap->setGridType(mapIndex, eType_Door);

	//connect the two doors
	m_pDoors.first->setOtherDoor(m_pDoors.second);
	m_pDoors.second->setOtherDoor(m_pDoors.first);
}

void ItemFactory::removeDoor()
{
	//reset the grid type
	m_pSnakeMap->setGridType(m_pDoors.first->getIndex(), eType_Empty);
	m_pSnakeMap->setGridType(m_pDoors.second->getIndex(), eType_Empty);

	this->removeChildByTag(eID_Door1);
	m_pDoors.first = nullptr;
	this->removeChildByTag(eID_Door2);
	m_pDoors.second = nullptr;
}

Door* ItemFactory::getDoor(cocos2d::Vec2 index)
{
	if (m_pDoors.first && m_pDoors.first->getIndex() == index)
		return m_pDoors.first;
	else if (m_pDoors.second && m_pDoors.second->getIndex() == index)
		return m_pDoors.second;
	else
		return nullptr;
}

void ItemFactory::addApple(float dt)
{
	//check if the apple exist
	if (m_pApple)
		return;

	//check if the random time has finished
	if (m_fTimeToAddApple > 0)
	{
		m_fTimeToAddApple -= dt;
		return;
	}

	// it's time to add apple, check the empty grid
	Snake* snake = m_pSnakeMap->getSnake();
	if (!snake)
		return;
	int left = m_pSnakeMap->getMovableNumbers() - snake->getLength();

	//no empty grids left
	if (left <= 0)
		return;

	//set the apple index
	int index = (int)(rand() % left + 1);
	auto mapIndex = m_pSnakeMap->getEmptyGridIndex(index);

	//create apple model
	m_pApple = Apple::create();
	this->addChild(m_pApple, 1, eID_Apple);
	m_pApple->setPosition(VisibleRect::getVisibleRect().origin + VisibleRect::getHalfGridVec() + VisibleRect::getGridLength()*mapIndex);
	m_pApple->setIndex(mapIndex);

	//add a scale animation
	m_pApple->setScale(0.1f);
	auto scaleBig = ScaleTo::create(0.7f, 1.2f);
	auto scaleBack = ScaleTo::create(0.3f, 1.0f);
	auto sequence = Sequence::create(scaleBig, scaleBack, nullptr);
	m_pApple->runAction(sequence);

	//set the grid type
	m_pSnakeMap->setGridType(mapIndex, eType_Apple);

	//set the duration from [15, 25] seconds
	auto duration = rand() % 16 + 10.0f;
	m_pApple->setDuration(duration);
}

void ItemFactory::eatApple()
{
	//stop apple's actions first
	m_pApple->stopAllActions();

	// set the apple invalid
	m_pApple->setValid(false);
	// set the removing variable to true to avoid call this function again while removing the item
	m_bRemovingApple = true;

	// zoom out the apple
	auto scaleSmall = ScaleTo::create(0.5f, 0.1f);
	auto doneAction = CallFunc::create(CC_CALLBACK_0(ItemFactory::removeApple, this));
	auto sequenceAction = Sequence::create(scaleSmall, doneAction, nullptr);
	m_pApple->runAction(sequenceAction);
}

void ItemFactory::removeApple()
{
	//here we should reset the grid type because the item maybe disappear itself
	//if the snake eat the item, the type will be set to eType_Snake later in the Snake::resetGridType
	m_pSnakeMap->setGridType(m_pApple->getIndex(), eType_Empty);

	this->removeChildByTag(eID_Apple);
	m_pApple = nullptr;

	//reset the time to add the apple
	//random time from [5, 15] seconds, after the time, an apple appears
	m_fTimeToAddApple = rand() % 11 + 5.0f;
	//reset the removing variable
	m_bRemovingApple = false;
}

void ItemFactory::addStar(float dt)
{
	//check if the star exist
	if (m_pStar)
		return;

	//check if the random time has finished
	if (m_fTimeToAddStar > 0)
	{
		m_fTimeToAddStar -= dt;
		return;
	}

	// it's time to add star, check the empty grid
	Snake* snake = m_pSnakeMap->getSnake();
	if (!snake)
		return;
	int left = m_pSnakeMap->getMovableNumbers() - snake->getLength();

	//no empty grids left
	if (left <= 0)
		return;

	//set the star index
	int index = (int)(rand() % left + 1);
	auto mapIndex = m_pSnakeMap->getEmptyGridIndex(index);

	//create star model
	m_pStar = Star::create();
	this->addChild(m_pStar, 1, eID_Star);
	m_pStar->setPosition(VisibleRect::getVisibleRect().origin + VisibleRect::getHalfGridVec() + VisibleRect::getGridLength()*mapIndex);
	m_pStar->setIndex(mapIndex);

	//add a scale animation
	m_pStar->setScale(0.1f);
	auto scaleBig = ScaleTo::create(0.7f, 1.2f);
	auto scaleBack = ScaleTo::create(0.3f, 1.0f);
	auto sequence = Sequence::create(scaleBig, scaleBack, nullptr);
	m_pStar->runAction(sequence);

	//set the grid type
	m_pSnakeMap->setGridType(mapIndex, eType_Star);

	//set the duration from [15, 25] seconds
	auto duration = rand() % 16 + 10.0f;
	m_pStar->setDuration(duration);
}

void ItemFactory::eatStar()
{
	//stop star's actions first
	m_pStar->stopAllActions();

	// set the star invalid
	m_pStar->setValid(false);
	// set the removing variable to true to avoid call this function again while removing the item
	m_bRemovingStar = true;

	// zoom out the star
	auto scaleSmall = ScaleTo::create(0.5f, 0.1f);
	auto doneAction = CallFunc::create(CC_CALLBACK_0(ItemFactory::removeStar, this));
	auto sequenceAction = Sequence::create(scaleSmall, doneAction, nullptr);
	m_pStar->runAction(sequenceAction);
}

void ItemFactory::removeStar()
{
	//here we should reset the grid type because the item maybe disappear itself
	//if the snake eat the item, the type will be set to eType_Snake later in the Snake::resetGridType
	m_pSnakeMap->setGridType(m_pStar->getIndex(), eType_Empty);

	//here we do not need to reset the map grid type, because it will be set later in Snake::resetGridType
	this->removeChildByTag(eID_Star);
	m_pStar = nullptr;

	//reset the time to add the star
	//random time from [0, 10] seconds, after the time, a star appears
	m_fTimeToAddStar = rand() % 11;
	//reset the removing variable
	m_bRemovingStar = false;
}

void ItemFactory::addBall(float dt)
{
	//check if the ball exist
	if (m_pBall)
		return;

	//check if the random time has finished
	if (m_fTimeToAddBall > 0)
	{
		m_fTimeToAddBall -= dt;
		return;
	}

	// it's time to add ball, check the empty grid
	Snake* snake = m_pSnakeMap->getSnake();
	if (!snake)
		return;
	int left = m_pSnakeMap->getMovableNumbers() - snake->getLength();

	//no empty grids left
	if (left <= 0)
		return;

	//set the star index
	int index = (int)(rand() % left + 1);
	auto mapIndex = m_pSnakeMap->getEmptyGridIndex(index);

	//create star model
	m_pBall = Ball::create();
	this->addChild(m_pBall, 1, eID_Ball);
	m_pBall->setPosition(VisibleRect::getVisibleRect().origin + VisibleRect::getHalfGridVec() + VisibleRect::getGridLength()*mapIndex);
	m_pBall->setIndex(mapIndex);

	//add a scale animation
	m_pBall->setScale(0.1f);
	auto scaleBig = ScaleTo::create(0.7f, 1.2f);
	auto scaleBack = ScaleTo::create(0.3f, 1.0f);
	auto sequence = Sequence::create(scaleBig, scaleBack, nullptr);
	m_pBall->runAction(sequence);

	//set the grid type
	m_pSnakeMap->setGridType(mapIndex, eType_Ball);

	//set the duration from [15, 25] seconds
	auto duration = rand() % 16 + 10.0f;
	m_pBall->setDuration(duration);
}

void ItemFactory::eatBall()
{
	//stop ball's actions first
	m_pBall->stopAllActions();

	// set the ball invalid
	m_pBall->setValid(false);
	// set the removing variable to true to avoid call this function again while removing the item
	m_bRemovingBall = true;

	// zoom out the ball
	auto scaleSmall = ScaleTo::create(0.5f, 0.1f);
	auto doneAction = CallFunc::create(CC_CALLBACK_0(ItemFactory::removeBall, this));
	auto sequenceAction = Sequence::create(scaleSmall, doneAction, nullptr);
	m_pBall->runAction(sequenceAction);
}

void ItemFactory::removeBall()
{
	//here we should reset the grid type because the item maybe disappear itself
	//if the snake eat the item, the type will be set to eType_Snake later in the Snake::resetGridType
	m_pSnakeMap->setGridType(m_pBall->getIndex(), eType_Empty);

	//here we do not need to reset the map grid type, because it will be set later in Snake::resetGridType
	this->removeChildByTag(eID_Ball);
	m_pBall = nullptr;

	//reset the time to add the ball
	//random time from [2, 11] seconds, after the time, a ball appears
	m_fTimeToAddBall = rand() % 10 + 2.0f;
	//reset the removing variable
	m_bRemovingBall = false;
}