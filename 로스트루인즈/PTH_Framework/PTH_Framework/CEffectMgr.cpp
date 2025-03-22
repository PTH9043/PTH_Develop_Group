#include "pch.h"
#include "CEffectMgr.h"
#include "CGameMgr.h"
#include "CBlood.h"

CEffectMgr::CEffectMgr() : m_iBloodEffectIndex(0)
{
	Initialize();
}

CEffectMgr::CEffectMgr(CGameMgr * pGameMgr) : CManager(pGameMgr), m_iBloodEffectIndex(0)
{
	Initialize();
}

CEffectMgr::~CEffectMgr()
{
	Release();
}

void CEffectMgr::Initialize()
{
	for (int i = 0; i < EFCT_END; ++i)
	{
		m_vecEffectList[i].clear();
		m_vecEffectList[i].reserve(MAX_EFFECT_SIZE);
	}

	for (int i = 0; i < MAX_EFFECT_SIZE; ++i)
	{
		m_vecEffectList[EFCT_BLOOD].emplace_back(new CBlood());
		m_pGameMgr->Get_ObjMgr()->Add_Object(OBJ_EFFECT, m_vecEffectList[EFCT_BLOOD][i]);
	}
}

int CEffectMgr::Update()
{
	return 0;
}

void CEffectMgr::Release()
{
	m_pGameMgr->Get_ObjMgr()->Register_Remove_All(OBJ_EFFECT);
	for (int i = 0; i < EFCT_END; ++i)
	{
		for (auto& iter : m_vecEffectList[i])
			DELETE_MEMORY(iter);

		m_vecEffectList[i].clear();
	}
}

void CEffectMgr::Blood_Effect(CObj* pObj, const VECTOR2& v2Dir, const int& iIndex)
{
	m_vecEffectList[EFCT_BLOOD][m_iBloodEffectIndex]->Set_Dir(v2Dir);
	static_cast<CBlood*>(m_vecEffectList[EFCT_BLOOD][m_iBloodEffectIndex])->Set_Obj(pObj);
	m_vecEffectList[EFCT_BLOOD][m_iBloodEffectIndex]->Obj_Active(true);
	static_cast<CBlood*>(m_vecEffectList[EFCT_BLOOD][m_iBloodEffectIndex])->Set_BloodBmp(iIndex);
	m_iBloodEffectIndex++;
	if (m_iBloodEffectIndex >= MAX_EFFECT_SIZE)
		m_iBloodEffectIndex = 0;
}
