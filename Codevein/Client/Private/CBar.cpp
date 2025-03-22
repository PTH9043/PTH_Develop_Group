#include "CBar.h"
#include "CGameInstance.h"

CBar::CBar(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CUserInterface(_pDevice, _pContext)
{
}

CBar::CBar(const CBar& _rhs)
	: CUserInterface(_rhs)
{
}

HRESULT CBar::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return S_OK;
	return E_FAIL;
}

HRESULT CBar::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		BARGROUP tBarGroup = *static_cast<BARGROUP*>(pArg);
		m_tUIDesc = tBarGroup.tUIDesc;
		m_tBarDesc = tBarGroup.tBarDesc;
	}

	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

void CBar::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CBar::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	if (nullptr != m_pRenderCom)
		m_pRenderCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(m_tBarDesc.strBarBack.c_str(), m_pBackRectTransform, m_pBackTransform)))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBackRectCom->Render();

	if (FAILED(SetUp_ConstantTable(m_tUIDesc.strImgName.c_str(), m_pRectTransformCom, m_pTransformCom)))
		return E_FAIL;

	m_pShaderCom->Begin(1);
	m_pVIRectCom->Render();
	return S_OK;
}

HRESULT CBar::SetUp_ConstantTable(const _wstring& _strUIName, CRectTransform* _pRectTransform
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

	if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DiffuseTexture", _strUIName.c_str())))
		return E_FAIL;

	return S_OK;
}

HRESULT CBar::Ready_Component()
{
	if (FAILED(__super::Ready_Transform(COM_TRANSFORM)))
		return E_FAIL;

	{
		{
			CRectTransform::RECTDESC tRectDesc;
			tRectDesc.v2Pos = m_tUIDesc.v2Pos;
			tRectDesc.v2Size = m_tUIDesc.v2Size;
			XMStoreFloat4x4(&tRectDesc.ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
			if (FAILED(__super::Ready_RectTransform(COM_RECTRANSFORM, &tRectDesc)))
				return E_FAIL;

			m_pRectTransformCom->Update_Rect(m_pTransformCom, g_iWinSizeX, g_iWinSizeY, m_tUIDesc.fZBufferOrder);
		}   
		{

			m_pBackTransform = CTransform::Create(m_pDevice, m_pContext);

			CRectTransform::RECTDESC tRectDesc;
			tRectDesc.v2Pos = m_tUIDesc.v2Pos;
			tRectDesc.v2Size;
			XMStoreFloat2(&tRectDesc.v2Size, XMLoadFloat2(&m_tUIDesc.v2Size) + XMVectorSet(1.f, 0.f, 0.f, 0.f));
			XMStoreFloat4x4(&tRectDesc.ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
			if (FAILED(__super::Ready_RectTransform(L"Com_BackRectTransform", &tRectDesc, (CRectTransform**)&m_pBackRectTransform)))
				return E_FAIL;

			_float fValue = 0.0001f;
			if (m_tUIDesc.fZBufferOrder > 0.0001f)
				fValue = m_tUIDesc.fZBufferOrder - 0.0001f;
			m_pBackRectTransform->Update_Rect(m_pBackTransform, g_iWinSizeX, g_iWinSizeY, fValue);
		}
	}

	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXTEX, (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_VIBUFFER, LEVEL_STATIC, PROTO_COMP_VIBUFFER_RECT, (CComponent**)&m_pVIRectCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(L"Com_BackVIBuffer", LEVEL_STATIC, PROTO_COMP_VIBUFFER_RECT, (CComponent**)&m_pVIBackRectCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRenderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_TEXTURE, LEVEL_STATIC, PROTO_COMP_TEXTURE_UIGROUP, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

void CBar::Update_Bar()
{
	float fDivValue = m_tBarDesc.fCurValue / m_tBarDesc.fMaxValue;
	m_tBarDesc.v2VerticalSize.x = static_cast<_float>(fDivValue * m_tBarDesc.v2MaxVerticalSize.x);

	if (nullptr != m_pRectTransformCom)
	{
		CRectTransform::RECTDESC tRectDesc = m_pRectTransformCom->Get_RectDesc();
		tRectDesc.v2Size = m_tBarDesc.v2VerticalSize;
		tRectDesc.v2Pos.x = m_pBackRectTransform->Get_RectDesc().v2Pos.x - (m_tBarDesc.v2MaxVerticalSize.x - m_tBarDesc.v2VerticalSize.x) * 0.5f;
		m_pRectTransformCom->Set_RectDesc(tRectDesc);
	}
}

CGameObject* CBar::Clone(void* _pArg)
{
	return nullptr;
}

void CBar::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIRectCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRenderCom);
	Safe_Release(m_pVIBackRectCom);
	Safe_Release(m_pBackRectTransform);
	Safe_Release(m_pBackTransform);
}
