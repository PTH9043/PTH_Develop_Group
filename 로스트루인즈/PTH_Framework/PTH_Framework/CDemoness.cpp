#include "pch.h"
#include "CDemoness.h"
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

CDemoness::CDemoness()
	: m_bShoot(false)
{
	Initialize();
}

CDemoness::CDemoness(CPlayer * pTarget) : CMonster(pTarget), m_bShoot(false)
{
	Initialize();
}

CDemoness::~CDemoness()
{
	Release();
}

void CDemoness::Initialize()
{
	// Name Text
	m_strName = L"Demoness";
	m_pNameText = new CFollowText(this, L"맑은고딕", tagRGB(255, 255, 255));
	m_pNameText->Set_CurPos(VECTOR2(3.f, 28.f) + m_v2CurPos);
	m_pNameText->Set_Size(VECTOR2(15.f, 0.f));
	m_pNameText->Set_Text(m_strName);
	m_eObjID = OBJ_MONSTER;
	// Monster AI
	CMonster* pMonster = this;
	m_pMonsterAI = new CMonsterAI(pMonster, m_pTarget);
	m_pMonsterAI->Set_Find_Range(400);
	m_pMonsterAI->Set_Attack_Range(100);
	m_v2Size = VECTOR2(80.f, 150);
	m_v2DrawSize = VECTOR2(400.f, 400.f);
	m_eDirection = DIR_BACK;
	// Jump Define
	m_fJumpPower = 10.f;
	m_fRestrictTime = 0.02f;
	// Status Define
	m_tStatus.iMoveSpeed = 3;
	m_tStatus.iAtkPower = 1;
	m_tStatus.iMaxHp = 30;
	m_tStatus.iMaxMp = 0;
	m_tStatus.iCurHp = m_tStatus.iMaxHp;
	m_tStatus.iCurMp = m_tStatus.iMaxMp;
	Anim_Setting();

	m_tRestrictDraw.left -= 100;
	m_tRestrictDraw.top -= 100;
	m_tRestrictDraw.right += 100;
	m_tRestrictDraw.bottom += 100;
}

int CDemoness::Update()
{
	if (true == m_bDead)
	{
		Dead();
		if (1 <= m_iDeadCount)
		{
			m_pNameText->Set_Dead(true);
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

void CDemoness::Late_Update()
{
	Collision_Wall();
	CObjMgr* pObjMgr = CGameMgr::Get_Instance()->Get_ObjMgr();
	list<CObj*> itemList = pObjMgr->Get_ObjList(OBJ_EQUIP);
	CObj* pDemoness = static_cast<CObj*>(this);
	CObj* pObj = static_cast<CObj*>(m_pTarget);
	for (auto& iter : itemList)
	{
		iter->Collision_Actor(pDemoness);
	}
	Collision_Actor(pObj);
}

void CDemoness::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
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

void CDemoness::Collision_Actor(CObj *& pObj)
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
			if (m_pAnimator->Get_Animation()->Get_AnimIndex() <= 4)
			{
				m_pTarget->Damaged(static_cast<float>(m_tStatus.iAtkPower));
				Blood_Effect_To_Player(pObj, 1);
			}
		}
		else
		{
			if (m_pAnimator->Get_Animation()->Get_AnimIndex() >= 8)
			{
				m_pTarget->Damaged(static_cast<float>(m_tStatus.iAtkPower));
				Blood_Effect_To_Player(pObj, 1);
			}
		}
	}
}

void CDemoness::Release()
{
	for (auto& iter : m_vecDamageText)
		DELETE_MEMORY(iter);

	DELETE_MEMORY(m_pNameText);
	DELETE_MEMORY(m_pAnimator);
	DELETE_MEMORY(m_pMonsterAI);
}

void CDemoness::Damaged(const float & fDamage)
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

void CDemoness::Change_State(const ECHARSTATE & eCharState)
{
	if (CHAR_HIT == eCharState)
	{
		if (DIR_FOR == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"HIT_R");
		else if (DIR_BACK == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"HIT_L");

		m_eCharState = eCharState;
		CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"DemonessHit1.wav", SOUND_DEMONASS, g_fSound);
	}
	else if (CHAR_DIE == eCharState)
	{
		if (DIR_FOR == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"DEAD_R");
		else if (DIR_BACK == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"DEAD_L");

		CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"DemonessDie.wav", SOUND_DEMONASS, g_fSound);
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
			m_bCollision = false;
		}
		m_eCharState = eCharState;
	}
}

CMonster * CDemoness::Copy_Monster()
{
	CMonster* pMonster = new CDemoness(m_pTarget);
	pMonster->Set_Enable(true);
	return pMonster;
}

void CDemoness::Obj_Active(const bool & IsActive)
{
	m_bEnable = IsActive;
	m_pNameText->Set_Enable(false);
}

void CDemoness::Update_Rect()
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

void CDemoness::Update_Attack_Rect()
{
	if (DIR_FOR == m_eDirection)
	{
		m_tAttackRect.left = static_cast<int>(m_v2CenterPos.x);
		m_tAttackRect.right = static_cast<int>(m_v2CenterPos.x + m_v2DrawSize.x / 4.f);
		m_tAttackRect.top = static_cast<int>(m_v2CenterPos.y + m_v2Size.y / 2.f - 50);
		m_tAttackRect.bottom = static_cast<int>(m_v2CenterPos.y + m_v2Size.y / 2.f);
	}
	else
	{
		m_tAttackRect.left = static_cast<int>(m_v2CenterPos.x - m_v2DrawSize.x / 4.f);
		m_tAttackRect.right = static_cast<int>(m_v2CenterPos.x);
		m_tAttackRect.top = static_cast<int>(m_v2CenterPos.y + m_v2Size.y / 2.f - 50);
		m_tAttackRect.bottom = static_cast<int>(m_v2CenterPos.y + m_v2Size.y / 2.f);
	}
}

void CDemoness::Update_Text()
{
	if (nullptr != m_pNameText)
	{
		m_pNameText->Set_CurPos(VECTOR2(-35.f, -m_v2Size.y - 20.f) + m_v2CurPos);
		m_pNameText->Update();
	}
}

void CDemoness::Dead()
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

void CDemoness::Attack()
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

void CDemoness::Move()
{
	CMonster::Move();
}

void CDemoness::Jump()
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

void CDemoness::Anim_Setting()
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

void CDemoness::ANIM_IDLE()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"DEMESS_IDLE_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(75);
	for (int i = 0; i < 15; ++i) {

		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 500) , 0 }, POINT{ 500, 500 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"IDLE_R", pAnimation);
	pAnimation = new CAnimation(L"DEMESS_IDLE_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 15; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 500), 0 }, POINT{ 500, 500 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"IDLE_L", pAnimation);
}

void CDemoness::ANIM_RUN()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"DEMESS_WALK_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(75);
	for (int i = 0; i < 6; ++i) {

		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 500) , 0 }, POINT{ 500, 500 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"RUN_R", pAnimation);
	pAnimation = new CAnimation(L"DEMESS_WALK_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 6; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 500), 0 }, POINT{ 500, 500 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"RUN_L", pAnimation);
}

void CDemoness::ANIM_ATTACK()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"DEMESS_ATTACK_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 11; ++i) {

		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 500) , 0 }, POINT{ 500, 500 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ATTACK_R", pAnimation);
	pAnimation = new CAnimation(L"DEMESS_ATTACK_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(75);
	for (int i = 0; i < 11; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 500), 0 }, POINT{ 500, 500 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ATTACK_L", pAnimation);
}

void CDemoness::ANIM_HIT()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"DEMESS_DAMAGE_TAKEN_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(175);
	for (int i = 0; i < 1; ++i) {

		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 500) , 0 }, POINT{ 500, 500 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"HIT_R", pAnimation);
	pAnimation = new CAnimation(L"DEMESS_DAMAGE_TAKEN_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(75);
	for (int i = 0; i < 1; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 500), 0 }, POINT{ 500, 500 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"HIT_L", pAnimation);
}

void CDemoness::ANIM_DAMAGED()
{

}

void CDemoness::ANIM_DEAD()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"DEMESS_DIE_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(175);
	for (int i = 0; i < 5; ++i) {

		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 500) , 0 }, POINT{ 500, 500 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"DEAD_R", pAnimation);
	pAnimation = new CAnimation(L"DEMESS_DIE_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(75);
	for (int i = 0; i < 5; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 500), 0 }, POINT{ 500, 500 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"DEAD_L", pAnimation);
}
