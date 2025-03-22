#include "pch.h"
#include "CObjMgr.h"
#include "CObj.h"

CObjMgr::CObjMgr()
{
	Initialize();
}

CObjMgr::CObjMgr(CGameMgr *& pGameMgr) : CManager(pGameMgr)
{
	Initialize();
}

CObjMgr::~CObjMgr()
{
	Release();
}

void CObjMgr::Initialize()
{

}

int CObjMgr::Update()
{
	for (int i = 0; i < OBJ_END; ++i)
	{
		list<CObj*>::iterator it = m_ObjList[i].begin();
		for (; it != m_ObjList[i].end();)
		{
			if (true == (*it)->Is_Enable())
			{
				int iEvent = (*it)->Update();
				if (OBJ_DEADEVENT == iEvent)
				{
					DELETE_MEMORY((*it));
					it = m_ObjList[i].erase(it);
				}
				else
					++it;
			}
			else
				++it;
		}
	}
	return 0;
}

void CObjMgr::Late_Update()
{
	for (int i = 0; i < OBJ_END; ++i)
	{
		for (auto& iter : m_ObjList[i])
		{
			if (true == iter->Is_Enable())
				iter->Late_Update();
		}
	}
}

void CObjMgr::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	for (int i = 0; i < OBJ_END; ++i)
	{
		for (auto& iter : m_ObjList[i])
		{
			if(true == iter->Is_Enable())
				iter->Render(_hdc, v2ScrollPos);
		}
	}
}

void CObjMgr::Release()
{
	for (int i = 0; i < OBJ_END; ++i)
	{
		for (auto& iter : m_ObjList[i])
			DELETE_MEMORY(iter);

		m_ObjList[i].clear();
	}
}

void CObjMgr::Add_Object(const EOBJID & eID, CObj * pObj)
{
	if (eID >= OBJ_END || nullptr == pObj)
		return;

	m_ObjList[eID].push_back(pObj);
}

void CObjMgr::Clear_Obj(const EOBJID & eID)
{
	if (eID >= OBJ_END)
		return;

	for (auto& iter : m_ObjList[eID])
		DELETE_MEMORY(iter);

	m_ObjList[eID].clear();
}

void CObjMgr::Active_Obj(const EOBJID & eID, const bool& IsActive)
{
	list<CObj*>::iterator it = m_ObjList[eID].begin();
	for (; it != m_ObjList[eID].end(); ++it)
	{
		if (nullptr != (*it))
			(*it)->Set_Enable(IsActive);
	}
}

void CObjMgr::Active_All_Obj(const bool& bIsActive)
{
	for (int i = 0; i < OBJ_END; ++i)
	{
		list<CObj*>::iterator it = m_ObjList[i].begin();
		for (; it != m_ObjList[i].end(); ++it)
		{
			if(nullptr != (*it))
				(*it)->Set_Enable(bIsActive);
		}
	}
}

void CObjMgr::Register_Remove(const EOBJID & eID, CObj * pObj)
{
	list<CObj*>::iterator it = m_ObjList[eID].begin();
	for (; it != m_ObjList[eID].end(); ++ it)
	{
		if ((*it) == pObj)
		{
			m_ObjList[eID].erase(it);
			break;
		}
	}
}

void CObjMgr::Register_Remove_All(const EOBJID & eID)
{
	list<CObj*>::iterator it = m_ObjList[eID].begin();
	m_ObjList[eID].clear();
}

CObj * CObjMgr::Get_Shop()
{
	CObj* pObj = nullptr;
	for (auto& iter : m_ObjList[OBJ_WORLD])
	{
		if (iter->Get_ID() == ID_WORLDOBJ_SHOP)
		{
			pObj = iter;
			break;
		}
	}
	return pObj;
}
