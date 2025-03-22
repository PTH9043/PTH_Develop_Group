#pragma once
#include "CItem.h"

class CEquip abstract : public CItem
{
public:
	explicit CEquip();
	explicit CEquip(CObj* pOwner, const TCHAR* szImgPath, const wstring& strName);
	virtual ~CEquip();
protected:
	EEQUIPID						m_eEquipID;
	STATUS						m_tStatus;
	float								m_fDistance;
public:
	// 
	virtual void Initialize() PURE;
	virtual int Update() PURE;
	virtual void Late_Update() PURE;
	virtual void Render(HDC _hdc, const VECTOR2& v2ScrollPos) PURE;
	virtual void Collision_Actor(CObj*& pObj) PURE;
	virtual void Release() PURE;
protected:
	virtual void Update_OnwerInfo(){}
public:
	// get 
	const EEQUIPID& Get_EquipID() const { return m_eEquipID; }
	const STATUS& Get_Status() const { return m_tStatus; }
	// set 
	void Set_EquipID(const EEQUIPID& eEquipType) { this->m_eEquipID = eEquipType; }
	void Set_Status(STATUS& tStatus) { this->m_tStatus = tStatus; }
};

