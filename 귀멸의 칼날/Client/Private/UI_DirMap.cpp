#include "UI_DirMap.h"
#include "GameInstance.h"
#include "Battle_Manager.h"
#include "Camera_Free.h"
#include "Level_Story.h"
#include "NPC.h"
#include "UI_Icon.h"

_uint CUI_DirMap::m_iCurOniLayer = 0;

CUI_DirMap::CUI_DirMap(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CUI_Base(_pDevice, _pContext)
{
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

CUI_DirMap::CUI_DirMap(const CUI_DirMap& _rhs)
    : CUI_Base(_rhs)
{
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

HRESULT CUI_DirMap::NativeConstruct_Prototype()
{
    return S_OK;
}

HRESULT CUI_DirMap::NativeConstruct_Clone(void* pArg)
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
		m_fX = 0.f;
		m_fY = 307.f;
		m_fSizeX = 28.f;
		m_fSizeY = 28.f;
		m_Depth = 1;
		Set_Texture(L"Icon_Oni");
	}
	

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));
	
	return S_OK;
}

void CUI_DirMap::Tick(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	__super::Tick(_dTimeDelta);
	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_Pos(XMVectorSet(m_fX, m_fY, 0.0f, 1.f));




	Dir_NPC();
	Dir_Oni();
	
	Safe_Release(pGameInstance);

}

void CUI_DirMap::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_DirMap::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;
	
		m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	
	if (Is_MyName(L"Icon_NPC") == true)
	{
		if (m_fAlpha == 1.f)
		{
			wsprintf(m_szFONTS, TEXT("%dm"),(_int)fNPCLengrh.x);
			pGameInstance->Render_Font(TEXT("Font_Maru10"), m_szFONTS, _float2(m_fX + 625.f, 65.f), XMVectorSet(255.f, 255.f, 255.f, 1.f));
		}
	}

	if (Is_MyName(L"Icon_NPC2") == true)
	{
		if (m_fAlpha == 1.f)
		{
			wsprintf(m_szFONTS, TEXT("%dm"), (_int)fBurgerLengrh.x);
			pGameInstance->Render_Font(TEXT("Font_Maru10"), m_szFONTS, _float2(m_fX + 625.f, 65.f), XMVectorSet(255.f, 255.f, 255.f, 1.f));
		}
	}
	
	Safe_Release(pGameInstance);
	return S_OK;
}

void CUI_DirMap::Extern_ImGuiClass()
{
	if (ImGui::DragFloat("angle", &theta, 0.5f, -100.f, 100.f))
	{ }

	if (ImGui::DragFloat("FX", &m_fX, 0.5f, -500.f, 500.f))
	{
	}
	
}

void CUI_DirMap::Dir_NPC()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	
	
	//카메라
	CCamera_Free* camera = dynamic_cast<CCamera_Free*>(pGameInstance->Find_GameObject(LEVEL_STORY, LAYER_CAM, 0));
	CTransform* cameraTransform = camera->Get_Transform();
	//카메라의 위치
	fCameraPos = cameraTransform->Get_PosF3();
	
	/////////////////첫번쨰 NPC
	
	//NPC
	CNPC* npc = dynamic_cast<CNPC*>(pGameInstance->Find_GameObject(LEVEL_STORY, LAYER_NPC, 0));
	if (npc == nullptr)
	{
		Safe_Release(pGameInstance);
		return;
	}
	CTransform* npcTransform = npc->Get_Transform();
	
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

	CTransform* BurgerTransform = Burger->Get_Transform();
	
	//NPC의 위치
	_float3 fNpcPos = npcTransform->Get_PosF3();
	_float3 fBurgerPos = BurgerTransform->Get_PosF3();
	
	//카메라의 look벡터 
	_float3 fCameraLook = cameraTransform->Get_LookF3();
	fCameraLook.x *= -1.f;
	fCameraLook.y = -0.f;
	fCameraLook.z *= -1.f;
	_vector vCameraLook = XMLoadFloat3(&fCameraLook);
		
	//NPC위치에서 카메라의 위치를 밴 벡터 
	_float3 fCameraToNpc = fNpcPos - fCameraPos;
	fCameraToNpc.y = 0.f;
	_vector vCameraToNpc = XMLoadFloat3(&fCameraToNpc);
	
	_float3 fBurgerToNpc = fBurgerPos - fCameraPos;
	fBurgerToNpc.y = 0.f;
	_vector vBurgerToNpc = XMLoadFloat3(&fBurgerToNpc);




	if (Is_MyName(L"Icon_NPC") == true)
	{
		//거리 
		_vector vLength = XMVector3Length(vCameraToNpc);
		XMStoreFloat3(&fNPCLengrh, vLength);
	}

	if (Is_MyName(L"Icon_NPC2") == true)
	{
		//거리 
		_vector vLength = XMVector3Length(vBurgerToNpc);
		XMStoreFloat3(&fBurgerLengrh, vLength);
	}

	
	//두벡터를 노말라이즈
	 _vector vnCameraLook = XMVector3Normalize(vCameraLook);
	 _vector vnCameraToNpc = XMVector3Normalize(vCameraToNpc);
	 _vector vnBurgerToNpc = XMVector3Normalize(vBurgerToNpc);
	
	//내적
	_float inner = XMVectorGetX(XMVector3Dot(vnCameraLook, vnCameraToNpc));
	_float Burgerinner = XMVectorGetX(XMVector3Dot(vnCameraLook, vnBurgerToNpc));
	
	//각도를 구해준다 
	theta = XMConvertToDegrees(acos(inner));
	Burgertheta = XMConvertToDegrees(acos(Burgerinner));
	
	//================왼쪽 오른쪽 판별
	//외적
	_vector cross = XMVector3Cross(vnCameraLook,vnCameraToNpc);
	_float3 LR;
	XMStoreFloat3(&LR,cross);
	_float fLR = LR.y;
	//값이 양수이면 NPC가 오른쪽에 있다
	//값이 음수이면 NPC가 왼쪽에 있다 

	_vector Burgercross = XMVector3Cross(vnCameraLook, vnBurgerToNpc);
	_float3 BurgerLR;
	XMStoreFloat3(&BurgerLR, Burgercross);
	_float fBurgerLR = BurgerLR.y;
	//값이 양수이면 NPC가 오른쪽에 있다
	//값이 음수이면 NPC가 왼쪽에 있다 

	if (Is_MyName(L"Icon_NPC") == true)
	{
		if (fLR < 0)
			theta *=-1.f;
		
		if (theta <= 45.f && theta >= -45.f)
		{
			m_fAlpha = 1.f;
			m_fX = theta * 7.f;
		}
		else
			m_fAlpha = 0.f;

		if (IconRender == true)
			m_fAlpha = 0.f;

	}

	if (Is_MyName(L"Icon_NPC2") == true)
	{
		if (fBurgerLR < 0)
			Burgertheta *= -1.f;

		if (Burgertheta <= 45.f && Burgertheta >= -45.f)
		{
			m_fAlpha = 1.f;
			m_fX = Burgertheta * 7.f;
		}
		else
			m_fAlpha = 0.f;

		if (IconRender == true)
			m_fAlpha = 0.f;

	}

	//====================================동서남북 설정 

	//=====동쪽 

	_float E_inner = XMVectorGetX(XMVector3Dot(vnCameraLook, XMVectorSet(1.f, 0.f, 0.f, 0.f)));

	_float E_theta = XMConvertToDegrees(acos(E_inner));
	
	_vector E_cross = XMVector3Cross(vnCameraLook,XMVectorSet(1.f,0.f,0.f,0.f));
	
	_float3 E_LR;
	_float E_fLR;
	XMStoreFloat3(&E_LR, E_cross);
	E_fLR = E_LR.y;


	if (Is_MyName(L"Dir_E") == true)
	{
		if (E_fLR < 0)
			E_theta *= -1.f;

		if (E_theta <= 45.f && E_theta >= -45.f)
		{
			m_fAlpha = 1.f;
			m_fX = E_theta * 7.f;
		}
		else
			m_fAlpha = 0.f;
	}

	//=====서쪽

	_float W_inner = XMVectorGetX(XMVector3Dot(vnCameraLook, XMVectorSet(-1.f, 0.f, 0.f, 0.f)));

	_float W_theta = XMConvertToDegrees(acos(W_inner));

	_vector W_cross = XMVector3Cross(vnCameraLook, XMVectorSet(-1.f, 0.f, 0.f, 0.f));

	_float3 W_LR;
	_float W_fLR;
	XMStoreFloat3(&W_LR, W_cross);
	W_fLR = W_LR.y;


	if (Is_MyName(L"Dir_W") == true)
	{
		if (W_fLR < 0)
			W_theta *= -1.f;

		if (W_theta <= 45.f && W_theta >= -45.f)
		{
			m_fAlpha = 1.f;
			m_fX = W_theta * 7.f;
		}
		else
			m_fAlpha = 0.f;
	}

	//=====남쪽

	_float S_inner = XMVectorGetX(XMVector3Dot(vnCameraLook, XMVectorSet(0.f, 0.f, -1.f, 0.f)));

	_float S_theta = XMConvertToDegrees(acos(S_inner));

	_vector S_cross = XMVector3Cross(vnCameraLook, XMVectorSet(0.f, 0.f, -1.f, 0.f));

	_float3 S_LR;
	_float S_fLR;
	XMStoreFloat3(&S_LR, S_cross);
	S_fLR = S_LR.y;


	if (Is_MyName(L"Dir_S") == true)
	{
		if (S_fLR < 0)
			S_theta *= -1.f;

		if (S_theta <= 45.f && S_theta >= -45.f)
		{
			m_fAlpha = 1.f;
			m_fX = S_theta * 7.f;
		}
		else
			m_fAlpha = 0.f;
	}

	//=====북쪽

	_float N_inner = XMVectorGetX(XMVector3Dot(vnCameraLook, XMVectorSet(0.f, 0.f, 1.f, 0.f)));

	_float N_theta = XMConvertToDegrees(acos(N_inner));

	_vector N_cross = XMVector3Cross(vnCameraLook, XMVectorSet(0.f, 0.f, 1.f, 0.f));

	_float3 N_LR;
	_float N_fLR;
	XMStoreFloat3(&N_LR, N_cross);
	N_fLR = N_LR.y;


	if (Is_MyName(L"Dir_N") == true)
	{
		if (N_fLR < 0)
			N_theta *= -1.f;

		if (N_theta <= 45.f && N_theta >= -45.f)
		{
			m_fAlpha = 1.f;
			m_fX = N_theta * 7.f;
		}
		else
			m_fAlpha = 0.f;
	}
	
	
	Safe_Release(pGameInstance);
}

void CUI_DirMap::Dir_Oni()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager);
	{
		_uint iCurLayer = pInstance->Get_CurrentOniLayer();
		if (m_iCurOniLayer == iCurLayer)
		{
			RELEASE_INSTANCE(CGameInstance);
			RELEASE_INSTANCE(CBattle_Manager);
			return;
		}

		m_iCurOniLayer = iCurLayer;

		_int    OniNameNumber = 0;

		if (m_iCurOniLayer == 1)
		{
			//오니 첫그륩 
			list<CGameObject*> zakolist = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_ONI1);

			for (auto it = zakolist.begin(); it != zakolist.end(); it++) //오니 개수만큼 아이콘 생성 
			{
				OniNameNumber++;
				//wsprintf(m_szNAMES, TEXT("Oni_Icon%d"), OniNameNumber);
				pGameInstance->Clone_GameObject(LEVEL_STORY,
					LAYER_MAP, PROTO_GOB_UI_DIRICON, &OniNameNumber);
			}
		}

		else if (m_iCurOniLayer == 2)
		{
			//오니 두번째그륩 
			list<CGameObject*> zakolist = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_ONI2);

			for (auto it = zakolist.begin(); it != zakolist.end(); it++) //오니 개수만큼 아이콘 생성 
			{
				OniNameNumber++;
				//wsprintf(m_szNAMES, TEXT("Oni_Icon%d"), OniNameNumber);
				pGameInstance->Clone_GameObject(LEVEL_STORY,
					LAYER_MAP, PROTO_GOB_UI_DIRICON, &OniNameNumber);
			}
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	RELEASE_INSTANCE(CBattle_Manager);

}

HRESULT CUI_DirMap::SetUp_Components()
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

HRESULT CUI_DirMap::SetUp_ConstantTable()
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

CUI_DirMap* CUI_DirMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_DirMap* pInstance = new CUI_DirMap(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Minimap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI_Base* CUI_DirMap::Clone(void* pArg)
{
	CUI_DirMap* pInstance = new CUI_DirMap(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_DirMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_DirMap::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
