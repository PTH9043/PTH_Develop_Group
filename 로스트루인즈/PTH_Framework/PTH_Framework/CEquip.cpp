#include "pch.h"
#include "CEquip.h"

CEquip::CEquip() : m_eEquipID(EQUIP_END), m_fDistance(0.f)
{
	m_eItemID = ITEM_EQUIP;
}

CEquip::CEquip(CObj * pOwner, const TCHAR * szImgPath, const wstring& strName) :
	CItem(pOwner, szImgPath, strName), m_eEquipID(EQUIP_END), m_fDistance(0.f)
{
	m_eItemID = ITEM_EQUIP;
}

CEquip::~CEquip()
{
}
