#pragma once
#include "CBase.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameObject;
class CModel;
class CTransform;
class CGameObject;
class CHierarchyNode;
END

BEGIN(Client)
class CWeapon;

class CStateMachine abstract : public CBase
{
public:
	const _uint& Get_State() const { return m_iState; }
	void Set_State(const _uint& _iState) {	this->m_iState = _iState; }

	const _uint& Get_PrevState() const { return m_iPrevState; }

	void Set_Target(const _bool& _bIsTarget) { this->m_bTarget = _bIsTarget; }
	const _bool& Is_Target() const { return m_bTarget; }
protected:
	explicit CStateMachine(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual ~CStateMachine() = default;

public:
	virtual HRESULT NativeConstruct(CGameObject* _pOwnerObject);
	virtual void Tick(const _double& _dTimeDelta, CGameObject* _pOwner) = 0;
	virtual void Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner) = 0;
protected:
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
protected:
	// Model, Transform
	CModel*								m_pModel = nullptr;
	CTransform*						m_pTransform = nullptr;
private:
	CHierarchyNode*				m_pLeftLeg = nullptr;
	CHierarchyNode*				m_pRightLeg = nullptr;
protected:
	// Weapon
	CWeapon*							m_pWeapon = nullptr;
	// State
	_uint										m_iState = 0;
	_uint										m_iPrevState = 0;
	_bool										m_bTarget = false;
public:
	virtual void Free() override;
};

END