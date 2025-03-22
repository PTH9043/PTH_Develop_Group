#pragma once
#include "CManager.h"

class CObj;

class CObjMgr final : public CManager
{ 
public:
	explicit CObjMgr();
	explicit CObjMgr(CGameMgr*& pGameMgr);
	virtual ~CObjMgr();
protected:
	list<CObj*>	m_ObjList[OBJ_END];
public:
	// CManager을(를) 통해 상속됨
	virtual void			Initialize() override;
	virtual int				Update() override;
	void						Late_Update();
	void						Render(HDC _hdc, const VECTOR2& v2ScrollPos);
	virtual void			Release() override;

	void						Add_Object(const EOBJID& eID, CObj* pObj);
	// Clear Inner Obj
	void						Clear_Obj(const EOBJID& eID);

	void						Active_Obj(const EOBJID& eID, const bool& IsActive);

	void						Active_All_Obj(const bool& bIsActive);

	void						Register_Remove(const EOBJID& eID, CObj* pObj);

	void						Register_Remove_All(const EOBJID& eID);
public:
	CObj*					Get_Player() { return m_ObjList[OBJ_PLAYER].front(); }
	CObj*					Get_Shop();
	list<CObj*>			Get_ObjList(const EOBJID& eID) { return m_ObjList[eID]; }
};

