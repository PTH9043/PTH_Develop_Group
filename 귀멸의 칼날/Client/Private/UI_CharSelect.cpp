#include "UI_CharSelect.h"
#include "GameInstance.h"
#include "UI_Select_Model.h"


CUI_CharSelect::CUI_CharSelect(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CUI_Base(_pDevice, _pContext)
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());

}

CUI_CharSelect::CUI_CharSelect(const CUI_CharSelect& _rhs)
	: CUI_Base(_rhs)
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

HRESULT CUI_CharSelect::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CUI_CharSelect::NativeConstruct_Clone(void* pArg)
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
		m_fX = 100.f;
		m_fY = 100.f;
		m_fSizeX = 300.f;
		m_fSizeY = 300.f;
		m_Depth = 1;
	}

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	if (Is_MyName(L"Char_Select1P") == true || Is_MyName(L"Char_Select2P") == true)
		m_fAlpha = 0.f;
	
	if (Is_MyName(L"MapSelect") == true || Is_MyName(L"MapName") == true || Is_MyName(L"MapCursor") == true || Is_MyName(L"MapTitle") == true && Is_MyName(L"MapText") == true)
		m_fAlpha = 0.f;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	
	if (g_iCurrentLevel == LEVEL_SELECT && Is_MyName(L"Char_Select1P") == true)
	{
		pGameInstance->Play_BGM(TEXT("UI_SelectBGM"),0.4f);
	}

	Safe_Release(pGameInstance);
	
	return S_OK;
}

void CUI_CharSelect::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_Pos(XMVectorSet(m_fX, m_fY, 0.0f, 1.f));
	
	if (Is_MyName(L"Char_WindowL") == true || Is_MyName(L"Char_WindowR") == true || Is_MyName(L"Char_WIndowBase") == true)
	{
		if (m_fSizeX / 2.f >= abs(m_fX))
		{
			Smaller_Effect(m_fSpeed, m_fEndTime, _dTimeDelta);
		}
	}
	
	if (Is_MyName(L"Char_1PCloud1") == true || Is_MyName(L"Char_1PCloud2") == true || Is_MyName(L"Char_1PCloud3") == true || Is_MyName(L"Char_Icon1P") == true ||
		Is_MyName(L"Char_2PCloud1") == true || Is_MyName(L"Char_2PCloud2") == true || Is_MyName(L"Char_2PCloud3") == true || Is_MyName(L"Char_Icon2P") == true ||
		Is_MyName(L"Char_Title") == true    || Is_MyName(L"Char_Icon0") == true    || Is_MyName(L"Char_Icon1") == true    || Is_MyName(L"Char_Icon2") == true  ||
		Is_MyName(L"Char_Icon3") == true || Is_MyName(L"Char_Icon4") == true || Is_MyName(L"Char_Name") == true || Is_MyName(L"Char_Name0") == true || Is_MyName(L"Char_Cursor") == true ||
		Is_MyName(L"Char_Cursor2") == true || Is_MyName(L"Char_2PName") == true || Is_MyName(L"Char_Name1") == true)
		Cloud_Effect(_dTimeDelta);
	
	if (Is_MyName(L"Char_Eff") == true)
	{
		m_fAlpha = 0.9f;
		m_UVFlowy += (_float)_dTimeDelta * 0.04f;
		m_UVFlowx = sin(m_TimeAcc += (_float)_dTimeDelta * 0.04f);
	}


	if (Is_MyName(L"Char_Eff3") == true)
	{
		m_fAlpha = 0.85f;
		m_UVFlowy += (_float)_dTimeDelta * 0.04f;
		m_UVFlowx = sin(m_TimeAcc += (_float)_dTimeDelta * 0.04f);
	}

	if (Is_MyName(L"Char_Eff2") == true)
	{
		m_Depth = 2;
		L_Diagonal(_float4(1016.8f, 800.2f, 1000.f, 1000.f), 1.0f, 1.f, 45.f, true, _dTimeDelta);
	}

	Select_Char(_dTimeDelta);
	Select_Save();
	Select_Map(_dTimeDelta);
}
void CUI_CharSelect::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (Is_MyName(L"Char_Name") == true || Is_MyName(L"Char_Name0") == true
		|| Is_MyName(L"Char_2PName") == true || Is_MyName(L"Char_Name1") == true
		|| Is_MyName(L"Char_Select1P") == true || Is_MyName(L"Char_Select2P") == true)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
	else
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);

}

HRESULT CUI_CharSelect::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (Is_MyName(L"Char_Eff") == true)
		m_pShaderCom->Begin(9);
	
	else if (Is_MyName(L"Char_Eff3") == true)
		m_pShaderCom->Begin(11);

	else if(Is_MyName(L"Char_Name") == true || Is_MyName(L"Char_Name0") == true
		|| Is_MyName(L"Char_2PName") == true || Is_MyName(L"Char_Name1") == true
		|| Is_MyName(L"Char_Select1P") == true || Is_MyName(L"Char_Select2P") == true)
		m_pShaderCom->Begin(1);
		
	else
	m_pShaderCom->Begin(10);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CUI_CharSelect::Smaller_Effect(_float speed, _float endtime, _double timedelta)
{
	if (0.f > m_fEndTime) return;
	
	if (Is_MyName(L"Char_WIndowBase") == true)
	{
			m_fSizeX = 800.f;
			m_fSizeY = 800.f;
			m_fAlpha = 1.0f;
	}
	
	else
	{
		m_fEndTime -= (_float)timedelta;

		m_fSizeX -= speed * (_float)timedelta * (m_fSizeX / m_fSizeY);
		m_fSizeY -= speed * (_float)timedelta;
		if (0.f > m_fX)
			m_fX += speed * (_float)timedelta * (m_fSizeX / m_fSizeY) / 2.f;
		else
			m_fX -= speed * (_float)timedelta * (m_fSizeX / m_fSizeY) / 2.f;
	}
		
}

void CUI_CharSelect::Cloud_Effect( _double timedelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	CloudEffect += (_float)timedelta * 0.8f;
	if (CloudEffect <= 2.0f)/////////////////////////////////
		m_fAlpha = 0.f;
	else
		Plus_Alpha(0.f, 0.5f, 1.5f, timedelta);
	
	if (m_fAlpha >= 1.f)
	{
		CUI_Select_Model* Model1P = dynamic_cast<CUI_Select_Model*>(pGameInstance->Find_GameObject(LEVEL_SELECT, LAYER_UIMODEL, 0));
		Model1P->Set_Render(true);
		CUI_Select_Model* Model2P = dynamic_cast<CUI_Select_Model*>(pGameInstance->Find_GameObject(LEVEL_SELECT, LAYER_UIMODEL,1));
		Model2P->Set_Render(true);
	}
	
	RELEASE_INSTANCE(CGameInstance);
}

void CUI_CharSelect::Select_Char(_double timedelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	//if (1P이고 선택이 안됬어)

	//1P캐릭터 선택 이동
	if (pGameInstance->Get_DIKeyUp(DIK_A) && m_P1_Select != true)
	{
		if (m_P1_Index != 0)
		{
			m_P1_Index--;
			//m_ChangeModel = true;
			if (Is_MyName(L"Char_Cursor") == true)
			{
				m_ChangeModel = true;
				m_fX -= 96.f;
				pGameInstance->Play_SoundW(TEXT("UI_Mouseover"), 0.2f);
				
			}
		}
	}
	
	if (pGameInstance->Get_DIKeyUp(DIK_D) && m_P1_Select != true)
	{
		if (m_P1_Index != 3)
		{
			m_P1_Index++;
			//m_ChangeModel = true;
			if (Is_MyName(L"Char_Cursor") == true)
			{
				m_ChangeModel = true;
				m_fX += 96.f;
				pGameInstance->Play_SoundW(TEXT("UI_Mouseover"), 0.2f);
				
			}
		}
	}

	//2P캐릭터 선택 이동
	if (pGameInstance->Get_DIKeyUp(DIK_LEFT) && m_P2_Select != true)
	{
		if (m_P2_Index != 0)
		{
			m_P2_Index--;
			//m_ChangeModel = true;
			if (Is_MyName(L"Char_Cursor2") == true)
			{
				m_ChangeModel = true;
				m_fX -= 96.f;
				pGameInstance->Play_SoundW(TEXT("UI_Mouseover"), 0.2f);
				
			}
		}
	}

	if (pGameInstance->Get_DIKeyUp(DIK_RIGHT) && m_P2_Select != true)
	{
		if (m_P2_Index != 3)
		{
			m_P2_Index++;
			//m_ChangeModel = true;
			if (Is_MyName(L"Char_Cursor2") == true)
			{
				m_ChangeModel = true;
				m_fX += 96.f;
				pGameInstance->Play_SoundW(TEXT("UI_Mouseover"), 0.2f);
				
			}
		}
	}

	if (m_P1_Index > 3 || m_P2_Index > 4)
	{
		m_P1_Index = 3;
		m_P2_Index = 3;
	}
	if (m_P1_Index < 0 || m_P2_Index < 0)
	{
		m_P1_Index = 0;
		m_P2_Index = 0;
	}

	//같은 캐릭터를 선택했을 경우
	if (m_P1_Index == m_P2_Index)
	{
		if (Is_MyName(L"Char_Cursor") == true)
			Set_Texture(L"Char_Cursor3");
	   if (Is_MyName(L"Char_Cursor2") == true)
		   Set_Texture(L"Char_Cursor3");
    }
	else
	{
		if (Is_MyName(L"Char_Cursor") == true)
			Set_Texture(L"Char_Cursor1");
			//m_TextureName = L"Char_Cursor1";
		if (Is_MyName(L"Char_Cursor2") == true)
			Set_Texture(L"Char_Cursor2");
			//m_TextureName = L"Char_Cursor2";
	}
	
	//======================================================
	// 
	//1P캐릭터 결정
	if (pGameInstance->Get_DIKeyUp(DIK_LSHIFT))
	{

			if (m_P1_Select != true)
			{
				m_P1_Select = true;
				if (Is_MyName(L"Char_Select1P") == true)
				{
					m_fAlpha = 1.f;
					m_bEffect = true;
					
					pGameInstance->Play_SoundW(TEXT("UI_CharSelect"), 0.4f);
					
					if(m_P1_Index == 0)
					 pGameInstance->Play_SoundW(TEXT("UI_Char_Tanjiro"), 1.0f);
					else if (m_P1_Index == 1)
						pGameInstance->Play_SoundW(TEXT("UI_Char_Zenitsu"), 0.7f);
					else if (m_P1_Index == 2)
						pGameInstance->Play_SoundW(TEXT("UI_Char_Sabito"), 0.7f);
					else if (m_P1_Index == 3)
						pGameInstance->Play_SoundW(TEXT("UI_Char_Akaza"), 0.7f);
					//else if (m_P1_Index == 4)
					//	pGameInstance->Play_SoundW(TEXT("UI_Char_Akaza"), 0.7f);
				}
			}
			else
			{
				m_P1_Select = false;
				if (Is_MyName(L"Char_Select1P") == true)
				m_fAlpha = 0.f;
			}
	}
	
	if (Is_MyName(L"Char_Select1P") == true)
	{
		if (m_bEffect == true)
		Grow_Combo(7.f, 150.f, 0.f, 0.f, timedelta);
		else
		{
			m_fSizeX = 100.f;
			m_fSizeY = 100.f;
		}
	}

	//2P캐릭터 결정
	if (pGameInstance->Get_DIKeyUp(DIK_RSHIFT))
	{

		if (m_P2_Select != true)
		{
			m_P2_Select = true;
			if (Is_MyName(L"Char_Select2P") == true)
			{
				m_fAlpha = 1.f;
				m_bEffect = true;
				pGameInstance->Play_SoundW(TEXT("UI_CharSelect"), 0.4f);

				if (m_P2_Index == 0)
					pGameInstance->Play_SoundW(TEXT("UI_Char_Tanjiro"), 1.0f);
				else if (m_P2_Index == 1)
					pGameInstance->Play_SoundW(TEXT("UI_Char_Zenitsu"), 0.7f);
				else if (m_P2_Index == 2)
					pGameInstance->Play_SoundW(TEXT("UI_Char_Sabito"), 0.7f);
				else if (m_P2_Index == 3)
					pGameInstance->Play_SoundW(TEXT("UI_Char_Akaza"), 0.7f);
			//	else if (m_P2_Index == 4)
					//pGameInstance->Play_SoundW(TEXT("UI_Char_Akaza"), 0.7f);
				
			}
		}
		else
		{
			m_P2_Select = false;
			if (Is_MyName(L"Char_Select2P") == true)
				m_fAlpha = 0.f;
		}
	}
	
	if (Is_MyName(L"Char_Select2P") == true)
	{
		if (m_bEffect == true)
			Grow_Combo(7.f, 150.f, 0.f, 0.f, timedelta);
		else
		{
			m_fSizeX = 100.f;
			m_fSizeY = 100.f;
		}
	}

	//1P캐릭터 이름 변경
	if(Is_MyName(L"Char_Name0") == true)
		wsprintf(m_TextureName, TEXT("Char_Name%d"), m_P1_Index);

	//2P캐릭터 이름 변경
	if (Is_MyName(L"Char_Name1") == true)
		wsprintf(m_TextureName, TEXT("Char_Name%d"), m_P2_Index);

	 CUI_Select_Model* Model1P = dynamic_cast<CUI_Select_Model*>(pGameInstance->Find_GameObject(LEVEL_SELECT, LAYER_UIMODEL,0));
	
	 CUI_Select_Model* Model2P = dynamic_cast<CUI_Select_Model*>(pGameInstance->Find_GameObject(LEVEL_SELECT, LAYER_UIMODEL,1));

	 if (Is_MyName(L"Char_Cursor") == true && m_ChangeModel == true)
	 {
		 Model1P->Change_Model(m_P1_Index);
		 //Model2P->Change_Model(m_P2_Index);
		 m_ChangeModel = false;
	 }

	 if (Is_MyName(L"Char_Cursor2") == true && m_ChangeModel == true)
	 {
		// Model1P->Change_Model(m_P1_Index);
		 Model2P->Change_Model(m_P2_Index);
		 m_ChangeModel = false;
	 }
		
	RELEASE_INSTANCE(CGameInstance);
}

void CUI_CharSelect::Select_Map(_double timedelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	list<CGameObject*> Prototype_GameObjects = pGameInstance->Find_GameObjectList(LEVEL_SELECT, LAYER_UI);

	CUI_CharSelect* Title = nullptr;

	CUI_CharSelect* Cursor = nullptr;

	for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
	{
		if (dynamic_cast<CUI_Base*>(*it)->Is_MyName(L"MapTitle") == true)
		{
			Title = dynamic_cast<CUI_CharSelect*>(*it);
			break;
		}
	}

	for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
	{
		if (dynamic_cast<CUI_Base*>(*it)->Is_MyName(L"MapCursor") == true)
		{
			Cursor = dynamic_cast<CUI_CharSelect*>(*it);
			break;
		}
	}


	if (Title == nullptr)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (Cursor == nullptr)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	
	if (Is_MyName(L"MapSelect") == true || Is_MyName(L"MapName") == true || Is_MyName(L"MapCursor") == true  || Is_MyName(L"MapText") == true)
	{
		if (m_P1_Select == true && m_P2_Select == true)
			m_fAlpha = 1.f;
		else
			m_fAlpha = 0.f;
	}

	if (Is_MyName(L"MapCursor") == true && m_P1_Select == true && m_P2_Select == true)
	{
		if (Is_MyName(L"MapCursor") == true)
		{
			if (pGameInstance->Get_DIKeyUp(DIK_LEFT) && m_Map_Index != 0 && Title->m_Map_Select != true)
			{
				pGameInstance->Play_SoundW(TEXT("UI_Mouseover"), 0.4f);
				//m_fX -= 31.f;
				m_Map_Index--;
			}

			if (pGameInstance->Get_DIKeyUp(DIK_RIGHT) && m_Map_Index != 2 && Title->m_Map_Select != true)
			{
				pGameInstance->Play_SoundW(TEXT("UI_Mouseover"), 0.4f);
				//m_fX += 31.f;
				m_Map_Index++;
			}
			if (m_Map_Index == 0)
				m_fX = -131.7f;
			else if(m_Map_Index == 1)
				m_fX = -0.7f;
			else if (m_Map_Index == 2)
				m_fX = 132.7f;
		}
	}

		if (Is_MyName(L"MapTitle") == true)
		{
			if (m_Map_Select == false && pGameInstance->Get_DIKeyUp(DIK_RETURN))
			{
				pGameInstance->Play_SoundW(TEXT("UI_CharSelect"), 0.4f);
				m_fAlpha = 1.f;
				m_bEffect = true;
				m_Map_Select = true;
				m_StartTime = true;
			}
			else if (m_Map_Select == true && pGameInstance->Get_DIKeyUp(DIK_RETURN))
			{
				m_fAlpha = 0.f;
				m_bEffect = false;
				m_Map_Select = false;

				m_StartTime = false;
				m_StartTimeAcc = 0.0;
			}
			
			if (m_bEffect == true)
				Grow_Combo(10.f, 500.f, 0.f, 0.f, timedelta);
			else
			{
				m_fSizeX = 400.f;
				m_fSizeY = 400.f;
				
			}

			if (m_StartTime == true)
				m_StartTimeAcc += timedelta;
			if (m_StartTimeAcc > 2)
				m_StartBattle = true;
		}

		if (Is_MyName(L"MapText") == true)
			wsprintf(m_TextureName, TEXT("MapSelectText%d"), Cursor->m_Map_Index);

	RELEASE_INSTANCE(CGameInstance);
}

void CUI_CharSelect::Select_Save()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	list<CGameObject*> Prototype_GameObjects = pGameInstance->Find_GameObjectList(LEVEL_SELECT, LAYER_UI);


	CUI_CharSelect* Cursor = nullptr;
	CUI_CharSelect* Player1 = nullptr;
	CUI_CharSelect* Player2 = nullptr;
	for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
	{
		if (dynamic_cast<CUI_Base*>(*it)->Is_MyName(L"MapCursor") == true)
		{
			Cursor = dynamic_cast<CUI_CharSelect*>(*it);
		}
		else if (dynamic_cast<CUI_Base*>(*it)->Is_MyName(L"Char_Cursor") == true)
		{
			Player1 = dynamic_cast<CUI_CharSelect*>(*it);
		}
		else if (dynamic_cast<CUI_Base*>(*it)->Is_MyName(L"Char_Cursor2") == true)
		{
			Player2 = dynamic_cast<CUI_CharSelect*>(*it);
		}
		if (Player1 != nullptr && Player2 != nullptr && Cursor != nullptr)
			break;
	}

	
	
		if (Player1->m_P1_Select == true && Player2->m_P2_Select == true && m_Map_Select == true)
		{
			HANDLE    hFile = CreateFile(TEXT("../../Resources/Data/Select_Index.dat"), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

			if (INVALID_HANDLE_VALUE == hFile)
				return;

			DWORD    dwByte = 0;

			SelectInfo info;
			info.PlayerIndex = Player1->m_P1_Index;
			info.AIIndex = Player2->m_P2_Index;
			info.MapIndex = Cursor->m_Map_Index;

			WriteFile(hFile, &info, sizeof(SelectInfo), &dwByte, nullptr);

			CloseHandle(hFile);
		}
	

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CUI_CharSelect::SetUp_Components()
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

HRESULT CUI_CharSelect::SetUp_ConstantTable()
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
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_UVFlowy", &m_UVFlowy, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_UVFlowx", &m_UVFlowx, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DiffuseTexture", m_TextureName)))
		return E_FAIL;


	return S_OK;
}

CUI_CharSelect* CUI_CharSelect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_CharSelect* pInstance = new CUI_CharSelect(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_CharSelect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI_Base* CUI_CharSelect::Clone(void* pArg)
{
	CUI_CharSelect* pInstance = new CUI_CharSelect(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_CharSelect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CharSelect::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
