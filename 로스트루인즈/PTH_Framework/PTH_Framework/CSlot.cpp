#include "pch.h"
#include "CSlot.h"
#include "CGameMgr.h"
#include "CBmpMgr.h"

CSlot::CSlot() : m_pOwner(nullptr), m_pSlotImg(nullptr), m_iIndex(0)
{
}

CSlot::CSlot(CObj * pOwner, const TCHAR * szSlotPath, const RGBINFO& tInfo)
	: m_pOwner(pOwner), m_pSlotImg(nullptr), m_tTranspercyRGB(tInfo), m_iIndex(0)
{
	// 이미지 가져옴
	m_pSlotImg = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(szSlotPath);
}

CSlot::~CSlot()
{
}

void CSlot::Initialize()
{
}

int CSlot::Update()
{
	return OBJ_NOEVENT;
}

void CSlot::Late_Update()
{
}

void CSlot::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{

}

void CSlot::Collision_Actor(CObj *& pObj)
{
}

void CSlot::Release()
{
}
