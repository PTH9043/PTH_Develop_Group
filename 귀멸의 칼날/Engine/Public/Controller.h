#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CController abstract : public CBase
{
protected:
	explicit CController(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	virtual ~CController() = default;
public:
	const _uint& Get_ID() const { return m_iID; }
	const _wstring& Get_Name() const { return m_strName; }
	class CGameObject* Get_OwnerObject() { return m_pOwnerObject; }
public:
	virtual HRESULT NativeConstruct_Prototype(class CGameObject* _pGameObject);
	virtual void Tick(const _double& _dTimeDelta);
	virtual void Late_Tick(const _double& _dTimeDelta);
	// 데이터를 내부에 집어 넣는 함수
	virtual void Insert_Datas(VOIDDATAS _pData);
	// 데이터를 바깥으로 빼는 함수
	virtual  VOIDDATAS Out_Datas();
protected:
	class CGameObject*			m_pOwnerObject = nullptr;
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	// Void Datas
	VOIDDATAS							m_ReceiveVoidDatas;
	VOIDDATAS							m_OutVoidDatas;
protected:
	_uint										m_iID = 0;
	_wstring								m_strName = L"";
public:
	virtual void Free() override;
};

END