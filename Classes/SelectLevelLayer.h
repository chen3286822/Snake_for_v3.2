#ifndef __SELECTLEVELLAYER_H__
#define __SELECTLEVELLAYER_H__

#include "cocos2d.h"
#include "cocos-ext.h"

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
};

#endif