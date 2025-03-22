#include "pch.h"
#include "CCastle.h"
#include "CMonsterAI.h"
#include "CBossMonsterAI.h"
#include "CAnimator.h"
#include "CAnimation.h"
#include "CAnimation_L.h"
#include "CAnimClip.h"
#include "CWall.h"
#include "CDownWall.h"
#include "CGameMgr.h"
#include "CObjMgr.h"
#include "CScrollMgr.h"
#include "CSceneMgr.h"
#include "CLineMgr.h"
#include "CAnimClip_N.h"
#include "CBossHpBar.h"
#include "CFollowText.h"
#include "CGameMgr.h"

CCastle::CCastle() : m_iDashAttackDistance(0), m_iMagicTwoDistance(0), m_iSpawnSwordIndex(0), m_iMagicAttackDistance(0),
m_tDashTimer(TIMER(100)), m_tChangeTimer(TIMER(2000)), m_tMagic1SpawnSwordTimer(TIMER(1000)), 
m_bDash(false), m_bMagic_1(false), m_bMagic_2(false)
{
	Initialize();
}

CCastle::CCastle(CPlayer * pTarget) : CBoss(pTarget), m_iDashAttackDistance(0), m_iMagicTwoDistance(0), m_iSpawnSwordIndex(0), m_iMagicAttackDistance(0),
m_tDashTimer(TIMER(800)), m_tChangeTimer(TIMER(2000)), m_tMagic1SpawnSwordTimer(TIMER(500)), m_tMagic1AttackTimer(TIMER(5000)), m_tGiveDamageTimer(TIMER(600)),
 m_tMagic2AttackTimer(TIMER(3000)),
m_bDash(false), m_bMagic_1(false), m_bMagic_2(false)
{
	Initialize();
}

CCastle::~CCastle()
{
	Release();
}

void CCastle::Initialize()
{
	m_strName = L"Castle";
	m_pNameText = new CFollowText(this, L"나눔고딕", tagRGB(255, 255, 255));
	m_pNameText->Set_CurPos(VECTOR2(3.f, 28.f) + m_v2CurPos);
	m_pNameText->Set_Size(VECTOR2(15.f, 0.f));
	m_pNameText->Set_Enable(true);
	m_pNameText->Set_Text(m_strName);
	m_eObjID = OBJ_MONSTER;

	CMonster* pMonster = this;
	// Monster AI
	m_pMonsterAI = new CBossMonsterAI(pMonster, m_pTarget);
	m_pMonsterAI->Set_Find_Range(400);
	m_pMonsterAI->Set_Attack_Range(100);
	m_iDashAttackDistance = 300;
	m_v2Size = VECTOR2(60.f, 90);
	m_v2DrawSize = VECTOR2(153.6f, 240.f);
	m_eDirection = DIR_BACK;
	// Jump Define
	m_fJumpPower = 10.f;
	m_fRestrictTime = 0.02f;
	// Status Define 
	m_v2Size = VECTOR2(60.f, 90);
	m_v2DrawSize = VECTOR2(153.6f, 240.f);
	m_tStatus.iMoveSpeed = 3;
	m_tStatus.iAtkPower = 5;
	m_tStatus.iMaxHp = 100;
	m_tStatus.iMaxMp = 0;
	m_tStatus.iCurHp = m_tStatus.iMaxHp;
	m_tStatus.iCurMp = m_tStatus.iMaxMp;
	// Define Special Status For Value
	m_eCharState = CHAR_IDLE;

	m_vecBarList.reserve(2);
	CHpBar* pHpBar = new CBossHpBar(this, L"HP_RED_BOSS", L"BAR_BACKGROUND", RGBINFO(0, 0, 0), 60);
	pHpBar->Set_CurPos(m_v2BossBarPos);
	pHpBar->Set_Enable(true);
	CGameMgr::Get_Instance()->Get_ObjMgr()->Add_Object(OBJ_UI, pHpBar);
	m_vecBarList.emplace_back(pHpBar);

	m_vecSwordList.reserve(20);
	for (int i = 0; i <10; ++i)
	{
		m_vecSwordList.emplace_back(new CBossSword(this, L"BOSS_SWORD1", L"BOSS_SWORD", SWORD_BOSS));
		CGameMgr::Get_Instance()->Get_ObjMgr()->Add_Object(OBJ_EQUIP, m_vecSwordList[i]);
		m_vecSwordList[i]->Set_Size(VECTOR2(100.f, 100.f));
	}
	for (int i = 10; i < 20; ++i)
	{
		m_vecSwordList.emplace_back(new CBossSword(this, L"BOSS_SWORD2", L"BOSS_SWORD", SWORD_BOSS));
		CGameMgr::Get_Instance()->Get_ObjMgr()->Add_Object(OBJ_EQUIP, m_vecSwordList[i]);
		m_vecSwordList[i]->Set_Size(VECTOR2(100.f, 100.f));
	}

	Anim_Setting();
}

int CCastle::Update()
{
	if (true == m_bDead)
	{
		if (MON_DEADCOUNT_2 <= m_iDeadCount)
		{
			return OBJ_DEADEVENT;
		}
		else
		{
			Dead();
			Phase_Change();
		}
	}
	if (nullptr != m_pMonsterAI)
	{
		m_pMonsterAI->Update();
		Magic_1();
		Magic_2();
		Dash();
		Move();
		Jump();
	}
	Update_Text();
	Update_Rect();
	if (nullptr != m_pAnimator)
	{
		m_pAnimator->Update_Animation();
	}
	return OBJ_NOEVENT;
}

void CCastle::Late_Update()
{
	if (nullptr != m_pMonsterAI)
	{
		m_pMonsterAI->Late_Update();
	}
	Collision_Wall();
	CObj* pObj = static_cast<CObj*>(m_pTarget);
	CObj* pCastle = static_cast<CObj*>(this);
	CObjMgr* pObjMgr = CGameMgr::Get_Instance()->Get_ObjMgr();
	list<CObj*> itemList = pObjMgr->Get_ObjList(OBJ_EQUIP);
	for (auto& iter : itemList)
	{
		iter->Collision_Actor(pCastle);
	}

	Collision_Actor(pObj);
}

void CCastle::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	if (false == Check_Pos_IsInRect(v2ScrollPos, m_tRestrictDraw))
	{
		for (auto& iter : m_vecBarList)
			iter->Set_Enable(false);
		m_bActiveUI = false;
		return;
	}
	else if(false == m_bActiveUI)
	{
		for (auto& iter : m_vecBarList)
			iter->Set_Enable(true);

		m_bActiveUI = true;
	}

	//Rectangle(_hdc, static_cast<int>(m_tRect.left + v2ScrollPos.x), static_cast<int>(m_tRect.top + v2ScrollPos.y),
	//	static_cast<int>(m_tRect.right + v2ScrollPos.x), static_cast<int>(m_tRect.bottom + v2ScrollPos.y));

	m_pNameText->Render(_hdc, v2ScrollPos);

	if (nullptr != m_pAnimator)
	{
		m_pAnimator->Render_Animation(_hdc, v2ScrollPos);
	}
}

void CCastle::Collision_Actor(CObj *& pObj)
{
	if (nullptr == pObj)
		return;

	if (OBJ_PLAYER != pObj->Get_ObjID())
		return;
	RECT rc;
	if (CHAR_ATTACK == m_eCharState && BOSS_END == m_eBossState)
	{
		// Update Attack Rect
		Update_Attack_Rect();
		// Intersect 
		if (FALSE == IntersectRect(&rc, &pObj->Get_Rect(), &m_tAttackRect))
			return;
		if (DIR_FOR == m_eDirection)
		{
			if (m_pAnimator->Get_Animation()->Get_AnimIndex() >= 7)
			{
				m_pTarget->Damaged(static_cast<float>(m_tStatus.iAtkPower));
				Blood_Effect_To_Player(pObj, 0);
			}
		}
		else
		{
			if (m_pAnimator->Get_Animation()->Get_AnimIndex() >= 7)
			{
				m_pTarget->Damaged(static_cast<float>(m_tStatus.iAtkPower));
				Blood_Effect_To_Player(pObj, 0);
			}
		}
		m_pTarget->Set_DamagedDir(m_v2Dir);
	}
	else if (BOSS_SP1 == m_eBossState)
	{
		if (FALSE == IntersectRect(&rc, &m_pTarget->Get_Rect(), &m_tRect))
			return;

		m_pTarget->Damaged(static_cast<float>(m_tStatus.iAtkPower));
		m_pTarget->Set_DamagedDir(m_v2Dir);
		Blood_Effect_To_Player(pObj, 1);
	}
	
}

void CCastle::Release()
{
	for (auto& iter : m_vecSwordList)
		iter->Set_Dead(true);
	DELETE_MEMORY(m_pNameText);
	DELETE_MEMORY(m_pMonsterAI);
	DELETE_MEMORY(m_pAnimator);
	m_pTarget = nullptr;
}

void CCastle::Damaged(const float & fDamage)
{
	if (false == m_bDead && false == m_bDash && CHAR_HIT != m_eCharState)
	{
		Change_State(CHAR_HIT);
		int iHp = static_cast<int>(m_tStatus.iCurHp - fDamage);
		m_tStatus.iCurHp = iHp;

		if (m_tStatus.iCurHp <= 0)
		{
			m_bDead = true;
			if (nullptr != m_vecBarList[0])
				static_cast<CBossHpBar*>(m_vecBarList[0])->Set_Size(static_cast<CBossHpBar*>(m_vecBarList[0])->Get_DrawSize());

			m_eDirection = m_pMonsterAI->Get_SaveDirection();
			Change_BossState(BOSS_END);
			Change_State(CHAR_DIE);
			m_bDash = false;
		}
		else
		{
			Active_DamageText(fDamage);
		}
	}
}

void CCastle::Change_State(const ECHARSTATE & eCharState)
{
	if (m_bChange == false)
	{
		if (CHAR_HIT == eCharState)
		{
			if (DIR_FOR == m_eDirection)
				m_pAnimator->Set_CurAnimation(L"HIT_R");
			else if (DIR_BACK == m_eDirection)
				m_pAnimator->Set_CurAnimation(L"HIT_L");

			m_eCharState = eCharState;
		}
		else if (CHAR_DIE == eCharState)
		{
			if (DIR_FOR == m_eDirection)
				m_pAnimator->Set_CurAnimation(L"DEAD_R");
			else if (DIR_BACK == m_eDirection)
				m_pAnimator->Set_CurAnimation(L"DEAD_L");


			m_eCharState = eCharState;
		}
		else
		{
			if (BOSS_END != m_eBossState)
				return;

			if (CHAR_IDLE == eCharState)
			{
				if (DIR_FOR == m_eDirection)
					m_pAnimator->Set_CurAnimation(L"IDLE_R");
				else if (DIR_BACK == m_eDirection)
					m_pAnimator->Set_CurAnimation(L"IDLE_L");
			}
			else if (CHAR_MOVE == eCharState)
			{
				if (DIR_FOR == m_eDirection)
					m_pAnimator->Set_CurAnimation(L"RUN_R");
				else if (DIR_BACK == m_eDirection)
					m_pAnimator->Set_CurAnimation(L"RUN_L");
			}
			else if (CHAR_ATTACK == eCharState)
			{
				if (DIR_FOR == m_eDirection)
					m_pAnimator->Set_CurAnimation(L"ATTACK_R");
				else if (DIR_BACK == m_eDirection)
					m_pAnimator->Set_CurAnimation(L"ATTACK_L");
			}
			m_eCharState = eCharState;
		}
	}
	else
	{
		if (CHAR_HIT == eCharState)
		{
			if (DIR_FOR == m_eDirection)
				m_pAnimator->Set_CurAnimation(L"HIT_P2_R");
			else if (DIR_BACK == m_eDirection)
				m_pAnimator->Set_CurAnimation(L"HIT_P2_L");

			m_eCharState = eCharState;
		}
		else if (CHAR_DIE == eCharState)
		{
			if (DIR_FOR == m_eDirection)
				m_pAnimator->Set_CurAnimation(L"P2_DIE_R");
			else if (DIR_BACK == m_eDirection)
				m_pAnimator->Set_CurAnimation(L"P2_DIE_L");


			m_eCharState = eCharState;
		}
		else
		{
			if (CHAR_IDLE == eCharState || CHAR_MOVE == eCharState)
			{
				if (DIR_FOR == m_eDirection)
					m_pAnimator->Set_CurAnimation(L"P2_IDLE_R");
				else if (DIR_BACK == m_eDirection)
					m_pAnimator->Set_CurAnimation(L"P2_IDLE_L");
			}
		}

		m_eCharState = eCharState;
	}
}

void CCastle::Change_BossState(const EBOSSSTATE & eBossState)
{
	if (BOSS_SP1_READY == eBossState)
	{
		if (DIR_FOR == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"DASHATK_R");
		else if (DIR_BACK == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"DASHATK_L");

	}
	else if (BOSS_SP1 == eBossState)
	{
		if (DIR_FOR == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"DASH_R");
		else if (DIR_BACK == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"DASH_L");

	}
	else if (BOSS_SP1_END == eBossState)
	{
		if (DIR_FOR == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"RUNSTOP_R");
		else if (DIR_BACK == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"RUNSTOP_L");

	}
	if (BOSS_SP2_READY == eBossState)
	{
		if (DIR_FOR == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"P2_CAST_R");
		else if (DIR_BACK == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"P2_CAST_L");

	}
	else if (BOSS_SP2 == eBossState)
	{
		if (DIR_FOR == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"P2_IDLE_R");
		else if (DIR_BACK == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"P2_IDLE_L");

	}
	else if (BOSS_SP2_END == eBossState)
	{


	}
	else if (BOSS_SP3_READY == eBossState)
	{
		if (DIR_FOR == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"P2_CAST_F_R");
		else if (DIR_BACK == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"P2_CAST_F_L");

	}
	else if (BOSS_SP3_END == eBossState)
	{


	}
	else if (BOSS_NEXT_PAGE == eBossState)
	{
		if (DIR_FOR == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"PHASE2_R");
		else if (DIR_BACK == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"PHASE2_L");
	}
	else if (BOSS_END == eBossState)
	{
		m_bDash = false;

	}
	m_eBossState = eBossState;
}

void CCastle::Obj_Active(const bool& IsActive)
{
	m_bEnable = IsActive;
	for (auto& iter : m_vecSwordList)
		iter->Set_Enable(false);

	for (auto& iter : m_vecBarList)
		iter->Set_Enable(false);

	m_pNameText->Set_Enable(false);
}

void CCastle::Update_Rect()
{
	VECTOR2 v2RealSize = (m_v2Size) / 2.0f;
	// Collision Size
	m_tRect.left = static_cast<long>(m_v2CurPos.x - v2RealSize.x);
	m_tRect.top = static_cast<long>(m_v2CurPos.y - m_v2Size.y);
	m_tRect.right = static_cast<long>(m_v2CurPos.x + v2RealSize.x);
	m_tRect.bottom = static_cast<long>(m_v2CurPos.y);
	// CenterPos, Left, Right
	m_v2CenterPos = VECTOR2(m_v2CurPos.x, m_v2CurPos.y - v2RealSize.y);
	m_v2LeftPos = VECTOR2(m_v2CurPos.x - v2RealSize.x / 6.f, m_v2CurPos.y);
	m_v2RightPos = VECTOR2(m_v2CurPos.x + v2RealSize.x / 6.f, m_v2CurPos.y);
	// Draw Size
	v2RealSize = m_v2DrawSize / 2.f;
	m_tDrawRect.left = static_cast<long>(m_v2CurPos.x - v2RealSize.x);
	m_tDrawRect.top = static_cast<long>(m_v2CurPos.y - m_v2DrawSize.y);
	m_tDrawRect.right = static_cast<long>(m_v2CurPos.x + v2RealSize.x);
	m_tDrawRect.bottom = static_cast<long>(m_v2CurPos.y);	
}

void CCastle::Update_Attack_Rect()
{
	if (DIR_FOR == m_eDirection)
	{
		m_tAttackRect.left = static_cast<int>(m_v2CenterPos.x);
		m_tAttackRect.right = static_cast<int>(m_v2CenterPos.x + m_v2DrawSize.x / 1.8f);
		m_tAttackRect.top = static_cast<int>(m_v2CenterPos.y - 10);
		m_tAttackRect.bottom = static_cast<int>(m_v2CenterPos.y + 10);
	}
	else
	{
		m_tAttackRect.left = static_cast<int>(m_v2CenterPos.x - m_v2DrawSize.x / 1.8f);
		m_tAttackRect.right = static_cast<int>(m_v2CenterPos.x);
		m_tAttackRect.top = static_cast<int>(m_v2CenterPos.y - 10);
		m_tAttackRect.bottom = static_cast<int>(m_v2CenterPos.y + 10);
	}
}

void CCastle::Update_Text()
{
	if (nullptr != m_pNameText)
	{
		m_pNameText->Set_CurPos(VECTOR2(-20.f, -m_v2Size.y - 20.f) + m_v2CurPos);
		m_pNameText->Update();
	}
}

void CCastle::Dead()
{
	if (true == m_bDead && false == m_bChange && MON_DEADCOUNT_0 == m_iDeadCount)
	{
		if (true == m_pMonsterAI->Is_DeadTimerOk())
		{
			m_eDirection = m_pMonsterAI->Get_SaveDirection();
			Change_BossState(BOSS_NEXT_PAGE);
			m_tChangeTimer.dwTime = GetTickCount();
			++m_iDeadCount;
		}
		else
		{
			m_pAnimator->Get_Animation()->Set_LastAnimFix(true);
		}
	}
	else if (true == m_bDead && true == m_bChange && MON_DEADCOUNT_1 == m_iDeadCount)
	{
		if (true == m_pAnimator->Get_Animation()->Is_last())
			m_v2CurPos.y += 20.f;
		m_pAnimator->Get_Animation()->Set_LastAnimFix(true);

		m_bMagic_1 = false;
		m_bMagic_2 = false;
		if (true == m_pMonsterAI->Is_DeadTimerOk())
		{
			++m_iDeadCount;
			for (auto& iter : m_vecBarList)
				iter->Set_Dead(true);

			m_pNameText->Set_Dead(true);

			m_v2DrawSize.x /= 2.f;
			m_v2DrawSize.y /= 2.f;
			m_v2Size.x /= 1.5f;
			m_v2Size.y /= 3.f;

			return;
		}
		else
		{
			for (auto& iter : m_vecSwordList)
				iter->Set_Dead(true);

			m_vecSwordList.clear();
		}
	}
}

void CCastle::Dash()
{
	if (true == m_bChange)
		return;
	if (true == m_bDead )
		return;

	if (CHAR_ATTACK == m_eCharState)
	{
		m_eBossState = BOSS_END;
		return;
	}
	else
	{
		if (BOSS_END == m_eBossState)
		{
			if (CHAR_MOVE == m_eCharState)
			{
				int iValue = 1;
				float fDistance = m_pMonsterAI->Get_Distance();
				if (true == m_pMonsterAI->Is_Catch_EqualYPos(m_v2CurPos.y, m_pTarget->Get_CurPos().y, 5, 20))
				{
					if (fDistance <= m_iDashAttackDistance + iValue && m_iDashAttackDistance - iValue <= fDistance
						&& m_pMonsterAI->Is_FindTarget())
					{
						Change_BossState(BOSS_SP1_READY);
						m_pMonsterAI->Set_SaveDirection(m_eDirection);
						m_eDirection = DIR_END;
					}
				}
			}
		}
		else
		{
			if (wstring(L"DASHATK_R").compare(m_pAnimator->Get_CurAnimName()) ||
				wstring(L"DASHATK_L").compare(m_pAnimator->Get_CurAnimName()))
			{
				if (true == m_pAnimator->Get_Animation()->Is_last())
				{
					if (false == m_bDash)
					{
						m_tDashTimer.dwTime = GetTickCount();
						m_eDirection = m_pMonsterAI->Get_SaveDirection();
						Change_BossState(BOSS_SP1);
					}
					m_bDash = true;
				}
				if (CHAR_HIT == m_eCharState && false == m_bDash)
				{
					m_eDirection = m_pMonsterAI->Get_SaveDirection();
					Change_State(CHAR_HIT);
					Change_BossState(BOSS_END);
				}
			}
			if (true == m_bDash && BOSS_SP1 == m_eBossState)
			{
				if (true == m_tDashTimer.Is_PassTime())
				{
					Change_BossState(BOSS_SP1_END);
				}
			}
			else if (true == m_bDash && BOSS_SP1_END == m_eBossState)
			{
				if (true == m_pAnimator->Get_Animation()->Is_last())
				{
					Change_State(CHAR_IDLE);
					Change_BossState(BOSS_END);
					m_pMonsterAI->Set_Distance(0.f);
					m_bDash = false;
				}
			}
		}
	}
}

void CCastle::Phase_Change()
{
	if (BOSS_NEXT_PAGE ==  m_eBossState && true == m_bDead)
	{
		if (true == m_tChangeTimer.Is_PassTime())
		{
			m_bDead = false;
			m_bChange = true;
			m_eDirection = m_pMonsterAI->Get_SaveDirection();
			Change_State(CHAR_IDLE);
			Change_BossState(BOSS_END);
			m_v2DrawSize.x *= 2.f;
			m_v2DrawSize.y *= 2.f;
			m_v2Size.x *= 1.5f;
			m_v2Size.y *= 3.f;

			m_pMonsterAI->Set_Find_Range(500);
			m_pMonsterAI->Set_Attack_Range(400);
			m_iMagicTwoDistance = 200;
			m_iMagicAttackDistance = 300;

			m_tStatus.iMoveSpeed = 2;
			m_tStatus.iAtkPower = 5;
			m_tStatus.iMaxHp = 200;
			m_tStatus.iMaxMp = 0;
			m_tStatus.iCurHp = m_tStatus.iMaxHp;
			m_tStatus.iCurMp = m_tStatus.iMaxMp;

			m_tRestrictDraw.left -= 200;
			m_tRestrictDraw.top -= 200;
			m_tRestrictDraw.right += 200;
			m_tRestrictDraw.bottom += 200;
		}
		else
		{
			m_pAnimator->Get_Animation()->Set_LastAnimFix(true);
		}
	}
}

void CCastle::Magic_1()
{
	if (false == m_bChange)
		return;
	if (true == m_bDead)
		return;
	if (true == m_bDash)
		return;
	if (BOSS_SP2 == m_eBossState || BOSS_SP2_READY == m_eBossState || BOSS_SP2_END == m_eBossState || BOSS_END == m_eBossState)
	{
		if (CHAR_ATTACK == m_eCharState
			&& BOSS_END == m_eBossState && m_iMagicTwoDistance < m_pMonsterAI->Get_Distance())
		{
			if (m_tMagic1AttackTimer.Is_PassTime())
			{
				m_eDirection = m_pMonsterAI->Get_SaveDirection();
				Change_BossState(BOSS_SP2_READY);
			}
		}
		else
		{
			VECTOR2 v2TargetPos = m_pTarget->Get_CurPos();
			VECTOR2 v2Dir = v2TargetPos - m_v2CurPos;
			float fDistance = v2Dir.Get_Length_Sqrt();

			v2Dir.y = 0.f;
			if (v2Dir.x > 0)
				m_eDirection = DIR_FOR;
			else
				m_eDirection = DIR_BACK;

			if (BOSS_SP2_READY == m_eBossState)
			{
				if (true == m_pAnimator->Get_Animation()->Is_last())
				{
					Change_BossState(BOSS_SP2);
					m_pMonsterAI->Set_SaveDirection(m_eDirection);
					m_bMagic_1 = true;
					m_tMagic1AttackTimer.dwTime = GetTickCount();
					m_tMagic1AttackTimer.iPassTime = 10000;
				}
			}
			else if (BOSS_SP2 == m_eBossState)
			{
				if (true == m_tMagic1SpawnSwordTimer.Is_PassTime())
				{
					if (m_iSpawnSwordIndex < 3)
					{
						m_vecSwordList[m_iSpawnSwordIndex++]->Set_Enable(true);
					}
					m_tMagic1SpawnSwordTimer.dwTime = GetTickCount();
				}

				if (true == m_tMagic1AttackTimer.Is_PassTime())
				{
					Change_BossState(BOSS_SP2_END);
				}
				else
				{
					if (fDistance < m_iMagicAttackDistance)
					{
						v2Dir = VECTOR2() - v2Dir;
						if (fDistance > m_iMagicAttackDistance - 5.f)
							v2Dir = VECTOR2();
					}

					v2Dir.Normalize();
					m_v2Dir = v2Dir;

					Change_State(CHAR_IDLE);
				}
			}
			else if (BOSS_SP2_END == m_eBossState)
			{
				for (int i = 0; i < m_iSpawnSwordIndex; ++i)
					m_vecSwordList[i]->Set_Enable(false);

				Change_BossState(BOSS_END);
				Change_State(CHAR_IDLE);
				m_bMagic_1 = false;
				m_iSpawnSwordIndex = 0;
				m_tMagic1AttackTimer.dwTime = GetTickCount();
				m_tMagic1AttackTimer.iPassTime = 5000;
				if (fDistance <= m_iMagicTwoDistance)
				{
					Change_BossState(BOSS_SP3_READY);
				}
			}
		}
	}
}

void CCastle::Magic_2()
{
	if (false == m_bChange)
		return;
	if (true == m_bDead)
		return;
	if (true == m_bDash)
		return;
	if (BOSS_SP3 == m_eBossState || BOSS_SP3_READY == m_eBossState || BOSS_SP3_END == m_eBossState || BOSS_END == m_eBossState)
	{
		if (CHAR_ATTACK == m_eCharState
			&& BOSS_END == m_eBossState && m_iMagicTwoDistance >= m_pMonsterAI->Get_Distance())
		{
			if (m_tMagic2AttackTimer.Is_PassTime())
			{
				m_eDirection = m_pMonsterAI->Get_SaveDirection();
				Change_BossState(BOSS_SP3_READY);
				m_tMagic1AttackTimer.iPassTime = 7000;
			}
		}
		else
		{
			VECTOR2 v2TargetPos = m_pTarget->Get_CurPos();
			VECTOR2 v2Dir = v2TargetPos - m_v2CurPos;
			float fDistance = v2Dir.Get_Length_Sqrt();
			if (BOSS_SP3_READY == m_eBossState)
			{
				if (true == m_pAnimator->Get_Animation()->Is_last())
				{
					Change_BossState(BOSS_SP3);
					m_pMonsterAI->Set_SaveDirection(m_eDirection);
					m_bMagic_2 = true;
				}
			}
			else if (BOSS_SP3 == m_eBossState)
			{
				if (20 > m_iSpawnSwordIndex)
				{
					v2Dir = VECTOR2() - v2Dir;
					v2Dir.y = 0.f;
					v2Dir.Normalize();
					VECTOR2 v2StartPos = v2Dir *200.f + m_v2CurPos;
					v2StartPos.y = 1000.f;
					VECTOR2 v2PosArr[20];
					v2Dir = VECTOR2() - v2Dir;
					for (int i = 0; i < 20; ++i)
					{
						v2StartPos = v2StartPos + v2Dir * 20;
						v2PosArr[i] = v2StartPos;
					}
					for (int i = 0; i < 100; ++i)
					{
						int r1 = rand() % 20;
						int r2 = rand() % 20;
						VECTOR2 v2Temp = v2PosArr[r1];
						v2PosArr[r1] = v2PosArr[r2];
						v2PosArr[r2] = v2Temp;
					}
					for (int i = 0; i < 20; ++i)
					{
						m_vecSwordList[m_iSpawnSwordIndex++]->Set_Enable(true);
						m_vecSwordList[i]->Set_SpawnPos(v2PosArr[i]);
						m_vecSwordList[i]->Set_CenterPos(v2PosArr[i]);
						m_vecSwordList[i]->Reset_Timer();
					}
					m_tMagic2AttackTimer.dwTime = GetTickCount();
				}
				if (true == m_tMagic2AttackTimer.Is_PassTime())
				{
					Change_BossState(BOSS_SP3_END);
				}
			}
			else if (BOSS_SP3_END == m_eBossState)
			{
				for (int i = 0; i < m_iSpawnSwordIndex; ++i)
					m_vecSwordList[i]->Set_Enable(false);

				m_iSpawnSwordIndex = 0;
				Change_BossState(BOSS_END);
				Change_State(CHAR_IDLE);
				m_bMagic_2 = false;
				m_tMagic2AttackTimer.dwTime = GetTickCount();
				m_tMagic1AttackTimer.iPassTime = 5000;
			}
		}
	}
}

void CCastle::Move()
{
	if (nullptr != m_pCollWall && DIR_END == m_eCollDirection)
	{
		if (WALL_LR == m_pCollWall->Get_WallID())
		{
			VECTOR2 v2CollPos = m_pCollWall->Get_CenterPos();
			VECTOR2 v2Dir = m_v2CurPos - v2CollPos;
			if (v2Dir.x > 0)
				m_eCollDirection = DIR_FOR;
			else
				m_eCollDirection = DIR_BACK;
		}
	}
	else if (nullptr == m_pCollWall)
		m_eCollDirection = DIR_END;

	if (BOSS_SP2 != m_eBossState)
	{
		if (DIR_FOR == m_eDirection)
			m_v2Dir = VECTOR2(1.f, 0.f);
		else if (DIR_BACK == m_eDirection)
			m_v2Dir = VECTOR2(-1.f, 0.f);
		else if (DIR_END == m_eDirection)
			m_v2Dir = VECTOR2();
	}

	float fSpeed = static_cast<float>(m_tStatus.iMoveSpeed);
	if (BOSS_SP1 == m_eBossState && true == m_bDash)
		fSpeed += 10.f;
	else if (BOSS_SP1_END == m_eBossState && true == m_bDash)
		fSpeed = 1;
	else if (CHAR_HIT == m_eCharState)
	{
		fSpeed = 3.f;
	}
	else if (CHAR_DIE == m_eCharState)
	{
		fSpeed = 10.f;
	}

	if (DIR_END != m_eDirection)
	{
		if (CHAR_IDLE == m_eCharState || CHAR_MOVE == m_eCharState)
			Change_State(CHAR_MOVE);
	}

	if (CHAR_MOVE == m_eCharState || BOSS_SP1 == m_eBossState  || BOSS_SP2 == m_eBossState) 
	{
		if (nullptr != m_pCollWall)
			m_v2Dir = VECTOR2() - m_v2Dir;
		m_v2CurPos = m_v2CurPos + (m_v2Dir * fSpeed);
	}
	else if (CHAR_HIT == m_eCharState || CHAR_DIE == m_eCharState)
	{
		if (nullptr != m_pCollWall)
			m_v2DamagedDir = VECTOR2() - m_v2DamagedDir;
		m_v2CurPos = m_v2CurPos + (m_v2DamagedDir * fSpeed);
	}
}

void CCastle::Jump()
{
	CLineMgr* pLineMgr = CGameMgr::Get_Instance()->Get_LineMgr();

	float fY = 0.f;
	bool bIsColl = pLineMgr->Collision_Line(m_v2LeftPos, &fY) || pLineMgr->Collision_Line(m_v2RightPos, &fY);
	// 점프
	if (false == m_bJump)
	{
		// 바닥에 닿은 상태 인지 확인하고 점프한다. (중복 점프 제한) 
		if (true == m_bIsGround)
		{
			// 만약 점프 상태라면
			if (CHAR_HIT == m_eCharState || CHAR_DIE == m_eCharState)
			{
				// 타이머 초기화
				// 땅에 닿은 상태가 아님을 명시
				if (CHAR_DIE == m_eCharState && m_iDeadCount >= 2)
					m_fRestrictTime = 0.028f;
				m_fDownTime = m_fRestrictTime;
				m_bIsGround = false;
				m_bJump = true;
			}
		}
		//자유낙하 공식	
		m_v2CurPos.y -= 50 * -9.8f * m_fDownTime;
		m_fDownTime += 0.001f;
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
		float fChangeValue = m_fJumpPower;
		if (CHAR_DIE == m_eCharState)
			fChangeValue += 20.f;
		m_v2CurPos.y += fChangeValue * -9.8f * m_fDownTime;
		m_fDownTime -= 0.0008f;
		// 만약 현재 점프한 지점에서 일정 거리 이상 뛰어 올랐다면
		if (m_fDownTime < 0.f)//y값보다 플레이어y값이 클때
		{
			// 점프 상태 아님, 타이머 초기화
			m_bJump = false;
			m_fDownTime = 0.f;
		}
	}
	// 만약 충돌 상태이고 점프하지 않은 상태라면
	if (true == bIsColl && false == m_bJump)
	{
		// 그라운드를 true로 변경
		// 현재 Y값을 고정시킨다. 
		if (m_v2CurPos.y >= fY)
		{
			m_v2CurPos.y = fY;
			m_bIsGround = true;
			if(CHAR_DIE == m_eCharState)
				m_eCharState = CHAR_END;
		}
	}

	// 일정 시간 지날 때 마다 타이머 초기화
	if (m_fDownTime > m_fRestrictTime)
		m_fDownTime = m_fRestrictTime;
}

void CCastle::Anim_Setting()
{
	CObj* pObj = this;
	m_pAnimator = new CAnimator(pObj);

	ANIM_IDLE();
	ANIM_RUN();
	ANIM_ATTACK();
	ANIM_DASHATTACK();
	ANIM_DASH();
	ANIM_RUNSTOP();
	ANIM_HIT();
	ANIM_DEAD();
	ANIM_PHASE2();
	ANIM_P2_IDLE();
	ANIM_P2_CAST();
	ANIM_P2_CAST_FRONT();
	ANIM_P2_HIT();
	ANIM_P2_DIE();
	// 마지막, 애니메이션 세팅해주는 함수 호출
	m_pAnimator->Set_CurAnimation(L"IDLE_L");
}
// IDLE
void CCastle::ANIM_IDLE()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"CASTLE_IDLE_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(50);
	for (int i = 0; i < 24; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 350.01987654321f + 45), 100 }, POINT{ 300, 372 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"IDLE_R", pAnimation);
	pAnimation = new CAnimation(L"CASTLE_IDLE_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(50);
	for (int i = 0; i < 24; ++i){
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 350.01987654321f + 75), 100 }, POINT{ 300, 372 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"IDLE_L", pAnimation);
}
// RUN
void CCastle::ANIM_RUN()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"CASTLE_RUN_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 6; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 340.005555555f + 70), 100 }, POINT{ 300, 372 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"RUN_R", pAnimation);
	pAnimation = new CAnimation(L"CASTLE_RUN_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 6; ++i) {
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 340.005555555f + 40), 100 }, POINT{ 300, 372 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"RUN_L", pAnimation);
}
// ATTACK
void CCastle::ANIM_ATTACK()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"CASTLE_ATTACK_R1");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(75);
	TCHAR szFile[100];

	for (int i = 1; i <= 10; ++i)
	{
		wsprintf(szFile, L"CASTLE_ATTACK_R%d", i);

		pAnimClip = new CAnimClip_N(POINT{ static_cast<int>(120), 128 }, POINT{ 300, 372 }, szFile);
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ATTACK_R", pAnimation);
	pAnimation = new CAnimation(L"CASTLE_ATTACK_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(125);
	for (int i = 1; i <= 10; ++i)
	{
		wsprintf(szFile, L"CASTLE_ATTACK_L%d", i);

		pAnimClip = new CAnimClip_N(POINT{ static_cast<int>(80), 128 }, POINT{ 300, 372 }, szFile);
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ATTACK_L", pAnimation);
}
// Dash Attack
void CCastle::ANIM_DASHATTACK()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"CASTLE_DASHATTACK_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(200);
	TCHAR szFile[100];

	for (int i = 1; i <= 8; ++i)
	{
		wsprintf(szFile, L"CASTLE_DASHATK_R%d", i);

		pAnimClip = new CAnimClip_N(POINT{ static_cast<int>(260), 428 }, POINT{ 300, 372 }, szFile);
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"DASHATK_R", pAnimation);
	pAnimation = new CAnimation(L"CASTLE_DASHATTACK_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(300);
	for (int i = 1; i <= 8; ++i)
	{
		wsprintf(szFile, L"CASTLE_DASHATK_L%d", i);

		pAnimClip = new CAnimClip_N(POINT{ static_cast<int>(240), 428 }, POINT{ 300, 372 }, szFile);
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"DASHATK_L", pAnimation);
}
// Dash 
void CCastle::ANIM_DASH()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"CASTLE_DASHATTACK_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(0);
	pAnimClip = new CAnimClip_N(POINT{ 300, 428 }, POINT{ 300, 372 }, L"CASTLE_DASH_R1");
	pAnimation->Insert_Anim(pAnimClip);
	m_pAnimator->Add_Animation(L"DASH_R", pAnimation);

	pAnimation = new CAnimation(L"CASTLE_DASHATTACK_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(0);
	pAnimClip = new CAnimClip_N(POINT{ 160, 428 }, POINT{ 300, 372 }, L"CASTLE_DASH_L1");
	pAnimation->Insert_Anim(pAnimClip);
	m_pAnimator->Add_Animation(L"DASH_L", pAnimation);
}
// Run Stop
void CCastle::ANIM_RUNSTOP()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"CASTLE_DASHATTACK_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(200);
	TCHAR szFile[100];
	for (int i = 1; i <= 3; ++i)
	{
		wsprintf(szFile, L"CASTLE_RUNSTOP_R%d", i);
		pAnimClip = new CAnimClip_N(POINT{ 130, 428 }, POINT{ 300, 372 }, szFile);
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"RUNSTOP_R", pAnimation);

	pAnimation = new CAnimation(L"CASTLE_DASHATTACK_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(200);
	for (int i = 1; i <= 3; ++i)
	{
		wsprintf(szFile, L"CASTLE_RUNSTOP_L%d", i);
		pAnimClip = new CAnimClip_N(POINT{ 130, 428 }, POINT{ 300, 372 }, szFile);
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"RUNSTOP_L", pAnimation);
}
// Hit
void CCastle::ANIM_HIT()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(0);
	pAnimClip = new CAnimClip_N(POINT{ 150, 128 }, POINT{ 300, 372 }, L"CASTLE_HIT_R1");
	pAnimation->Insert_Anim(pAnimClip);
	m_pAnimator->Add_Animation(L"HIT_R", pAnimation);

	pAnimation = new CAnimation(L"");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(0);
	pAnimClip = new CAnimClip_N(POINT{ 150, 128 }, POINT{ 300, 372 }, L"CASTLE_HIT_L1");
	pAnimation->Insert_Anim(pAnimClip);
	m_pAnimator->Add_Animation(L"HIT_L", pAnimation);
}
// Dead
void CCastle::ANIM_DEAD()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"DEAD");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(200);
	TCHAR szFile[100];
	for (int i = 1; i <= 5; ++i)
	{
		wsprintf(szFile, L"CASTLE_DEAD_R%d", i);
		pAnimClip = new CAnimClip_N(POINT{ 160, 428 }, POINT{ 300, 372 }, szFile);
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"DEAD_R", pAnimation);

	pAnimation = new CAnimation(L"DEAD");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(200);
	for (int i = 1; i <= 5; ++i)
	{
		wsprintf(szFile, L"CASTLE_DEAD_L%d", i);
		pAnimClip = new CAnimClip_N(POINT{ 160, 428 }, POINT{ 300, 372 }, szFile);
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"DEAD_L", pAnimation);
}
// Phase 2
void CCastle::ANIM_PHASE2()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"DEAD");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(200);
	TCHAR szFile[100];
	for (int i = 1; i <= 5; ++i)
	{
		wsprintf(szFile, L"CASTLE_PHASE2_R%d", i);
		pAnimClip = new CAnimClip_N(POINT{ 160, 428 }, POINT{ 300, 372 }, szFile);
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"PHASE2_R", pAnimation);

	pAnimation = new CAnimation(L"DEAD");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(200);
	for (int i = 1; i <= 5; ++i)
	{
		wsprintf(szFile, L"CASTLE_PHASE2_L%d", i);
		pAnimClip = new CAnimClip_N(POINT{ 160, 428 }, POINT{ 300, 372 }, szFile);
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"PHASE2_L", pAnimation);
}
// IDLE p2
void CCastle::ANIM_P2_IDLE()
{

	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"CASTLE_P2_IDLE_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(50);
	for (int i = 0; i < 24; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 500.f ), 0 }, POINT{ 500, 500 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"P2_IDLE_R", pAnimation);
	pAnimation = new CAnimation(L"CASTLE_P2_IDLE_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(50);
	for (int i = 0; i < 24; ++i) {
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 500.f), 0 }, POINT{ 500, 500 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"P2_IDLE_L", pAnimation);
}
// CAST
void CCastle::ANIM_P2_CAST()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"P2_CAST");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(300);
	TCHAR szFile[100];
	for (int i = 1; i <= 7; ++i)
	{
		wsprintf(szFile, L"CASTLE_P2_CAST_R%d", i);
		pAnimClip = new CAnimClip_N(POINT{ 0, 0 }, POINT{ 500, 500 }, szFile);
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"P2_CAST_R", pAnimation);

	pAnimation = new CAnimation(L"P2_CAST");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(300);
	for (int i = 1; i <= 7; ++i)
	{
		wsprintf(szFile, L"CASTLE_P2_CAST_L%d", i);
		pAnimClip = new CAnimClip_N(POINT{ 0, 0 }, POINT{ 500, 500 }, szFile);
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"P2_CAST_L", pAnimation);
}
// CAST FRONT
void CCastle::ANIM_P2_CAST_FRONT()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"P2_CAST_F");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(300);
	TCHAR szFile[100];
	for (int i = 1; i <= 7; ++i)
	{
		wsprintf(szFile, L"CASTLE_P2_CAST_F_R%d", i);
		pAnimClip = new CAnimClip_N(POINT{ 0, 0 }, POINT{ 500, 500 }, szFile);
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"P2_CAST_F_R", pAnimation);

	pAnimation = new CAnimation(L"P2_CAST_F");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(300);
	for (int i = 1; i <= 7; ++i)
	{
		wsprintf(szFile, L"CASTLE_P2_CAST_F_L%d", i);
		pAnimClip = new CAnimClip_N(POINT{ 0, 0 }, POINT{ 500, 500 }, szFile);
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"P2_CAST_F_L", pAnimation);
}
// HIT
void CCastle::ANIM_P2_HIT()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(0);
	pAnimClip = new CAnimClip_N(POINT{ 0, 0 }, POINT{ 500, 500 }, L"CASTLE_HIT_P2_R");
	pAnimation->Insert_Anim(pAnimClip);
	m_pAnimator->Add_Animation(L"HIT_P2_R", pAnimation);

	pAnimation = new CAnimation(L"");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(0);
	pAnimClip = new CAnimClip_N(POINT{ 0, 0 }, POINT{ 500, 500 }, L"CASTLE_HIT_P2_L");
	pAnimation->Insert_Anim(pAnimClip);
	m_pAnimator->Add_Animation(L"HIT_P2_L", pAnimation);
}
// DIE
void CCastle::ANIM_P2_DIE()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation(L"P2_DIE_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(200);
	TCHAR szFile[100];
	for (int i = 1; i <= 12; ++i)
	{
		wsprintf(szFile, L"CASTLE_DIE_P2_R%d", i);
		pAnimClip = new CAnimClip_N(POINT{ 0, 0 }, POINT{ 500, 500 }, szFile);
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"P2_DIE_R", pAnimation);

	pAnimation = new CAnimation(L"CASTLE_DIE_P2");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(200);
	for (int i = 1; i <= 12; ++i)
	{
		wsprintf(szFile, L"CASTLE_DIE_P2_L%d", i);
		pAnimClip = new CAnimClip_N(POINT{ 0, 0 }, POINT{ 500, 500 }, szFile);
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"P2_DIE_L", pAnimation);
}
