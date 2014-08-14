#ifndef __ITEM_H__
#define __ITEM_H__

#include "cocos2d.h"
#include "GameDefine.h"

class Snake;
class Item : public cocos2d::Node
{
public:
	virtual ~Item(){}

	// the item will be effective
	virtual void effect(Snake*) = 0;

	// the map index
	CC_SYNTHESIZE(cocos2d::Vec2, m_Index, Index);

	// duration that the apple exist
	CC_SYNTHESIZE(float, m_fDuration, Duration);
protected:
	// the item model
	cocos2d::Sprite3D* m_pModel{ nullptr };
};

// snake eat it, will grow a new body rect
class Food : public Item
{
public:
	CREATE_FUNC(Food);

	virtual bool init() override;
	virtual void effect(Snake* snake) override;
};

// the transfer door, snake can transfer to another grid from cross the door
class Door : public Item
{
public:
	CREATE_FUNC(Door);

	virtual bool init() override;
	virtual void effect(Snake* snake) override;

	//the direction that snake can cross the door, if the snake's direction isn't equal to this, the door is blocked
	CC_SYNTHESIZE(eDirection, m_eTransferDirection, TransferDirection);

	//the other door that connect to this
	CC_SYNTHESIZE(Door*, m_otherDoor, OtherDoor);
};

// score apple, snake eat it will gain points
class Apple : public Item
{
public:
	CREATE_FUNC(Apple);

	virtual bool init() override;
	virtual void effect(Snake* snake) override;
};

// speed star, snake eat it will speed up and gain the score obtained
class Star : public Item
{
public:
	CREATE_FUNC(Star);

	virtual bool init() override;
	virtual void effect(Snake* snake) override;
};

// slow ball, snake eat it will slow down itself for a while
class Ball : public Item
{
public:
	CREATE_FUNC(Ball);

	virtual bool init() override;
	virtual void effect(Snake* snake) override;
};

class SnakeMapLayer;
// response for producing all the items
class ItemFactory : public cocos2d::Node
{
public:
	static ItemFactory* create(SnakeMapLayer* snakeMap);
	bool initWithMap(SnakeMapLayer* snakeMap);

	//get the item
	Item* getItem(cocos2d::Vec2 index);

	//get items number
	int getItemsNumber();

	// produce items, it it called every frame
	void produce(float dt);

	//remove the food
	void eatFood();

	//remove the door
	void removeDoor();

	//get the door
	Door* getDoor(cocos2d::Vec2 index);

	//eat or make the apple disappear
	void eatApple();
private:
	SnakeMapLayer* m_pSnakeMap{ nullptr };

	Food* m_pFood{ nullptr };
	std::pair<Door*, Door*> m_pDoors;
	Apple* m_pApple{ nullptr };
	float m_fTimeToAddApple{ 0 };

	//remove expired item
	void removeExpiredItem(float dt);

	//remove apple
	void removeApple();
	
	void addFood();
	void addDoor();
	// apple exist in finite time
	void addApple(float dt);
};
#endif