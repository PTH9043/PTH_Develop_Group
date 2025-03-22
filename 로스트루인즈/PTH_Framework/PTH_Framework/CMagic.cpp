#include "pch.h"
#include "CMagic.h"



CMagic::CMagic()
{
}

CMagic::CMagic(CObj * pOwner, const TCHAR * szImgPath, const wstring & strName, const int & iGold)
	: CEquip(pOwner, szImgPath, strName)
{
	m_iGold = iGold;
}

CMagic::~CMagic()
{
	Release();
}

void CMagic::Initialize()
{
	m_v2DrawSize = VECTOR2(180.f, 180.f);
	m_v2Size = VECTOR2(30.f, 60.f);
	m_eObjID = OBJ_EQUIP;
	m_iShootSpeed = 20;
}

int CMagic::Update()
{
	return 0;
}

void CMagic::Late_Update()
{
}

void CMagic::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
}

void CMagic::Collision_Actor(CObj *& pObj)
{
}

void CMagic::Release()
{
}


CItem * CMagic::Copy_Item()
{
	return nullptr;
}
