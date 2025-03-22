#pragma once
#include "CMob.h"
BEGIN(Engine)
class UGameInstance;
class UParticle;
class UParticleSystem;
END

BEGIN(Client)
/*
@ Date: 2024-05-13, Writer: 이성현
@ Explain
-  미믹 몬스터 클래스.
*/
class CMimic final : public CMob {
public:
	CMimic(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	CMimic(const CMimic& _rhs);
	DESTRUCTOR(CMimic)
public:
	// UPlayer을(를) 통해 상속됨
	CLONE_MACRO(CMimic, "CMimic::CloneToFailed")
		virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	void CreateParticles();
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _Datas) override;
	SHPTR<UParticle>& GetParticle() { return m_spBloodParticle; }
	void SetOpeningState(const _bool& _newState) { m_bisOpen = _newState; }
	const _bool& GetOpeningState() { return m_bisOpen; }
protected:
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;

private:
	SHPTR<UParticle>			m_spBloodParticle;
	SHPTR<UParticle>			m_spSlashParticle;
	SHPTR<UParticle>			m_spAttackParticle;
	SHPTR<UParticle>			m_spAttackParticleTwo;
	PARTICLEPARAM* m_stParticleParam;
	ComputeParticleType* m_stParticleType;

	UNavigation::PathFindingState m_PathFindingState;
	VECTOR<_float3> m_AstarPath;
	_bool m_isPathFinding = false;
	size_t m_currentPathIndex = 0;
	_bool			m_bisOpen;

	_bool m_bDissolveSound = false;
};

END

