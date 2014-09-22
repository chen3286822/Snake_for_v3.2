#ifndef __SELECTLEVELLAYER_H__
#define __SELECTLEVELLAYER_H__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"

class SelectLevelLayer : public cocos2d::Layer, public cocos2d::extension::TableViewDataSource, public cocos2d::extension::TableViewDelegate
{
public:
	static cocos2d::Scene* createScene();
	CREATE_FUNC(SelectLevelLayer);

	virtual bool init() override;
	virtual void onEnterTransitionDidFinish() override;

	virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view) {};
	virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view) {}
	virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell);
	virtual cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView *table, ssize_t idx);
	virtual cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx);
	virtual ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table);

private:
	void backToMainLayer(cocos2d::Ref* pSender);
	cocos2d::Vec2 m_iTableViewLeftDownPos;
	cocos2d::Size m_iTableViewSize;
	static const int m_snCellNum = 30;
	// if the cell appeared before, if true, reset the cell variable m_bMoved
	bool m_bAppearBefore[m_snCellNum];
};


class LevelTableViewCell : public cocos2d::extension::TableViewCell
{
public:
	static LevelTableViewCell* createWithTableViewRect(cocos2d::Vec2 leftDownPos, cocos2d::Size rectSize);
	virtual bool initWithTableViewRect(cocos2d::Vec2 leftDownPos, cocos2d::Size rectSize);
	virtual void onEnter() override;
	void resetAppeared();
private:
	void chooseLevel(cocos2d::Ref* pSender, int index);
	cocos2d::Menu* m_pMenu{ nullptr };
	static const int m_snItemNum = 3;
	cocos2d::MenuItemImage* m_pItems[m_snItemNum];
	// save the states that if menu items ever moved before
	bool m_bAppeared[m_snItemNum];
};

#endif