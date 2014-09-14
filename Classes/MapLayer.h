#ifndef __MAPLAYER_SCENE_H__
#define __MAPLAYER_SCENE_H__

#include "cocos2d.h"
#include "GameDefine.h"

#define DEBUG_DRAW

struct Grid
{
	cocos2d::Vec2 m_Index;
	bool m_bOccupied{ false };
	eType m_eType{ eType_Empty };
};

class Block : public cocos2d::Node
{
public:
	CREATE_FUNC(Block);

	virtual bool init() override;

	// the map index
	CC_SYNTHESIZE(cocos2d::Vec2, m_Index, Index);

protected:
	// the item model
	cocos2d::Sprite3D* m_pModel{ nullptr };
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

	// load a map from configuration file
	bool loadMap(int level);
public:
	virtual void update(float dt) override;
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event* event);
	void die();
	void restart(cocos2d::Ref* pSender);

	ItemFactory* getItemFactory(){ return m_pItemFactory; }
	Snake* getSnake(){ return m_pSnake; }
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

	// update UI data, label = eID_All means all the UI data should be updated
	void updateUIData(int label = eID_All);

	// check if successfully or failed complete the level
	eResult isCompleted();
protected:
	//for debugging
	void onDraw(const cocos2d::Mat4& transform, uint32_t flags);
	cocos2d::CustomCommand m_customCommand;

private:
	Grid m_iGrids[MAPWIDTH][MAPHEIGHT];

	Snake* m_pSnake{ nullptr };
	ItemFactory* m_pItemFactory{ nullptr };
	std::list<Block*> m_lBlocks;

	cocos2d::EventListenerKeyboard* m_pKeyboardListener{ nullptr };

	// victory condition
	VictoryCondition m_VictoryCondition;

	// time recorder
	float m_fTimer{ 0 };

	//init all the blocks
	void initBlocks();
};

#endif // __HELLOWORLD_SCENE_H__
