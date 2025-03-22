#include "UI_Tool.h"
#include "GameInstance.h"

CUI_Tool::CUI_Tool(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CUI_Base(_pDevice, _pContext)

{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
	
}

CUI_Tool::CUI_Tool(const CUI_Tool& _rhs)
	: CUI_Base(_rhs)
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
} 

HRESULT CUI_Tool::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CUI_Tool::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;
	
	FileInfo* fileInfo = static_cast<FileInfo*>(pArg);

	if (pArg != nullptr && fileInfo != nullptr)
	{
		m_fX = fileInfo->m_fX;
		m_fY = fileInfo->m_fY;
		m_fSizeX = fileInfo->m_fSizeX;
		m_fSizeY = fileInfo->m_fSizeY;
		m_Depth = fileInfo->m_Depth;
	    lstrcpy(m_pName, fileInfo->m_pName);
		lstrcpy(m_TextureName, fileInfo->m_TextureName);
		lstrcpy(m_PrototypeTag, fileInfo->m_PrototypeTag);
		//CurrentLevel = Currentlevel;
		
		m_State = fileInfo->m_State;
		m_Speed = fileInfo->m_Speed;
		m_MaxSize = fileInfo->m_Max;
		m_MinSize = fileInfo->m_Min;
		m_MoveX = fileInfo->m_MoveX;
		m_MoveY = fileInfo->m_MoveX;
		m_CreatTime = fileInfo->m_CreatTime;
		m_DeadTime = fileInfo->m_DeadTime;

		m_fAlpha = fileInfo->m_Alpha;
		m_fAlphaCreat = fileInfo->m_AlphaCreat;
		m_fAlphaEnd = fileInfo->m_AlphaDead;
		m_AlphaState = fileInfo->m_AlphaState;
	}
	else
	{
		m_fX = 100.f;
		m_fY = 100.f;
		m_fSizeX = 300.f;
		m_fSizeY = 300.f;
		m_Depth = 1;
	}
	
	if (FAILED(SetUp_Components()))
		return E_FAIL;


	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	
	return S_OK;
}

void CUI_Tool::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_Pos(XMVectorSet(m_fX, m_fY, 0.0f, 1.f));

}


void CUI_Tool::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Tool::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();
	


	return S_OK;
}

HRESULT CUI_Tool::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(COM_TEXTURE, LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Test"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_UI, (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(COM_VIBUFFER, LEVEL_STATIC, PROTO_COMP_VIBUFFER_RECT, (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CUI_Tool::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4		ViewMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	
	_float4x4 World = m_pTransformCom->Get_WorldFloat4x4_TP();
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &World, sizeof(_float4x4))))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

		if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DiffuseTexture", m_TextureName)))
		return E_FAIL;
			
	return S_OK;
}

CUI_Tool* CUI_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Tool* pInstance = new CUI_Tool(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Tool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI_Base* CUI_Tool::Clone(void* pArg)
{
	CUI_Tool* pInstance = new CUI_Tool(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Tool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Tool::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
