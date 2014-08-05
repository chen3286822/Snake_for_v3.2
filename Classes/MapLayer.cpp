#include "MapLayer.h"
#include "VisibleRect.h"
#include "Snake.h"
USING_NS_CC;

bool SnakeMap::init()
{
	if (!Node::init())
	{
		return false;
	}

	//init blocks
	for (int i = 0; i < MAPWIDTH;i++)
	{
		for (int j = 0; j < MAPHEIGHT;j++)
			m_iBlocks[i][j].m_iPos = Point(i,j);
	}


#ifdef DEBUG_DRAW
	auto drawLines = DrawNode::create();
	this->addChild(drawLines, 1);
	auto visualRect = VisibleRect::getVisibleRect();
	auto length = visualRect.size.width / MAPWIDTH;
	for (int i = 1; i < MAPWIDTH;i++)
	{
		drawLines->drawSegment(Vec2(i*length, 0) + visualRect.origin, Vec2(i*length, visualRect.size.height) + visualRect.origin, 1, Color4F(0.4f, 0.4f, 0.4f, 1));
	}
 	for (int j = 1; j < MAPHEIGHT; j++)
 	{
 		drawLines->drawSegment(Vec2(0, j*length) + visualRect.origin, Vec2(visualRect.size.width, j*length) + visualRect.origin, 1, Color4F(0.4f, 0.4f, 0.4f, 1));
 	}
#endif
	
	return true;
}

void SnakeMap::onEnter()
{
	Node::onEnter();

	//init food
	addFood();
}

void SnakeMap::setOccupy(Vec2 index)
{
	int x = (int)index.x;
	int y = (int)index.y;
	if (x < 0 || y < 0 || x >= MAPWIDTH || y >= MAPHEIGHT)
		return;

	m_iBlocks[x][y].m_bOccupied = true;
}

//modify later
int SnakeMap::getMovableNumbers()
{
	return MAPWIDTH*MAPHEIGHT;
}

void SnakeMap::setGridType(cocos2d::Vec2 index, eType type)
{
	int x = (int)index.x;
	int y = (int)index.y;
	if (x < 0 || y < 0 || x >= MAPWIDTH || y >= MAPHEIGHT)
		return;

	m_iBlocks[x][y].m_eType = type;
}

eType SnakeMap::getGridType(cocos2d::Vec2 index)
{
	int x = (int)index.x;
	int y = (int)index.y;
	if (x < 0 || y < 0 || x >= MAPWIDTH || y >= MAPHEIGHT)
		return eType_None;

	return m_iBlocks[x][y].m_eType;
}

void SnakeMap::addFood()
{
	//check the empty block
	Snake* snake = dynamic_cast<Snake*>(getParent()->getChildByTag(eID_Snake));
	if (!snake)
		return;
	int left = getMovableNumbers() - snake->getLength();

	//no empty blocks left, you win
	if (left <= 0)
		return;

	//set the food index
	int index = (int)(rand() % left + 1);
	int count = 0;
	for (int i = 0; i < MAPWIDTH*MAPHEIGHT; i++)
	{
		if (m_iBlocks[i % MAPWIDTH][i / MAPWIDTH].m_bOccupied == false)
		{
			count++;
			if (count == index)
			{
				m_foodIndex = Vec2(i % MAPWIDTH, i / MAPWIDTH);
				m_iBlocks[i % MAPWIDTH][i / MAPWIDTH].m_bOccupied = true;
				break;
			}
		}
	}
	
	//create food model
	Sprite3D* food = dynamic_cast<Sprite3D*>(this->getChildByTag(eID_Food));
	if (food == NULL)
	{
		food = Sprite3D::create(FoodModel);
		this->addChild(food, 1, eID_Food);
	}
	food->setPosition(VisibleRect::getVisibleRect().origin + VisibleRect::getHalfGridVec() + VisibleRect::getGridLength()*m_foodIndex);

	//set the block type
	setGridType(m_foodIndex, eType_Food);
}

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

	srand((unsigned int)time(NULL));

	m_pKeyboardListener = EventListenerKeyboard::create();
	m_pKeyboardListener->onKeyReleased = CC_CALLBACK_2(MapLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_pKeyboardListener, this);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

 	m_pMap = SnakeMap::create();
	this->addChild(m_pMap, 1, eID_SnakeMap);

	m_pBox = Sprite3D::create(SnakeBodyModel);
	//m_pBox->setAnchorPoint(Vec2(0, 0));	//not work for 3D sprite
	//m_pBox->setScale(3);
	//m_pBox->setRotation3D(Vec3(60, 60, 0));
	this->addChild(m_pBox,2);
	m_pBox->setPosition(Vec2(visibleSize.width/2 + 16 +  (-5)*32 + origin.x, visibleSize.height/2 + 16 + (-5)*32+ origin.y));
	m_iLastPt = m_pBox->getPosition(); 
	//m_pBox->ignoreAnchorPointForPosition(true);
	//m_pBox->setContentSize(Size(VisibleRect::getGridLength(), VisibleRect::getGridLength()));
	//m_pBox->setAnchorPoint(Vec2(0.5,0.5));
	log("%f, %f",m_pBox->getPositionX(),m_pBox->getPositionY());

	auto rotateAction = RotateBy::create(2.5, Vec3(0,0,90));
	auto doneAction = CallFunc::create(CC_CALLBACK_0(MapLayer::getPos, this, m_pBox));
	auto rotateAction2 = RotateBy::create(2.5, Vec3(0, 0, 90));
	auto moveAction = MoveBy::create(2.5, Vec2(VisibleRect::getGridLength(),0));
	auto moveAction2 = MoveTo::create(2.5, m_pBox->getPosition() + Vec2(32,0));
	auto sequenceAction = Sequence::create(rotateAction, doneAction, NULL);
	auto spawnAction = Spawn::create(rotateAction,moveAction,NULL);
	m_pBox->runAction(spawnAction);
	//scheduleUpdate();

	m_pSnake = Snake::create();
	this->addChild(m_pSnake, 2, eID_Snake);

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

void MapLayer::getPos(cocos2d::Sprite3D* snake)
{
	snake->setPosition(m_pBox->getPosition() + Vec2(0, 32));
	snake->setAnchorPoint(Vec2(0, 0));
	log("%f, %f", m_pBox->getPositionX(), m_pBox->getPositionY());
}

void MapLayer::update(float dt)
{
	if (m_pBox)
	{
		auto pos = m_pBox->getPosition();
		if (pos.x - m_iLastPt.x > 32)
		{
			m_pBox->setPosition(Vec2(pos.x, pos.y + 32));
			m_iLastPt = m_pBox->getPosition();
			log("%f\n", m_fLastTime);
			m_fLastTime = 0;
		}
		else
		{
			auto offset = dt * 40;
			m_pBox->setPosition(Vec2(pos.x + offset, pos.y));
			m_fLastTime += dt;
		}

// 		auto vec = m_pBox->getRotation3D();
// 		auto degreeY = vec.y;
// 		degreeY += dt * 20;
// 		if (degreeY > 360)
// 			degreeY -= 360;
// 		m_pBox->setRotation3D(Vec3(vec.x, degreeY, vec.z));
	}
	
}

void MapLayer::onKeyReleased(EventKeyboard::KeyCode keycode, Event* event)
{
	switch (keycode)
	{
	case EventKeyboard::KeyCode::KEY_ESCAPE:
	{
											   Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
											   exit(0);
#endif
	}
		break;
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
	{
												   if (m_pSnake)
												   {
													   auto keyToDirection = [=](EventKeyboard::KeyCode key) -> eDirection
													   {
														   if (key == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
															   return eDir_Left;
														   else if (key == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
															   return eDir_Right;
														   else if (key == EventKeyboard::KeyCode::KEY_UP_ARROW)
															   return eDir_Up;
														   else if (key == EventKeyboard::KeyCode::KEY_DOWN_ARROW)
															   return eDir_Down;
														   return eDir_None;
													   };
													   m_pSnake->setDirection(keyToDirection(keycode));
												   }
	}
		break;
	case EventKeyboard::KeyCode::KEY_F1:
	{
										   Director::getInstance()->replaceScene(MapLayer::createScene());
	}
		break;
	case EventKeyboard::KeyCode::KEY_F2:
	{
										   m_pSnake->pauseAll();
	}
		break;
	case EventKeyboard::KeyCode::KEY_F3:
	{
										   m_pSnake->resumeAll();
	}
		break;
	default:
		break;
	}
}
