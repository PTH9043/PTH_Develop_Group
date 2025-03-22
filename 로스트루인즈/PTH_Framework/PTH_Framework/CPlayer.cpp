#include "pch.h"
#include "CPlayer.h"
#include "CGameMgr.h"
#include "CInputMgr.h"
#include "CScrollMgr.h"
#include "CFloor.h"
#include "CScene.h"
#include "CSceneMgr.h"
#include "CLineMgr.h"
#include "CAnimation.h"
#include "CAnimation_L.h"
#include "CAnimClip.h"
#include"CAnimator.h"
#include "CInventory.h"
#include "CWall.h" 
#include "CEquip.h"
#include "CAnimClip_N.h"
#include "CSword.h"
#include "CBow.h"

CPlayer::CPlayer() : m_ePlayerState(PL_END), m_v2EquipPos(VECTOR2())
, m_pInventory(nullptr), m_iAttackIndex(0), m_tHurtTimer(TIMER(200)), m_bRealDead(false), m_bPressTab(false)
, m_bOpenShop(false), m_iDamageSoundIndex(0)
{
	m_vecEquipList.resize(4);
}

CPlayer::~CPlayer()
{
	Release();
}

void CPlayer::Initialize()
{
	CObj* pObj = this;
	m_pAnimator = new CAnimator(pObj);
	m_v2Size = VECTOR2(50.f, 90.f);
	m_v2DrawSize = VECTOR2(80.f, 125.f);
	m_fJumpPower = 100.f;
	m_fRestrictTime = 0.02f;
	m_eDirection = DIR_FOR;
	Anim_Setting();
	m_eObjID = OBJ_PLAYER;
	// Status Define 
	m_tStatus.iMoveSpeed = 8;
	m_tStatus.iAtkPower = 1;
	m_tStatus.iMaxHp = 20;
	m_tStatus.iMaxMp = 15;
	m_tStatus.iCurHp = m_tStatus.iMaxHp;
	m_tStatus.iCurMp = m_tStatus.iMaxMp;
}

int CPlayer::Update()
{
	if (false == m_bDead)
	{
		if (false == m_bOpenShop)
		{
			Show_Inven();
			Hit();
			if (false == m_pInventory->Is_Use())
			{
				Jump();
				Attack();
				Move();
				Change_Dir();
			}
		}
		Update_Rect();
	}
	else
	{
		Dead();
	}
	if (nullptr != m_pAnimator)
		m_pAnimator->Update_Animation();
	return OBJ_NOEVENT;
}

void CPlayer::Late_Update()
{
	CObjMgr* pObjMgr = CGameMgr::Get_Instance()->Get_ObjMgr();
	CObj* pSelf = static_cast<CObj*>(this);
	for (auto& iter : pObjMgr->Get_ObjList(OBJ_EQUIP))
	{
		Collision_Actor(iter);
		iter->Collision_Actor(pSelf);
	}
	for (auto& iter : pObjMgr->Get_ObjList(OBJ_WORLD))
	{
		Collision_Actor(iter);
		iter->Collision_Actor(pSelf);
	}
}

void CPlayer::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	//Rectangle(_hdc, static_cast<int>(m_tRect.left + v2ScrollPos.x), static_cast<int>(m_tRect.top + v2ScrollPos.y),
	//	static_cast<int>(m_tRect.right + v2ScrollPos.x), static_cast<int>(m_tRect.bottom + v2ScrollPos.y));
	// Animator 렌더링
	if (nullptr != m_pAnimator)
		m_pAnimator->Render_Animation(_hdc, v2ScrollPos);
}

void CPlayer::Collision_Actor(CObj *& pObj)
{
	if (nullptr == pObj)
		return;
	if (false == pObj->Is_Enable())
		return;
	RECT rc;
	VECTOR2 v2ObjPos = pObj->Get_CurPos();
	VECTOR2 v2Distance = v2ObjPos - m_v2CurPos;
	if (v2Distance.Get_Length() >= 100 * 100)
		return;
	
	if (FALSE == IntersectRect(&rc, &pObj->Get_Rect(), &m_tRect))
		return;

	if (OBJ_ITEM == pObj->Get_ObjID())
	{
		CItem* pItem = static_cast<CItem*>(pObj);
		if (nullptr == pItem)
			return;
		if (true == pItem->Is_Use())
			return;
		if (true == pItem->Is_ItemPicked())
			return;
		CInputMgr* pInputMgr = CGameMgr::Get_Instance()->Get_InputMgr();
		if (true == pInputMgr->Is_Press(VK_UP))
		{
			if (pItem->Get_Owner() == nullptr)
				pItem->Set_Owner(this);
			m_pInventory->Add_Item(pItem);
			pItem->Set_Enable(false);
			pItem->Set_ItemPicked(true);
		}
	}
	else if (OBJ_MONSTER == pObj->Get_ObjID())
	{

	}
}

void CPlayer::Release()
{
	CGameMgr::Get_Instance()->Get_ObjMgr()->Register_Remove(OBJ_UI, m_pInventory);
	DELETE_MEMORY(m_pAnimator);
	DELETE_MEMORY(m_pInventory);
}

void CPlayer::Damaged(const float & fDamage)
{
	if (false == CGameMgr::Get_Instance()->Is_GobMode())
	{
		if (false == m_bDead && CHAR_HIT != m_eCharState && false == m_bDamaged
			&& CHAR_ROLL_NOR != m_eCharState)
		{
			float iHp = static_cast<float>(m_tStatus.iCurHp) - fDamage;
			m_tStatus.iCurHp = static_cast<int>(iHp);
			if (iHp <= 0)
			{
				Change_State(CHAR_DIE);
				m_bDead = true;
			}
			else
			{
				m_tHurtTimer.dwTime = GetTickCount();
				m_bDamaged = true;
				Change_State(CHAR_HIT);

				CSword* pSword = static_cast<CSword*>(m_vecEquipList[m_iAttackIndex]);
				if (nullptr != pSword)
				{
					pSword->Set_Rotate(false);
					pSword->Set_Enable(false);
				}
			}
		}
	}
}

void CPlayer::Change_State(const ECHARSTATE & eCharState)
{
	if (CHAR_HIT == eCharState)
	{
		if (DIR_FOR == m_eDirection)
		{
			m_pAnimator->Set_CurAnimation(L"DAMAGED_R");
		}
		else
		{
			m_pAnimator->Set_CurAnimation(L"DAMAGED_L");
		}
		if(m_iDamageSoundIndex == 0)
			CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"damage_01.wav", SOUND_PLAYER, g_fSound);
		else if(m_iDamageSoundIndex == 1)
			CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"damage_02.wav", SOUND_PLAYER, g_fSound);
		else if (m_iDamageSoundIndex == 2)
			CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"damage_03.wav", SOUND_PLAYER, g_fSound);
		else if (m_iDamageSoundIndex == 3)
			CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"damage_04.wav", SOUND_PLAYER, g_fSound);
		else if (m_iDamageSoundIndex == 4)
			CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"damage_05.wav", SOUND_PLAYER, g_fSound);

		++m_iDamageSoundIndex;
		if (m_iDamageSoundIndex >= 5)
			m_iDamageSoundIndex = 0;
	}
	else if (CHAR_DIE== eCharState)
	{
		if (DIR_FOR == m_eDirection)
		{
			m_pAnimator->Set_CurAnimation(L"DIE_R");
		}
		else
		{
			m_pAnimator->Set_CurAnimation(L"DIE_L");
		}

		CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"death_01.wav", SOUND_PLAYER, g_fSound);
	}

	if (CHAR_DIE == m_eCharState)
		return;

	if (CHAR_HIT == m_eCharState)
		return;

	if (CHAR_IDLE == eCharState)
	{
		if (CHAR_JUMP == m_eCharState || CHAR_FALL == m_eCharState || CHAR_FALL_TO_IDLE == m_eCharState
			|| CHAR_ROLL_NOR == m_eCharState)
			return;
		if (DIR_FOR == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"IDLE_R");
		else
			m_pAnimator->Set_CurAnimation(L"IDLE_L");
	}
	else if (CHAR_MOVE == eCharState)
	{
		if (CHAR_JUMP == m_eCharState || CHAR_FALL == m_eCharState || CHAR_FALL_TO_IDLE == m_eCharState
			|| CHAR_ROLL_NOR == m_eCharState)
			return;

		if (DIR_FOR == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"RUN_R");
		else
			m_pAnimator->Set_CurAnimation(L"RUN_L");
	}
	else if (CHAR_JUMP == eCharState)
	{
		if (CHAR_FALL == m_eCharState || CHAR_ATTACK == m_eCharState)
			return;

		int iIndex = m_pAnimator->Get_Animation()->Get_AnimIndex();

		if (DIR_FOR == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"RISE_R");
		else
			m_pAnimator->Set_CurAnimation(L"RISE_L");

		if (m_eCharState == eCharState)
		{
			m_pAnimator->Get_Animation()->Set_AnimationIndex(iIndex);
			m_pAnimator->Get_Animation()->Set_LastAnimFix(true);
		}
	}
	else if (CHAR_FALL == eCharState)
	{
		if (CHAR_ATTACK == m_eCharState)
			return;

		if (DIR_FOR == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"FALL_R");
		else
			m_pAnimator->Set_CurAnimation(L"FALL_L");
	}
	else if (CHAR_FALL_TO_IDLE == eCharState)
	{
		int iIndex = m_pAnimator->Get_Animation()->Get_AnimIndex();

		if (DIR_FOR == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"FALL_TO_IDLE_R");
		else
			m_pAnimator->Set_CurAnimation(L"FALL_TO_IDLE_L");

		if (m_eCharState == eCharState)
		{
			m_pAnimator->Get_Animation()->Set_AnimationIndex(iIndex);
			m_pAnimator->Get_Animation()->Set_LastAnimFix(true);
		}
	}
	else if (CHAR_ROLL_NOR == eCharState)
	{
		if (DIR_FOR == m_eDirection)
		{
			m_pAnimator->Set_CurAnimation(L"ROLL_NOR_R");
			m_v2Dir = VECTOR2(1.f, 0.f);
		}
		else
		{
			m_pAnimator->Set_CurAnimation(L"ROLL_NOR_L");
			m_v2Dir = VECTOR2(-1.f, 0.f);
		}

		CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"ROLL.wav", SOUND_PLAYER, g_fSound);
	}
	else if (CHAR_ATTACK == eCharState)
	{
		if (nullptr != m_vecEquipList[m_iAttackIndex])
		{
			CSword* pSword = dynamic_cast<CSword*>(m_vecEquipList[m_iAttackIndex]);
			if (nullptr != pSword)
			{
				if (SWORD_ONE == pSword->Get_SwordID())
				{
					if (DIR_FOR == m_eDirection)
						m_pAnimator->Set_CurAnimation(L"ST_ONEHAND_R");
					else
						m_pAnimator->Set_CurAnimation(L"ST_ONEHAND_L");

					CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"ONEHANDSWORD.wav", SOUND_PLAYER, g_fSound);
				}
				else if (SWORD_TWO == pSword->Get_SwordID())
				{
					if (DIR_FOR == m_eDirection)
						m_pAnimator->Set_CurAnimation(L"ST_TWOHAND_R");
					else
						m_pAnimator->Set_CurAnimation(L"ST_TWOHAND_L");

					CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"TWOHANDSWORD.wav", SOUND_PLAYER, g_fSound);
				}
				else if (SWORD_DAGGER == pSword->Get_SwordID())
				{
					if (DIR_FOR == m_eDirection)
						m_pAnimator->Set_CurAnimation(L"ST_DAGGER_R");
					else
						m_pAnimator->Set_CurAnimation(L"ST_DAGGER_L");

					CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"DAGGER.wav", SOUND_PLAYER, g_fSound);
				}
			}
			CBow* pBow = dynamic_cast<CBow*>(m_vecEquipList[m_iAttackIndex]);
			if (nullptr != pBow)
			{
				if (DIR_FOR == m_eDirection)
					m_pAnimator->Set_CurAnimation(L"BOW_R");
				else
					m_pAnimator->Set_CurAnimation(L"BOW_L");
			}
		}
	}
	m_eCharState = eCharState;
}

void CPlayer::Change_PlayerState(const EPlayerState & ePlayerState)
{
}

void CPlayer::Reback()
{
	m_tStatus.iMaxHp = 20;
	m_tStatus.iMaxMp = 15;
	m_tStatus.iCurHp = m_tStatus.iMaxHp;
	m_tStatus.iCurMp = m_tStatus.iMaxMp;
	m_bDead = false;
	m_bRealDead = false;
}

void CPlayer::Show_Inven()
{
	if (!(CHAR_ROLL_NOR == m_eCharState || CHAR_DIE == m_eCharState || CHAR_HIT == m_eCharState))
	{
		if (nullptr == m_pInventory)
			return;
		// Manager 들 세팅
		CInputMgr* pInputMgr = CGameMgr::Get_Instance()->Get_InputMgr();
		if (true == pInputMgr->Is_Press(VK_TAB))
		{
			CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"Inventory_Open_00.wav", SOUND_INVENTORY, g_fSound);
			m_pInventory->Set_Use(!m_pInventory->Is_Use());
			m_pInventory->Reset_Info();
			Change_State(CHAR_IDLE);
			if (nullptr != m_vecEquipList[m_iAttackIndex])
			{
				Change_State(CHAR_IDLE);
				CSword* pSword = dynamic_cast<CSword*>(m_vecEquipList[m_iAttackIndex]);
				if (nullptr != pSword)
					pSword->Set_Rotate(false);
				m_vecEquipList[m_iAttackIndex]->Set_Enable(false);
			}
		}
	}
}

void CPlayer::Change_Dir()
{
	if (m_eDirection != m_eSaveDirection)
	{
		m_eSaveDirection = m_eDirection;
		Change_State(m_eCharState);
	}
}

void CPlayer::Update_Rect()
{
	VECTOR2 v2RealSize = (m_v2Size) / 2.0f;
	// Collision Size
	m_tRect.left = static_cast<long>(m_v2CurPos.x - v2RealSize.x);
	m_tRect.top = static_cast<long>(m_v2CurPos.y - m_v2Size.y);
	m_tRect.right = static_cast<long>(m_v2CurPos.x + v2RealSize.x);
	m_tRect.bottom = static_cast<long>(m_v2CurPos.y  );
 	 //   CenterPos, Left, Right
	m_v2CenterPos = VECTOR2(m_v2CurPos.x, m_v2CurPos.y - v2RealSize.y);
	m_v2LeftPos = VECTOR2(m_v2CurPos.x - v2RealSize.x / 6.f, m_v2CurPos.y);
	m_v2RightPos = VECTOR2(m_v2CurPos.x + v2RealSize.x / 6.f, m_v2CurPos.y);
	// Draw Siz e
	v2RealSize = m_v2DrawSize / 2.f;
	m_tDrawRect.left = static_cast<long>(m_v2CurPos.x - v2RealSize.x); 
	m_tDrawRect.top = static_cast<long>(m_v2CurPos.y - m_v2DrawSize.y);
	m_tDrawRect.right = static_cast<long>(m_v2CurPos.x + v2RealSize.x);
	m_tDrawRect.bottom = static_cast<long>(m_v2CurPos.y);
}

bool CPlayer::Dead()
{
	if (true == m_bDead)
	{
		if (true == m_pAnimator->Get_Animation()->Is_last())
		{
			m_bRealDead = true;
		}
		return true;
	}

	return false;
}

void CPlayer::Hit()
{
	if (CHAR_HIT == m_eCharState)
	{
		if (true == m_tHurtTimer.Is_PassTime() && true == m_bDamaged)
		{
			m_bDamaged = false;
		}
	}
}

void CPlayer::Item_Equip()
{
	if (CHAR_ATTACK != m_eCharState)
	{
		if (nullptr != m_vecEquipList[m_iAttackIndex])
		{
			Change_State(CHAR_ATTACK);
			CSword* pSword = dynamic_cast<CSword*>(m_vecEquipList[m_iAttackIndex]);
			if (nullptr != pSword)
			{
				if (DIR_FOR == m_eDirection)
				{
					m_v2EquipPos = m_v2CenterPos + VECTOR2(-10.f, 0.f);
					pSword->Set_RotValue(-275.f);
					pSword->Set_SpinValue(90.f);
					pSword->Set_MultipleValue(1);
				}
				else
				{
					m_v2EquipPos = m_v2CenterPos + VECTOR2(10.f, 0.f);
					pSword->Set_RotValue(275.f);
					pSword->Set_SpinValue(-270.f);
					pSword->Set_MultipleValue(-1);
				}
				pSword->Obj_Active(true);
			}
			CBow* pBow = dynamic_cast<CBow*>(m_vecEquipList[m_iAttackIndex]);
			if (nullptr != pBow)
			{
				pBow->Set_CenterPos(m_v2CenterPos);
				pBow->Set_Shoot(false);
			}
			m_vecEquipList[m_iAttackIndex]->Set_Enable(true);
		}
	}
}

void CPlayer::Attack()
{
	if (CHAR_HIT == m_eCharState)
		return;
	CInputMgr* pInputMgr = CGameMgr::Get_Instance()->Get_InputMgr();

	 if (CHAR_ATTACK == m_eCharState)
	{
		 CSword* pSword = dynamic_cast<CSword*>(m_vecEquipList[m_iAttackIndex]);
		 if (nullptr != pSword)
		 {
			 if (true == m_pAnimator->Get_Animation()->Is_last())
			 {
				 Change_State(CHAR_IDLE);
				 pSword->Set_Enable(false);
			 }
			 return;
		 }
		 CBow* pBow = dynamic_cast<CBow*>(m_vecEquipList[m_iAttackIndex]);
		 if (nullptr != pBow)
		 {
			 // 애니메이션 고정
			 if (DIR_FOR == m_pAnimator->Get_Animation()->Get_Direction())
			 {
				 if (5 <= m_pAnimator->Get_Animation()->Get_AnimIndex())
				 {
					 Shoot_Arrow(pBow, 2, 'Z');
					 Shoot_Arrow(pBow, 3, 'X');
				 }
			 }
			 else if (DIR_BACK == m_pAnimator->Get_Animation()->Get_Direction())
			 {
				 if (6 >= m_pAnimator->Get_Animation()->Get_AnimIndex())
				 {
					 Shoot_Arrow(pBow, 2, 'Z');
					 Shoot_Arrow(pBow, 3, 'X');
				 }
			 }

			 if (true == m_pAnimator->Get_Animation()->Is_last())
			 {
				 Change_State(CHAR_IDLE);
				 pBow->Set_Enable(false);
			 }
		 }
	}
	 else
	 {
		 if (true == pInputMgr->Is_Pressing('Z'))
		 {
			 m_iAttackIndex = 2;
			 Item_Equip(); 
		 }
		 else if (true == pInputMgr->Is_Pressing('X'))
		 {
			 m_iAttackIndex = 3;
			 Item_Equip();
		 }
	 }
}

void CPlayer::Move()
{

	if (nullptr != m_pCollWall && DIR_END == m_eCollDirection)
	{
		if (WALL_LR == m_pCollWall->Get_WallID())
			m_eCollDirection = m_eDirection;
	}
	else if (nullptr == m_pCollWall)
		m_eCollDirection = DIR_END;

	if (true == m_bCollisionDoor && DIR_END == m_eCollDirection)
	{
		m_eCollDirection = m_eDirection;
	}
	else if(false == m_bCollisionDoor)
		m_eCollDirection = DIR_END;

	if (CHAR_ATTACK == m_eCharState || CHAR_DIE == m_eCharState)
		return;
	CInputMgr* pInputMgr = CGameMgr::Get_Instance()->Get_InputMgr();
	CScrollMgr* pScrollMgr = CGameMgr::Get_Instance()->Get_CScrollMgr();

	VECTOR2 v2ScrollPos = pScrollMgr->Get_Pos();
	v2ScrollPos.x = -m_v2CenterPos.x + 400.f;
	pScrollMgr->Set_Pos(v2ScrollPos);

	if (CHAR_HIT == m_eCharState)
	{
		if (CHAR_HIT == m_eCharState || CHAR_DIE == m_eCharState)
		{
			if (m_eDirection == m_eCollDirection)
				m_v2DamagedDir = VECTOR2() - m_v2DamagedDir;

			VECTOR2 v2MovePos = m_v2CurPos + ((m_v2DamagedDir * (static_cast<float>(m_tStatus.iMoveSpeed))));
			m_v2CurPos = v2MovePos;
			VECTOR2 v2ScrollPos = pScrollMgr->Get_Pos();
			v2ScrollPos.x = -m_v2CenterPos.x + 400.f;
			pScrollMgr->Set_Pos(v2ScrollPos);
			return;
		}
	}

	// Manager 들 세팅
	bool bIsRoll = pInputMgr->Is_Press(VK_CONTROL);
	if (true == bIsRoll && CHAR_JUMP != m_eCharState && CHAR_FALL != m_eCharState && 
		CHAR_ROLL_NOR != m_eCharState)
	{
		Change_State(CHAR_ROLL_NOR);
		m_v2Dir = m_v2Dir * 1.5f;
		// 마지막 애니메이션으로 고정 
		m_pAnimator->Get_Animation()->Set_LastAnimFix(true);
	}
	if (CHAR_ROLL_NOR != m_eCharState)
	{
		bool bIsPressedLeft = pInputMgr->Is_Pressing(VK_LEFT);
		bool bIsPressedRight = pInputMgr->Is_Pressing(VK_RIGHT);
		if ((false == (bIsPressedLeft || bIsPressedRight)))
		{
			Change_State(CHAR_IDLE);
			return;
		}
		if (true == bIsPressedLeft)
		{
			m_v2Dir = VECTOR2(-1.f, 0.f);
			m_eDirection = DIR_BACK;
		}
		else if (true == bIsPressedRight)
		{
			m_v2Dir = VECTOR2(1.f, 0.f);
			m_eDirection = DIR_FOR;
		}

		Change_State(CHAR_MOVE);
	}
	else
	{
		if (true == m_pAnimator->Get_Animation()->Is_last())
		{
			Change_State(CHAR_END);
			return;
		}
	}

	if (m_eDirection == m_eCollDirection)
		m_v2Dir = VECTOR2();

	VECTOR2 v2MovePos = m_v2CurPos + ((m_v2Dir * (static_cast<float>(m_tStatus.iMoveSpeed))));
	m_v2CurPos = v2MovePos;
}

void CPlayer::Jump()
{
	// Manager 들 세팅
	CInputMgr* pInputMgr = CGameMgr::Get_Instance()->Get_InputMgr();
	CScrollMgr* pScrollMgr = CGameMgr::Get_Instance()->Get_CScrollMgr();
	CLineMgr* pLineMgr = CGameMgr::Get_Instance()->Get_LineMgr();


	float fY = 0.f;
	bool bIsColl = pLineMgr->Collision_Line(m_v2LeftPos, &fY) || pLineMgr->Collision_Line(m_v2RightPos, &fY);
	float fJumpPower = m_fJumpPower;
	// 점프
	if (false == m_bJump)
	{
		// 바닥에 닿은 상태 인지 확인하고 점프한다. (중복 점프 제한) 
		if (true == m_bIsGround && true == bIsColl)
			m_bJump = pInputMgr->Is_Press(VK_SPACE);
		if (CHAR_HIT == m_eCharState)
		{
			m_bJump = true;
			// 땅에 닿은 상태가 아님을 명시
			m_fDownTime = m_fRestrictTime / 2.f;
			m_bIsGround = false;
			fJumpPower = 10;
		}
		else
		{
			// 만약 점프 상태라면
			if (true == m_bJump)
			{
				// 타이머 초기화
				// 땅에 닿은 상태가 아님을 명시
				m_fDownTime = m_fRestrictTime;
				m_bIsGround = false;
				Change_State(CHAR_JUMP);
				// 마지막 애니메이션으로 고정 
				m_pAnimator->Get_Animation()->Set_LastAnimFix(true);
			}
		}
		//자유낙하 공식	
		m_v2CurPos.y -= m_fJumpPower * -9.8f * m_fDownTime;
		m_fDownTime += 0.001f;
		if (false == bIsColl)
		{
			if (!(CHAR_ROLL_NOR == m_eCharState || CHAR_HIT == m_eCharState))
			{
				Change_State(CHAR_FALL);
				// 마지막 애니메이션으로 고정 
				m_pAnimator->Get_Animation()->Set_LastAnimFix(true);
			}
		}
	} 
	// 점프 상태 일 때
	if (true == m_bJump)
	{
		// 점프일 때, Wall 인지 확인
		if (nullptr != m_pCollWall)
		{
			if (WALL_DW == m_pCollWall->Get_WallID())
			{
				// 점프 상태 아님, 타이머 초기화
				m_bJump = false;
				m_fDownTime = 0.f;
			}
		}
		// 위로 솟아오르고 있는 중
		m_v2CurPos.y += fJumpPower * -9.8f * m_fDownTime;
		m_fDownTime -= 0.0008f;
		// 만약 현재 점프한 지점에서 일정 거리 이상 뛰어 올랐다면
		if (m_fDownTime < 0.f)//y값보다 플레이어y값이 클때
		{
			// 점프 상태 아님, 타이머 초기화
			m_bJump = false;
			m_fDownTime = 0.f;
			if (CHAR_HIT == m_eCharState)
			{
				m_eCharState = CHAR_IDLE;
				Change_State(CHAR_IDLE);
			}
		}
	}
	// 만약 충돌 상태이고 점프하지 않은 상태라면
	if (true == bIsColl && false == m_bJump)
	{
		if (m_v2CenterPos.y >= fY)
		{
			Change_State(CHAR_FALL_TO_IDLE);
			// 마지막 애니메이션으로 고정 
			m_pAnimator->Get_Animation()->Set_LastAnimFix(true);
		}
		
		// 그라운드를 true로 변경
		// 현재 Y값을 고정시킨다. 
		if (m_v2CurPos.y >= fY)
		{
			m_v2CurPos.y = fY;
			m_bIsGround = true;
			if(CHAR_ROLL_NOR != m_eCharState && CHAR_ATTACK != m_eCharState && CHAR_HIT != m_eCharState && CHAR_DIE != m_eCharState)
				Change_State(CHAR_END);
		}
	}

	// 일정 시간 지날 때 마다 타이머 초기화
	if (m_fDownTime > m_fRestrictTime)
		m_fDownTime = m_fRestrictTime;

	VECTOR2 v2ScrollPos = pScrollMgr->Get_Pos();
	v2ScrollPos.y = -m_v2CenterPos.y + 350.f;
	pScrollMgr->Set_Pos(v2ScrollPos);
}

void CPlayer::Shoot_Arrow(CBow* pBow, const int& iIndex, const char& chString)
{
	CInputMgr* pInputMgr = CGameMgr::Get_Instance()->Get_InputMgr();
	if (m_iAttackIndex == iIndex)
	{
		if (true == pInputMgr->Is_Pressing(chString))
		{
			m_pAnimator->Get_Animation()->Set_AnimFix(true);
			pBow->Update_Shoot_Speed();
		}
		else
		{
			if (false == pBow->Is_Shoot())
			{
				pBow->Set_Shoot(true);
				pBow->Shoot_Arrow();
			}
		}
	}
}

void CPlayer::Anim_Setting()
{
	Anim_Stand_IDLE();
	Anim_Stand_RUN();
	Anim_Stand_RISE();
	Anim_Stand_FALL();
	Anim_Stand_FALL_TO_IDLE();
	Anim_Stand_ROLL_NORMAL();
	Anim_Stand_ROLL_JP();
	Anim_Stand_OneHand();
	Anim_Stand_TwoHand();
	Anim_Stand_Dagger();
	Anim_Damaged();
	Anim_Stand_SpellCast();
	Anim_Stand_SpellFire();
	Anim_Stand_Block();
	Anim_Bow();
	Anim_Die();
		// 마지막, 애니메이션 세팅해주는 함수 호출
	m_pAnimator->Set_CurAnimation(L"ST_DAGGER_R");
}
// Idle 
void CPlayer::Anim_Stand_IDLE()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"PL_IDLE_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(25);
	for (int i = 0; i < 56; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 223 + 99 + (i * 0.25f)), 80, }, POINT{ 170, 194 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"IDLE_R", pAnimation);
	pAnimation = new CAnimation_L(L"PL_IDLE_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(25);
	for (int i = 0; i < 56; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 223 + 99 + (i * 0.25f)), 80, }, POINT{ 170, 194 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"IDLE_L", pAnimation);

}
// Run
void CPlayer::Anim_Stand_RUN()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"PL_RUN_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(75);
	for (int i = 0; i < 8; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 227 + 100 - (i * 0.25f)), 80, }, POINT{ 170, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"RUN_R", pAnimation);
	pAnimation = new CAnimation_L(L"PL_RUN_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(75);
	for (int i = 0; i < 8; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 227 + 100 - (i * 0.25f)), 80, }, POINT{ 170, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"RUN_L", pAnimation);
}
// Rise 
void CPlayer::Anim_Stand_RISE()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"PL_RISE_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 3; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 227 + 100 - (i * 0.25f)), 80, }, POINT{ 170, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"RISE_R", pAnimation);
	pAnimation = new CAnimation_L(L"PL_RISE_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 3; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 227 + 100 - (i * 0.25f)), 80, }, POINT{ 170, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"RISE_L", pAnimation);
}
// Fall 
void CPlayer::Anim_Stand_FALL()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"PL_FALL_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 5; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 227 + 100 - (i * 0.25f)), 80, }, POINT{ 170, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"FALL_R", pAnimation);
	pAnimation = new CAnimation_L(L"PL_FALL_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 5; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 227 + 100 - (i * 0.25f)), 80, }, POINT{ 170, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"FALL_L", pAnimation);
}

void CPlayer::Anim_Stand_FALL_TO_IDLE()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"PL_FALL_TO_IDLE_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 3; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 227 + 100 - (i * 0.25f)), 80, }, POINT{ 170, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"FALL_TO_IDLE_R", pAnimation);
	pAnimation = new CAnimation_L(L"PL_FALL_TO_IDLE_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 3; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 227 + 100 - (i * 0.25f)), 80, }, POINT{ 170, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"FALL_TO_IDLE_L", pAnimation);
}

void CPlayer::Anim_Stand_ROLL_NORMAL()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"PL_ROLL_NOR_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(75);
	for (int i = 0; i < 7; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 240 + 100 - (i * 0.25f)), 80, }, POINT{ 172, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ROLL_NOR_R", pAnimation);
	pAnimation = new CAnimation_L(L"PL_ROLL_NOR_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(75);
	for (int i = 0; i < 7; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 226 + 100 - (i * 0.25f)), 80, }, POINT{ 172, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ROLL_NOR_L", pAnimation);
}

void CPlayer::Anim_Stand_ROLL_JP()
{
}

void CPlayer::Anim_Stand_OneHand()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"PL_ONEHAND_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(25);
	for (int i = 0; i < 16; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 240 + 100 - (i * 0.25f)), 80, }, POINT{ 172, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ST_ONEHAND_R", pAnimation);
	pAnimation = new CAnimation_L(L"PL_ONEHAND_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(25);
	for (int i = 0; i < 16; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 240 + 100 - (i * 0.25f)), 80, }, POINT{ 172, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ST_ONEHAND_L", pAnimation);
}

void CPlayer::Anim_Stand_TwoHand()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"PL_TWOHAND_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 16; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) + 80, 105 }, POINT{ 172, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ST_TWOHAND_R", pAnimation);
	pAnimation = new CAnimation_L(L"PL_TWOHAND_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 16; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) + 80, 105 }, POINT{ 172, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ST_TWOHAND_L", pAnimation);
}

void CPlayer::Anim_Stand_Dagger()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"PL_DAGGER_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(25);
	for (int i = 0; i < 12; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) + 60, 105 }, POINT{ 172, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ST_DAGGER_R", pAnimation);
	pAnimation = new CAnimation_L(L"PL_DAGGER_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(25);
	for (int i = 0; i < 12; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) + 60, 105 }, POINT{ 172, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ST_DAGGER_L", pAnimation);
}

void CPlayer::Anim_Stand_SpellCast()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"PL_SPELLCAST_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 8; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) + 60, 105 }, POINT{ 172, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ST_SPELLCAST_R", pAnimation);
	pAnimation = new CAnimation_L(L"PL_SPELLCAST_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 8; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) + 60, 105 }, POINT{ 172, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ST_SPELLCAST_L", pAnimation);
}

void CPlayer::Anim_Stand_SpellFire()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"PL_SPELLFIRE_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 4; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) + 60, 105 }, POINT{ 172, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ST_SPELLFIRE_R", pAnimation);
	pAnimation = new CAnimation_L(L"PL_SPELLFIRE_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 4; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) + 60, 105 }, POINT{ 172, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ST_SPELLFIRE_L", pAnimation);
}

void CPlayer::Anim_Stand_Block()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	// Block
	pAnimation = new CAnimation(L"PL_BLOCK_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	pAnimClip = new CAnimClip(POINT{  60, 105 }, POINT{ 172, 195 });
	pAnimation->Insert_Anim(pAnimClip);
	m_pAnimator->Add_Animation(L"ST_BLOCK_R", pAnimation);
	pAnimation = new CAnimation_L(L"PL_BLOCK_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	pAnimClip = new CAnimClip({ 60, 105 }, POINT{ 172, 195 });
	pAnimation->Insert_Anim(pAnimClip);
	m_pAnimator->Add_Animation(L"ST_BLOCK_L", pAnimation);
	// Block Hit
	pAnimation = new CAnimation(L"PL_BLOCKHIT_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	pAnimClip = new CAnimClip(POINT{ 60, 105 }, POINT{ 172, 195 });
	pAnimation->Insert_Anim(pAnimClip);
	m_pAnimator->Add_Animation(L"ST_BLOCKHIT_R", pAnimation);
	pAnimation = new CAnimation_L(L"PL_BLOCKHIT_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	pAnimClip = new CAnimClip({ 60, 105 }, POINT{ 172, 195 });
	pAnimation->Insert_Anim(pAnimClip);
	m_pAnimator->Add_Animation(L"ST_BLOCKHIT_L", pAnimation);
	// Blocked
	pAnimation = new CAnimation(L"PL_BLOCKED_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	pAnimClip = new CAnimClip(POINT{ 60, 105 }, POINT{ 172, 195 });
	pAnimation->Insert_Anim(pAnimClip);
	m_pAnimator->Add_Animation(L"ST_BLOCKED_R", pAnimation);
	pAnimation = new CAnimation_L(L"PL_BLOCKED_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	pAnimClip = new CAnimClip({ 60, 105 }, POINT{ 172, 195 });
	pAnimation->Insert_Anim(pAnimClip);
	m_pAnimator->Add_Animation(L"ST_BLOCKED_L", pAnimation);
}

void CPlayer::Anim_Damaged()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"PL_DAMAGED");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(0);
	TCHAR szFile[100];
	wsprintf(szFile, L"PL_DAMAGED_R");
	pAnimClip = new CAnimClip_N(POINT{ 30, 55 }, POINT{ 172, 195 }, szFile);
	pAnimation->Insert_Anim(pAnimClip);
	m_pAnimator->Add_Animation(L"DAMAGED_R", pAnimation);

	pAnimation = new CAnimation(L"CASTLE_DASHATTACK_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(0);
	wsprintf(szFile, L"PL_DAMAGED_L");
	pAnimClip = new CAnimClip_N(POINT{ 30, 55 }, POINT{ 172, 195 }, szFile);
	pAnimation->Insert_Anim(pAnimClip);
	m_pAnimator->Add_Animation(L"DAMAGED_L", pAnimation);
}

void CPlayer::Anim_Bow()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"PL_BOW_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(150);
	for (int i = 0; i < 12; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300 +  80), 105 }, POINT{ 172, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"BOW_R", pAnimation);
	pAnimation = new CAnimation_L(L"PL_BOW_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(150);
	for (int i = 0; i < 12; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300 + 80), 105, }, POINT{ 172, 195 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"BOW_L", pAnimation);
}

void CPlayer::Anim_Die()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"PL_DAMAGED");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(200);
	TCHAR szFile[100];
	for (int i = 1; i <= 9; ++i)
	{
		wsprintf(szFile, L"PL_DIE_R%d", i);
		pAnimClip = new CAnimClip_N(POINT{ 30, 55 }, POINT{ 172, 195 }, szFile);
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"DIE_R", pAnimation);

	pAnimation = new CAnimation(L"CASTLE_DASHATTACK_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(200);
	for (int i = 1; i <= 9; ++i)
	{
		wsprintf(szFile, L"PL_DIE_L%d", i);
		pAnimClip = new CAnimClip_N(POINT{ 30, 55 }, POINT{ 172, 195 }, szFile);
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"DIE_L", pAnimation);
}
