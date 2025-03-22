#pragma once

#include "CBase.h"
#include "CTransform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	explicit CGameObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CGameObject(const CGameObject& _Prototype);
	virtual ~CGameObject() = default;
public:
	const _bool Is_Active() const { return m_bActive; }
	void Set_Active(const _bool& _bActive) { this->m_bActive = _bActive; }
public:
	class CComponent* Get_Component(const _tchar* pComponentTag);
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct_Clone(void* pArg);
	virtual void Tick(const _double& _dTimeDelta);
	virtual void Late_Tick(const _double& _dTimeDelta);
	virtual HRESULT  Render();
public:
	/* 부모에 있는 m_Components에도 데이터를 추가.
자식에 있는 멤버변수에도 데이터를 추가한다. */
	HRESULT Add_Component(const _tchar* pComponentTag, const _uint& _iLevelIndex,
		const _tchar* pPrototypeTag, class CComponent** ppOut, void* pArg = nullptr);
protected:
	// Ready Transform
	HRESULT Ready_Transform(const _tchar* _pTag);
protected:
	// Com 객체
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
protected:
	// Transform 
	CTransform*						m_pTransformCom = nullptr;
protected:
	_bool										m_bActive = false;
protected:
	map<const _tchar*, class CComponent*>			m_Components;
	typedef map<const _tchar*, class CComponent*>	COMPONENTS;
private:
	//Find Component
	class CComponent* Find_Component(const _tchar* pComponentTag);
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END