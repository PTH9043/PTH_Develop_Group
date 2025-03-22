#pragma once
#include "CMob.h"
#include "USound.h"

BEGIN(Engine)
class UGameInstance;
class UParticle;
class UParticleSystem;
END

BEGIN(Client)
/*
@ Date: 2024-05-13, Writer: 이성현
@ Explain
-  미라 몬스터 클래스.
*/
class CMummy final : public CMob {
public:
	CMummy(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	CMummy(const CMummy& _rhs);
	enum MUMMYTYPE {
		TYPE_STANDING, TYPE_LYING, TYPE_WALKING, TYPE_END
	};
	DESTRUCTOR(CMummy)
public:
	// UPlayer을(를) 통해 상속됨
	CLONE_MACRO(CMummy, "CMummy::CloneToFailed")
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _Datas) override;
	SHPTR<UParticle>& GetParticle() { return m_spBloodParticle; }
	void SetMummyType(MUMMYTYPE _type) { m_MummyType = _type; }
	MUMMYTYPE GetMummyType() { return m_MummyType; }
protected:
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;
	void CreateParticles();
private:
	MUMMYTYPE					m_MummyType;
	SHPTR<UParticle>			m_spBloodParticle;
	SHPTR<UParticle>			m_spSlashParticle;
	SHPTR<UParticle>			m_spAttackParticle;
	SHPTR<UParticle>			m_spAttackParticleTwo;
	SHPTR<UParticle>										m_spFootPrintParticle;
	PARTICLEPARAM*			m_stParticleParam;
	ComputeParticleType*	m_stParticleType;


	UNavigation::PathFindingState m_PathFindingState;
	VECTOR<_float3> m_AstarPath;
	_bool m_isPathFinding = false;
	size_t m_currentPathIndex = 0;

	_bool m_bDissolveSound = false;

	FMOD::Channel* m_pDeathChannel;
	FMOD::Channel* m_pDeath2Channel;
	FMOD::Channel* m_pDissolveChannel;
};

END

