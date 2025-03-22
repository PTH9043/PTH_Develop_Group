#include "CShader.h"

CShader::CShader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CShader::CShader(const CShader& _rhs)
	: CComponent(_rhs), 
	m_pEffect(_rhs.m_pEffect),
	m_ShaderDesc(_rhs.m_ShaderDesc)
{
	Safe_AddRef(m_pEffect);
	for (auto& iter : m_ShaderDesc)
	{
		Safe_AddRef(iter.pInputLayout);
		Safe_AddRef(iter.pPass);
	}
}

HRESULT CShader::NativeConstruct_Prototype(const _tchar* _pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* _pElement, const _uint& _iNumElement)
{
	_uint iFlag = 0;
#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_OPTIMIZATION_LEVEL0 | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	if (FAILED(D3DX11CompileEffectFromFile(_pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	// Technique Index 
	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;
	// Technique Desc
	D3DX11_TECHNIQUE_DESC TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof(D3DX11_TECHNIQUE_DESC));
	pTechnique->GetDesc(&TechniqueDesc);
	// Passes
	for (_uint i = 0; i < TechniqueDesc.Passes; ++i)
	{
		SHADERDESC ShaderDesc;
		ShaderDesc.pPass = pTechnique->GetPassByIndex(i);

		D3DX11_PASS_DESC PassDesc;
		ZeroMemory(&PassDesc, sizeof(D3DX11_TECHNIQUE_DESC));

		ShaderDesc.pPass->GetDesc(&PassDesc);
		// Create InputLayout 
		if (FAILED(m_pDevice->CreateInputLayout(_pElement, _iNumElement, 
			PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &ShaderDesc.pInputLayout)))
			return E_FAIL;

		m_ShaderDesc.push_back(ShaderDesc);
	}

	return S_OK;
}

HRESULT CShader::NativeConstruct_Clone(void* _pArg)
{
	return S_OK;
}

HRESULT CShader::Set_RawValue(const char* _pConstantName, const void* _pData, const _uint& _iDataSize)
{
	if (nullptr == m_pEffect)
		return E_FAIL;
	// 지정해준 이름에 해당하는 전역 변수의 컴 객체를 가지고 온다.
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;
	// 해당 변수에 값을 전달
	/* 텍스쳐의 값은 전달할 수 없다.  */
	pVariable->SetRawValue(_pData, 0, _iDataSize);
	return S_OK;
}

HRESULT CShader::Set_SRV(const char* _pConstantName, ID3D11ShaderResourceView* _pSRV)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pSRVVairable = pVariable->AsShaderResource();
	if (nullptr == pSRVVairable)
		return E_FAIL;

	return pSRVVairable->SetResource(_pSRV);
}

HRESULT CShader::Begin(const _uint& _iPassIndex)
{
	if (_iPassIndex >= m_ShaderDesc.size())
		return E_FAIL;

	m_pContext->IASetInputLayout(m_ShaderDesc[_iPassIndex].pInputLayout);
	m_ShaderDesc[_iPassIndex].pPass->Apply(0, m_pContext);
	return S_OK;
}

CShader* CShader::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pShaderFilePath, 
	D3D11_INPUT_ELEMENT_DESC* _pElement, const _uint& _iNumElement)
{
	CShader* pInstance = new CShader(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(_pShaderFilePath, _pElement, _iNumElement)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CShader::Create to Failed");
	}
	return pInstance;
}

CComponent* CShader::Clone(void* _pArg)
{
	CShader* pInstance = new CShader(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CShader::Clone to Failed");
	}
	return pInstance;
}

void CShader::Free()
{
	for (auto& iter : m_ShaderDesc)
	{
		Safe_Release(iter.pInputLayout);
		Safe_Release(iter.pPass);
	}
	Safe_Release(m_pEffect);

	__super::Free();
}
