#include "CButton.h"
#include "CGameInstance.h"

CButton::CButton(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CUserInterface(_pDevice, _pContext)
{
}

CButton::CButton(const CButton& _rhs)
	: CUserInterface(_rhs)
{
}

HRESULT CButton::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CButton::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		BUTTONDESC tButtonDesc = *static_cast<BUTTONDESC*>(pArg);
		m_tUIDesc = tButtonDesc.tUIDesc;
		m_strOtherImg = tButtonDesc.strOtherImgName;
	}

	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

void CButton::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);

	Judge_MouseOnRect();
	Mouse_ClickButton(_dTimeDelta);
}

void CButton::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	if (nullptr != m_pRenderCom)
		m_pRenderCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CButton::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIRectCom->Render();

	return S_OK;
}

HRESULT CButton::SetUp_ConstantTable()
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

	if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DiffuseTexture", m_strStandardImg.c_str())))
		return E_FAIL;

	return S_OK;
}

HRESULT CButton::Ready_Component()
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

void CButton::Judge_MouseOnRect()
{
	m_bOnRect = Is_MouseOnRect();
	if (false == m_bOnRect)
	{
		m_strStandardImg = m_tUIDesc.strImgName;
	}
	else
	{
		m_strStandardImg = m_strOtherImg;
	}
}

void CButton::Mouse_ClickButton(const _double& _dTimeDelta)
{
	if (true == m_bOnRect)
	{
		BEGIN_SINGLETON(CGameInstance, pGameInstance)
		{
			if (true == pGameInstance->Get_DIKeyDown(DIK_E))
			{
				m_bPress = true;
				m_tPressTimer.Reset_Timer();
			}
		}
		END_SINGLETON(pGameInstance);
	}

	if (true == m_bPress)
	{
		if (true == m_tPressTimer.Is_Over(_dTimeDelta))
		{
			m_tPressTimer.Reset_Timer();
		}
	}
}

CButton* CButton::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CButton* pInstance = new CButton(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CButton::Create To Failed");
	}
	return pInstance;
}

CGameObject* CButton::Clone(void* _pArg)
{
	CButton* pInstance = new CButton(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CButton::Clone To Failed");
	}
	return pInstance;
}

void CButton::Free()
{
	__super::Free();

	Safe_Release(m_pRenderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIRectCom);
}