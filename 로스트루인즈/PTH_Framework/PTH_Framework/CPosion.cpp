#include "pch.h"
#include "CPosion.h"
#include "CPlayer.h"

CPosion::CPosion() : m_iRecoverValue(0)
{
}

CPosion::CPosion(CObj * pOwner, const TCHAR * szImgPath, const wstring & strName, const EPOSIONTYPE& ePosionType, 
	const int& iGold, const int& iRecoverValue)
	: CItem(pOwner, szImgPath, strName), m_ePosionType(ePosionType),m_iRecoverValue(iRecoverValue)
{
	m_iGold = iGold;
}

CPosion::~CPosion()
{
	Release();
}

void CPosion::Initialize()
{
}

int CPosion::Update()
{
	return 0;
}

void CPosion::Late_Update()
{
}

void CPosion::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
}

void CPosion::Collision_Actor(CObj *& pObj)
{
}

void CPosion::Release()
{
}

CItem * CPosion::Copy_Item()
{
	CPosion* pPosion = new  CPosion(m_pOwner, m_szImgPath, m_strName,m_ePosionType, m_iGold, m_iRecoverValue);
	pPosion->Set_Name(m_strName);
	return pPosion;
}


void CPosion::Update_Rect()
{
}
