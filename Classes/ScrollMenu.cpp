#include "ScrollMenu.h"

USING_NS_CC;

ScrollMenu* ScrollMenu::createWithEffectiveRange(cocos2d::Vec2 leftDownPos, cocos2d::Size rectSize)
{
	auto ret = new ScrollMenu();
	if (ret && ret->initWithEffectiveRange(leftDownPos, rectSize))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}

	return ret;
}

bool ScrollMenu::initWithEffectiveRange(cocos2d::Vec2 leftDownPos, cocos2d::Size rectSize)
{
	if (!Layer::init())
		return false;

	_enabled = true;
	// menu in the center of the screen
	Size s = Director::getInstance()->getWinSize();

	this->ignoreAnchorPointForPosition(true);
	setAnchorPoint(Vec2(0.5f, 0.5f));
	this->setContentSize(s);

	setPosition(Vec2(s.width / 2, s.height / 2));


	_selectedItem = nullptr;
	_state = Menu::State::WAITING;

	// enable cascade color and opacity on menus
	setCascadeColorEnabled(true);
	setCascadeOpacityEnabled(true);


	auto touchListener = EventListenerTouchOneByOne::create();
	// here we do not swallow the touches, so parent tableview will get them
	touchListener->setSwallowTouches(false);

	touchListener->onTouchBegan = CC_CALLBACK_2(ScrollMenu::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(ScrollMenu::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(ScrollMenu::onTouchEnded, this);
	touchListener->onTouchCancelled = CC_CALLBACK_2(ScrollMenu::onTouchCancelled, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);


	m_iEffectiveRange.setRect(leftDownPos.x, leftDownPos.y, rectSize.width, rectSize.height);

	return true;
}

bool ScrollMenu::onTouchBegan(Touch* touch, Event* event)
{
	if (_state != Menu::State::WAITING || !_visible || !_enabled)
	{
		return false;
	}

	for (Node *c = this->_parent; c != nullptr; c = c->getParent())
	{
		if (c->isVisible() == false)
		{
			return false;
		}
	}

	m_iStartPos = touch->getLocation();
	if (m_iEffectiveRange.containsPoint(m_iStartPos))
	{
		m_bTouchMoved = false;
		_selectedItem = this->getItemForTouch(touch);
		if (_selectedItem)
		{
			_state = Menu::State::TRACKING_TOUCH;
			_selectedItem->selected();

			return true;
		}
	}


	return false;
}

void ScrollMenu::onTouchEnded(Touch* touch, Event* event)
{
	CCASSERT(_state == Menu::State::TRACKING_TOUCH, "[Menu ccTouchEnded] -- invalid state");
	this->retain();
	if (_selectedItem && !m_bTouchMoved)
	{
		_selectedItem->unselected();
		_selectedItem->activate();
	}
	_state = Menu::State::WAITING;
	this->release();
}

void ScrollMenu::onTouchCancelled(Touch* touch, Event* event)
{
	CCASSERT(_state == Menu::State::TRACKING_TOUCH, "[Menu ccTouchCancelled] -- invalid state");
	this->retain();
	if (_selectedItem && !m_bTouchMoved)
	{
		_selectedItem->unselected();
	}
	_state = Menu::State::WAITING;
	this->release();
}

void ScrollMenu::onTouchMoved(Touch* touch, Event* event)
{
	if (m_bTouchMoved)
		return;

	CCASSERT(_state == Menu::State::TRACKING_TOUCH, "[Menu ccTouchMoved] -- invalid state");
	MenuItem *currentItem = this->getItemForTouch(touch);

	Vec2 movePos = touch->getLocation();
	if (fabs(movePos.x - m_iStartPos.x) > m_fMoveDelta || fabs(movePos.y - m_iStartPos.y) > m_fMoveDelta)
	{
		if (_selectedItem)
			_selectedItem->unselected();
		m_bTouchMoved = true;


		return;
	}
	

	if (currentItem != _selectedItem)
	{
		if (_selectedItem)
		{
			_selectedItem->unselected();
		}
		_selectedItem = currentItem;
		if (_selectedItem)
		{
			_selectedItem->selected();
		}
	}
}