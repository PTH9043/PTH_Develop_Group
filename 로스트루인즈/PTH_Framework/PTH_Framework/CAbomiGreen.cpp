#include "pch.h"
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
#include "CArrow.h"

CAbomiGreen::CAbomiGreen() :  m_bShoot(false)
{
	Initialize();
}

CAbomiGreen::CAbomiGreen(CPlayer * pTarget) : CMonster(pTarget), m_bShoot(false)
{
	Initialize();
}

CAbomiGreen::~CAbomiGreen()
{
	Release();
}

void CAbomiGreen::Initialize()
{
	// Name Text
	m_strName = L"AbomiGreen";
	m_pNameText = new CFollowText(this, L"맑은고딕", tagRGB(255, 255, 255));
	m_pNameText->Set_CurPos(VECTOR2(3.f, 28.f) + m_v2CurPos);
	m_pNameText->Set_Size(VECTOR2(15.f, 0.f));
	m_pNameText->Set_Text(m_strName);
	m_eObjID = OBJ_MONSTER;
	// Monster AI
	CMonster* pMonster = this;
	m_pMonsterAI = new CMonsterAI(pMonster, m_pTarget);
	m_pMonsterAI->Set_Find_Range(400);
	m_pMonsterAI->Set_Attack_Range(300);
	m_v2Size = VECTOR2(60.f, 90);
	m_v2DrawSize = VECTOR2(200.f, 200.f);
	m_eDirection = DIR_BACK;
	// Jump Define
	m_fJumpPower = 10.f;
	m_fRestrictTime = 0.02f;
	// Status Define
	m_tStatus.iMoveSpeed = 1;
	m_tStatus.iAtkPower = 1;
	m_tStatus.iMaxHp = 5;
	m_tStatus.iMaxMp = 0;
	m_tStatus.iCurHp = m_tStatus.iMaxHp;
	m_tStatus.iCurMp = m_tStatus.iMaxMp;
	m_iGold = 10;
	Anim_Setting();
}

int CAbomiGreen::Update()
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

void CAbomiGreen::Late_Update()
{
	Collision_Wall();
	CObjMgr* pObjMgr = CGameMgr::Get_Instance()->Get_ObjMgr();
	list<CObj*> itemList = pObjMgr->Get_ObjList(OBJ_EQUIP);
	CObj* pAbomi = static_cast<CObj*>(this);
	for (auto& iter : itemList)
	{
		iter->Collision_Actor(pAbomi);
	}
}

void CAbomiGreen::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
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

void CAbomiGreen::Collision_Actor(CObj *& pObj)
{
	if (nullptr == pObj)
		return;

	if (OBJ_PLAYER != pObj->Get_ObjID())
		return;
}

void CAbomiGreen::Release()
{
	for (auto& iter : m_vecDamageText)
		DELETE_MEMORY(iter);

	DELETE_MEMORY(m_pNameText);
	DELETE_MEMORY(m_pAnimator);
	DELETE_MEMORY(m_pMonsterAI);
}

void CAbomiGreen::Damaged(const float & fDamage)
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

void CAbomiGreen::Change_State(const ECHARSTATE & eCharState)
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

CMonster * CAbomiGreen::Copy_Monster()
{
	CMonster* pMonster = new CAbomiGreen(m_pTarget);
	pMonster->Set_Enable(true);
	return pMonster;
}

void CAbomiGreen::Obj_Active(const bool & IsActive)
{
	m_bEnable = IsActive;
	m_pNameText->Set_Enable(false);
}

void CAbomiGreen::Update_Rect()
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

void CAbomiGreen::Update_Attack_Rect()
{
}

void CAbomiGreen::Update_Text()
{
	if (nullptr != m_pNameText)
	{
		m_pNameText->Set_CurPos(VECTOR2(-10.f, -m_v2Size.y - 20.f) + m_v2CurPos);
		m_pNameText->Update();
	}
}

void CAbomiGreen::Dead()
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

void CAbomiGreen::Attack()
{
	if (CHAR_ATTACK == m_eCharState)
	{
		bool shooting = false;
		if(DIR_FOR == m_pAnimator->Get_Animation()->Get_Direction())
			shooting = 6 <= m_pAnimator->Get_Animation()->Get_AnimIndex();
		else 
			shooting = 6 >= m_pAnimator->Get_Animation()->Get_AnimIndex();

		if (shooting)
		{
			if (false == m_bShoot)
			{
				CArrow* pArrow = new CArrow(this, L"Arrow_1", L"Arrow");
				pArrow->Set_CenterPos(m_v2CenterPos + m_v2Dir * VECTOR2(5.f, 1.f));
				pArrow->Set_Dir(m_v2Dir);
				pArrow->Set_DrawSize(VECTOR2(100.f, 100.f));
				pArrow->Set_Direction(m_eDirection);
				pArrow->Set_Speed(20);
				pArrow->Obj_Active(true);
				CGameMgr::Get_Instance()->Get_ObjMgr()->Add_Object(OBJ_ITEM, pArrow);
				m_bShoot = true;
				CGameMgr::Get_Instance()->Get_SoundMgr()->PlaySound(L"BOW_Release_Arrow_mono.wav", SOUND_ABOMIGREEN, g_fSound);
			}

			if (m_pMonsterAI->Is_FindTarget())
				Change_State(CHAR_MOVE);
			else
				Change_State(CHAR_IDLE);
		}
		else
			m_bShoot = false;
	}
}

void CAbomiGreen::Move()
{
	CMonster::Move();
}

void CAbomiGreen::Jump()
{
	CMonster::Jump();
}

void CAbomiGreen::Anim_Setting()
{
	CObj* pObj = this;
	m_pAnimator = new CAnimator(pObj);
	ANIM_IDLE();
	ANIM_RUN();
	ANIM_ATTACK();
	ANIM_HIT();
	ANIM_DEAD();
	// 마지막, 애니메이션 세팅해주는 함수 호출
	m_pAnimator->Set_CurAnimation(L"IDLE_L");
}
// IDLE
void CAbomiGreen::ANIM_IDLE()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"ABOMI_GREEN_IDLE_R");
	pAnimation->Set_RGB(85, 255, 0);
	pAnimation->Set_Fps(50);
	for (int i = 0; i < 24; ++i){
	
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) , 0}, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"IDLE_R", pAnimation);
	pAnimation = new CAnimation(L"ABOMI_GREEN_IDLE_L");
	pAnimation->Set_RGB(85, 255, 0);
	pAnimation->Set_Fps(50);
	for (int i = 0; i < 24; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300), 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"IDLE_L", pAnimation);
}
// RUN
void CAbomiGreen::ANIM_RUN()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"ABOMI_GREEN_RUN_R");
	pAnimation->Set_RGB(85, 255, 0);
	pAnimation->Set_Fps(125);
	for (int i = 0; i < 6; ++i) {

		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) , 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"RUN_R", pAnimation);
	pAnimation = new CAnimation(L"ABOMI_GREEN_RUN_L");
	pAnimation->Set_RGB(85, 255, 0);
	pAnimation->Set_Fps(125);
	for (int i = 0; i < 6; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300), 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"RUN_L", pAnimation);
}
// ATTACK
void CAbomiGreen::ANIM_ATTACK()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"ABOMI_GREEN_ATTACK_R");
	pAnimation->Set_RGB(85, 255, 0);
	pAnimation->Set_Fps(250);
	for (int i = 0; i < 12; ++i) {

		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) , 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ATTACK_R", pAnimation);
	pAnimation = new CAnimation(L"ABOMI_GREEN_ATTACK_L");
	pAnimation->Set_RGB(85, 255, 0);
	pAnimation->Set_Fps(250);
	for (int i = 0; i < 12; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300), 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ATTACK_L", pAnimation);
}
// HIT
void CAbomiGreen::ANIM_HIT()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"ABOMI_GREEN_DAMAGED_R");
	pAnimation->Set_RGB(85, 255, 0);
	pAnimation->Set_Fps(150);
	for (int i = 0; i < 1; ++i) {

		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) , 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"HIT_R", pAnimation);
	pAnimation = new CAnimation(L"ABOMI_GREEN_DAMAGED_L");
	pAnimation->Set_RGB(85, 255, 0);
	pAnimation->Set_Fps(150);
	for (int i = 0; i < 1; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300), 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"HIT_L", pAnimation);
}
// DEAD
void CAbomiGreen::ANIM_DEAD()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"ABOMI_GREEN_DIE_R");
	pAnimation->Set_RGB(85, 255, 0);
	pAnimation->Set_Fps(150);
	for (int i = 0; i < 9; ++i) {

		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) , 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"DEAD_R", pAnimation);
	pAnimation = new CAnimation(L"ABOMI_GREEN_DIE_L");
	pAnimation->Set_RGB(85, 255, 0);
	pAnimation->Set_Fps(150);
	for (int i = 0; i < 9; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300), 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"DEAD_L", pAnimation);
}
