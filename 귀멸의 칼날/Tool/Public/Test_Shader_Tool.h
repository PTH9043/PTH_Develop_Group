#pragma once
#include "GameObject_Tool.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CVIBuffer_Rect;
END

BEGIN(Tool)

class CTest_Shader_Tool final : public CGameObject_Tool
{
protected:
	explicit CTest_Shader_Tool(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CTest_Shader_Tool(const CTest_Shader_Tool& _rhs);
	virtual ~CTest_Shader_Tool() = default;
public:
	virtual const char* Get_Name() override;
	virtual const _tchar* Get_Proto() override;
	virtual const _tchar* Get_Layer() override;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
protected:
	// Set U Constant Value
	virtual HRESULT SetUp_ConstantValue() override;
	// Ready  Components 
	virtual HRESULT Ready_Components() override;
protected:
	// VIBufferRect
	CVIBuffer_Rect* m_pVIBufferRect = nullptr;
public:
#pragma region USE_TOOL_METHOD
	// Extern EditValue
	virtual _bool Extern_EditValue() override;
	// Extern Hierakey Value
	virtual _bool Extern_HierakeyView(const string& _str) override;
	// Create Self 
	virtual HRESULT Create_Self(CGameObject** _ppOut) override;
	// Delete Self
	virtual HRESULT Delete_Self() override;
	// Save Data 
	virtual HRESULT Save_Data() override;
	// Load Data 
	virtual HRESULT Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData = nullptr) override;
	// Get Picking
	virtual _bool Is_Picking(_float3* _pOut = nullptr) override;
	// Reset Data 
	virtual void Reset_Data() override;
#pragma endregion USE_TOOL_METHOD

public:
	static CTest_Shader_Tool* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END