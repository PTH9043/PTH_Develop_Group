#pragma once
#include "CComponent.h"


BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
public:
	typedef struct tagShaderDesc
	{
		ID3DX11EffectPass* pPass;
		ID3D11InputLayout* pInputLayout;
	}SHADERDESC;

private:
	explicit CShader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CShader(const CShader& _rhs);
	virtual ~CShader() = default;
public:
	// NativeConstruct Prototype
	HRESULT NativeConstruct_Prototype(const _tchar* _pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* _pElement, const _uint& _iNumElement);
	// NativeConstruct Clone
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
public:
	// Set Raw Value
	HRESULT Set_RawValue(const char* _pConstantName, const void* _pData, const _uint& _iDataSize);
	// Set ShaderResourceView
	HRESULT Set_SRV(const char* _pConstantName, ID3D11ShaderResourceView* _pSRV);


	// Begin
	HRESULT Begin(const _uint& _iPassIndex);
protected:
	// ID3DX11 Effect
	ID3DX11Effect* m_pEffect = nullptr;
	// Shader Desc
	vector<SHADERDESC> m_ShaderDesc;
public:
	static CShader* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		const _tchar* _pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* _pElement, const _uint& _iNumElement);
	// Clone
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END