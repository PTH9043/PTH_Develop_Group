#include "pch.h"
#include "CDemonAssasin.h"
#include "CAbomiGreen.h"
#include "CFollowText.h"
#include "CMonsterAI.h"
#include "CGameMgr.h"
#include "CAnimator.h"
#include "CAnimation.h"
#include "CAnimation_L.h"
#include "CAnimClip.h"
#include "CAnimClip_N.h"
#include "CCollision.h"
#include "CWall.h"
#include "CDownWall.h"

CDemonAssasin::CDemonAssasin() : m_iSaveSpeed(0)
{
	Initialize();
}

CDemonAssasin::CDemonAssasin(CPlayer * pTarget)
	: CMonster(pTarget), m_iSaveSpeed(0)
{
	Initialize();
}

CDemonAssasin::~CDemonAssasin()
{
	Release();
}

void CDemonAssasin::Initialize()
{
	// Name Text
	m_strName = L"DemonAssasin";
	m_pNameText = new CFollowText(this, L"맑은고딕", tagRGB(255, 255, 255));
	m_pNameText->Set_CurPos(VECTOR2(3.f, 28.f) + m_v2CurPos);
	m_pNameText->Set_Size(VECTOR2(15.f, 0.f));
	m_pNameText->Set_Text(m_strName);
	m_pNameText->Set_Enable(true);
	m_eObjID = OBJ_MONSTER;
	// Monster AI
	CMonster* pMonster = this;
	m_pMonsterAI = new CMonsterAI(pMonster, m_pTarget);
	m_pMonsterAI->Set_Find_Range(400);
	m_pMonsterAI->Set_Attack_Range(70);
	m_v2Size = VECTOR2(60.f, 90);
	m_v2DrawSize = VECTOR2(200.f, 200.f);
	m_eDirection = DIR_BACK;
	// Jump Define
	m_fJumpPower = 10.f;
	m_fRestrictTime = 0.02f;
	// Status Define
	m_tStatus.iMoveSpeed = 2;
	m_iSaveSpeed = m_tStatus.iMoveSpeed;
	m_tStatus.iAtkPower = 3;
	m_tStatus.iMaxHp = 20;
	m_tStatus.iMaxMp = 0;
	m_iGold = 50;
	m_tStatus.iCurHp = m_tStatus.iMaxHp;
	m_tStatus.iCurMp = m_tStatus.iMaxMp;
	Anim_Setting();
}

int CDemonAssasin::Update()
{
	if (true == m_bDead)
	{
		Dead();
		if (1 <= m_iDeadCount)
		{
			for (auto& iter : m_vecDamageText)
				iter->Set_Enable(false);
			return OBJ_DEADEVENT;
		}
	}

	if (nullptr != m_pMonsterAI)
	{
		if (CHAR_ATTACK != m_eCharState)
			m_pMonsterAI->Update();
		Attack();
		Move();
		Jump();
	}
	Update_Text();
	Update_Rect();

	if (nullptr != m_pAnimator)
	{
		m_pAnimator->Update_Animation();
	}

	for (auto& iter : m_vecDamageText)
		iter->Update();

	return OBJ_NOEVENT;
}

void CDemonAssasin::Late_Update()
{
	Collision_Wall();
	CObjMgr* pObjMgr = CGameMgr::Get_Instance()->Get_ObjMgr();
	list<CObj*> itemList = pObjMgr->Get_ObjList(OBJ_EQUIP);
	CObj* pAss = static_cast<CObj*>(this);
	CObj* pObj = static_cast<CObj*>(m_pTarget);
	for (auto& iter : itemList)
	{
		iter->Collision_Actor(pAss);
	}
	Collision_Actor(pObj);
}

void CDemonAssasin::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	if (false == Check_Pos_IsInRect(v2ScrollPos, m_tRestrictDraw))
	{
		m_pNameText->Set_Enable(false);
		m_bActiveUI = false;
		return;
	}
	else if (false == m_bActiveUI)
	{
		m_pNameText->Set_Enable(true);
		m_bActiveUI = true;
	}

	//Rectangle(_hdc, static_cast<int>(m_tRect.left + v2ScrollPos.x), static_cast<int>(m_tRect.top + v2ScrollPos.y),
	//	static_cast<int>(m_tRect.right + v2ScrollPos.x), static_cast<int>(m_tRect.bottom + v2ScrollPos.y));

	//Rectangle(_hdc, static_cast<int>(m_tAttackRect.left + v2ScrollPos.x), static_cast<int>(m_tAttackRect.top + v2ScrollPos.y),
	//	static_cast<int>(m_tAttackRect.right + v2ScrollPos.x), static_cast<int>(m_tAttackRect.bottom + v2ScrollPos.y));

	if (nullptr != m_pAnimator)
	{
		m_pAnimator->Render_Animation(_hdc, v2ScrollPos);
	}

	m_pNameText->Render(_hdc, v2ScrollPos);

	for (auto& iter : m_vecDamageText)
	{
		if (iter->Is_Enable())
			iter->Render(_hdc, v2ScrollPos);
	}
}

void CDemonAssasin::Collision_Actor(CObj *& pObj)
{
	if (nullptr == pObj)
		return;
	if (true == m_bCollision)
		return;

	if (OBJ_PLAYER != pObj->Get_ObjID())
		return;
	RECT rc;
	if (CHAR_ATTACK == m_eCharState)
	{
		// Update Attack Rect
		Update_Attack_Rect();
		// Intersect 
		if (FALSE == IntersectRect(&rc, &pObj->Get_Rect(), &m_tAttackRect))
			return;
		if (DIR_FOR == m_eDirection)
		{
			if (m_pAnimator->Get_Animation()->Get_AnimIndex() >= 3)
			{
				m_pTarget->Damaged(static_cast<float>(m_tStatus.iAtkPower));
				Blood_Effect_To_Player(pObj, 0);
			}
		}
		else
		{
			if (m_pAnimator->Get_Animation()->Get_AnimIndex() >= 4)
			{
				m_pTarget->Damaged(static_cast<float>(m_tStatus.iAtkPower));
				Blood_Effect_To_Player(pObj, 0);
			}
		}
	}
}

void CDemonAssasin::Release()
{
	for (auto& iter : m_vecDamageText)
		DELETE_MEMORY(iter);

	DELETE_MEMORY(m_pNameText);

	DELETE_MEMORY(m_pAnimator);
	DELETE_MEMORY(m_pMonsterAI);
}

void CDemonAssasin::Damaged(const float & fDamage)
{
	if (false == m_bDead && CHAR_HIT != m_eCharState)
	{
		Change_State(CHAR_HIT);
		int iHp = static_cast<int>(m_tStatus.iCurHp - fDamage);
		m_tStatus.iCurHp = iHp;
		if (m_tStatus.iCurHp <= 0)
		{
			m_bDead = true;
			m_eDirection = m_pMonsterAI->Get_SaveDirection();
			Change_State(CHAR_DIE);
		}
		else
		{
			Active_DamageText(fDamage);
		}
	}
}

void CDemonAssasin::Change_State(const ECHARSTATE & eCharState)
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
			m_pAnimator->Set_CurAnimation(L"DIE_R");
		else if (DIR_BACK == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"DIE_L");


		m_eCharState = eCharState;
	}
	else
	{
		if (CHAR_IDLE == eCharState)
		{
			if (DIR_FOR == m_eDirection)
				m_pAnimator->Set_CurAnimation(L"IDLE_R");
			else if (DIR_BACK == m_eDirection)
				m_pAnimator->Set_CurAnimation(L"IDLE_L");
		}
		else if (CHAR_MOVE == eCharState)
		{
			if (true == m_pMonsterAI->Is_FindTarget())
			{
				if (DIR_FOR == m_eDirection)
					m_pAnimator->Set_CurAnimation(L"RUN_R");
				else if (DIR_BACK == m_eDirection)
					m_pAnimator->Set_CurAnimation(L"RUN_L");

				m_tStatus.iAtkSpeed = 15;
			}
			else
			{
				if (DIR_FOR == m_eDirection)
					m_pAnimator->Set_CurAnimation(L"WALK_R");
				else if (DIR_BACK == m_eDirection)
					m_pAnimator->Set_CurAnimation(L"WALK_L");

				m_tStatus.iAtkSpeed = m_iSaveSpeed;
			}
		}
		else if (CHAR_ATTACK == eCharState)
		{
			if (DIR_FOR == m_eDirection)
				m_pAnimator->Set_CurAnimation(L"ATTACK_R");
			else if (DIR_BACK == m_eDirection)
				m_pAnimator->Set_CurAnimation(L"ATTACK_L");

			m_bCollision = false;
		}
		m_eCharState = eCharState;
	}
}

CMonster * CDemonAssasin::Copy_Monster()
{
	CMonster* pMonster = new CDemonAssasin(m_pTarget);
	pMonster->Set_Enable(true);
	return pMonster;
}

void CDemonAssasin::Obj_Active(const bool & IsActive)
{
	m_bEnable = IsActive;
	m_pNameText->Set_Enable(false);
}

void CDemonAssasin::Update_Rect()
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

void CDemonAssasin::Update_Attack_Rect()
{
	if (DIR_FOR == m_eDirection)
	{
		m_tAttackRect.left = static_cast<int>(m_v2CenterPos.x);
		m_tAttackRect.right = static_cast<int>(m_v2CenterPos.x + m_v2DrawSize.x / 4.f);
		m_tAttackRect.top = static_cast<int>(m_v2CenterPos.y - 10);
		m_tAttackRect.bottom = static_cast<int>(m_v2CenterPos.y + 10);
	}
	else
	{
		m_tAttackRect.left = static_cast<int>(m_v2CenterPos.x - m_v2DrawSize.x / 4.f);
		m_tAttackRect.right = static_cast<int>(m_v2CenterPos.x);
		m_tAttackRect.top = static_cast<int>(m_v2CenterPos.y - 10);
		m_tAttackRect.bottom = static_cast<int>(m_v2CenterPos.y + 10);
	}
}

void CDemonAssasin::Update_Text()
{
	if (nullptr != m_pNameText)
	{
		m_pNameText->Set_CurPos(VECTOR2(-40.f, -m_v2Size.y - 20.f) + m_v2CurPos);
		m_pNameText->Update();
	}
}

void CDemonAssasin::Dead()
{
	if (CHAR_DIE == m_eCharState)
	{
		if (true == m_pMonsterAI->Is_DeadTimerOk())
		{
			++m_iDeadCount;
		}
		m_pAnimator->Get_Animation()->Set_LastAnimFix(true);
	}
}

void CDemonAssasin::Attack()
{
	if (CHAR_ATTACK == m_eCharState)
	{
		if (true == m_pAnimator->Get_Animation()->Is_last())
		{
			if (m_pMonsterAI->Is_FindTarget())
				Change_State(CHAR_MOVE);
			else
				Change_State(CHAR_IDLE);

			m_bCollision = false;
		}
	}
}

void CDemonAssasin::Move()
{
	CMonster::Move();
}

void CDemonAssasin::Jump()
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
			if ((CHAR_HIT == m_eCharState || CHAR_DIE == m_eCharState) && false == m_bDamaged)
			{
				// 타이머 초기화
				// 땅에 닿은 상태가 아님을 명시
				m_fDownTime = m_fRestrictTime;
				m_bIsGround = false;
				m_bJump = true;
				m_bDamaged = true;
			}
			else
			{
				if (!(CHAR_HIT == m_eCharState || CHAR_DIE == m_eCharState))
					m_bDamaged = false;
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
		if (CHAR_DIE != m_eCharState)
		{
			// 그라운드를 true로 변경
			// 현재 Y값을 고정시킨다. 
			if (m_v2CurPos.y >= fY)
			{
				m_v2CurPos.y = fY;
				m_bIsGround = true;
			}
		}
	}

	// 일정 시간 지날 때 마다 타이머 초기화
	if (m_fDownTime > m_fRestrictTime)
		m_fDownTime = m_fRestrictTime;
}

void CDemonAssasin::Anim_Setting()
{
	CObj* pObj = this;
	m_pAnimator = new CAnimator(pObj);
	ANIM_IDLE();
	ANIM_RUN();
	ANIM_ATTACK();
	ANIM_HIT();
	ANIM_DAMAGED();
	ANIM_DEAD();
	// 마지막, 애니메이션 세팅해주는 함수 호출
	m_pAnimator->Set_CurAnimation(L"DIE_R");
}

void CDemonAssasin::ANIM_IDLE()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"DEMASS_IDLE_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 16; ++i) {

		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) , 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"IDLE_R", pAnimation);
	pAnimation = new CAnimation(L"DEMASS_IDLE_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 16; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300), 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"IDLE_L", pAnimation);
}

void CDemonAssasin::ANIM_RUN()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"DEMASS_RUN_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 6; ++i) {

		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) , 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"RUN_R", pAnimation);
	pAnimation = new CAnimation(L"DEMASS_RUN_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 6; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300), 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"RUN_L", pAnimation);
	// ---------------------------- WALK ----------------------------
	pAnimation = nullptr;
	pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"DEMASS_WALK_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(50);
	for (int i = 0; i < 24; ++i) {

		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) , 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"WALK_R", pAnimation);
	pAnimation = new CAnimation(L"DEMASS_WALK_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(50);
	for (int i = 0; i < 24; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300), 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"WALK_L", pAnimation);
}

void CDemonAssasin::ANIM_ATTACK()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"DEMASS_ATTACK_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(75);
	for (int i = 0; i < 6; ++i) {

		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) , 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ATTACK_R", pAnimation);
	pAnimation = new CAnimation(L"DEMASS_ATTACK_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(75);
	for (int i = 0; i < 6; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300), 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ATTACK_L", pAnimation);
}

void CDemonAssasin::ANIM_HIT()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"DEMASS_DAMAGE_TAKEN_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 1; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) , 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"HIT_R", pAnimation);
	pAnimation = new CAnimation(L"DEMASS_DAMAGE_TAKEN_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 1; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300), 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"HIT_L", pAnimation);
}

void CDemonAssasin::ANIM_DAMAGED()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"DEMASS_DAMAGED_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 1; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) , 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"DAMAGED_R", pAnimation);
	pAnimation = new CAnimation(L"DEMASS_DAMAGED_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 1; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300), 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"DAMAGED_L", pAnimation);
}

void CDemonAssasin::ANIM_DEAD()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"DEMASS_DIE_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 3; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) , 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"DIE_R", pAnimation);
	pAnimation = new CAnimation(L"DEMASS_DIE_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 3; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(300 * i) , 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"DIE_L", pAnimation);
}
