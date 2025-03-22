#pragma once
#include "CMob.h"


BEGIN(Engine)
class UGameInstance;
class UParticle;
class UParticleSystem;
class UMat;
class UGuard;
END

BEGIN(Client)
/*
@ Date: 2024-05-13, Writer: 이성현
@ Explain
-  미라 몬스터 클래스.
*/
class CAnubisStaff;
class CAnubis final : public CMob {
public:
	CAnubis(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	CAnubis(const CAnubis& _rhs);
	enum AnubisTYPE {
		TYPE_STANDING, TYPE_LYING, TYPE_END
	};
	DESTRUCTOR(CAnubis)
public:
	// UPlayer을(를) 통해 상속됨
	CLONE_MACRO(CAnubis, "CAnubis::CloneToFailed")
		virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _Datas) override;
	SHPTR<UParticle>& GetParticle() { return m_spBloodParticle; }
	void SetAnubisType(AnubisTYPE _type) { m_AnubisType = _type; }
	AnubisTYPE GetAnubisType() { return m_AnubisType; }

	const _float3& GetOriginPos() const { return m_f3OriginPos; }
	void SetOriginPos(_float3 _newPos) { m_f3OriginPos = _newPos; }

	const _float3& GetOriginDirection() const { return m_f3OriginDirection; }
	void SetOriginDirection(_float3 _newDir) { m_f3OriginDirection = _newDir; }

	CSHPTRREF<UMat> GetMagicCircle() { return m_spMagicCircle; }
	CSHPTRREF<UMat> GetFireCircle() { return m_spFireCircle; }
	CSHPTRREF<UMat> GetFireCircle1() { return m_spFireCircle1; }

	const _bool& GetShieldState() { return m_bisShield; }
	void SetShieldState(_bool _newState) { m_bisShield = _newState; }

	const _bool& GetFireAttackState() { return m_bisFireAttack; }
	void SetFireAttackState(_bool _newState) { m_bisFireAttack = _newState; }
protected:
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;
	void CreateParticles();
private:
	AnubisTYPE					m_AnubisType;
	SHPTR<UParticle>										m_spFootPrintParticle;
	SHPTR<UParticle>			m_spBloodParticle;
	SHPTR<UParticle>			m_spSlashParticle;
	SHPTR<UParticle>			m_spAttackParticle;
	SHPTR<UParticle>			m_spAttackParticleTwo;
	PARTICLEPARAM*			m_stParticleParam;
	ComputeParticleType*	m_stParticleType;

	SHPTR<UMat>				m_spMagicCircle;
	SHPTR<UMat>				m_spFireCircle;
	SHPTR<UMat>				m_spFireCircle1;
	SHPTR<UGuard>			m_spMagicSphere;
	SHPTR<CAnubisStaff>			m_spAnubisStaff;

	_float3						m_f3OriginPos;
	_float3						m_f3OriginDirection;

	_bool						m_bisShield;
	_bool						m_bisFireAttack;
	_float						m_fShieldRadius;
	_float						m_fMagicCircleRadius;
	_double						m_dShieldTimer;

	UNavigation::PathFindingState m_PathFindingState;
	VECTOR<_float3> m_AstarPath;
	_bool m_isPathFinding = false;
	size_t m_currentPathIndex = 0;

	_bool m_bDissolveSound = false;
};

END

