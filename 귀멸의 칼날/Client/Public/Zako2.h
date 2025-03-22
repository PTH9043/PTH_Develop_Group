#pragma once
#include "Oni.h"

BEGIN(Client)
class CZako2 final : public COni
{
protected:
	explicit CZako2(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CZako2(const CZako2& _rhs);
	virtual ~CZako2(void) = default;
public:
	virtual HRESULT NativeConstruct_Prototype(void) override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render(void) override;
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
private:
	virtual void Collision_Oni(const _double& _dTimeDelta) override;
	virtual void Behavior_State(const _double& _dTimeDelta) override;
	//virtual HRESULT Delete_Self(void) override;
protected:
	virtual HRESULT Initialize_Sound(void) override;
public:
	static CZako2* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
public:
	static _bool m_bSkillCheck;
};
END