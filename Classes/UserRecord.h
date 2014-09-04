#ifndef __USERRECORD_H__
#define __USERRECORD_H__

#include "cocos2d.h"

int readUserData(void* pv, int argc, char** argv, char** col);

class UserRecord : public cocos2d::Ref
{
public:
	static UserRecord* getInstance();
	static void destroyInstance();

	virtual ~UserRecord();
	virtual bool init();

	// set user ID and load his/her data
	void changeUser(std::string ID);

	//current userID
	CC_SYNTHESIZE(std::string, m_strCurID, curUserID);

	//score
	CC_SYNTHESIZE(int, m_nScore, Score);
private:
	static UserRecord* m_sUserRecord;

	void loadUserRecord(const std::string& ID);
	void saveUserRecord(const std::string& ID);

	void setUpDB();
	bool m_bSetupDone{ false };
};

#endif // !__USERRECORD_H__
