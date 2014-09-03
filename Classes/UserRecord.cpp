#include "UserRecord.h"
#include "sqlite/sqlite3.h"

USING_NS_CC;

UserRecord* UserRecord::m_sUserRecord = nullptr;

UserRecord* UserRecord::getInstance()
{
	if (!m_sUserRecord)
	{
		m_sUserRecord = new (std::nothrow) UserRecord();
		CCASSERT(m_sUserRecord, "FATAL: Not enough memory");
		m_sUserRecord->init();
	}

	return m_sUserRecord;
}

void UserRecord::destroyInstance()
{
	CC_SAFE_DELETE(m_sUserRecord);
}

UserRecord::~UserRecord()
{
	destroyInstance();
}

bool UserRecord::init()
{
	m_strCurID = "";
	return true;
}

void UserRecord::changeUser(std::string ID)
{
	// save previous user's data
	if (!m_strCurID.empty())
	{
		saveUserRecord(m_strCurID);
	}

	m_strCurID = ID;

	if (!m_strCurID.empty())
	{
		loadUserRecord(m_strCurID);
	}
}

void UserRecord::loadUserRecord(const std::string& ID)
{
	auto userRecordID = "UserRecord" + ID;

	sqlite3 *pdb = NULL;
	auto path = FileUtils::getInstance()->getWritablePath() + "user.db";

	int result = sqlite3_open(path.c_str(), &pdb);
	if (result != SQLITE_OK)
	{
		log("open database failed,  number%d", result);
	}

	auto sql = "";
}

void UserRecord::saveUserRecord(const std::string& ID)
{

}