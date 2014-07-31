#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#define DEBUG_DRAW

struct Block
{
	cocos2d::Point m_iPos;
};
#define MAPWIDTH 30
#define MAPHEIGHT 20
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

class MapLayer : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(MapLayer);

public:
	virtual void update(float dt) override;

private:
	SnakeMap* m_pMap{ nullptr };
	cocos2d::Sprite3D* m_pBox{nullptr};
};

#endif // __HELLOWORLD_SCENE_H__
