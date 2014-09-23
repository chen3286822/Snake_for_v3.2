#include "GameDefine.h"
#include "cocos2d.h"

eDirection randomDirection()
{
	auto index = (int)(rand() % 4);
	cocos2d::log("%d", index);
	if (index == 0)
		return eDir_Up;
	else if (index == 1)
		return eDir_Right;
	else if (index == 2)
		return eDir_Down;
	else
		return eDir_Left;
}

float arcByDirection(eDirection dir)
{
	auto arc = 0.0f;
	if (dir == eDir_Right)
		arc = 90.0f;
	else if (dir == eDir_Down)
		arc = 180.0f;
	else if (dir == eDir_Left)
		arc = 270.0f;
	return arc;
}

eDirection oppositeDirection(eDirection dir)
{
	if (dir == eDir_Up)
		return eDir_Down;
	else if (dir == eDir_Down)
		return eDir_Up;
	else if (dir == eDir_Left)
		return eDir_Right;
	else if (dir == eDir_Right)
		return eDir_Left;
	return eDir_None;
}

void traversalFolder(std::vector<std::string>& fileNames, std::string folderPath, std::string ext)
{
#ifdef WIN32
 	_finddata_t FileInfo;
 	std::string strfind = folderPath + "\\*";
 	long Handle = _findfirst(strfind.c_str(), &FileInfo);
 
 	if (Handle == -1L)
 	{
 		return;
 	}
 	do{
 		// check if has child folder
 		if (FileInfo.attrib & _A_SUBDIR)
 		{
 			if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0))
 			{
 				std::string newPath = folderPath + "\\" + FileInfo.name;
 				traversalFolder(fileNames, newPath, ext);
 			}
 		}
 		else
 		{
 			std::string filename = (folderPath + "\\" + FileInfo.name);
 			if (!ext.empty())
 			{
 				char fileExt[20];
 				size_t len = strlen(FileInfo.name);
 				strncpy(fileExt, FileInfo.name + (len - ext.size()), ext.size());
 				fileExt[ext.size()] = '\0';
 				if (stricmp(ext.c_str(), fileExt) == 0)
 				{
 					fileNames.push_back(filename);
 				}
 			}
 			else
 				fileNames.push_back(filename);		
 		}
 	} while (_findnext(Handle, &FileInfo) == 0);
 
 	_findclose(Handle);
 #else
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	if ((dp = opendir(folderPath.c_str())) == NULL)
	{
		return;
	}
	chdir(folderPath.c_str());
	while ((entry = readdir(dp)) != NULL)
	{
		lstat(entry->d_name, &statbuf);
		if (S_ISDIR(statbuf.st_mode))
		{

			if (strcmp(".", entry->d_name) == 0 ||
				strcmp("..", entry->d_name) == 0)
				continue;
			traversalFolder(fileNames, entry->d_name, ext);
		}
		else
		{
			std::string filename = entry->d_name;
			if (!ext.empty())
			{
				if (stricmp(ext.c_str(), entry->d_type) == 0)
				{
					fileNames.push_back(filename);
				}
			}
			else
				fileNames.push_back(filename);
		}
	}
	chdir("..");
	closedir(dp);
#endif
}