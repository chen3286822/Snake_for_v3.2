#include "Item.h"
#include "GameDefine.h"
#include "MapLayer.h"
#include "Snake.h"
#include "VisibleRect.h"

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

	//set  the food model
	m_pModel = Sprite3D::create(AppleModel);
	this->addChild(m_pModel, 1);

	m_fDuration = 0.0f;

	return true;
}

void Apple::effect(Snake* snake)
{
	// speed up the snake
	if (snake)
	{
		snake->setSpeed(snake->getSpeed() + 16.0f);
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

	return number;
}

void ItemFactory::produce(float dt)
{
	//produce food when necessary
	addFood();

	//add the transfer doors
	addDoor();

	//add the speed apple
	addApple(dt);

	//check and remove expired item
	removeExpiredItem(dt);
}

void ItemFactory::removeExpiredItem(float dt)
{
	//check the apple's duration
	if (m_pApple)
	{
		if (m_pApple->getDuration() <= 0)
			removeApple();
		else
		{
			m_pApple->setDuration(m_pApple->getDuration() - dt);
		}
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

	//set the door index
	int index = (int)(rand() % left + 1);
	auto mapIndex = m_pSnakeMap->getEmptyGridIndex(index);

	//create apple model
	m_pApple = Apple::create();
	this->addChild(m_pApple, 1, eID_Apple);
	m_pApple->setPosition(VisibleRect::getVisibleRect().origin + VisibleRect::getHalfGridVec() + VisibleRect::getGridLength()*mapIndex);
	m_pApple->setIndex(mapIndex);

	//set the grid type
	m_pSnakeMap->setGridType(mapIndex, eType_Apple);

	//set the duration from [15, 25] seconds
	auto duration = rand() % 16 + 10.0f;
	m_pApple->setDuration(duration);
}

void ItemFactory::removeApple()
{
	//here we do not need to reset the map grid type, because it will be set later in Snake::resetGridType
	this->removeChildByTag(eID_Apple);
	m_pApple = nullptr;

	//reset the time to add the apple
	//random time from [5, 15] seconds, after the time, an apple appears
	m_fTimeToAddApple = rand() % 11 + 5.0f;
}