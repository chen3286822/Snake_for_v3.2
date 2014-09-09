#include "UserRecord.h"
#include "sqlite/sqlite3.h"
#include "MapLayer.h"

USING_NS_CC;

#define CLOSE_DB(db) sqlite3_close(db); \
	db = nullptr;

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
	m_nScore = 0;

	setUpDB();
	return true;
}

void UserRecord::setUpDB()
{
	sqlite3 *pdb = NULL;
	auto path = FileUtils::getInstance()->getWritablePath() + "user.db";

	// open the db, create if not exist
	int result = sqlite3_open(path.c_str(), &pdb);
	if (result != SQLITE_OK)
	{
		log("open database failed,  number%d", result);
		m_bSetupDone = false;
		return;
	}

	// create table if not exists
	std::string sql = "create table if not exists UserRecord(ID integer primary key autoincrement,name text,score integer)";
	result = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);
	if (result != SQLITE_OK)
	{
		log("create table failed");
		m_bSetupDone = false;
		return;
	}

	// create unique index
	sql = "create unique index if not exists unique_name on UserRecord(name)";
	result = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);
	if (result != SQLITE_OK)
	{
		log("create unique index failed");
		m_bSetupDone = false;
		return;
	}

	CLOSE_DB(pdb);
	m_bSetupDone = true;
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
	if (!m_bSetupDone)
		return;

	sqlite3 *pdb = NULL;
	auto path = FileUtils::getInstance()->getWritablePath() + "user.db";

	// open the db, create if not exist
	int result = sqlite3_open(path.c_str(), &pdb);
	if (result != SQLITE_OK)
	{
		log("open database failed,  number%d", result);
		return;
	}

	// query the data
	std::string sql = "select * from UserRecord where name='" + ID + "'";
	sqlite3_exec(pdb, sql.c_str(), readUserData, NULL, NULL);
	CLOSE_DB(pdb);
}

int readUserData(void* pv, int argc, char** argv, char** col)
{
	for (int i = 0; i < argc; ++i)
	{
		if (stricmp(col[i], "score") == 0)
			UserRecord::getInstance()->setScore(atoi(argv[i]));
	}

	return 1;
}

void UserRecord::saveUserRecord(const std::string& ID)
{
	if (!m_bSetupDone)
		return;

	sqlite3 *pdb = NULL;
	auto path = FileUtils::getInstance()->getWritablePath() + "user.db";

	// open the db, create if not exist
	int result = sqlite3_open(path.c_str(), &pdb);
	if (result != SQLITE_OK)
	{
		log("open database failed,  number%d", result);
		return;
	}

	//save data
	char sql[256];
	sprintf(sql, "replace into UserRecord(name, score) values('%s', %d)", ID.c_str(), m_nScore);
	result = sqlite3_exec(pdb, sql, NULL, NULL, NULL);
	if (result != SQLITE_OK)
	{
		log("insert data failed");
		m_bSetupDone = false;
		return;
	}
	CLOSE_DB(pdb);
}

void UserRecord::saveData()
{
	saveUserRecord(m_strCurID);
}

// property set get
void UserRecord::addScore(int var)
{
	int tmp = var + m_nScore;
	setScore(tmp);
}

void UserRecord::setScore(int var, bool needUpdate)
{
	m_nScore = var;

	if (needUpdate)
	{
		// inform the UI for the data changing
		auto scene = Director::getInstance()->getRunningScene();
		if (scene)
		{
			auto layer = dynamic_cast<SnakeMapLayer*>(scene->getChildByTag(eID_SnakeMap));
			if (layer)
			{
				layer->updateUIData(eID_ScoreLabel);
			}
		}
	}
}

int UserRecord::getScore()
{
	return m_nScore;
}