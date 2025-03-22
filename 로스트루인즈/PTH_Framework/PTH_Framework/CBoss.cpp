#include "pch.h"
#include "CBoss.h"


CBoss::CBoss() : m_eBossState(BOSS_END), m_bChange(false)
{
	m_vecBarList.clear();
}

CBoss::CBoss(CPlayer * pTarget) : CMonster(pTarget), m_eBossState(BOSS_END)
, m_bChange(false)
{
	m_vecBarList.clear();
	m_v2BossBarPos.x = 100;
	m_v2BossBarPos.y = 510;
}

CBoss::~CBoss()
{
	Release();
}

void CBoss::Collision_Actor(CObj *& pObj)
{
}

void CBoss::Release()
{
	for (auto& iter : m_vecBarList)
	{
		iter->Set_Dead(true);
	}
}

void CBoss::Damaged(const float & fDamage)
{
}