#include "MapLayer.h"

USING_NS_CC;

Scene* MapLayer::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MapLayer::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MapLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(MapLayer::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

	std::string fileName = "models/box2.c3b";
	m_pBox = Sprite3D::create(fileName);
	m_pBox->setScale(3);
	m_pBox->setRotation3D(Vec3(60, 60, 0));
	addChild(m_pBox);
	m_pBox->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	scheduleUpdate();

// 	auto animation = Animation3D::create(fileName);
// 	if (animation)
// 	{
// 		auto animate = Animate3D::create(animation);
// 		bool inverse = (std::rand() % 3 == 0);
// 
// 		int rand2 = std::rand();
// 		float speed = 1.0f;
// 		if (rand2 % 3 == 1)
// 		{
// 			speed = animate->getSpeed() + CCRANDOM_0_1();
// 		}
// 		else if (rand2 % 3 == 2)
// 		{
// 			speed = animate->getSpeed() - 0.5 * CCRANDOM_0_1();
// 		}
// 		animate->setSpeed(inverse ? -speed : speed);
// 
// 		sprite->runAction(RepeatForever::create(animate));
// 	}
    
    return true;
}

void MapLayer::update(float dt)
{
	if (m_pBox)
	{
		auto vec = m_pBox->getRotation3D();
		auto degreeY = vec.y;
		degreeY += dt * 20;
		if (degreeY > 360)
			degreeY -= 360;
		m_pBox->setRotation3D(Vec3(vec.x, degreeY, vec.z));
	}
	
}

void MapLayer::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
