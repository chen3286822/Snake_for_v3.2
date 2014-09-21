#include "SelectLevelLayer.h"
#include "GameDefine.h"
#include "MainLayer.h"

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
	auto levelView = TableView::create(this, VisibleRect::getVisibleRect().size - Size(60.0f, 60.0f));
	levelView->setDirection(ScrollView::Direction::VERTICAL);
	levelView->setPosition(Vec2(30, 0));
	levelView->setDelegate(this);
	this->addChild(levelView, 1, eID_LevelView);
	levelView->reloadData();
}

void SelectLevelLayer::tableCellTouched(TableView* table, TableViewCell* cell)
{
	CCLOG("cell touched at index: %ld", cell->getIdx());
}

Size SelectLevelLayer::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	return Size(900, 300);
}

TableViewCell* SelectLevelLayer::tableCellAtIndex(TableView *table, ssize_t idx)
{
	auto string = String::createWithFormat("%ld", idx);
	TableViewCell *cell = table->dequeueCell();
	if (!cell) {
		cell = new TableViewCell();
		cell->autorelease();
		auto sprite = Sprite::create("next.png");
		sprite->setAnchorPoint(Vec2::ZERO);
		sprite->setPosition(Vec2(0, 0));
		cell->addChild(sprite);

		auto label = Label::createWithSystemFont(string->getCString(), "Helvetica", 20.0);
		label->setPosition(Vec2::ZERO);
		label->setAnchorPoint(Vec2::ZERO);
		label->setTag(123);
		cell->addChild(label);
	}
	else
	{
		auto label = (Label*)cell->getChildByTag(123);
		label->setString(string->getCString());
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