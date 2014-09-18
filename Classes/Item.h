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

	// if the item can work any more
	CC_SYNTHESIZE(bool, m_bValid, Valid);
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
	static Door* create(std::string model);

	virtual bool init() override;
	virtual bool initWithModel(std::string model);
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
private:
	const int m_nBasicScore{ 100 };
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

	// add door
	void addDoor(eID doorID, eDirection dir, cocos2d::Vec2 pos, std::string model = DoorModel);

	//get the door
	Door* getDoor(cocos2d::Vec2 index);

	//eat or make the apple disappear
	void eatApple();

	//eat or make the star disappear
	void eatStar();

	//eat or make the ball disappear
	void eatBall();
private:
	SnakeMapLayer* m_pSnakeMap{ nullptr };

	Food* m_pFood{ nullptr };
	std::pair<Door*, Door*> m_pDoors;
	Apple* m_pApple{ nullptr };
	Star* m_pStar{ nullptr };
	Ball* m_pBall{ nullptr };
	float m_fTimeToAddApple{ 0 };
	float m_fTimeToAddStar{ 0 };
	float m_fTimeToAddBall{ 0 };
	bool m_bRemovingApple{ false };
	bool m_bRemovingStar{ false };
	bool m_bRemovingBall{ false };

	//remove expired item
	void removeExpiredItem(float dt);

	//remove apple
	void removeApple();
	//remove door
	void removeDoor(eID doorID);
	//remove all the door
	void removeDoors();
	//remove star
	void removeStar();
	//remove ball
	void removeBall();
	
	void addFood();
	// add doors randomly
	void addDoor();
	// apple exist in finite time
	void addApple(float dt);
	// star exist in finite time
	void addStar(float dt);
	// ball exist in finite time
	void addBall(float dt);
};
#endif