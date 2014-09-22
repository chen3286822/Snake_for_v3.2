#ifndef __TOUCHTABLEVIEW_H__
#define __TOUCHTABLEVIEW_H__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"

class TouchTableView : public cocos2d::extension::TableView
{
public:
	static TouchTableView* createWithSize(cocos2d::extension::TableViewDataSource* dataSource, cocos2d::Size size);
	virtual bool initWithSize(cocos2d::Size size, cocos2d::Node* container = NULL);
	virtual bool onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent) override;
	virtual void onTouchMoved(cocos2d::Touch *pTouch, cocos2d::Event *pEvent) override;
	virtual void onTouchEnded(cocos2d::Touch *pTouch, cocos2d::Event *pEvent) override;
	virtual void onTouchCancelled(cocos2d::Touch *pTouch, cocos2d::Event *pEvent) override;

protected:
	cocos2d::Vec2 m_iStartPos;
	float m_fMoveDelta{ 10.0f };
};

#endif