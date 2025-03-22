#include "pch.h"
#include "CManager.h"

CManager::CManager() : m_pGameMgr(nullptr)
{
}

CManager::CManager(CGameMgr *& pGameMgr) : m_pGameMgr(pGameMgr)
{
}

CManager::~CManager()
{
}