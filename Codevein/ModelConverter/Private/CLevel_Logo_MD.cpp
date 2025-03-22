#include "CLevel_Logo_MD.h"
#include"CGameInstance.h"
#include "CLevel_Loading_MD.h"
#include "CCamera_MD.h"


CLevel_Logo_MD::CLevel_Logo_MD(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Logo_MD::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_LightDesc(L"")))
		return E_FAIL;

	if (FAILED(Ready_Camera(LAYER_CAM)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo_MD::Tick(const _double & _dDeltaSeconds)
{

}

void CLevel_Logo_MD::Late_Tick(const _double & _dDeltaSeconds)
{

}

HRESULT CLevel_Logo_MD::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_Logo_MD::Ready_LightDesc(const _tchar * _pLayerTag)
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		LIGHTDESC			LightDesc;
		ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

		LightDesc.eLightType = tagLightDesc::TYPE_DIRECTIONAL;
		LightDesc.v4Direction = _float4(1.f, -1.f, 1.f, 1.f);
		LightDesc.v4Diffuse = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.v4Ambient = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.v4Specular = _float4(1.f, 1.f, 1.f, 1.f);

		if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc, nullptr)))
			return E_FAIL;
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

HRESULT CLevel_Logo_MD::Ready_Camera(const _tchar* _pLayerTag)
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		CCamera::CAMDESC CameraDesc;
		ZeroMemory(&CameraDesc, sizeof(CCamera::CAMDESC));

		CameraDesc.v3Eye = _float3(0.f, 20.f, -20.0f);
		CameraDesc.v3At = _float3(0.f, 0.f, 0.f);
		CameraDesc.fSpeed = 30.f;
		CameraDesc.fRotSpeed = XMConvertToRadians(90.0f);
		CameraDesc.fFovy = XMConvertToRadians(60.0f);
		CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 20000.f;
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_LOGO, _pLayerTag, PROTO_GOB_CAM_MD, &CameraDesc)))
		{
			END_SINGLETON(pGameInstance);
			return E_FAIL;
		}
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}


CLevel * CLevel_Logo_MD::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext)
{
	CLevel_Logo_MD* pInstance = new CLevel_Logo_MD(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CLevel_Logo::Create To Failed");
	}
	return pInstance;
}

void CLevel_Logo_MD::Free()
{
	__super::Free();
}
