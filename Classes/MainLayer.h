#ifndef __MAINLAYER_H__
#define __MAINLAYER_H__

#include "cocos2d.h"

class MainLayer : public cocos2d::LayerColor
{
public:
	static cocos2d::Scene* createScene();
	CREATE_FUNC(MainLayer);

	virtual bool init() override;

private:
	void goSelectLevelScene(cocos2d::Ref* pSender);
};

#endif