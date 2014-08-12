#ifndef __MAPLAYER_SCENE_H__
#define __MAPLAYER_SCENE_H__

#include "cocos2d.h"
#include "GameDefine.h"

#define DEBUG_DRAW

struct Block
{
	cocos2d::Vec2 m_Index;
	bool m_bOccupied{ false };
	eType m_eType{ eType_Empty };
};

class BodyRect;
class Snake;
class ItemFactory;
class SnakeMapLayer : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init() override;  

	//override draw for debugging
	virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;
    
    // implement the "static create()" method manually
    CREATE_FUNC(SnakeMapLayer);

public:
	virtual void update(float dt) override;
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event* event);

	//get all the movable grids' sum, some map may contains some immovable blocks and should be excluded
	int getMovableNumbers();

	//get the empty grid index for produce item
	cocos2d::Vec2 getEmptyGridIndex(int index);
	
	//set snake body rect's destination and move type
	//just pass the ptr value to the map, so the snake doesn't need to check the map
	void setDestinationOfBodyRect(BodyRect* bodyRect);

	//get the grid's type
	void setGridType(cocos2d::Vec2 index, eType type);
	eType getGridType(cocos2d::Vec2 index);

	void setOccupy(cocos2d::Vec2 index, bool bOccupy);
	bool getOccupy(cocos2d::Vec2 index);

	//check if the body rect stays in the out door
	bool isInDoor(cocos2d::Vec2 index);
protected:
	//for debugging
	void onDraw(const cocos2d::Mat4& transform, uint32_t flags);
	cocos2d::CustomCommand m_customCommand;

private:
	Block m_iBlocks[MAPWIDTH][MAPHEIGHT];

	Snake* m_pSnake{ nullptr };
	ItemFactory* m_pItemFactory{ nullptr };

	cocos2d::EventListenerKeyboard* m_pKeyboardListener{ nullptr };
};

#endif // __HELLOWORLD_SCENE_H__
