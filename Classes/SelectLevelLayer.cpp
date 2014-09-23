#include "SelectLevelLayer.h"
#include "GameDefine.h"
#include "MainLayer.h"
#include "ScrollMenu.h"
#include "TouchTableView.h"

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
	backItem->setScale(0.8f);
	backItem->setPosition(VisibleRect::leftTop() + Vec2(-5, 5));
	auto menu = Menu::create(backItem, nullptr);
	this->addChild(menu, 1);
	menu->setPosition(Vec2::ZERO);

	// add up and down hint arrows
	auto upSprite = Sprite::create("up.png");
	upSprite->setScale(0.6f);
	upSprite->setPosition(VisibleRect::top() - Vec2(0, 50));
	this->addChild(upSprite, 1);

	auto downSprite = Sprite::create("down.png");
	downSprite->setScale(0.6f);
	downSprite->setPosition(VisibleRect::bottom() + Vec2(0, 25));
	this->addChild(downSprite, 1);

	for (int i = 0; i < m_snCellNum; ++i)
	{
		m_bAppearBefore[i] = false;
	}

	return true;
}

void SelectLevelLayer::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();

	// read level numbers
	std::vector<std::string> vLevelFiles;
	std::string indexFullPath = FileUtils::getInstance()->fullPathForFilename("map/index");
	std::string filePath = indexFullPath.substr(0, indexFullPath.size() - 6);
	traversalFolder(vLevelFiles, filePath.c_str(), "json");
	m_nLevelNums = vLevelFiles.size();
	if (m_nLevelNums == 0)
		m_nTableCells = 0;
	else
		m_nTableCells = (m_nLevelNums - 1) / 3 + 1;

	// create the level table view
	m_iTableViewLeftDownPos = Vec2(30, 40);
	m_iTableViewSize = VisibleRect::getVisibleRect().size - Size(60.0f, 128.0f);
	auto levelView = TouchTableView::createWithSize(this, m_iTableViewSize);
	levelView->setDirection(ScrollView::Direction::VERTICAL);
	levelView->setPosition(m_iTableViewLeftDownPos);
	levelView->setDelegate(this);
	this->addChild(levelView, 1, eID_LevelView);
	//levelView->reloadData();
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
		// the cell appears
		m_bAppearBefore[idx] = true;
	}
	else
	{
		// reset the LevelTableViewCell's variable m_bMoved if the idx cell never appears before
		if (idx < m_snCellNum && !m_bAppearBefore[idx])
		{
			auto levelTableViewCell = dynamic_cast<LevelTableViewCell*>(cell);
			if (levelTableViewCell)
			{
				levelTableViewCell->resetAppeared();
			}
			m_bAppearBefore[idx] = true;
		}
	}

	return cell;
}

ssize_t SelectLevelLayer::numberOfCellsInTableView(TableView *table)
{
	return m_nTableCells;
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

	m_pMenu = ScrollMenu::createWithEffectiveRange(leftDownPos, rectSize);
	for (int i = 0; i < m_snItemNum; ++i)
	{
		m_pItems[i] = MenuItemImage::create("border.png", "border.png", CC_CALLBACK_1(LevelTableViewCell::chooseLevel, this, i));
		m_pItems[i]->setAnchorPoint(Vec2::ZERO);
		m_pItems[i]->setPosition(Vec2(975 + 300*i, 20));
		m_pMenu->addChild(m_pItems[i], 1);
		m_bAppeared[i] = false;
	}
	m_pMenu->setPosition(Vec2::ZERO);
	this->addChild(m_pMenu, 1);

	return true;
}

void LevelTableViewCell::resetAppeared()
{
	for (int i = 0; i < m_snItemNum; ++i)
	{
		m_bAppeared[i] = false;
	}
}

void LevelTableViewCell::onEnter()
{
	TableViewCell::onEnter();

	for (int i = 0; i < m_snItemNum; ++i)
	{
		if (m_pItems[i])
		{
			auto moveAction = m_pItems[i]->getActionByTag(eID_LevelViewItemAction1 + i);
			if ((moveAction && !moveAction->isDone()) || m_bAppeared[i])
			{
				//reset the position if the action didn't "done" because the cell scrolls too fast
				m_pItems[i]->setPosition(Vec2(15 + 300 * i, 20));
				continue;;
			}
			m_pItems[i]->setPosition(Vec2(975 + 300*i, 20));
			ActionInterval* newAction = MoveBy::create(0.8f + 0.2*i, Vec2(-960, 0));
			ActionInterval *easeElasticOut = CCEaseExponentialOut::create(newAction);
			easeElasticOut->setTag(eID_LevelViewItemAction1 + i);
			m_pItems[i]->runAction(easeElasticOut);
			m_bAppeared[i] = true;
		}
	}
}

void LevelTableViewCell::chooseLevel(cocos2d::Ref* pSender, int index)
{
	log("%d\n", index);
}