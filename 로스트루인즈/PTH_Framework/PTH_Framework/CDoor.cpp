#include "pch.h"
#include "CDoor.h"
#include "CAnimation.h"
#include "CAnimation_L.h"
#include "CAnimClip.h"
#include "CAnimator.h"
#include "CCharacter.h"
#include "CGameMgr.h"

CDoor::CDoor() : m_pAnimator(nullptr), m_bUseKey(false), m_iState(0)
{
	Initialize();
}

CDoor::~CDoor()
{
	Release();
}

void CDoor::Initialize()
{
	m_v2Size = VECTOR2(40.f, 170.f);
	m_v2DrawSize = VECTOR2(170.f, 170.f);
	Animatior();
}

int CDoor::Update()
{
	if (!m_bUseKey)
	{
		m_pAnimator->Get_Animation()->Set_AnimationIndex(0);
		m_pAnimator->Get_Animation()->Set_AnimFix(true);
	}
	else
		m_pAnimator->Get_Animation()->Set_LastAnimFix(true);
	Update_Rect();
	Fall();
	if (nullptr != m_pAnimator)
	{
		m_pAnimator->Update_Animation();
	}
	return 0;
}

void CDoor::Late_Update()
{
	for (auto& iter : CGameMgr::Get_Instance()->Get_ObjMgr()->Get_ObjList(OBJ_MONSTER))
		Collision_Actor(iter);
	for (auto& iter : CGameMgr::Get_Instance()->Get_ObjMgr()->Get_ObjList(OBJ_PLAYER))
		Collision_Actor(iter);
}

void CDoor::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	if (nullptr != m_pAnimator)
	{
		m_pAnimator->Render_Animation(_hdc, v2ScrollPos);
	}
}

void CDoor::Collision_Actor(CObj *& pObj)
{
	if (nullptr == pObj)
		return;
	CCharacter* pCharacter = dynamic_cast<CCharacter*>(pObj);
	if (nullptr == pCharacter)
		return;

	RECT rc;
	if (FALSE == IntersectRect(&rc, &m_tRect, &pCharacter->Get_Rect()))
	{
		if (1 == m_iState)
		{
			if (OBJ_PLAYER == pCharacter->Get_ObjID())
			{
				if (false == pCharacter->Is_CollDoor())
					pCharacter->Set_CollDoor(true);
			}
			return;
		}
		else
		{
			if (OBJ_PLAYER == pCharacter->Get_ObjID())
			{
				pCharacter->Set_CollDoor(false);
			}
			return;
		}

		if (OBJ_MONSTER == pCharacter->Get_ObjID())
		{
			pCharacter->Set_CollDoor(false);
		}
		return;
	}

	if (OBJ_PLAYER == pCharacter->Get_ObjID())
	{
		if (true == m_bUseKey)
		{

			m_iState = 1;
			return;
		}
	}

	if (false == pCharacter->Is_CollDoor())
		pCharacter->Set_CollDoor(true);
}

void CDoor::Release()
{
	DELETE_MEMORY(m_pAnimator);
}

void CDoor::Update_Rect()
{
	VECTOR2 v2RealSize = (m_v2Size) / 2.0f;
	// Collision Size
	m_tRect.left = static_cast<long>(m_v2CurPos.x - m_v2DrawSize.x / 2.f);
	m_tRect.top = static_cast<long>(m_v2CurPos.y - m_v2Size.y);
	m_tRect.right = static_cast<long>(m_v2CurPos.x + v2RealSize.x);
	m_tRect.bottom = static_cast<long>(m_v2CurPos.y);
	// CenterPos, Left, Right
	m_v2CenterPos = VECTOR2(m_v2CurPos.x, m_v2CurPos.y - v2RealSize.y);
	// Draw Size
	v2RealSize = m_v2DrawSize / 2.f;
	m_tDrawRect.left = static_cast<long>(m_v2CurPos.x - v2RealSize.x);
	m_tDrawRect.top = static_cast<long>(m_v2CurPos.y - m_v2DrawSize.y);
	m_tDrawRect.right = static_cast<long>(m_v2CurPos.x + v2RealSize.x);
	m_tDrawRect.bottom = static_cast<long>(m_v2CurPos.y);
}

void CDoor::Animatior()
{
	CObj* pObj = this;
	m_pAnimator = new CAnimator(pObj);
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"DOOR");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(200);
	for (int i = 0; i < 7; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 200), 0, }, POINT{ 200, 200 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"DOOR", pAnimation);
	m_pAnimator->Set_CurAnimation(L"DOOR");
}
