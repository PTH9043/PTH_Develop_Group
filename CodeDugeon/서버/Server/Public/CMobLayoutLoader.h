#ifndef _SERVERFRAMEWORK_SERVER_PUBLIC_CMOBLAYOUTLOADER_H
#define _SERVERFRAMEWORK_SERVER_PUBLIC_CMOBLAYOUTLOADER_H

#include "ACoreBase.h"

BEGIN(Server)

class CMobLayoutLoader {
public:
	CMobLayoutLoader(const _string& _strPath);
public:
	VECTOR<MOBDATA>& GetMobData() { return m_MobData; }
private:
	VECTOR< MOBDATA>			m_MobData;
};

class CMobServerLayoutLoader {
public:
	CMobServerLayoutLoader(const _string& _strPath);
public:
	VECTOR<MOBSERVERDATA>& GetMobData() { return m_MobData; }
private:
	VECTOR< MOBSERVERDATA>			m_MobData;
};

END

#endif // _SERVERFRAMEWORK_SERVER_PUBLIC_CMOBLAYOUTLOADER_H