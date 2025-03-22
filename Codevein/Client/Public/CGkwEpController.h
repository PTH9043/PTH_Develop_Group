#pragma once
#include "Client_Defines.h"
#include "CBase.h"

BEGIN(Engine)
class CCollider;
class CHierarchyNode;
END

BEGIN(Client)
class CGateKeeperWoman;
class CEffectParticleGroup;
class CGKWStateMachine;
class CParticle;
class CEffect;
class CEffectGroup;

class CGkwEpController : public CBase
{
public:
	typedef struct tagGKEpDesc
	{
		CGateKeeperWoman* pInstance = nullptr;
	}GKWEPDESC;
private:
	explicit CGkwEpController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CGkwEpController() = default;
public:
	HRESULT NativeConstruct(void* _pArg);
	void Update(const _double& _dDeltaSeconds, CGKWStateMachine* _pMonsterStateMachine, CGateKeeperWoman* _pMonster);
	void Late_Update(const _double& _dDeltaSeconds, CGKWStateMachine* _pMonsterStateMachine, CGateKeeperWoman* _pMonster);

public:
	void Boss_RoadEffect(_fvector _vPos);
	void Boss_RoadEnd();

	void Boss_UpEffect(_fvector _vPos);
	void Boss_UpEffect_End();

	void Boss_Shoot(_fvector _vPos, _fvector _vDir);
	void Boss_Shoot_End();

	void Boss_RangeShoot(_fvector _vPos, _fvector _vRight);
	void Boss_RangeShoot_End();

	void Reset_State();

	_bool Is_Collision_UpEffect(CCollider* _pCollider);
	_bool Is_Collision_Shoot(CCollider* _pCollider);
protected:
	ID3D11Device*														m_pDevice = nullptr;
	ID3D11DeviceContext*											m_pContext = nullptr;

	vector<CEffectGroup*>										m_BossAttackEffectList;
	vector<CEffectGroup*>										m_BossRoadEffectList;
	vector<CEffectGroup*>										m_BossUpEffectList;
	vector<CEffectParticleGroup*>							m_GateKeeprWomanMagic;
	vector<CCollider*>												m_CollisionList;
	list<CCollider*>														m_GetCollisionList;
	// Attack Positon List
	vector<_float3>														m_vAttackPositionList;

	_bool																			m_bTurnOnLifeLoop_UpEffect = false;
	_bool																			m_bOneShoot = false;
	_bool																			m_bRangeShoot = false;
	_bool																			m_bActive_UpEffect = false;
	_uint																			m_iBossRoadEffect = 0;
public:
	static CGkwEpController* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	virtual void Free() override;
};

END