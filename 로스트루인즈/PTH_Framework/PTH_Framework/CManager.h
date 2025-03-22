#pragma once
#include "Include.h"

class CGameMgr;

class CManager abstract
{
protected:
	explicit CManager();
	explicit CManager(CGameMgr*& pGameMgr);
	virtual ~CManager();

protected:
	CGameMgr* m_pGameMgr;

public:
	virtual void Initialize() PURE;
	virtual int Update() PURE;
	virtual void Release() PURE;
};

