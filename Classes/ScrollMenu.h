#ifndef __SCROLLMENU_H__
#define __SCROLLMENU_H__

#include "cocos2d.h"

// a menu that will be invalid when scroll out of an effective range
// and it will deliver touches to others when it isn't accept them
class ScrollMenu : public cocos2d::Menu
{
public:
	static ScrollMenu* createWithEffectiveRange(cocos2d::Vec2 leftDownPos, cocos2d::Size rectSize);
	virtual bool initWithEffectiveRange(cocos2d::Vec2 leftDownPos, cocos2d::Size rectSize);

	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) override;
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;
	virtual void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event) override;
	virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event) override;
private:
	cocos2d::Rect m_iEffectiveRange;
	cocos2d::Vec2 m_iStartPos;
	bool m_bTouchMoved{ false };
	float m_fMoveDelta{ 10.0f };
};

#endif