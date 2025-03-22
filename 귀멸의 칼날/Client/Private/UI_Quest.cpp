#include "UI_Quest.h"
#include "GameInstance.h"
#include "UI_Icon.h"
#include "NPC.h"
#include "UI_DirMap.h"

//퀘스트 목록 
//상호작용 텍스트 

CUI_Quest::CUI_Quest(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CUI_Base(_pDevice, _pContext)
{
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());

}

CUI_Quest::CUI_Quest(const CUI_Quest& _rhs)
    : CUI_Base(_rhs)
{
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

HRESULT CUI_Quest::NativeConstruct_Prototype()
{
    return S_OK;
}

HRESULT CUI_Quest::NativeConstruct_Clone(void* pArg)
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

	//임시
	if (Is_MyName(L"Quest_Base") == true || Is_MyName(L"Quest_Icon") == true || Is_MyName(L"Quest_Text") == true)
		m_fAlpha = 0.f;

	if (Is_MyName(L"Msg_Base") || Is_MyName(L"Msg_Name") || Is_MyName(L"Msg_KeyF"))
		m_fAlpha = 0.f;
	
	if (Is_MyName(L"Msg_Text"))
		m_fAlpha = 0.f;
	
	if (Is_MyName(L"Rattan_Base") == true || Is_MyName(L"Rattan_Icon") == true || Is_MyName(L"Rattan_Quest") == true)
		m_fAlpha = 0.f;

	return S_OK;
}

void CUI_Quest::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_Pos(XMVectorSet(m_fX, m_fY, 0.0f, 1.f));
	
	Effect_Story_MainQuest(_dTimeDelta);
	Effect_Rattan_MainQuest(_dTimeDelta);
	Quest_Text(_dTimeDelta);

}

void CUI_Quest::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

}

HRESULT CUI_Quest::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

		m_pShaderCom->Begin(1);
		
		
			m_pVIBufferCom->Render();

		
	return S_OK;
}

void CUI_Quest::Effect_Story_MainQuest(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (Is_MyName(L"Quest_Base") == true || Is_MyName(L"Quest_Icon") == true || Is_MyName(L"Quest_Text") == true)
	{
		m_AlphaTime += _dTimeDelta;
		if (m_AlphaTime >= 1)
		{
			m_PlusAlpha += 0.01f;
			m_AlphaTime = 0.0;
		}

		m_fAlpha += m_PlusAlpha;
		if (m_fAlpha >= 1.f)
		{
			m_fAlpha = 1.f;
			if (Is_MyName(L"Quest_Base") == true && SoundPlay == false)
			{
				pGameInstance->Play_SoundW(TEXT("UI_MainQuest"), 0.4f);
				SoundPlay = true;
			}
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}
void CUI_Quest::Effect_Rattan_MainQuest(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (Is_MyName(L"Rattan_Base") == true || Is_MyName(L"Rattan_Icon") == true || Is_MyName(L"Rattan_Quest") == true)
	{
		m_AlphaTime += _dTimeDelta;
	
		if (m_AlphaTime >= 1)
		{
			m_PlusAlpha += 0.01f;
			m_AlphaTime = 0.0;
		}

		m_fAlpha += m_PlusAlpha;
		if (m_fAlpha >= 1.f)
		{
			m_fAlpha = 1.f;
			if (Is_MyName(L"Rattan_Base") == true && SoundPlay == false)
			{
				pGameInstance->Play_SoundW(TEXT("UI_MainQuest"), 0.4f);
				SoundPlay = true;
			}
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}
void CUI_Quest::Quest_Text(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	
	//첫NPC 
	CNPC* Candidate = dynamic_cast<CNPC*>(pGameInstance->Find_GameObject(LEVEL_STORY, LAYER_NPC, 0));
	CUI_Icon* Icon = dynamic_cast<CUI_Icon*>(pGameInstance->Find_GameObject(LEVEL_STORY, LAYER_QUEST, 0));
	CUI_DirMap* Map = dynamic_cast<CUI_DirMap*>(pGameInstance->Find_GameObject(LEVEL_STORY, LAYER_MAP, 1));

	//버거형 NPC 

	list<CGameObject*> Prototype_NPCObjects = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_NPC);

	if (Prototype_NPCObjects.empty())
	{
		Safe_Release(pGameInstance);
		return;
	}
	
	CNPC* Burger = nullptr;
	for (auto it = Prototype_NPCObjects.begin(); it != Prototype_NPCObjects.end(); it++)
	{
		if (dynamic_cast<CNPC*>(*it)->Get_Type() == CNPC::NPC_SIT_CANDIDATE)
		{
			Burger = dynamic_cast<CNPC*>(*it);
			break;
		}
	}

	if (Burger == nullptr)
	{
		Safe_Release(pGameInstance);
		return;
	}

	list<CGameObject*> Prototype_IconObjects = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_QUEST);

	CUI_Icon* Burgertalk = nullptr;
	for (auto it = Prototype_IconObjects.begin(); it != Prototype_IconObjects.end(); it++)
	{
		if (dynamic_cast<CUI_Icon*>(*it)->Get_Type() == CUI_Icon::QUEST_CANDIDATE2)
		{
			Burgertalk = dynamic_cast<CUI_Icon*>(*it);
			break;
		}
	}

	if (Burgertalk == nullptr)
	{
		Safe_Release(pGameInstance);
		return;
	}


	list<CGameObject*> Prototype_DirObjects = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_MAP);

	CUI_DirMap* BurgerIcon = nullptr;
	for (auto it = Prototype_DirObjects.begin(); it != Prototype_DirObjects.end(); it++)
	{
		if (dynamic_cast<CUI_DirMap*>(*it)->Is_MyName(L"Icon_NPC2") == true)
		{
			BurgerIcon = dynamic_cast<CUI_DirMap*>(*it);
			break;
		}
	}

	if (BurgerIcon == nullptr)
	{
		Safe_Release(pGameInstance);
		return;
	}



	//================================
	
	if (Is_MyName(L"Msg_Text"))
	{
		if (m_NPCChoice == false)
		{
			if (Candidate != nullptr && Candidate->Get_PopRender() == true && TextureIndex < 6)
				m_fAlpha = 1.f;

			if (pGameInstance->Get_DIKeyDown(DIK_F) && m_fAlpha == 1.f && TextureIndex < 6)
			{
				pGameInstance->Play_SoundW(TEXT("UI_Talk"), 0.3f);
				_int test = TextureIndex;
				TextureIndex++;
				wsprintf(m_TextureName, TEXT("Candidate_Text%d"), TextureIndex);
			}

			if (TextureIndex > 5)
			{
				m_fAlpha = 0.f;
				Icon->Set_TextureType(CUI_Icon::QUSEST_CLEAR);
				Map->Set_IconRender(true);
				m_NPCChoice = true;
				Candidate->Change_State(CNPC::TALK_END);
			}
		}
			///////
		if (m_NPCChoice == true)
		{
			if (Burger != nullptr && Burger->Get_PopRender() == true && BurgerTextureIndex > 6)
			{
				wsprintf(m_TextureName, TEXT("Candidate_Text%d"), BurgerTextureIndex);
				m_fAlpha = 1.f;
			}
			if (pGameInstance->Get_DIKeyDown(DIK_F) && m_fAlpha == 1.f && BurgerTextureIndex > 6)
			{
				pGameInstance->Play_SoundW(TEXT("UI_Talk"), 0.3f);
				_int test = BurgerTextureIndex;
				BurgerTextureIndex++;
				wsprintf(m_TextureName, TEXT("Candidate_Text%d"), BurgerTextureIndex);
			}

			if (BurgerTextureIndex > 11)
			{
				m_fAlpha = 0.f;
				Burgertalk->Set_TextureType(CUI_Icon::QUSEST_CLEAR);
				BurgerIcon->Set_IconRender(true);
				Burger->Change_State(CNPC::TALK_END);
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}
HRESULT CUI_Quest::SetUp_Components()
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

HRESULT CUI_Quest::SetUp_ConstantTable()
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

CUI_Quest* CUI_Quest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Quest* pInstance = new CUI_Quest(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Quest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI_Base* CUI_Quest::Clone(void* pArg)
{
	CUI_Quest* pInstance = new CUI_Quest(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Quest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Quest::Free()
{
	__super::Free();

Safe_Release(m_pTextureCom);
Safe_Release(m_pRendererCom);
Safe_Release(m_pShaderCom);
Safe_Release(m_pVIBufferCom);
}