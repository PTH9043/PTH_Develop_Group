#pragma once
#include "Client_Defines.h"
#include "CBase.h"

BEGIN(Engine)
class CCollider;
class CHierarchyNode;
END

BEGIN(Client)
class CAncientHannibal;
class CEffectParticleGroup;
class CAncHanibalStateMachine;
class CParticle;

class CAncEPController : public CBase
{  
public:
	typedef struct tagAncEPDesc
	{
		CAncientHannibal* pInstance = nullptr;
	}ANCEPDESC;
private:
	explicit CAncEPController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CAncEPController() = default;

public:
	const _bool& Is_StopAnimation() const { return m_bStopAnimation; }
public:
	HRESULT NativeConstruct(void* _pArg);
	void Update(const _double& _dDeltaSeconds, CAncHanibalStateMachine* _pMonsterStateMachine, CAncientHannibal* _pMonster);
	void Late_Update(const _double& _dDeltaSeconds, CAncHanibalStateMachine* _pMonsterStateMachine, CAncientHannibal* _pMonster);

	void Attack_FireWallStatre(_fvector vPlayerPos, _fvector vPos);
	void Attack_Fire_WallState_Just5(_fvector vPlayerPos, _fvector vPos);

	void End_FireWallState();

	void Jump_State(_fvector vPos);

	void Reset_Value();

	_bool Is_Collision(CCollider* _pCollider);
protected:
	void Update_HandParticlePositon(const _double& _dDeltaSeconds, CAncHanibalStateMachine* _pMonsterStateMachine, CAncientHannibal* _pMonster);

protected:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	// Jump Dust List
	vector< CEffectParticleGroup*>						m_JumpDustList;
	vector<CEffectParticleGroup*>							m_FireWallList;
	vector< CEffectParticleGroup*>						 m_HandParticle;
	vector<CHierarchyNode*>									m_HandHierackyNodeList;
	vector<CEffectParticleGroup*>							m_DragonMagic;
	vector<CCollider*>												m_CollisionList;
	// Attack Positon List
	vector<_float3>								m_vAttackPositionList;


	_bool													m_bTurnOnLifeLoop_FireWall = false;
	_bool													m_bActive_FireWall = false;
	_float													m_fPlusDisalbe_FireWallTime = 0.f;
	_bool													m_bStopAnimation = false;
	_bool													m_bJumpState = false;
	_uint													m_iJumpDustIndex = 0;

	_uint													m_iColliderCount = 0;
public:
	static CAncEPController* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	virtual void Free() override;
};
END