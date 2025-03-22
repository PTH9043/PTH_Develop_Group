#pragma once
#include "Oni.h"

BEGIN(Client)
class CZako3 final : public COni
{
protected:
	explicit CZako3(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CZako3(const CZako3& _rhs);
	virtual ~CZako3(void) = default;
public:
	virtual HRESULT NativeConstruct_Prototype(void) override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render(void) override;
private:
	void Extern_ImGuiClass(void) override;
private:
	virtual HRESULT SetUp_ConstantTable(void) override;
	virtual HRESULT SetUp_Components(void) override;
private:
	virtual void Idle_Oni(const _double& _dTimeDelta) override;
	virtual void Recognize_Oni(const _double& _dTimeDelta) override;
	virtual void Battle_Oni(const _double& _dTimeDelta) override;

	virtual void Idle_State(const _double& _dTimeDelta) override;
	virtual void Recognize_State(const _double& _dTimeDelta) override;
	virtual void Battle_State(const _double& _dTimeDelta) override;
	virtual void Behavior_State(const _double& _dTimeDelta) override;
private:
	virtual void Hit_ByEnemy(CCharacter* _pEnemyCharcter, CBattle_Manager* _pBattleManager) override;
private:
	virtual void Collision_Oni(const _double& _dTimeDelta) override;
	//virtual HRESULT Delete_Self(void) override;
protected:
	virtual HRESULT Initialize_Sound(void) override;
public:
	static CZako3* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
public:
	static _bool m_bSkillCheck;
};
END