#pragma once
#include "CMob.h"
BEGIN(Engine)
class UGameInstance;
class UParticle;
class UParticleSystem;
END

BEGIN(Client)
class CShuriken;
class CShurikenThrowing;
/*
@ Date: 2024-07-05, Writer: 이성현
@ Explain
-  할리퀸 몬스터 클래스.
*/
class CHarlequinn final : public CMob {
public:
	CHarlequinn(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	CHarlequinn(const CHarlequinn& _rhs);
	enum MUMMYTYPE {
		TYPE_STANDING, TYPE_LYING, TYPE_END
	};
	DESTRUCTOR(CHarlequinn)
public:
	// UPlayer을(를) 통해 상속됨
	CLONE_MACRO(CHarlequinn, "CHarlequinn::CloneToFailed")
		virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _Datas) override;

	void ThrowShurikens(_int _shurikenIndex, const _double& _dTimeDelta, const _float3& _dir);

	SHPTR<UParticle>& GetParticle() { return m_spBloodParticle; }


	CSHPTRREF<VECTOR<SHPTR<CShurikenThrowing>>> GetShurikens() { return m_spShurikensForThrowing; }
protected:
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;
	void CreateParticles();
private:
	SHPTR<UParticle>										m_spFootPrintParticle;
	SHPTR<UParticle>			m_spBloodParticle;
	SHPTR<UParticle>			m_spSlashParticle;
	SHPTR<UParticle>			m_spAttackParticle;
	SHPTR<UParticle>			m_spAttackParticleTwo;
	PARTICLEPARAM*			m_stParticleParam;
	ComputeParticleType*	m_stParticleType;

	VECTOR<SHPTR<CShuriken>>		m_spShurikens;
	SHPTR<VECTOR<SHPTR<CShurikenThrowing>>> m_spShurikensForThrowing;

	UNavigation::PathFindingState m_PathFindingState;
	VECTOR<_float3> m_AstarPath;
	_bool m_isPathFinding = false;
	size_t m_currentPathIndex = 0;

	_bool m_bDissolveSound = false;
};

END

