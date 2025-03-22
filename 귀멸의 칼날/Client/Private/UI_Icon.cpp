#include "UI_Icon.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Camera_Free.h"
#include "Player.h"
#include "Level_Story.h"
#include "Battle_Manager.h"
#include "NPC.h"

//3DUI
//퀘스트 아이콘 , 오토 타겟팅 
CUI_Icon::CUI_Icon(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CUI_Base(_pDevice, _pContext)
{
  
}

CUI_Icon::CUI_Icon(const CUI_Icon& _rhs)
    : CUI_Base(_rhs)
{
 
}

HRESULT CUI_Icon::NativeConstruct_Prototype()
{
    return S_OK;
}

HRESULT CUI_Icon::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;
	
    if (FAILED(SetUp_Components()))
        return E_FAIL;
	
	m_iType = *(_int*)pArg;
	
	Setting_Positon(m_iType);
  
	
    return S_OK;
}

void CUI_Icon::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);

	Change_Texture(CurrentTexture);
	
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (g_iCurrentLevel == LEVEL_STORY)
	{
		CCamera_Free* Camera = dynamic_cast<CCamera_Free*>(pGameInstance->Find_GameObject(LEVEL_STORY, LAYER_CAM, 0));
		if (Camera == nullptr)
		{
			Safe_Release(pGameInstance);
			return;
		}
		CTransform* CameraTransform = Camera->Get_Transform();

		_vector vCamerarPos = CameraTransform->Get_Pos();
		m_pTransformCom->LookBack(vCamerarPos);

	}

	else if (g_iCurrentLevel == LEVEL_RATTAN)
	{
		CCamera_Free* Camera = dynamic_cast<CCamera_Free*>(pGameInstance->Find_GameObject(LEVEL_RATTAN, LAYER_CAM, 0));
		if (Camera == nullptr)
		{
			Safe_Release(pGameInstance);
			return;
		}
		CTransform* CameraTransform = Camera->Get_Transform();

		_vector vCamerarPos = CameraTransform->Get_Pos();
		m_pTransformCom->LookBack(vCamerarPos);

	}
	

	
	Safe_Release(pGameInstance);
}

void CUI_Icon::Late_Tick(const _double& _dTimeDelta)
{
    if (nullptr == m_pRendererCom)
        return;

	Tick_Event(m_iType, _dTimeDelta);

    m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CUI_Icon::Render()
{
    if (nullptr == m_pVIBufferCom)
        return E_FAIL;

    if (FAILED(SetUp_ConstantTable()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

	if (m_iType == QUEST_TALK && TalkQuest == true)
		m_pVIBufferCom->Render();

	else if (m_iType != QUEST_TALK && TalkQuest == true)
		m_pVIBufferCom->Render();

	else if (m_iType == TARGETING_ICON || m_iType == TARGETTING_RING)
		m_pVIBufferCom->Render();

	
	
    return S_OK;
}

void CUI_Icon::Extern_ImGuiClass()
{
	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_Pos());

	if (ImGui::DragFloat3("Pos", &vPos.x, 0.1f, -1000.f, 1000.f))
		m_pTransformCom->Set_PosF3(vPos);
	
	_float3 fScale;
	fScale = m_pTransformCom->Get_Scale();

	if (ImGui::DragFloat3("Scale", &fScale.x, 0.1f, -1000.f, 1000.f))
		m_pTransformCom->Set_Scale(fScale);

	_float fRot;
	if (ImGui::DragFloat("Rotatey", &fRot, 0.1f, -1000.f, 1000.f))
		m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), fRot);
	
	_float fRotx;
	if (ImGui::DragFloat("Rotatex", &fRotx, 0.1f, -1000.f, 1000.f))
		m_pTransformCom->Rotate_Fix(XMVectorSet(1.f, 0.f, 0.f, 1.f), fRotx);

	_float fRotz;
	if (ImGui::DragFloat("Rotatez", &fRotz, 0.1f, -1000.f, 1000.f))
		m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 0.f, 1.f, 1.f), fRotz);
	
}

void CUI_Icon::Change_Texture(_uint Type)
{
	if (PrevTexture == CurrentTexture)
		return;
	PrevTexture = CurrentTexture;
	
	switch (CurrentTexture)
	{
	case Client::CUI_Icon::QUEST:
		wsprintf(m_TextureName, TEXT("Icon_Quest"));
		break;
	case Client::CUI_Icon::TALK:
		wsprintf(m_TextureName, TEXT("Quest_Pop"));
		break;
	case Client::CUI_Icon::MAIN:
		wsprintf(m_TextureName, TEXT("Quest_MainIcon"));
		break;
	case Client::CUI_Icon::QUSEST_CLEAR:
		wsprintf(m_TextureName, TEXT("Icon_QuestCler"));
		break;
	case Client::CUI_Icon::ARROW_TALK:
		wsprintf(m_TextureName, TEXT("Icon_Talk"));
		break;
	case Client::CUI_Icon::TARGETING_ICONTEX:
		wsprintf(m_TextureName, TEXT("Targeting_Icon"));
		break;
	case Client::CUI_Icon::TARGETING_BASETEX:
		wsprintf(m_TextureName, TEXT("Targeting_Base"));
		break;
	case Client::CUI_Icon::TARGETTING_RINGTEX:
		wsprintf(m_TextureName, TEXT("Targeting_Ring"));
		break;
	default:
		break;
	}
}

void CUI_Icon::Setting_Positon(_uint Type)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (g_iCurrentLevel == LEVEL_STORY)
	{
		switch (Type)
		{
		case Client::CUI_Icon::QUEST_CANDIDATE:
			m_pTransformCom->Set_PosF3(_float3(-521.5f, 180.f, 346.4f));
			m_pTransformCom->Set_Scale(_float3(5.f, 5.f, 5.f));
			CurrentTexture = QUEST;
			break;
		case Client::CUI_Icon::QUEST_TALK:
			m_pTransformCom->Set_PosF3(_float3(-521.5f, 185.f, 346.4f));
			m_pTransformCom->Set_Scale(_float3(11.f, 2.5f, 5.f));
			CurrentTexture = TALK;
			break;
		case Client::CUI_Icon::QUEST_CANDIDATE2:
			m_pTransformCom->Set_PosF3(_float3(-1192.f, 223.f, 546.2f));
			m_pTransformCom->Set_Scale(_float3(5.f, 5.f, 5.f));
			CurrentTexture = QUEST;
			break;
		case Client::CUI_Icon::QUEST_TALK2:
			m_pTransformCom->Set_PosF3(_float3(-1192.f, 228.f, 546.2f));
			m_pTransformCom->Set_Scale(_float3(11.f, 2.5f, 5.f));
			CurrentTexture = TALK;
			break;
		case Client::CUI_Icon::TARGETING_ICON:
			m_pTransformCom->Set_Scale(_float3(2.f, 2.f, 2.f));
			CurrentTexture = TARGETING_ICONTEX;
			break;
		case Client::CUI_Icon::TARGETING_BASE:
			CurrentTexture = TARGETING_BASETEX;
			break;
		case Client::CUI_Icon::TARGETTING_RING:
			CurrentTexture = TARGETTING_RINGTEX;
			break;
		}
	}

	else if (g_iCurrentLevel == LEVEL_RATTAN)
	{
		switch (Type)
		{
		case Client::CUI_Icon::QUEST_AOI:
			m_pTransformCom->Set_PosF3(_float3(-5.5f, 25.5f, -27.6f));
			m_pTransformCom->Set_Scale(_float3(5.f, 5.f, 5.f));
			CurrentTexture = QUEST;
			break;
		case Client::CUI_Icon::AOI_TALK:
			m_pTransformCom->Set_PosF3(_float3(-5.5f, 28.6f, -27.6f));
			m_pTransformCom->Set_Scale(_float3(8.5f, 1.8f, 5.f));
			CurrentTexture = TALK;
			break;
		case Client::CUI_Icon::QUEST_MAIN:
			m_pTransformCom->Set_PosF3(_float3(19.7f, 25.2f, -6.f));
			m_pTransformCom->Set_Scale(_float3(3.9f, 4.1f, 5.f));
			CurrentTexture = MAIN;
			break;
		case Client::CUI_Icon::SISTER_TALK:
			m_pTransformCom->Set_PosF3(_float3(19.7f, 28.f, -6.f));
			m_pTransformCom->Set_Scale(_float3(8.5f, 1.8f, 5.f));
			CurrentTexture = TALK;
			break;
		case Client::CUI_Icon::KANAWO_ICON:
			m_pTransformCom->Set_PosF3(_float3(-35.f, 26.1f, -21.9f));
			m_pTransformCom->Set_Scale(_float3(5.f, 5.f, 5.f));
			CurrentTexture = ARROW_TALK;
			break;
		case Client::CUI_Icon::KANAWO_TALK:
			m_pTransformCom->Set_PosF3(_float3(-35.f, 8.6f + 19.f, -21.9f));
			m_pTransformCom->Set_Scale(_float3(8.5f, 1.8f, 5.f));
			CurrentTexture = TALK;
			break;
		case Client::CUI_Icon::CANDIDATE_ICON:
			m_pTransformCom->Set_PosF3(_float3(-17.7f, 27.f, 22.6f));
			m_pTransformCom->Set_Scale(_float3(5.f, 5.f, 5.f));
			CurrentTexture = ARROW_TALK;
			break;
		case Client::CUI_Icon::CANDIDATE_TALK:
			m_pTransformCom->Set_PosF3(_float3(-17.7f, 27.3f, 22.6f));
			m_pTransformCom->Set_Scale(_float3(8.5f, 1.8f, 5.f));
			CurrentTexture = TALK;
			break;
		case Client::CUI_Icon::GENYA_ICON:
			m_pTransformCom->Set_PosF3(_float3(11.9f, 27.f, 22.4f));
			m_pTransformCom->Set_Scale(_float3(5.f, 5.f, 5.f));
			CurrentTexture = ARROW_TALK;
			break;
		case Client::CUI_Icon::GENYA_TALK:
			m_pTransformCom->Set_PosF3(_float3(11.9f, 27.f, 22.4f));
			m_pTransformCom->Set_Scale(_float3(8.5f, 1.8f, 5.f));
			CurrentTexture = TALK;
			break;
		
		}
	}
	Safe_Release(pGameInstance);
}

void CUI_Icon::Tick_Event(_uint Type, const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (g_iCurrentLevel == LEVEL_STORY)
	{
		_float3 Onipos = _float3(0.f, 0.f, 0.f);
		{
			CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_PLAYER, 0);
			if (nullptr != pPlayer && nullptr != pPlayer->Get_TargetTransform())
			{
				Onipos = pPlayer->Get_TargetTransform()->Get_Pos();
			}
		}

		switch (Type)
		{
		case Client::CUI_Icon::QUEST_CANDIDATE:
			break;
		case Client::CUI_Icon::QUEST_TALK:
			break;
		case Client::CUI_Icon::TARGETING_ICON:
			m_pTransformCom->Set_PosF3(_float3(Onipos.x, Onipos.y +17.5f, Onipos.z));
			TalkQuest = true;
			break;
		case Client::CUI_Icon::TARGETING_BASE:
			m_pTransformCom->Set_PosF3(_float3(Onipos.x, Onipos.y + 1.f, Onipos.z));
			m_pTransformCom->Set_Scale(_float3(13.f, 13.f, 13.f));
			m_pTransformCom->Rotate_Fix(XMVectorSet(1.f, 0.f, 0.f, 1.f), 90.f);
			break;
		case Client::CUI_Icon::TARGETTING_RING:
			//Onipos.y += 2.f;
			m_pTransformCom->Set_PosF3(_float3(Onipos.x, Onipos.y + 1.f, Onipos.z));
			m_pTransformCom->Set_Scale(_float3(15.f, 15.f, 15.f));
			m_pTransformCom->Rotate_Fix(XMVectorSet(1.f, 0.f, 0.f, 1.f), 90.f);
			break;
		}
	}

	if (g_iCurrentLevel == LEVEL_RATTAN)
	{
		list<CGameObject*> Prototype_NPCObjects = pGameInstance->Find_GameObjectList(LEVEL_RATTAN, LAYER_NPC);

		CNPC* zenitsu = nullptr;
		for (auto it = Prototype_NPCObjects.begin(); it != Prototype_NPCObjects.end(); it++)
		{
			if (dynamic_cast<CNPC*>(*it)->Get_Type() == CNPC::NPC_ZENITSU)
			{
				zenitsu = dynamic_cast<CNPC*>(*it);
				break;
			}
		}
		if (zenitsu == nullptr)
		{
			Safe_Release(pGameInstance);
			return;
		}
		CTransform* zenitsuTransform = zenitsu->Get_Transform();
		_float3 zenitsuPos = zenitsuTransform->Get_PosF3();
		switch (Type)
		{
		case Client::CUI_Icon::ZENITSU_ICON:
			m_pTransformCom->Set_PosF3(_float3(zenitsuPos.x, zenitsuPos.y + 18.5f, zenitsuPos.z));
			m_pTransformCom->Set_Scale(_float3(5.f, 5.f, 5.f));
			CurrentTexture = ARROW_TALK;
			break;
		case Client::CUI_Icon::ZENITSU_TALK:
			m_pTransformCom->Set_PosF3(_float3(zenitsuPos.x, zenitsuPos.y + 19.f, zenitsuPos.z));
			m_pTransformCom->Set_Scale(_float3(8.5f, 1.8f, 5.f));
			CurrentTexture = TALK;
			break;
		}
	}
	Safe_Release(pGameInstance);
}

_float3 CUI_Icon::Oni_Targeting(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);



		//플레이어
		CPlayer* player = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_PLAYER, 0));
		if (player == nullptr)
		{
			Safe_Release(pGameInstance);
			return _float3{ 0.f,0.f,0.f };
		}
		CTransform* playerTransform = player->Get_Transform();
		//플레이어의 위치
		_float3 PlayerPos = playerTransform->Get_PosF3();

		//플레이어의 look벡터 
		_float3 fPlayerLook = playerTransform->Get_LookF3();
		fPlayerLook.x *= -1.f;
		fPlayerLook.y = -0.f;
		fPlayerLook.z *= -1.f;
		_vector vPlayerLook = XMLoadFloat3(&fPlayerLook);

		list<CGameObject*> zakolist;
		if (m_iCurOniLayer == 1)
		{
			//오니(첫번째 그륩)
			zakolist = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_ONI1);
			if (zakolist.empty())
			{
				Safe_Release(pGameInstance);
				return _float3{ 0.f,0.f,0.f };
			}
		}

		else if (m_iCurOniLayer == 2)
		{
			//오니(두번째 그륩)
			zakolist = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_ONI2);
			if (zakolist.empty())
			{
				Safe_Release(pGameInstance);
				return _float3{ 0.f,0.f,0.f };
			}
		}

		_float MinOnitheta = 180.f;
		_float MinLength = 200.f;
		CGameObject* pMinOni = nullptr;

		for (auto it = zakolist.begin(); it != zakolist.end(); it++)
		{
			CGameObject* pOni = *it;
			CTransform* pOniTransform = pOni->Get_Transform();
			//오니의 위치
			_float3 fOniPos = pOniTransform->Get_PosF3();

			//플레이어 위치에서 카메라의 위치를 밴 벡터 
			_float3 fPlayerToOni = fOniPos - PlayerPos;
			fPlayerToOni.y = 0.f;
			_vector vPlayerToOni = XMLoadFloat3(&fPlayerToOni);

			//오니에서 플레이어를 뺀 길이 
			_vector vLength = XMVector3Length(vPlayerToOni);
			_float3 fLength;
			XMStoreFloat3(&fLength, vLength);

			//두벡터를 노말라이즈
			_vector vnPlayerLook = XMVector3Normalize(vPlayerLook);
			_vector vnPlayerToOni = XMVector3Normalize(vPlayerToOni);

			//내적
			_float inner = XMVectorGetX(XMVector3Dot(vnPlayerLook, vnPlayerToOni));

			//각도를 구해준다 
			_float TmpOnitheta = XMConvertToDegrees(acos(inner));

			if (MinOnitheta + 5.f > TmpOnitheta)
			{
				MinOnitheta = TmpOnitheta;
				if (MinLength > fLength.x)
				{
					MinLength = fLength.x;
					pMinOni = *it;
				}
				else
					continue;
			}
			else
				continue;
		}

		if (pMinOni == nullptr)
		{
			Safe_Release(pGameInstance);
			return _float3{ 0.f,0.f,0.f };
		}


		pMinOniTransform = pMinOni->Get_Transform();
		if (pMinOniTransform == nullptr)
		{
			Safe_Release(pGameInstance);
			return _float3{ 0.f,0.f,0.f };
		}
		_float3 pMinOniPos = pMinOniTransform->Get_PosF3();

		//플레이어에 자코 트랜스폼넘겨줄것 =======================================================================
		player->Set_TargetTransform(pMinOniTransform);
		
			return pMinOniPos;

	
	Safe_Release(pGameInstance);


}


HRESULT CUI_Icon::SetUp_Components()
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

HRESULT CUI_Icon::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (nullptr == m_pTransformCom || nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		_float4x4 WorldMatrix, ViewMatrix, ProjMatrix;
		WorldMatrix = m_pTransformCom->Get_WorldFloat4x4_TP();
		ViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
		ProjMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);
		_float camFar = pGameInstance->Get_CamFar();

		if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Set_RawValue("g_CamFar", &camFar, sizeof(_float))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
		if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DiffuseTexture", m_TextureName)))
			return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CUI_Icon* CUI_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Icon* pInstance = new CUI_Icon(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Icon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI_Base* CUI_Icon::Clone(void* pArg)
{
	CUI_Icon* pInstance = new CUI_Icon(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Icon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Icon::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
