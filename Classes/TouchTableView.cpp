#include "TouchTableView.h"

USING_NS_CC;
USING_NS_CC_EXT;

TouchTableView* TouchTableView::createWithSize(TableViewDataSource* dataSource, Size size)
{
	TouchTableView *table = new TouchTableView();
	table->initWithViewSize(size, nullptr);
	table->autorelease();
	table->setDataSource(dataSource);
	table->_updateCellPositions();
	table->_updateContentSize();

	return table;
}

bool TouchTableView::initWithSize(Size size, Node* container)
{
	if (TableView::initWithViewSize(size, container))
	{
		CC_SAFE_DELETE(_indices);
		_indices = new std::set<ssize_t>();
		_vordering = VerticalFillOrder::BOTTOM_UP;
		this->setDirection(Direction::VERTICAL);

		ScrollView::setDelegate(this);
		return true;
	}
	return false;
}

void TouchTableView::onTouchEnded(Touch *pTouch, Event *pEvent)
{
	if (!this->isVisible()) {
		return;
	}

	if (_touchedCell){
		Rect bb = this->getBoundingBox();
		bb.origin = _parent->convertToWorldSpace(bb.origin);

		if (bb.containsPoint(pTouch->getLocation()) && _tableViewDelegate != nullptr)
		{
			_tableViewDelegate->tableCellUnhighlight(this, _touchedCell);
			_tableViewDelegate->tableCellTouched(this, _touchedCell);
		}

		_touchedCell = nullptr;
	}

	TableView::onTouchEnded(pTouch, pEvent);
}

bool TouchTableView::onTouchBegan(Touch *pTouch, Event *pEvent)
{
	if (!this->isVisible())
	{
		return false;
	}

	m_iStartPos = pTouch->getLocation();
	bool touchResult = TableView::onTouchBegan(pTouch, pEvent);

	if (_touches.size() == 1)
	{
		long index;
		Vec2 point;

		point = this->getContainer()->convertTouchToNodeSpace(pTouch);

		index = this->_indexFromOffset(point);
		if (index == CC_INVALID_INDEX)
		{
			_touchedCell = nullptr;
		}
		else
		{
			_touchedCell = this->cellAtIndex(index);
		}

		if (_touchedCell && _tableViewDelegate != nullptr)
		{
			_tableViewDelegate->tableCellHighlight(this, _touchedCell);
		}
	}
	else if (_touchedCell)
	{
		if (_tableViewDelegate != nullptr)
		{
			_tableViewDelegate->tableCellUnhighlight(this, _touchedCell);
		}

		_touchedCell = nullptr;
	}

	return touchResult;
}

void TouchTableView::onTouchMoved(Touch *pTouch, Event *pEvent)
{
	Vec2 movePos = pTouch->getLocation();
	if (fabs(movePos.x - m_iStartPos.x) < m_fMoveDelta && fabs(movePos.y - m_iStartPos.y) < m_fMoveDelta)
	{
		return;
	}

	TableView::onTouchMoved(pTouch, pEvent);

	if (_touchedCell && isTouchMoved())
	{
		if (_tableViewDelegate != nullptr)
		{
			_tableViewDelegate->tableCellUnhighlight(this, _touchedCell);
		}

		_touchedCell = nullptr;
	}
}

void TouchTableView::onTouchCancelled(Touch *pTouch, Event *pEvent)
{
	TableView::onTouchCancelled(pTouch, pEvent);

	if (_touchedCell)
	{
		if (_tableViewDelegate != nullptr)
		{
			_tableViewDelegate->tableCellUnhighlight(this, _touchedCell);
		}

		_touchedCell = nullptr;
	}
}