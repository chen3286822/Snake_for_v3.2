#ifndef __ITEM_H__
#define __ITEM_H__

#include "cocos2d.h"

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

class SnakeMapLayer;
// response for producing all the items
class ItemFactory : public cocos2d::Node
{
public:
	static ItemFactory* create(SnakeMapLayer* snakeMap);
	bool initWithMap(SnakeMapLayer* snakeMap);

	// produce items, it it called every frame
	void produce();

private:
	SnakeMapLayer* m_pSnakeMap{ nullptr };

	Food* m_pFood{ nullptr };

	void addFood();
};
#endif