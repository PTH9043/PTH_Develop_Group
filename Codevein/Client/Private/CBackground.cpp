#include "CBackground.h"
#include "..\Public\CBackground.h"
#include "CGameInstance.h"

CBackground::CBackground(ID3D11Device *& _pDevice, ID3D11DeviceContext *& _pContext)
	: CUserInterface(_pDevice, _pContext)
{
}

CBackground::CBackground(const CBackground & _rhs)
	: CUserInterface(_rhs)
{
}

HRESULT CBackground::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackground::NativeConstruct_Clone(void * pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
		m_tUIDesc = *static_cast<UIDESC*>(pArg);

	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

void CBackground::Tick(const _double & _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CBackground::Late_Tick(const _double & _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	if (nullptr != m_pRenderCom)
		m_pRenderCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBackground::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIRectCom->Render();
	return S_OK;
}

HRESULT CBackground::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (nullptr == m_pRectTransformCom || nullptr == m_pTransformCom)
		return E_FAIL;

	_float4x4 ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_pRectTransformCom->Get_RectDesc().ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DiffuseTexture", m_tUIDesc.strImgName.c_str())))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackground::Ready_Component()
{
	if (FAILED(__super::Ready_Transform(COM_TRANSFORM)))
		return E_FAIL;

	CRectTransform::RECTDESC tRectDesc;
	tRectDesc.v2Pos = m_tUIDesc.v2Pos;
	tRectDesc.v2Size = m_tUIDesc.v2Size;
	XMStoreFloat4x4(&tRectDesc.ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	if (FAILED(__super::Ready_RectTransform(COM_RECTRANSFORM, &tRectDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXTEX, (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_VIBUFFER, LEVEL_STATIC, PROTO_COMP_VIBUFFER_RECT, (CComponent**)&m_pVIRectCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRenderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_TEXTURE, LEVEL_LOGO, PROTO_COMP_TEXTURE_LOGO, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (nullptr != m_pRectTransformCom)
		m_pRectTransformCom->Update_Rect(m_pTransformCom, g_iWinSizeX, g_iWinSizeY, m_tUIDesc.fZBufferOrder);

	return S_OK;
}

CBackground * CBackground::Create(ID3D11Device *& _pDevice, ID3D11DeviceContext *& _pContext)
{
	CBackground* pInstance = new CBackground(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CBackground::Create To Failed");
	}
	return pInstance;
}

CGameObject * CBackground::Clone(void * pArg)
{
	CBackground* pInstance = new CBackground(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CBackground::Clone To Failed");
	}
	return pInstance;
}

void CBackground::Free()
{
	__super::Free();

	Safe_Release(m_pRenderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIRectCom);
}
