#include "pch.h"
#include "CParticleMgr.h"
#include "CGameMgr.h"

CParticleMgr::CParticleMgr()
{
}

CParticleMgr::CParticleMgr(CGameMgr *& pGameMgr) : CManager(pGameMgr), m_iUseParticleIndex(0)
{
	Initialize();
}

CParticleMgr::~CParticleMgr()
{
	Release();
}

void CParticleMgr::Initialize()
{
	for (int i = 0; i < PTCL_END; ++i)
	{
		m_vecParticleList[i].clear();
		m_vecParticleList[i].reserve(MAX_PARTICLE_NUM);
	}
	// Particle Blood
	for (auto& iter : m_vecParticleList[PTCL_BOSS])
	{
		m_vecParticleList[PTCL_BOSS].emplace_back(nullptr);
	}
}

int CParticleMgr::Update()
{
	return 0;
}

void CParticleMgr::Release()
{

}

void CParticleMgr::Out_Blood(const VECTOR2& v2Pos, const VECTOR2& v2Dir, const float& fPower)
{


}
