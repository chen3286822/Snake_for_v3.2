#ifndef __MAPLAYER_SCENE_H__
#define __MAPLAYER_SCENE_H__

#include "cocos2d.h"
#include "GameDefine.h"

#define DEBUG_DRAW

struct Block
{
	cocos2d::Point m_iPos;
};

class SnakeMap : public cocos2d::Node
{
public:
	CREATE_FUNC(SnakeMap);
	virtual bool init();
	//virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
	virtual void onEnter(){}
	virtual void onExit(){}
private:
	Block m_iBlocks[MAPWIDTH][MAPHEIGHT];
};

class Snake;
class MapLayer : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // implement the "static create()" method manually
    CREATE_FUNC(MapLayer);

public:
	virtual void update(float dt) override;
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keycode, cocos2d::Event* event);

private:
	SnakeMap* m_pMap{ nullptr };
	Snake* m_pSnake{ nullptr };
	cocos2d::Sprite3D* m_pBox{nullptr};
	cocos2d::Vec2 m_iLastPt;
	float m_fLastTime{ 0.0f };
	cocos2d::EventListenerKeyboard* m_pKeyboardListener{ nullptr };

	void getPos(cocos2d::Sprite3D* snake);
};

#endif // __HELLOWORLD_SCENE_H__
