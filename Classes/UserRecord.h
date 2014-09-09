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
	void addScore(int var);
	void setScore(int var, bool needUpdate = true);
	int getScore();

	// save data
	void saveData();
private:
	static UserRecord* m_sUserRecord;

	void loadUserRecord(const std::string& ID);
	void saveUserRecord(const std::string& ID);

	void setUpDB();
	bool m_bSetupDone{ false };

	int m_nScore;
};

#endif // !__USERRECORD_H__
