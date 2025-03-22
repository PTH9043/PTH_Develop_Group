#include "Minimap_Track.h"
#include "GameInstance.h"
#include "Zenitsu.h"
#include "UI_Minimap.h"

CMinimap_Track::CMinimap_Track(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CUI_Base(_pDevice, _pContext)
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

CMinimap_Track::CMinimap_Track(const CMinimap_Track& _rhs)
	: CUI_Base(_rhs)
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

HRESULT CMinimap_Track::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CMinimap_Track::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	m_fSizeX = 150.f;
	m_fSizeY = 150.f;
	Set_Texture(L"Icon_Track");
	m_Depth = 3;


	return S_OK;
}

void CMinimap_Track::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*> Prototype_GameObjects = pGameInstance->Find_GameObjectList(LEVEL_GAMEPLAY, LAYER_UI);

	for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
	{
		if (dynamic_cast<CUI_Minimap*>(*it)->Is_MyName(L"Mini_Map") == true)
		{
			CUI_Minimap* Map = dynamic_cast<CUI_Minimap*>(*it);
			 _float2 pos = Map->Make_UIPos_Form_WorldPos(XMLoadFloat4(&m_WorldPos));
			 m_fX = pos.x;
			 m_fY = pos.y-300.f;
			break;
		}
	}
	Safe_Release(pGameInstance);

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_Pos(XMVectorSet(m_fX, m_fY, 0.0f, 1.f));

}

void CMinimap_Track::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMinimap_Track::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

		m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CMinimap_Track::SetUp_Components()
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

HRESULT CMinimap_Track::SetUp_ConstantTable()
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

CMinimap_Track* CMinimap_Track::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMinimap_Track* pInstance = new CMinimap_Track(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_MCMinimap_Trackinimap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI_Base* CMinimap_Track::Clone(void* pArg)
{
	CMinimap_Track* pInstance = new CMinimap_Track(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMinimap_Track");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMinimap_Track::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
