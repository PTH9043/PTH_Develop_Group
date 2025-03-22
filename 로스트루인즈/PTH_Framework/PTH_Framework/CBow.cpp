#include "pch.h"
#include "CBow.h"
#include "CArrow.h"
#include"CGameMgr.h"

CBow::CBow() : m_iShootSpeed(0)
{
}

CBow::CBow(CObj * pOwner, const TCHAR * szImgPath, const wstring & strName)
	: CEquip(pOwner, szImgPath, strName), m_iShootSpeed(10)
{
	Initialize();
}

CBow::~CBow()
{
	Release();
}

void CBow::Initialize()
{
	m_iGold = 150;
	m_v2DrawSize = VECTOR2(180.f, 180.f);
	m_v2Size = VECTOR2(30.f, 60.f);
	m_eObjID = OBJ_ITEM;
}

int CBow::Update()
{
	return 0;
}

void CBow::Late_Update()
{
}

void CBow::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
}

void CBow::Collision_Actor(CObj *& pObj)
{
}

void CBow::Release()
{
}

void CBow::Shoot_Arrow()
{
	if (nullptr != m_pOwner && true == m_bShoot)
	{
		CArrow* pArrow = new CArrow(this, L"Arrow_1", L"Arrow");
		pArrow->Set_Direction(static_cast<CActor*>(m_pOwner)->Get_Direction());
		VECTOR2 v2CenterPos = m_pOwner->Get_CenterPos();
		VECTOR2 v2Dir = pArrow->Get_Dir();
		pArrow->Set_CenterPos(v2CenterPos + VECTOR2(0.f, -5.f) + v2Dir * VECTOR2(10.f, 0.f));
		pArrow->Set_Speed(m_iShootSpeed);
		pArrow->Set_Owner(m_pOwner);
		pArrow->Set_DrawSize(VECTOR2(100.f, 100.f));
		pArrow->Obj_Active(true);
		CGameMgr::Get_Instance()->Get_ObjMgr()->Add_Object(OBJ_ITEM, pArrow);
		m_iShootSpeed = 10;

		CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"BOW_Release_Arrow_mono.wav", SOUND_PLAYER, g_fSound);
	}
}

void CBow::Update_Shoot_Speed()
{
	++m_iShootSpeed;
	if (25 < m_iShootSpeed)
		m_iShootSpeed = 25;
}

CItem * CBow::Copy_Item()
{
	CBow* pBow = new  CBow(m_pOwner, m_szImgPath, m_strName);
	pBow->Set_Name(m_strName);
	return pBow;
}

