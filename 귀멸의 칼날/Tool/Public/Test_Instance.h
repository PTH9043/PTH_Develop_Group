#pragma once
#include "Pawn_Tool.h"

BEGIN(Engine)
class CModel_Instance;
END

BEGIN(Tool)

class CTest_Instance final : public CPawn_Tool
{
protected:
	explicit CTest_Instance(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CTest_Instance(const CTest_Instance& _rhs);
	virtual ~CTest_Instance() = default;
public:
	virtual const char* Get_Name() { return "Test_Instance"; }
	virtual const _tchar* Get_Proto() { return L"ProtoTest"; }
	virtual const _tchar* Get_Layer() { return L"Layer"; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	// Set Up Constant
	virtual HRESULT SetUp_ConstantTable();
	// Ready Model Components
	virtual  HRESULT Ready_Components() override;

protected:
	CModel_Instance* m_pModelInstance = nullptr;
	vector<TRANSFORMVECTOR>			m_TransformVector;
public:
	static CTest_Instance* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END