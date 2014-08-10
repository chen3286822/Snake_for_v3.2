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

void ItemFactory::produce()
{
	//produce food when necessary
	addFood();
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

	//set the block type
	m_pSnakeMap->setGridType(mapIndex, eType_Food);
}

void ItemFactory::eatFood()
{
	//here we do not need to reset the map grid type, because it will be set later in Snake::resetGridType
	this->removeChildByTag(eID_Food);
	m_pFood = nullptr;
}