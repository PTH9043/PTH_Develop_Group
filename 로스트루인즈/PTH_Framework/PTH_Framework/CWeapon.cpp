#include "pch.h"
#include "CWeapon.h"

CWeapon::CWeapon() : m_eWeaponID(WEAP_END)
{
	m_eEquipID = EQUIP_WEAPON;
	m_iID = ID_ITEM;
}

CWeapon::CWeapon(CObj * pOwner, const TCHAR * szImgPath, const wstring& strName)
	: CEquip(pOwner, szImgPath, strName), m_eWeaponID(WEAP_END)
{
	m_eEquipID = EQUIP_WEAPON;
	m_iID = ID_ITEM;
}

CWeapon::~CWeapon()
{
}

void CWeapon::Initialize()
{
}

int CWeapon::Update()
{
	Update_Rect();
	return 0;
}

void CWeapon::Late_Update()
{
}

void CWeapon::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
}

void CWeapon::Collision_Actor(CObj *& pObj)
{
}

void CWeapon::Release()
{
}


void CWeapon::Rotate()
{
}
