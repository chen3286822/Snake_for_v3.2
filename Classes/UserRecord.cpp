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
	sqlite3 *pdb = NULL;
	auto path = FileUtils::getInstance()->getWritablePath() + "user.db";

	// open the db, create if not exist
	int result = sqlite3_open(path.c_str(), &pdb);
	if (result != SQLITE_OK)
	{
		log("open database failed,  number%d", result);
	}

	// check if the user table exist
	std::string sql = "SELECT COUNT(*) FROM sqlite_master where type='table' and name='UserRecord'";
	char **re; //query result
	int r, c;// row column
	sqlite3_get_table(pdb, sql.c_str(), &re, &r, &c, NULL);
	if (re == nullptr || r == 0)
	{
		// create the user table
		// ID  name   score
		sql = "create table UserRecord(ID integer primary key autoincrement,name text,score integer)";
		result = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);
		if (result != SQLITE_OK)
			log("create table failed");
	}

	// query the data
	sql = "select * where name='" + ID + "'";
	sqlite3_get_table(pdb, sql.c_str(), &re, &r, &c, NULL);
	for (int i = 1; i <= r; i++)
	{
		for (int j = 0; j < c; j++)
		{
			log("%s", re[i*c + j]);
		}
	}
	sqlite3_free_table(re);
}

void UserRecord::saveUserRecord(const std::string& ID)
{

}