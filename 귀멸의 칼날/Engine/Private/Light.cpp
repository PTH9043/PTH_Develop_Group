#include "Light.h"
#include "GameInstance.h"

CLight::CLight(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LIGHTDESC& _tLightDesc)
	: m_pDevice(_pDevice), m_pContext(_pContext), m_tLightDesc(_tLightDesc)
{
	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);
}

HRESULT CLight::NativeConstruct()
{

	return S_OK;
}

HRESULT CLight::Render(CShader* _pShader, CVIBuffer_Rect* _pRect)
{
	if (false == m_bNotCalculation)
	{
		if (FAILED(_pShader->Set_RawValue("g_vLightDiffuse", &m_tLightDesc.vDiffuse, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(_pShader->Set_RawValue("g_vLightAmbient", &m_tLightDesc.vAmbient, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(_pShader->Set_RawValue("g_vLightSpecular", &m_tLightDesc.vSpecular, sizeof(_float4))))
			return E_FAIL;

		_uint iPass = 0;
		if (LIGHTDESC::TYPE_DIRECTIONAL == m_tLightDesc.eLightType)
		{
			if (FAILED(_pShader->Set_RawValue("g_vLightDir", &m_tLightDesc.vDirection, sizeof(_float3))))
				return E_FAIL;
			iPass = 0;
		}
		else if (LIGHTDESC::TYPE_POINT == m_tLightDesc.eLightType)
		{
			if (FAILED(_pShader->Set_RawValue("g_vLightPos", &m_tLightDesc.vPosition, sizeof(_float3))))
				return E_FAIL;
			if (FAILED(_pShader->Set_RawValue("g_fRange", &m_tLightDesc.fRange, sizeof(_float))))
				return E_FAIL;
			iPass = 1;
		}

		_pShader->Begin(iPass);
		_pRect->Render();
	}
	return S_OK;
}
void CLight::Set_LightDesc(const LIGHTDESC& _tLightDesc)
{
	this->m_tLightDesc = _tLightDesc;
}

CLight* CLight::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LIGHTDESC& _tLightDesc)
{
	CLight* pInstance = new CLight(_pDevice, _pContext, _tLightDesc);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("CLight::Create To Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLight::Free()
{
	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pContext);
}
