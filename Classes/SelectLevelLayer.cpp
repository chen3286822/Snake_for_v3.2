#include "SelectLevelLayer.h"
#include "GameDefine.h"
#include "MainLayer.h"
#include "ScrollMenu.h"

USING_NS_CC;
USING_NS_CC_EXT;

Scene* SelectLevelLayer::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = SelectLevelLayer::create();

	// add layer as a child to scene
	scene->addChild(layer, 1, eID_SelectLevelLayer);

	// return the scene
	return scene;
}

bool SelectLevelLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto bgSprite = Sprite::create("sky.png");
	this->addChild(bgSprite, 1, eID_SelectLevelBg);
	bgSprite->setPosition(VisibleRect::center());


	auto backItem = MenuItemImage::create("back.png", "back.png", CC_CALLBACK_1(SelectLevelLayer::backToMainLayer, this));
	backItem->setAnchorPoint(Vec2(0, 1));
	backItem->setPosition(VisibleRect::leftTop() + Vec2(-5, 5));
	auto menu = Menu::create(backItem, nullptr);
	this->addChild(menu, 1);
	menu->setPosition(Vec2::ZERO);

	return true;
}

void SelectLevelLayer::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();

	// create the level table view
	m_iTableViewLeftDownPos = Vec2(30, 0);
	m_iTableViewSize = VisibleRect::getVisibleRect().size - Size(60.0f, 128.0f);
	auto levelView = TableView::create(this, m_iTableViewSize);
	levelView->setDirection(ScrollView::Direction::VERTICAL);
	levelView->setPosition(m_iTableViewLeftDownPos);
	levelView->setDelegate(this);
	this->addChild(levelView, 1, eID_LevelView);
	levelView->reloadData();
}

void SelectLevelLayer::tableCellTouched(TableView* table, TableViewCell* cell)
{
	log("cell touched at index: %ld", cell->getIdx());
}

Size SelectLevelLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	return Size(900, 400);
}

TableViewCell* SelectLevelLayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
	TableViewCell *cell = table->dequeueCell();
	if (!cell) {
		cell = LevelTableViewCell::createWithTableViewRect(m_iTableViewLeftDownPos, m_iTableViewSize);
	}

	return cell;
}

ssize_t SelectLevelLayer::numberOfCellsInTableView(TableView *table)
{
	return 3;
}

void SelectLevelLayer::backToMainLayer(cocos2d::Ref* pSender)
{
	auto scene = TransitionSlideInL::create(0.8f, MainLayer::createScene());
	Director::getInstance()->replaceScene(scene);
}

LevelTableViewCell* LevelTableViewCell::createWithTableViewRect(Vec2 leftDownPos, Size rectSize)
{
	auto ret = new LevelTableViewCell();
	if (ret && ret->initWithTableViewRect(leftDownPos, rectSize))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}

	return ret;
}

bool LevelTableViewCell::initWithTableViewRect(Vec2 leftDownPos, Size rectSize)
{
	if (! TableViewCell::init())
		return false;

	auto level1 = MenuItemImage::create("border.png", "border.png", CC_CALLBACK_1(LevelTableViewCell::chooseLevel, this, 1));
	level1->setAnchorPoint(Vec2::ZERO);
	level1->setPosition(Vec2(0, 0));

	auto level2 = MenuItemImage::create("border.png", "border.png", CC_CALLBACK_1(LevelTableViewCell::chooseLevel, this, 2));
	level2->setAnchorPoint(Vec2::ZERO);
	level2->setPosition(Vec2(300, 0));

	auto level3 = MenuItemImage::create("border.png", "border.png", CC_CALLBACK_1(LevelTableViewCell::chooseLevel, this, 3));
	level3->setAnchorPoint(Vec2::ZERO);
	level3->setPosition(Vec2(600, 0));
	
	auto menu = ScrollMenu::createWithEffectiveRange(leftDownPos, rectSize);
	menu->addChild(level1, 1);
	menu->addChild(level2, 1);
	//menu->addChild(level3, 1);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	return true;
}

void LevelTableViewCell::chooseLevel(cocos2d::Ref* pSender, int index)
{
	log("%d\n", index);
}