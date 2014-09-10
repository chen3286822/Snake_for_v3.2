#include "MapLayer.h"
#include "VisibleRect.h"
#include "Snake.h"
#include "Item.h"
#include "UserRecord.h"
USING_NS_CC;

bool Block::init()
{
	if (!Node::init())
		return false;

	// create block model
	m_pModel = Sprite3D::create(BlockModel);
	this->addChild(m_pModel, 1);

	return true;
}

Scene* SnakeMapLayer::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = SnakeMapLayer::create();

    // add layer as a child to scene
	scene->addChild(layer, 1, eID_SnakeMap);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool SnakeMapLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	//init the time seed
	srand((unsigned int)time(NULL));

	//add keyboard event listener
	m_pKeyboardListener = EventListenerKeyboard::create();
	m_pKeyboardListener->onKeyReleased = CC_CALLBACK_2(SnakeMapLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_pKeyboardListener, this);

	// score viewer
	TTFConfig labelConfig("fonts/msyh.ttf", 16);
	auto scoreLabel = Label::createWithTTF(labelConfig, "Score: 0", TextHAlignment::CENTER);
	scoreLabel->setPosition(VisibleRect::rightBottom());
	scoreLabel->setAnchorPoint(Vec2(1, 0));
	this->addChild(scoreLabel, 3, eID_ScoreLabel);

	//init blocks
	for (int i = 0; i < MAPWIDTH; i++)
	{
		for (int j = 0; j < MAPHEIGHT; j++)
		{
			m_iGrids[i][j].m_Index = Vec2(i, j);
			m_iGrids[i][j].m_eType = eType_Empty;
		}
	}

	//draw the grid lines for debug
#ifdef DEBUG_DRAW
	auto drawLines = DrawNode::create();
	this->addChild(drawLines, 1);
	auto visualRect = VisibleRect::getVisibleRect();
	auto length = visualRect.size.width / MAPWIDTH;
	for (int i = 1; i < MAPWIDTH; i++)
	{
		drawLines->drawSegment(Vec2(i*length, 0) + visualRect.origin, Vec2(i*length, visualRect.size.height) + visualRect.origin, 1, Color4F(0.4f, 0.4f, 0.4f, 1));
	}
	for (int j = 1; j < MAPHEIGHT; j++)
	{
		drawLines->drawSegment(Vec2(0, j*length) + visualRect.origin, Vec2(visualRect.size.width, j*length) + visualRect.origin, 1, Color4F(0.4f, 0.4f, 0.4f, 1));
	}
#endif

	//init blocks
	initBlocks();

	//create the snake
	m_pSnake = Snake::create(this);
	this->addChild(m_pSnake, 2, eID_Snake);
	
	//create the items factory
	m_pItemFactory = ItemFactory::create(this);
	this->addChild(m_pItemFactory, 2, eID_ItemFactory);

	//schedule the udpate function to maintain the food and other items' production
	scheduleUpdate();

	//the snake crawl
	m_pSnake->crawl();


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

	// load user's data
	UserRecord::getInstance()->changeUser("aaa");
    
    return true;
}

void SnakeMapLayer::initBlocks()
{
	m_lBlocks.clear();

	auto block = Block::create();
	this->addChild(block, 2);
	block->setIndex(Vec2(5, 5));
	block->setPosition(VisibleRect::getVisibleRect().origin + VisibleRect::getGridLength() * block->getIndex() + VisibleRect::getHalfGridVec());
	m_lBlocks.push_back(block);

	//set grid type
	setGridType(block->getIndex(), eType_Blocked);
}

void SnakeMapLayer::draw(Renderer* renderer, const Mat4 &transform, uint32_t flags)
{
	m_customCommand.init(1);
	m_customCommand.func = CC_CALLBACK_0(SnakeMapLayer::onDraw, this, transform, flags);
	renderer->addCommand(&m_customCommand);
}

void SnakeMapLayer::onDraw(const Mat4& transform, uint32_t flags)
{
	Director* director = Director::getInstance();
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);

#ifdef DEBUG_DRAW
	glLineWidth(1);

	for (int i = 0; i < MAPWIDTH; ++i)
	{
		for (int j = 0; j < MAPHEIGHT; ++j)
		{
			auto color = Color4F::BLACK;
			auto type = getGridType(Vec2(i, j));
			if (type == eType_Snake)
				color = Color4F(0.5f, 0.5f, 1, 0.8f);
			else if (type == eType_Food)
				color = Color4F(0, 1, 0, 0.8f);
			else if (type == eType_Apple)
				color = Color4F(1, 0, 0, 0.8f);
			else if (type == eType_Star)
				color = Color4F(1, 1, 0, 0.8f);
			else if (type == eType_Ball)
				color = Color4F(0, 0, 1, 0.8f);

			if (color != Color4F::BLACK)
			{
				auto x = VisibleRect::getGridLength() * i + VisibleRect::getVisibleRect().origin.x;
				auto y = VisibleRect::getGridLength() * j + VisibleRect::getVisibleRect().origin.y;
				auto x1 = x + VisibleRect::getGridLength();
				auto y1 = y + VisibleRect::getGridLength();
				Vec2 filledVertices[] = { Vec2(x, y), Vec2(x1, y), Vec2(x1, y1), Vec2(x, y1) };
				DrawPrimitives::drawSolidPoly(filledVertices, 4, color);

				CHECK_GL_ERROR_DEBUG();
			}
		}
	}
#endif

	//end draw
	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void SnakeMapLayer::die()
{
	//pause the snake
	m_pSnake->pauseAll();

	auto restartLabel = Label::createWithSystemFont("Restart", "Arial", 20);
	auto restartItem = MenuItemLabel::create(restartLabel, CC_CALLBACK_1(SnakeMapLayer::restart, this));
	restartItem->setPosition(VisibleRect::center());
	auto menu = Menu::create(restartItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 10);
}

void SnakeMapLayer::restart(Ref* pSender)
{
	Director::getInstance()->replaceScene(SnakeMapLayer::createScene());
}

void SnakeMapLayer::setDestinationOfBodyRect(BodyRect* bodyRect)
{
	if (!bodyRect)
		return;

	auto mapOffset = [=](eDirection dir) -> Vec2
	{
		if (dir == eDir_Up)
			return Vec2(0, 1);
		else if (dir == eDir_Down)
			return Vec2(0, -1);
		else if (dir == eDir_Left)
			return Vec2(-1, 0);
		else if (dir == eDir_Right)
			return Vec2(1, 0);
		return Vec2(0, 0);
	};
	auto curIndex = bodyRect->getMapIndex();
	//expected destination index
	auto tempIndex = curIndex + mapOffset(bodyRect->getCurDirection());
	//real index
	auto realIndex = tempIndex;
	//move type
	auto moveType = eMoveType_None;
	bodyRect->setTransferDirection(bodyRect->getCurDirection());
	//check if the 'destination' is valid
	if (tempIndex.x < 0 || tempIndex.x >= MAPWIDTH || tempIndex.y < 0 || tempIndex.y >= MAPHEIGHT)
	{
		//the snake is crossing over the border, get the real destination
		if (tempIndex.x < 0)
		{
			realIndex.x = MAPWIDTH - 1;
		}
		else if (tempIndex.x >= MAPWIDTH)
		{
			realIndex.x = 0;
		}
		if (tempIndex.y < 0)
		{
			realIndex.y = MAPHEIGHT - 1;
		}
		else if (realIndex.y >= MAPHEIGHT)
		{
			realIndex.y = 0;
		}

		bodyRect->setCrossing(true);
	}
	
	//get the destination grid type
	auto type = getGridType(realIndex);
	switch (type)
	{
	case eType_Door:
	{
					   //check if the direction is right to the door
					   auto door = m_pItemFactory->getDoor(realIndex);
					   if (door)
					   {
						   //cross the door
						   if (bodyRect->getCurDirection() == door->getTransferDirection())
						   {
							   moveType = eMoveType_Transfer;
							   //the destination is close to the other door
							   auto otherDoor = door->getOtherDoor();
							   if (otherDoor)
							   {
								   auto outDirection = oppositeDirection(otherDoor->getTransferDirection());
								   //just stop in the out door, and the direction cannot be changed until the body rect move out of the door througn the transfer direction
								   realIndex = otherDoor->getIndex();
								   bodyRect->setTransferDirection(outDirection);
							   }
						   }
						   else
						   {
							   //like hit the wall
							   moveType = eMoveType_Dead;
						   }
					   }
					   else
					   {
						   //wrong branch
					   }
	}
		break;
	case eType_Food:
	{
					   moveType = eMoveType_Eat;
	}
		break;
	case eType_Blocked:
	case eType_Snake:
	case eType_Dead:
	{
					   moveType = eMoveType_Dead;
	}
		break;
	}

	//set the values
	bodyRect->setDestinationIndex(realIndex);
	bodyRect->setMoveType(moveType);
}

void SnakeMapLayer::update(float dt)
{
	if (m_pItemFactory)
		m_pItemFactory->produce(dt);	
}

void SnakeMapLayer::onKeyReleased(EventKeyboard::KeyCode keycode, Event* event)
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
		Director::getInstance()->replaceScene(SnakeMapLayer::createScene());
	}
		break;
	case EventKeyboard::KeyCode::KEY_F2:
	{
		m_pSnake->pauseAll();
		unscheduleUpdate();
	}
		break;
	case EventKeyboard::KeyCode::KEY_F3:
	{
		scheduleUpdate();
		m_pSnake->resumeAll();

	}
		break;
	default:
		break;
	}
}

void SnakeMapLayer::setOccupy(Vec2 index, bool bOccupy)
{
	int x = (int)index.x;
	int y = (int)index.y;
	if (x < 0 || y < 0 || x >= MAPWIDTH || y >= MAPHEIGHT)
		return;

	m_iGrids[x][y].m_bOccupied = bOccupy;
}

bool SnakeMapLayer::getOccupy(Vec2 index)
{
	int x = (int)index.x;
	int y = (int)index.y;
	if (x < 0 || y < 0 || x >= MAPWIDTH || y >= MAPHEIGHT)
		return false;

	return m_iGrids[x][y].m_bOccupied;
}

//modify later
int SnakeMapLayer::getMovableNumbers()
{
	int numbers = MAPWIDTH*MAPHEIGHT - m_lBlocks.size();
	if (m_pItemFactory)
		numbers -= m_pItemFactory->getItemsNumber();
	return numbers;
}

void SnakeMapLayer::setGridType(cocos2d::Vec2 index, eType type)
{
	int x = (int)index.x;
	int y = (int)index.y;
	if (x < 0 || y < 0 || x >= MAPWIDTH || y >= MAPHEIGHT)
		return;

	m_iGrids[x][y].m_eType = type;
	if (type != eType_None)
	{
		if (type == eType_Empty)
			m_iGrids[x][y].m_bOccupied = false;
		else
			m_iGrids[x][y].m_bOccupied = true;
	}
}

eType SnakeMapLayer::getGridType(cocos2d::Vec2 index)
{
	int x = (int)index.x;
	int y = (int)index.y;
	if (x < 0 || y < 0 || x >= MAPWIDTH || y >= MAPHEIGHT)
		return eType_None;

	return m_iGrids[x][y].m_eType;
}

Vec2 SnakeMapLayer::getEmptyGridIndex(int index)
{
	auto emptyIndex = Vec2(-1, -1);
	int count = 0;
	for (int i = 0; i < MAPWIDTH*MAPHEIGHT; i++)
	{
		if (m_iGrids[i % MAPWIDTH][i / MAPWIDTH].m_bOccupied == false)
		{
			count++;
			if (count == index)
			{
				emptyIndex = Vec2(i % MAPWIDTH, i / MAPWIDTH);
				break;
			}
		}
	}
	return emptyIndex;
}

bool SnakeMapLayer::isInDoor(cocos2d::Vec2 index)
{
	auto door = m_pItemFactory->getDoor(index);
	if (door)
		return true;

	return false;
}

void SnakeMapLayer::updateUIData(int label)
{
	bool IsAll = (label == eID_All ? true : false);
	std::vector<int> vIDs;
	if (IsAll)
	{
		// push all the UI ID that need to be updated
		vIDs.push_back(eID_ScoreLabel);
	}
	else
		vIDs.push_back(label);

	for (auto id : vIDs)
	{
		switch (id)
		{
		case eID_ScoreLabel:
		{
							   auto scoreLabel = dynamic_cast<Label*>(getChildByTag(eID_ScoreLabel));
							   if (scoreLabel)
							   {
								   char temp[256];
								   sprintf(temp, "Score: %d", UserRecord::getInstance()->getScore());
								   scoreLabel->setString(temp);
							   }
		}
			break;
		default:
			break;
		}
	}

}