#include "UI.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Level_Loading.h"
#include "NPC.h"
#include "Camera_Free.h"
#include "Player.h"
#include "Sound_Manager.h"

//로고화면, 로딩화면 

CUI::CUI(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CUI_Base(_pDevice, _pContext)
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());

}

CUI::CUI(const CUI& _rhs)
	: CUI_Base(_rhs)
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

HRESULT CUI::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CUI::NativeConstruct_Clone(void* pArg)
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

	if (Is_MyName(L"Logo_Smoke_Eff") == true)
		m_fAlpha = 0.f;

	if (Is_MyName(L"Logo_Text") == true)
		m_fAlpha = 0.f;

	if(Is_MyName(L"Mode_StartEff") == true)
	{
	m_fFrame = 48.f;
	m_bModeStartEff = true;
	}

	if (Is_MyName(L"Rattan_Text") == true || Is_MyName(L"Rattan_Select0") == true || Is_MyName(L"Rattan_Select1") == true || Is_MyName(L"Rattan_Select") == true)
		m_fAlpha = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));
	
	UI_Sound();
	
	return S_OK;
}

void CUI::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_Pos(XMVectorSet(m_fX, m_fY, 0.0f, 1.f));

	Logo_SmokeEffect(_dTimeDelta);
	Logo_Effect(_dTimeDelta);

	if (Is_MyName(L"Load_Anim") == true)
	{
		m_fFrame += (_float)_dTimeDelta * 8.0f;

		if (6.0f <= m_fFrame)
			m_fFrame = 0.f;


		wsprintf(m_TextureName, TEXT("Load_Anim%d"), (_int)m_fFrame);
	}
	
	Loading_Effect(_dTimeDelta);
	
	Story_Text(_dTimeDelta);
	
	Mode_Effect(_dTimeDelta);

	Rattan_Text(_dTimeDelta);

	Eednig_Effect(_dTimeDelta);

	
}

void CUI::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	

}
HRESULT CUI::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;
	
	if (Is_MyName(L"Logo_Somke") == true)
		m_pShaderCom->Begin(5);

	else if (Is_MyName(L"Logo_Smoke_Eff") == true || Is_MyName(L"Mode_SmokeEff") == true || Is_MyName(L"Mode_StartEff") == true)
		m_pShaderCom->Begin(2);

	else if (Is_MyName(L"Mode_Eff") == true || Is_MyName(L"End_BackEff") == true)
		m_pShaderCom->Begin(9);
	
	else
	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}
void CUI::Logo_Effect(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	v2MousePos = pGameInstance->Get_MousePosition();
	POINT pt = { (_long)v2MousePos.x, (_long)v2MousePos.y };
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);
	


	//if (Is_MyName(L"Select1") == true || Is_MyName(L"Select2") == true || Is_MyName(L"Logo_Smoke_Eff") == true)
	//{
	//	if (pGameInstance->Get_DIKeyDown(DIK_UP) && m_MoadIndex != 0)
	//		m_MoadIndex--;
	//
	//	if (pGameInstance->Get_DIKeyDown(DIK_DOWN) && m_MoadIndex != 1)
	//		m_MoadIndex++;
	//	
	//	if (m_MoadIndex == 0 )
	//	{
	//		if (Is_MyName(L"Select1") == true && SoundPlay == true)
	//		{
	//			pGameInstance->Play_SoundW(TEXT("UI_Mouseover"), SOUND_MAX);
	//			SoundPlay = false;
	//			Set_Texture(L"Logo_Title_Select");
	//		}
	//		if (Is_MyName(L"Select2") == true)
	//		{
	//			Set_Texture(L"Logo_Title_Base");
	//			SoundPlay = true;
	//		}
	//		
	//		if (pGameInstance->Get_DIKeyUp(DIK_RETURN))
	//		{
	//			if (Is_MyName(L"Select1") == true)
	//			{
	//				pGameInstance->Play_SoundW(TEXT("UI_Select"), SOUND_MAX);
	//				m_fAlpha = 1.8f;
	//			}
	//			if (Is_MyName(L"Logo_Smoke_Eff") == true)
	//				m_bLogoEffect = true;
	//		}
	//	}
	//	
	//	else if (m_MoadIndex == 1 )
	//	{
	//		if (Is_MyName(L"Select1") == true)
	//		{
	//			Set_Texture(L"Logo_Title_Base");
	//			SoundPlay = true;
	//		}
	//		if (Is_MyName(L"Select2") == true && SoundPlay == true)
	//		{
	//			Set_Texture(L"Logo_Title_Select");
	//			pGameInstance->Play_SoundW(TEXT("UI_Mouseover"), SOUND_MAX);
	//			SoundPlay = false;
	//		}
	//	}

	//}
	
	if (Is_MyName(L"Select1") == true)//Is_MyName(L"Logo_Smoke_Eff") == true)
	{
		
		if (true == InterSect((_int)v2MousePos.x - (_int)g_iWinSizeX / 2, (_int)-v2MousePos.y + (_int)g_iWinSizeY / 2))
		{
			Set_Texture(L"Logo_Title_Select");


			if (SoundPlay == true)
			{
				pGameInstance->Play_SoundW(TEXT("UI_Mouseover"), 0.2f);
				SoundPlay = false;
			}
			
			if (pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
			{
				pGameInstance->Play_SoundW(TEXT("UI_Select"), 0.2f);
				m_fAlpha = 1.8f;
				m_bLogoEffect = true;
				
			}
		}
		else
		{
			Set_Texture(L"Logo_Title_Base");
			SoundPlay = true;
		}
	}

	if (Is_MyName(L"Logo_Smoke_Eff") == true) //임시 
	{
		if (true == InterSect((_int)v2MousePos.x - (_int)g_iWinSizeX / 2, (_int)-v2MousePos.y + (_int)g_iWinSizeY / 2))
		{
			if (CGameInstance::Get_Instance()->Get_DIMBtnDown(CInput_Device::DIMB_L))
			{
				m_bLogoEffect = true;
			}
		}
	}

	if (Is_MyName(L"Select2") == true)
	{
		if (true == InterSect((_int)v2MousePos.x - (_int)g_iWinSizeX / 2, (_int)-v2MousePos.y + (_int)g_iWinSizeY / 2))
		{
			Set_Texture(L"Logo_Title_Select");

			if (SoundPlay == true)
			{
				pGameInstance->Play_SoundW(TEXT("UI_Mouseover"), 0.2f);
				SoundPlay = false;
			}
		}
		else
		{
			Set_Texture(L"Logo_Title_Base");
			SoundPlay = true;
		}
	}
	
	if (Is_MyName(L"Logo_Eff") == true)
		L_Diagonal(_float4(450.f, 241.f, 950.f, 500.f), 0.5f, 1.f, 40.f, true,_dTimeDelta);
	
	if (Is_MyName(L"Logo_Eff2") == true)
		L_Diagonal(_float4(-105.6f, 357.4f, 950.f, 500.f), 0.4f, 0.5f, 40.f, true,_dTimeDelta);
	

	if (Is_MyName(L"Logo_Somke") == true)
		m_UVFlow += (_float)_dTimeDelta * 0.05f;

	//if(Is_MyName(L"Logo_Text") == true)
	//Plus_Alpha(1.f, 2.f, 0.0f, _dTimeDelta);
	
		Safe_Release(pGameInstance);
}
void CUI::Logo_SmokeEffect(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	
	if (m_bLogoEffect == true)
	{
		m_fFrame += (_float)_dTimeDelta * 20.0f;

		if (48.0f <= m_fFrame)
		{
			pGameInstance->Stop_All();
			m_fFrame = 0.f;
			m_bLogoEffect = false;
			m_bStartGame = true;
		}
		if (Is_MyName(L"Logo_Smoke_Eff") == true)
			wsprintf(m_TextureName, TEXT("Xcmn_Ink_%d"), (_int)m_fFrame);
	}

	
	Safe_Release(pGameInstance);
}
void CUI::Loading_Effect(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	
	if (Is_MyName(L"Load_WindowL") == true || Is_MyName(L"Load_WindowR") == true)
	{
		m_fAlpha -= (_float)_dTimeDelta*0.2f;
		if (m_fAlpha <= 0.f)
			m_fAlpha = 0.f;
	}
	
	if (Is_MyName(L"Load_Cloud") == true)
	{
		if (-637.f <= m_fX && m_brepeat != true)
			m_fX -= (_float)_dTimeDelta*3.f;

		if (-637.f >= m_fX)
			m_brepeat = true;

		if (m_brepeat == true)
		{
			m_fX += (_float)_dTimeDelta * 3.f;

			if (m_fX == -600.f)
				m_brepeat = false;
		}
	}

	if (Is_MyName(L"Load_Cloud2") == true)
	{
		if (600.f <= m_fX && m_brepeat != true)
			m_fX -= (_float)_dTimeDelta * 3.f;

		if (600.f >= m_fX)
			m_brepeat = true;

		if (m_brepeat == true)
		{
			m_fX += (_float)_dTimeDelta *3.f;

			if (m_fX == 637.f)
				m_brepeat = false;
		}
	}

	if (pGameInstance->Get_DIKeyUp(DIK_SPACE))
	{
		m_LoadAni_fx -= 5.f;

		if (Is_MyName(L"Load_Anim") == true || Is_MyName(L"Load_Space") == true)
			m_fX -= 5.f;
	}

	if (Is_MyName(L"Load_Item0") == true)
	{
		m_fY += (_float)_dTimeDelta * 55.f;
		if (m_fY >= -293.3f)
			m_fY = -293.3f;

		if(m_LoadAni_fx < 303.7f)
		m_fAlpha = 0.f;
	}

	if (Is_MyName(L"Load_Item1") == true)
	{
		m_fY += (_float)_dTimeDelta * 50.f;
		if (m_fY >= -293.3f)
			m_fY = -293.3f;

		if (m_LoadAni_fx < 29.5f)
		m_fAlpha = 0.f;
	}

	if (Is_MyName(L"Load_Item2") == true)
	{
		m_fY += (_float)_dTimeDelta * 45.f;
		if (m_fY >= -293.3f)
			m_fY = -293.3f;

		if (m_LoadAni_fx < -363.8f)
		m_fAlpha = 0.f;
	}

	Safe_Release(pGameInstance);
}

void CUI::Mode_Effect(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (Is_MyName(L"Mode_Eff") == true)
	{
		m_fAlpha = 0.9f;
		m_UVFlowy += (_float)_dTimeDelta* 0.05f;
		m_UVFlowx = sin(m_TimeAcc += (_float)_dTimeDelta * 0.05f);
	}

	if (Is_MyName(L"Mode_CloudR") == true)
	{
		if (m_fX >= 669.f)
			m_brepeat = true;

		if (m_fX <= 640.f)
			m_brepeat = false;

		if (m_brepeat == true)
			m_fX -= (_float)_dTimeDelta * 25.f;
		else
			m_fX += (_float)_dTimeDelta * 25.f;
	}

	if (Is_MyName(L"Mode_CloudL") == true)
	{
		if (m_fX <= -690.f)
			m_brepeat = true;

		if (m_fX >= -628.f)
			m_brepeat = false;

		if (m_brepeat == true)
			m_fX += (_float)_dTimeDelta * 25.f;
		else
			m_fX -= (_float)_dTimeDelta * 25.f;
	}

	if (Is_MyName(L"Mode_Bar") == true || Is_MyName(L"Mode_Title0") == true || Is_MyName(L"Mode_Title1") == true || Is_MyName(L"Mode_Cursor") == true
		|| Is_MyName(L"Mode_Text") == true || Is_MyName(L"Mode_Dia") == true)
	{
		if (pGameInstance->Get_DIKeyDown(DIK_UP) && m_MoadIndex != 0)
		{
			pGameInstance->Play_SoundW(TEXT("UI_Mouseover"), 0.2f);
			pGameInstance->Play_SoundW(TEXT("UI_Tanjiro"), 0.1f);
			m_MoadIndex--;
		}
		
		if (pGameInstance->Get_DIKeyDown(DIK_DOWN) && m_MoadIndex != 1)
		{
			pGameInstance->Play_SoundW(TEXT("UI_Mouseover"), 0.2f);
			pGameInstance->Play_SoundW(TEXT("UI_Nezuko"), 0.1f);
			m_MoadIndex++;
		}
		if (Is_MyName(L"Mode_Bar") == true)
		wsprintf(m_TextureName, TEXT("Mode_Bar%d"),m_MoadIndex);

		if (Is_MyName(L"Mode_Text") == true)
			wsprintf(m_TextureName, TEXT("Mode_Text%d"), m_MoadIndex);

		if (Is_MyName(L"Mode_Dia") == true)
			wsprintf(m_TextureName, TEXT("Mode_Dia%d"), m_MoadIndex);

		if (Is_MyName(L"Mode_Title0") == true)
		{
			if (m_MoadIndex == 0)
			{
				wsprintf(m_TextureName, TEXT("Mode_Select0"));
				m_fSizeX = 310.f;
				m_fSizeY = 110.f;
			}
			
			else
			{
				wsprintf(m_TextureName, TEXT("Mode_Title0"));
				m_fSizeX = 300.f;
				m_fSizeY = 100.f;
			}
		}

		if (Is_MyName(L"Mode_Title1") == true)
		{
			if (m_MoadIndex == 1)
				wsprintf(m_TextureName, TEXT("Mode_Select1"));
			else
				wsprintf(m_TextureName, TEXT("Mode_Title1"));
		}
		if (Is_MyName(L"Mode_Cursor") == true)
		{
			if (m_MoadIndex == 0)
				m_fY = 135.f;
			else
				m_fY = 25.f;
		}
	}

	if (Is_MyName(L"Mode_Char") == true)
	{
		if (pGameInstance->Get_DIKeyDown(DIK_UP) && m_MoadIndex != 0)
		{
			m_MoadIndex--;
			m_fAlpha = 0.f;
			m_ModeCharEff = true;
			m_fY = 500.f;
			m_ModeCharMove = true;
		}
		
		if (pGameInstance->Get_DIKeyDown(DIK_DOWN) && m_MoadIndex != 1)
		{
			m_MoadIndex++;
			m_fAlpha = 0.f;
			m_ModeCharEff = true;
			m_fY = 500.f;
			m_ModeCharMove = true;
			
		}
		wsprintf(m_TextureName, TEXT("Mode_Char%d"), m_MoadIndex);
	}
	
	if (Is_MyName(L"Mode_Char") == true && m_ModeCharEff == true)
	{
		m_fAlpha += (_float)_dTimeDelta * 0.7f;
		if (m_fAlpha >= 1.f)
		{
			m_fAlpha = 1.f;
			m_ModeCharEff = false;
		}
	}

	if (Is_MyName(L"Mode_Char") == true && m_ModeCharMove == true)
	{
		m_fY -= (_float)_dTimeDelta * 700.f;
		if (m_fY <= -1.1f)
		{
			m_fY = -1.1f;
			m_ModeCharMove = false;
		}
	}

	if (pGameInstance->Get_DIKeyDown(DIK_RETURN))
	{
		pGameInstance->Play_SoundW(TEXT("UI_Select"), 0.2f);
		m_bModeEffect = true;
	}
	
	if (m_bModeEffect == true)
	{
		m_fFrame += (_float)_dTimeDelta * 20.0f;

		if (29.0f <= m_fFrame)
		{
			//pGameInstance->Stop_All();
			//pGameInstance->Stop_Sound(CSound_Manager::CHANNELID::BGM);
			m_fFrame = 0.f;
			if (false == m_bModeEffect)
				pGameInstance->Stop_All();

			m_bModeStart = true;
			m_bModeEffect = false;
		//	pGameInstance->Stop_Sound(CSound_Manager::CHANNELID::BGM);
		}
		if (Is_MyName(L"Mode_SmokeEff") == true)
			wsprintf(m_TextureName, TEXT("Xcmn_Ink2_%d"), (_int)m_fFrame);
	}

	if (m_bModeStartEff == true && Is_MyName(L"Mode_StartEff") == true)
	{
		m_fFrame -= (_float)_dTimeDelta * 20.0f;
		if (0.f >= m_fFrame)
		{
			m_fFrame = 0.f;
			m_bModeStartEff = false;
		}
		
			wsprintf(m_TextureName, TEXT("Xcmn_Ink_%d"), (_int)m_fFrame);
	}
	
	Safe_Release(pGameInstance);
}

void CUI::Story_Text(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*> Prototype_NPCObjects = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_NPC);

	if (Prototype_NPCObjects.empty())
	{
		Safe_Release(pGameInstance);
		return;
	}

	CNPC* DeadCandidate = nullptr;
	for (auto it = Prototype_NPCObjects.begin(); it != Prototype_NPCObjects.end(); it++)
	{
		if (dynamic_cast<CNPC*>(*it)->Get_Type() == CNPC::NPC_DEAD_CANDIDATE)
		{
			DeadCandidate = dynamic_cast<CNPC*>(*it);
			break;
		}
	}

	if (DeadCandidate == nullptr)
	{
		Safe_Release(pGameInstance);
		return;
	}
	
	//카메라
	///CCamera_Free* camera = dynamic_cast<CCamera_Free*>(pGameInstance->Find_GameObject(LEVEL_STORY, LAYER_CAM, 0));
	CPlayer* Player = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_STORYPLAYER, 0));

	CTransform* cameraTransform = Player->Get_Transform();
	//카메라의 위치
	_float3 fCameraPos = cameraTransform->Get_PosF3();
	
	//NPC의 위치 
	CTransform* NPCTransform = DeadCandidate->Get_Transform();
	_float3 fNPCPos = NPCTransform->Get_PosF3();

	//NPC위치에서 카메라의 위치를 밴 벡터 
	_float3 fCameraToNpc = fNPCPos - fCameraPos;
	fCameraToNpc.y = 0.f;
	_vector vCameraToNpc = XMLoadFloat3(&fCameraToNpc);

	//거리 
	_vector vLength = XMVector3Length(vCameraToNpc);
	_float3 fNPCLengrh = { 0.f, 0.f, 0.f };
	XMStoreFloat3(&fNPCLengrh, vLength);

	
	
	//=================================================================================
	
	if (Is_MyName(L"Story_Text") == true || Is_MyName(L"Msg_Arrow") == true)
	{
		if (pGameInstance->Get_DIKeyDown(DIK_F) && m_fAlpha == 1.f && Story_TextureIndex != 4)
		{
			Story_TextureIndex++;
			if (Is_MyName(L"Story_Text") == true)
			{
				pGameInstance->Play_SoundW(TEXT("UI_Talk"), 0.1f);
				wsprintf(m_TextureName, TEXT("Story_Text%d"), Story_TextureIndex);
			}
		}

		if (Story_TextureIndex == 4)
			m_fAlpha = 0.f;
	}

	if (Is_MyName(L"Story_Text") == true || Is_MyName(L"Msg_Arrow") == true) //NPC 근처일때 
	{
		if (fNPCLengrh.x < 300.f && Story_NpcText == false)
		{
			wsprintf(m_TextureName, TEXT("Story_Text6"));
			m_fAlpha = 1.f;
			if (m_fAlpha == 1.f && pGameInstance->Get_DIKeyDown(DIK_F))
			{
				pGameInstance->Play_SoundW(TEXT("UI_Talk"), 0.1f);
				m_fAlpha = 0.f;
				Story_NpcText = true;
			}
		}
	}

	if (Is_MyName(L"Story_Text") == true || Is_MyName(L"Msg_Arrow") == true) //NPC를 발견했을때 
	{
		if (fNPCLengrh.x < 50.f && Story_NpcText2 == false)
		{
			wsprintf(m_TextureName, TEXT("Story_Text5"));
			m_fAlpha = 1.f;
			if (m_fAlpha == 1.f && pGameInstance->Get_DIKeyDown(DIK_F))
			{
				m_fAlpha = 0.f;
				Story_NpcText2 = true;
			}
		}
	}

	/////////////
	// 
	//막다른길 위치  -1661 ,158 ,1485

	_float3 fStopPos = _float3(-1804.699f, 156.767f, 1567.541f);

	//위치에서 카메라의 위치를 밴 벡터 
	_float3 fCameraToStop = fStopPos - fCameraPos;
	fCameraToStop.y = 0.f;
	_vector vCameraToStop = XMLoadFloat3(&fCameraToStop);

	//거리 
	_vector vStopLength = XMVector3Length(vCameraToStop);
	_float3 fStopLengrh = { 0.f, 0.f, 0.f };
	XMStoreFloat3(&fStopLengrh, vStopLength);


	if (Is_MyName(L"Story_Text") == true || Is_MyName(L"Msg_Arrow") == true) //막다른길
	{
		if (fStopLengrh.x < 40.f && Story_Stop == false)
		{
			wsprintf(m_TextureName, TEXT("Story_Text7"));
			m_fAlpha = 1.f;
			if (m_fAlpha == 1.f && pGameInstance->Get_DIKeyDown(DIK_F))
			{
				pGameInstance->Play_SoundW(TEXT("UI_Talk"), 0.1f);
				m_fAlpha = 0.f;
				Story_Stop = true;
			}
		}
	}

	//손혈귀 자국 위치 _float3(-1404.000f, 227.002f, 564.000f)

	_float3 fBossPos = _float3(-1404.000f, 227.002f, 564.000f);


	_float3 fCameraToBoss = fBossPos - fCameraPos;
	fCameraToBoss.y = 0.f;
	_vector vCameraToBoss = XMLoadFloat3(&fCameraToBoss);

	//거리 
	_vector vBossLength = XMVector3Length(vCameraToBoss);
	_float3 fBossLengrh = { 0.f, 0.f, 0.f };
	XMStoreFloat3(&fBossLengrh, vBossLength);

	if (Is_MyName(L"Story_Text") == true || Is_MyName(L"Msg_Arrow") == true) //손혈귀 자국 
	{
		if (fBossLengrh.x < 50.f && Story_Boss == false)
		{
			wsprintf(m_TextureName, TEXT("Story_Text8"));
			m_fAlpha = 1.f;
			if (m_fAlpha == 1.f && pGameInstance->Get_DIKeyDown(DIK_F))
			{
				pGameInstance->Play_SoundW(TEXT("UI_Talk"), 0.1f);
				m_fAlpha = 0.f;
				Story_Boss = true;
			}
		}
	}
	
	
	
	if (Is_MyName(L"Msg_Arrow") == true)
	{
		if (m_brepeat == false)
		{
			m_fY -= 1.f;
			if (m_fY <= -333.f)
				m_brepeat = true;
		}
		else
		{
			m_fY += 1.f;
			if (m_fY >= -323.f)
				m_brepeat = false;
		}
	}
	
	Safe_Release(pGameInstance);
}
void CUI::Rattan_Text(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*> Prototype_NPCObjects = pGameInstance->Find_GameObjectList(LEVEL_RATTAN, LAYER_NPC);

	if (Prototype_NPCObjects.empty())
	{
		Safe_Release(pGameInstance);
		return;
	}


	CCamera_Free* Camera = dynamic_cast<CCamera_Free*>(pGameInstance->Find_GameObject(LEVEL_RATTAN, LAYER_CAM, 0));

	if (Camera == nullptr)
	{
		Safe_Release(pGameInstance);
		return;
	}


#pragma region 아오이
	CNPC* Aoi = nullptr;
	for (auto it = Prototype_NPCObjects.begin(); it != Prototype_NPCObjects.end(); it++)
	{
		if (dynamic_cast<CNPC*>(*it)->Get_Type() == CNPC::NPC_AOI)
		{
			Aoi = dynamic_cast<CNPC*>(*it);
			break;
		}
	}

	if (Is_MyName(L"Rattan_Text") == true && Aoi->Get_Trigger() == CNPC::TURN)
	{
		m_fAlpha = 1.f;
		wsprintf(m_TextureName, TEXT("Rattan_Text%d"), 0);
		Camera->Set_Active(false);
	}

	if ((Is_MyName(L"Rattan_Select0") == true || Is_MyName(L"Rattan_Select1") == true) && Aoi->Get_Trigger() == CNPC::TURN)
	{
		m_fAlpha = 1.f;
		if (true == InterSect((_int)v2MousePos.x - (_int)g_iWinSizeX / 2, (_int)-v2MousePos.y + (_int)g_iWinSizeY / 2))
		{

			Set_Texture(L"Logo_Title_Select");
			if (SoundPlay == true)
			{
				pGameInstance->Play_SoundW(TEXT("UI_Mouseover"), 0.3f);
				SoundPlay = false;
			}
			if (CGameInstance::Get_Instance()->Get_DIMBtnDown(CInput_Device::DIMB_L))
			{
				Aoi->Change_State(CNPC::START);
				MiniGameStart = true;
			}
		}
		else
		{
			Set_Texture(L"Logo_Title_Base");
			SoundPlay = true;
		}
	}
	if (Is_MyName(L"Rattan_Select") == true && Aoi->Get_Trigger() == CNPC::TURN)
		m_fAlpha = 1.f;

#pragma endregion 

#pragma region 쌍둥이 자매 
	CNPC* Sister = nullptr;
	for (auto it = Prototype_NPCObjects.begin(); it != Prototype_NPCObjects.end(); it++)
	{
		if (dynamic_cast<CNPC*>(*it)->Get_Type() == CNPC::NPC_SISTER_R)
		{
			Sister = dynamic_cast<CNPC*>(*it);
			break;
		}
	}

	if (Is_MyName(L"Rattan_Text") == true && Sister->Get_Trigger() == CNPC::TALK && Rattan_TextRender == false)
	{
		m_fAlpha = 1.f;
		wsprintf(m_TextureName, TEXT("Rattan_Text%d"), 1);
		Rattan_TextRender = true;
	}
	if (Is_MyName(L"Rattan_Text") == true && Rattan_TextRender == true && pGameInstance->Get_DIKeyUp(DIK_F))
	{
		m_fAlpha = 1.f;
		wsprintf(m_TextureName, TEXT("Rattan_Text%d"), 2);
	}

	list<CGameObject*> Prototype_UIObjects = pGameInstance->Find_GameObjectList(LEVEL_RATTAN, LAYER_UI);

	if (Prototype_UIObjects.empty())
	{
		Safe_Release(pGameInstance);
		return;
	}

	CUI* RattanText = nullptr;
	for (auto it = Prototype_UIObjects.begin(); it != Prototype_UIObjects.end(); it++)
	{
		if (dynamic_cast<CUI*>(*it)->Is_MyName(L"Rattan_Text") == true)
		{
			RattanText = dynamic_cast<CUI*>(*it);
			break;
		}
	}

	if (Is_MyName(L"Rattan_SelectMain0") == true || Is_MyName(L"Rattan_SelectMain1") == true || Is_MyName(L"Rattan_BaseDeco0") == true || Is_MyName(L"Rattan_BaseDeco1") == true
		|| Is_MyName(L"Rattan_SelectBase") == true)
	{
	
		if (RattanText != nullptr && RattanText->Is_MyTexture(L"Rattan_Text2"))
		{
			m_fAlpha = 1.f;
			Camera->Set_Active(false);
		}
	}

	//첫번째 선택지 준비되었다
	if (Is_MyName(L"Rattan_SelectMain0") == true)
	{
		if (RattanText->Is_MyTexture(L"Rattan_Text2") && true == InterSect((_int)v2MousePos.x - (_int)g_iWinSizeX / 2, (_int)-v2MousePos.y + (_int)g_iWinSizeY / 2))
		{
			Set_Texture(L"CheckMark0");
			if (SoundPlay == true)
			{
				pGameInstance->Play_SoundW(TEXT("UI_Mouseover"), 0.3f);
				SoundPlay = false;
			}
			if (CGameInstance::Get_Instance()->Get_DIMBtnDown(CInput_Device::DIMB_L))
			{
				pGameInstance->Play_SoundW(TEXT("UI_RattanSelect"), 0.3f);
				StoryStart = true;
			}
		}
		else
		{
			Set_Texture(L"CheckMark1");
			SoundPlay = true;
		}
	}

	//두번째 선택지 준비되지않았다 
	if (Is_MyName(L"Rattan_SelectMain1") == true)
	{
		if (RattanText->Is_MyTexture(L"Rattan_Text2") && true == InterSect((_int)v2MousePos.x - (_int)g_iWinSizeX / 2, (_int)-v2MousePos.y + (_int)g_iWinSizeY / 2))
		{
			Set_Texture(L"CheckMark0");
			if (SoundPlay == true)
			{
				pGameInstance->Play_SoundW(TEXT("UI_Mouseover"), 0.3f);
				SoundPlay = false;
			}
		}
		
		else
		{
			Set_Texture(L"CheckMark1");
			SoundPlay = true;
		}
	}
	if (Is_MyName(L"Rattan_Chapter") == true)
	{
		list<CGameObject*> Prototype_GameObjects = pGameInstance->Find_GameObjectList(LEVEL_RATTAN, LAYER_UI);

		CUI* Storygame = nullptr;
		for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
		{
			if (dynamic_cast<CUI*>(*it)->Is_MyName(L"Rattan_SelectMain0") == true)
			{
				Storygame = dynamic_cast<CUI*>(*it);
				break;
			}
		}
		if (Storygame->Get_StoryStart() == true)//최종선별 챕터 
		{
			RattanTime += _dTimeDelta;
			m_fAlpha = 1.f;
			if (RattanTime > 1.0)
			{
				m_MoadIndex++;
				RattanTime = 0.0;
			}
			if(m_MoadIndex >= 3)
				StoryStart = true;
		}
	}

#pragma endregion 

#pragma region 젠이츠

	CNPC* zenitsu = nullptr;
	for (auto it = Prototype_NPCObjects.begin(); it != Prototype_NPCObjects.end(); it++)
	{
		if (dynamic_cast<CNPC*>(*it)->Get_Type() == CNPC::NPC_ZENITSU)
		{
			zenitsu = dynamic_cast<CNPC*>(*it);
			break;
		}
	}

	if (Is_MyName(L"Rattan_Text") == true && zenitsu->Get_Trigger() == CNPC::STAND) //대화시작
	{
		m_fAlpha = 1.f;
		wsprintf(m_TextureName, TEXT("Rattan_Text%d"), Rattan_ZenitsuText);
		Camera->Set_Active(false);
	}
	if (Is_MyName(L"Rattan_Text") == true && pGameInstance->Get_DIKeyUp(DIK_F) && Rattan_ZenitsuText != 6 && zenitsu->Get_Trigger() == CNPC::STAND) //대화넘김
	{
		pGameInstance->Play_SoundW(TEXT("UI_Talk"), 0.1f);
		if (m_fAlpha == 1.f)
			++Rattan_ZenitsuText;
	}
	if (Is_MyName(L"Rattan_Text") == true && Rattan_ZenitsuText == 6 && zenitsu->Get_Trigger() != CNPC::SCARY) //대화 끝 
	{
		m_fAlpha = 0.f;
		zenitsu->Change_State(CNPC::SCARY);
		Camera->Set_Active(true);
		
	}
#pragma endregion 

#pragma region 카나오
	CNPC* Kanawo = nullptr;
	for (auto it = Prototype_NPCObjects.begin(); it != Prototype_NPCObjects.end(); it++)
	{
		if (dynamic_cast<CNPC*>(*it)->Get_Type() == CNPC::NPC_KANAWO)
		{
			Kanawo = dynamic_cast<CNPC*>(*it);
			break;
		}
	}

	if (Is_MyName(L"Rattan_Text") == true && Kanawo->Get_Trigger() == CNPC::TALK && KanawoRender == false)
	{
		m_fAlpha = 1.f;
		wsprintf(m_TextureName, TEXT("Rattan_Text%d"), 7);
		Camera->Set_Active(false);
		KanawoRender = true;
		Kanawo->Change_State(CNPC::TRIGGER_END);
	}
	if (Is_MyName(L"Rattan_Text") == true && pGameInstance->Get_DIKeyUp(DIK_F) && KanawoRender == true && Kanawo->Get_Trigger() == CNPC::TRIGGER_END)
	{
		pGameInstance->Play_SoundW(TEXT("UI_Talk"), 0.1f);
		m_fAlpha = 0.f;
		Camera->Set_Active(true);
		Kanawo->Change_State(CNPC::TALK_END);
	}

#pragma endregion


#pragma region 견습대원
	CNPC* Candidate = nullptr;
	for (auto it = Prototype_NPCObjects.begin(); it != Prototype_NPCObjects.end(); it++)
	{
		if (dynamic_cast<CNPC*>(*it)->Get_Type() == CNPC::RATTAN_CANDIDATE)
		{
			Candidate = dynamic_cast<CNPC*>(*it);
			break;
		}
	}

	if (Is_MyName(L"Rattan_Text") == true && Candidate->Get_Trigger() == CNPC::TALK && CandidateRender == false)
	{
		m_fAlpha = 1.f;
		wsprintf(m_TextureName, TEXT("Rattan_Text%d"), 8);
		Camera->Set_Active(false);
		CandidateRender = true;
		Candidate->Change_State(CNPC::TRIGGER_END);
	}
	if (Is_MyName(L"Rattan_Text") == true && pGameInstance->Get_DIKeyUp(DIK_F) && CandidateRender == true && Candidate->Get_Trigger() == CNPC::TRIGGER_END)
	{
		pGameInstance->Play_SoundW(TEXT("UI_Talk"), 0.1f);
		m_fAlpha = 0.f;
		Camera->Set_Active(true);
		Candidate->Change_State(CNPC::TALK_END);
	}

#pragma endregion

#pragma region 겐야
	CNPC* Genya = nullptr;
	for (auto it = Prototype_NPCObjects.begin(); it != Prototype_NPCObjects.end(); it++)
	{
		if (dynamic_cast<CNPC*>(*it)->Get_Type() == CNPC::NPC_GENYA)
		{
			Genya = dynamic_cast<CNPC*>(*it);
			break;
		}
	}

	if (Is_MyName(L"Rattan_Text") == true && Genya->Get_Trigger() == CNPC::TALK && GanyaRender == false)
	{
		m_fAlpha = 1.f;
		wsprintf(m_TextureName, TEXT("Rattan_Text%d"), 9);
		Camera->Set_Active(false);
		GanyaRender = true;
		Genya->Change_State(CNPC::TRIGGER_END);
	}
	if (Is_MyName(L"Rattan_Text") == true && pGameInstance->Get_DIKeyUp(DIK_F) && GanyaRender == true && Genya->Get_Trigger() == CNPC::TRIGGER_END)
	{
		pGameInstance->Play_SoundW(TEXT("UI_Talk"), 0.1f);
		m_fAlpha = 0.f;
		Camera->Set_Active(true);
		Genya->Change_State(CNPC::TALK_END);
	}

#pragma endregion
	Safe_Release(pGameInstance);


}
void CUI::Eednig_Effect(const _double& _dTimeDelta)
{
	//TimeAcc를 사용 
	EndingTime += _dTimeDelta;
	if (EndingTime >= 1)
	{
		Flower_MinusY = 1.f;
		Text_PlusY = 2.f;
		EndingTime = 0.0;
	}
	
	//내려오는 꽃 
	if (Is_MyName(L"End_Flower") == true)
	{
		if(m_fY >= 215.f)
			m_fY -= Flower_MinusY;
	}

	//백그라운드 UV흐르게 
	if (Is_MyName(L"End_BackEff") == true)
	{
		m_UVFlowy += (_float)_dTimeDelta * 0.04f;
		m_UVFlowx = sin(m_TimeAcc += (_float)_dTimeDelta * 0.04f);
	}

	//글자들이 올라오자 
	if (Is_MyName(L"End_Text") == true)
	{
		if (m_fY <= 495.f)
			m_fY += Text_PlusY;

		if (m_fY >= 495.f)
		{
			if (Text_Index <= 5)
				Text_Index++;
			else if (Text_Index == 6)
				Text_Index = 0;
			
			m_fY = -316.f;
		}
		
		wsprintf(m_TextureName, TEXT("End_Text%d"), Text_Index);
	}

	//꽃가루가 흩날리자 
	if (Is_MyName(L"End_Eff") == true)
	{
			if (m_fY >= -533.f)
			{
				m_fY -= 1.f;
				m_fX -= 1.5f;
			}

			else if (m_fY <= -533.f)
			{
				m_fX = 931.f;
				m_fY = 576.f;
			}
	}
}

void CUI::UI_Sound()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (g_iCurrentLevel == LEVEL_LOGO && Is_MyName(L"Select1") == true)
	{
		pGameInstance->Play_BGM(TEXT("UI_LogoBgm"), 0.4f);
		pGameInstance->Play_SoundW(TEXT("UI_Title"), 0.2f);
		pGameInstance->Play_SoundW(TEXT("UI_LogoStart"), 0.2f);
	}

	else if (g_iCurrentLevel == LEVEL_MODE && Is_MyName(L"Mode_Eff") == true)
		pGameInstance->Play_BGM(TEXT("UI_ModeBGM"), 0.4f);

	else if (g_iCurrentLevel == LEVEL_RATTAN && Is_MyName(L"Rattan_Text") == true)
		pGameInstance->Play_BGM(TEXT("UI_RattanBGM"), 0.4f);

	else if (g_iCurrentLevel == LEVEL_STORY && Is_MyName(L"Story_Text") == true)
		pGameInstance->Play_BGM(TEXT("UI_Ambiant"), 0.1f);

	else if(g_iCurrentLevel == LEVEL_ENDING && Is_MyName(L"End_Flower") == true)
		pGameInstance->Play_BGM(TEXT("UI_EndingBGM"), 0.9f);

		
	Safe_Release(pGameInstance);
}

HRESULT CUI::SetUp_Components()
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

HRESULT CUI::SetUp_ConstantTable()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_UVFlow", &m_UVFlow, sizeof(_float))))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_UVFlowy", &m_UVFlowy, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_UVFlowx", &m_UVFlowx, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DiffuseTexture", m_TextureName)))
			return E_FAIL;

	return S_OK;
}

CUI* CUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI* pInstance = new CUI(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI_Base* CUI::Clone(void* pArg)
{
	CUI* pInstance = new CUI(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI::Free()
{

	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
