#ifndef __ITEM_H__
#define __ITEM_H__

#include "cocos2d.h"
#include "GameDefine.h"

class Item : public cocos2d::Node
{
public:
	virtual ~Item(){}

	// the item will be effective
	virtual void effect() = 0;

	// the map index
	CC_SYNTHESIZE(cocos2d::Vec2, m_Index, Index);
protected:
	// the item model
	cocos2d::Sprite3D* m_pModel{ nullptr };
};

class Food : public Item
{
public:
	CREATE_FUNC(Food);

	virtual bool init() override;
	virtual void effect() override;
};

class Door : public Item
{
public:
	CREATE_FUNC(Door);

	virtual bool init() override;
	virtual void effect() override;

	//the direction that snake can cross the door, if the snake's direction isn't equal to this, the door is blocked
	CC_SYNTHESIZE(eDirection, m_eTransferDirection, TransferDirection);

	//the other door that connect to this
	CC_SYNTHESIZE(Door*, m_otherDoor, OtherDoor);
};

class SnakeMapLayer;
// response for producing all the items
class ItemFactory : public cocos2d::Node
{
public:
	static ItemFactory* create(SnakeMapLayer* snakeMap);
	bool initWithMap(SnakeMapLayer* snakeMap);

	//get items number
	int getItemsNumber();

	// produce items, it it called every frame
	void produce();

	//remove the food
	void eatFood();

	//remove the door
	void removeDoor();

	//get the door
	Door* getDoor(cocos2d::Vec2 index);
private:
	SnakeMapLayer* m_pSnakeMap{ nullptr };

	Food* m_pFood{ nullptr };
	std::pair<Door*, Door*> m_pDoors;

	void addFood();
	//the doors should not be set close, and can not be set at the border
	void addDoor();
};
#endif