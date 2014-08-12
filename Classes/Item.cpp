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

void Food::effect()
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

void Door::effect()
{

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

int ItemFactory::getItemsNumber()
{
	int number = 0;
	if (m_pFood)
		number++;
	if (m_pDoors.first && m_pDoors.second)
		number += 2;

	return number;
}

void ItemFactory::produce()
{
	//produce food when necessary
	addFood();

	//add the transfer doors
	addDoor();
}

void ItemFactory::addFood()
{
	//check if the food still exist
	if (m_pFood)
		return;

	//check the empty block
	Snake* snake = dynamic_cast<Snake*>(getParent()->getChildByTag(eID_Snake));
	if (!snake)
		return;
	int left = m_pSnakeMap->getMovableNumbers() - snake->getLength();

	//no empty blocks left, you win
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

	//check the empty block
	Snake* snake = dynamic_cast<Snake*>(getParent()->getChildByTag(eID_Snake));
	if (!snake)
		return;
	int left = m_pSnakeMap->getMovableNumbers() - snake->getLength();

	//no empty blocks left
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
	auto transferDir = randomDirection();
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

	transferDir = randomDirection();
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