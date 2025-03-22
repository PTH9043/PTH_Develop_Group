#include "CMinimap.h"
#include "CCamera_Minimap.h"
#include "CPlayer.h"
#include "CGameInstance.h"

CMinimap::CMinimap(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CUserInterface(_pDevice, _pContext)
{
}

CMinimap::CMinimap(const CMinimap& _rhs)
	: CUserInterface(_rhs)
{
}

HRESULT CMinimap::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CMinimap::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		CCamera_Free::CAMDESC CameraDesc;
		ZeroMemory(&CameraDesc, sizeof(CCamera::CAMDESC));

		CameraDesc.v3Eye = _float3(0.f, 20.f, -20.0f);
		CameraDesc.v3At = _float3(0.f, 0.f, 0.f);
		CameraDesc.fSpeed = 10.f;
		CameraDesc.fRotSpeed = XMConvertToRadians(90.0f);
		CameraDesc.fFovy = XMConvertToRadians(60.0f);
		CameraDesc.fAspect = _float(g_iWinSizeX) / g_iWinSizeY;
		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 500.f;
		m_pCamMinimap = static_cast<CCamera_Minimap*>(pGameInstance->Clone_GameObject_Add(
		LEVEL_STATIC, LAYER_CAM, PROTO_GOB_CAMERA_MINIMAP, &CameraDesc));
		Safe_AddRef(m_pCamMinimap);
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

void CMinimap::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CMinimap::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CMinimap::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMinimap::Ready_Component()
{
	if (FAILED(__super::Ready_Transform(COM_TRANSFORM)))
		return E_FAIL;

	{
		CRectTransform::RECTDESC tRectDesc;
		tRectDesc.v2Pos = m_tUIDesc.v2Pos;
		tRectDesc.v2Size = m_tUIDesc.v2Size;
		XMStoreFloat4x4(&tRectDesc.ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
		if (FAILED(__super::Ready_RectTransform(COM_RECTRANSFORM, &tRectDesc)))
			return E_FAIL;

		m_pRectTransformCom->Update_Rect(m_pTransformCom, g_iWinSizeX, g_iWinSizeY, m_tUIDesc.fZBufferOrder);
	}

	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXTEX, (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRenderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_TEXTURE, LEVEL_STATIC, PROTO_COMP_TEXTURE_UIGROUP, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_VIBUFFER, LEVEL_STATIC, PROTO_COMP_VIBUFFER_RECT, (CComponent**)&m_pVIRectCom)))
		return E_FAIL;

	return S_OK ;
}

HRESULT CMinimap::SetUp_ConstantTable()
{


	return S_OK;
}

CMinimap* CMinimap::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CMinimap* pInstance = new CMinimap(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CMinimap::Create To Failed");
	}
	return pInstance;
}

CGameObject* CMinimap::Clone(void* _pArg)
{
	CMinimap* pInstance = new CMinimap(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CMinimap::Create To Failed");
	}
	return pInstance;
}

void CMinimap::Free()
{
	__super::Free();
	Safe_Release(m_pCamMinimap);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRenderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIRectCom);
}