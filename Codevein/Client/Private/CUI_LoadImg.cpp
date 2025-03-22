#include "CUI_LoadImg.h"
#include "CGameInstance.h"

CUI_LoadImg::CUI_LoadImg(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CUserInterface(_pDevice, _pContext)
{
}

CUI_LoadImg::CUI_LoadImg(const CUI_LoadImg& _rhs) 
	: CUserInterface(_rhs)
{
}

HRESULT CUI_LoadImg::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_LoadImg::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		UIDESC tUIDesc = *static_cast<UIDESC*>(pArg);
		m_tUIDesc = tUIDesc;
	}

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_bActive = true;
	return S_OK;
}

void CUI_LoadImg::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CUI_LoadImg::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	if (true == m_bActive)
	{
		if (m_fLoadPlusTime > m_fLoadMaxTime)
		{
			m_fLoadPlusTime = 0.f;
			m_bActive = false;
		}
		m_fLoadPlusTime += (_float)(_dTimeDelta);

		if (nullptr != m_pRenderCom)
			m_pRenderCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CUI_LoadImg::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(m_pRectTransformCom, m_pTransformCom)))
		return E_FAIL;

	m_pVIRectCom->Render();
	return S_OK;
}

HRESULT CUI_LoadImg::Ready_Component()
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

	if (FAILED(__super::Add_Component(COM_VIBUFFER, LEVEL_STATIC, PROTO_COMP_VIBUFFER_RECT, (CComponent**)&m_pVIRectCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_TEXTURE, LEVEL_STATIC, PROTO_COMP_TEXTURE_LOADING, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_LoadImg::SetUp_ConstantTable(CRectTransform* _pRectTransform
	, CTransform* _pTransform)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (nullptr == _pRectTransform || nullptr == _pTransform)
		return E_FAIL;

	_float4x4 ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &_pTransform->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &_pRectTransform->Get_RectDesc().ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, 
		"g_DiffuseTexture", m_tUIDesc.strImgName.c_str())))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	return S_OK;
}

CUI_LoadImg* CUI_LoadImg::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CUI_LoadImg* pInstance = new CUI_LoadImg(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CUI_LoadImg::Create to Failed");
	}
	return pInstance;
}

CGameObject* CUI_LoadImg::Clone(void* _pArg)
{
	CUI_LoadImg* pInstance = new CUI_LoadImg(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CUI_LoadImg::Clone to Failed");
	}
	return pInstance;
}

void CUI_LoadImg::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIRectCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRenderCom);

	__super::Free();
}
