#include "MainLayer.h"
#include "GameDefine.h"
#include "SelectLevelLayer.h"

USING_NS_CC;

Scene* MainLayer::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = MainLayer::create();

	// add layer as a child to scene
	scene->addChild(layer, 1, eID_MainLayer);

	// return the scene
	return scene;
}

bool MainLayer::init()
{
	if (!LayerColor::init())
	{
		return false;
	}

	auto playItem = MenuItemImage::create("start.png", "start.png", CC_CALLBACK_1(MainLayer::goSelectLevelScene, this));
	playItem->setPosition(VisibleRect::center());
	auto menu = Menu::create(playItem, nullptr);
	this->addChild(menu, 1);
	menu->setPosition(Vec2::ZERO);

	return true;
}

void MainLayer::goSelectLevelScene(cocos2d::Ref* pSender)
{
	auto scene = TransitionSlideInR::create(0.8f, SelectLevelLayer::createScene());
	Director::getInstance()->replaceScene(scene);
}