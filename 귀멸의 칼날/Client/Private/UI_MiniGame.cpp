#include "UI_MiniGame.h"
#include "GameInstance.h"
#include "MiniGame_Note.h"

CUI_MiniGame::CUI_MiniGame(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CUI_Base(_pDevice, _pContext)
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

CUI_MiniGame::CUI_MiniGame(const CUI_MiniGame& _rhs)
	: CUI_Base(_rhs)
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

HRESULT CUI_MiniGame::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CUI_MiniGame::NativeConstruct_Clone(void* pArg)
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

	Setting_Postion();

	if (Is_MyName(L"Mini_Combo") == true)
		m_fAlpha = 0.f;

	if (Is_MyName(L"Mini_Dia") == true)
		m_fAlpha = 0.f;

	return S_OK;
}

void CUI_MiniGame::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_Pos(XMVectorSet(m_fX, m_fY, 0.0f, 1.f));
	
	if (Is_MyName(L"Mini_Result") == true)
	{
		if (Result_Eff == true)
		{
			m_fAlpha = 1.f;
			Result_time += _dTimeDelta;
			if (Result_time > 0.3f)
			{
				m_fAlpha = 0.f;
				Result_time = 0;
				Result_Eff = false;
			}
		}
	}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//사운드
	if (Is_MyName(L"Mini_Start") == true)
	{
		if (m_StartNumber == 3 && SoundPlay == false)
		{
			pGameInstance->Play_SoundW(TEXT("UI_Three"), 0.5f);
			SoundPlay = true;
		}
		else if (m_StartNumber == 2 && SoundPlay == true)
		{
			pGameInstance->Play_SoundW(TEXT("UI_Two"), 0.5f);
			SoundPlay = false;
		}
		else if (m_StartNumber == 1 && SoundPlay == false)
		{
			pGameInstance->Play_SoundW(TEXT("UI_One"), 0.5f);
			SoundPlay = true;
		}
	}

	Safe_Release(pGameInstance);
	Count_Number(_dTimeDelta);
	Gauge(_dTimeDelta);
	KeyButton(_dTimeDelta);
	Combo();
	Dialog(_dTimeDelta);
}
	
void CUI_MiniGame::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

}

HRESULT CUI_MiniGame::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (Is_MyName(L"Mini_Bar") == true)
		m_pShaderCom->Begin(12);

	else if (Is_MyName(L"Mini_Line0") == true || Is_MyName(L"Mini_Line1") == true ||
		Is_MyName(L"Mini_Line2") == true || Is_MyName(L"Mini_Line3") == true)
		m_pShaderCom->Begin(13);

	else
		m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (Is_MyName(L"Mini_Bar") == true)
	{
		wsprintf(m_szFONTS, TEXT("%d"), m_Score);
		pGameInstance->Render_Font(TEXT("Font_Maru18"), m_szFONTS, _float2(238.5f, 86.f), XMVectorSet(255.f, 255.f, 255.f, 1.f));
	}
	Safe_Release(pGameInstance);

	return S_OK;
}

void CUI_MiniGame::Extern_ImGuiClass()
{


	if (ImGui::DragFloat("ScaleX", &m_fSizeX, 0.1f, -1000.f, 1000.f))
	{
	}
	if (ImGui::DragFloat("ScaleY", &m_fSizeY, 0.1f, -1000.f, 1000.f))
	{
	}
	if (ImGui::DragFloat("fX", &m_fX, 0.1f, -1000.f, 1000.f))
	{
	}
	if (ImGui::DragFloat("fY", &m_fY, 0.1f, -1000.f, 1000.f))
	{
	}

}

void CUI_MiniGame::Setting_Postion()
{
	//초기 세팅 
	if (Is_MyName(L"Mini_Line0") == true)
	{
		m_fX = -211.7f;
		m_fY = 143.6f;
		m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 0.f, 1.f, 1.f), 190.f);
		m_RGBColor = _float3(0.1294f, 0.817f, 0.6392f);
	}

	if (Is_MyName(L"Mini_Line1") == true)
	{
		m_fX = 307.8f;
		m_fY = 102.8f;
		m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 0.f, 1.f, 1.f), 45.f);
		m_RGBColor = _float3(0.8079f, 0.2431f, 0.3058f);
	}

	if (Is_MyName(L"Mini_Line2") == true)
		m_RGBColor = _float3(0.0784f, 0.5333f, 0.7686f);

	if (Is_MyName(L"Mini_Line3") == true)
	{
		m_fX = -313.4f;
		m_fY = -86.f;
		m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 0.f, 1.f, 1.f), -140.f);
		m_RGBColor = _float3(0.8980f, 0.1764f, 0.5294f);
	}


}


void CUI_MiniGame::Count_Number(const _double& _dTimeDelta) //10초밑으로 빨간색 숫자 텍스쳐로 바꾸기 
{
	//총 타이머 시간과 시작카운트를 계산하여 텍스쳐를 바꾼다 


	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//타이머
	if (m_StartNumber == 3)
		m_Time += _dTimeDelta; //시작카운터가 다 되고 시작이 되고나서부터 감소되자 

	m_StartTime += _dTimeDelta;

	if (m_Time >= 1 && m_Number != 0)
	{
		m_Number--;
		m_Time = 0;
	}

	if (m_StartTime >= 1 && m_StartNumber != 3)
	{
		m_StartNumber++;
		m_Grow = true;
		m_StartTime = 0;
	}

	m_CountNumber10 = m_Number / 10;
	m_CountNumber1 = m_Number % 10;

	if (Is_MyName(L"Mini_Count1") == true) //1의 자리숫자
		wsprintf(m_TextureName, TEXT("Num_Timer%d"), m_CountNumber1);


	if (Is_MyName(L"Mini_Count10") == true)//10의 자리숫자 
	{
		if (m_CountNumber10 == 0)
			m_fAlpha = 0.f;
		else
			m_fAlpha = 1.f;

		wsprintf(m_TextureName, TEXT("Num_Timer%d"), m_CountNumber10);
	}

	//시작 카운트 
	if (Is_MyName(L"Mini_Start") == true)
	{
		if (m_StartNumber == 3)
		{
			pGameInstance->Play_BGM(TEXT("UI_MinigameBGM"), 1.0f);
			m_fSizeX = 300.f;
			m_fSizeY = 200.f;
		}
		wsprintf(m_TextureName, TEXT("Mini_Start%d"), m_StartNumber);
	}

	if (Is_MyName(L"Mini_Start") == true)
	{
		if (m_Grow == true && m_StartNumber != 3)
		{
			m_bEffect = true;
			Grow_Combo(5.f, 200.f, 0.f, 0.f, _dTimeDelta);
			if (m_fSizeX >= 200.f)
				m_Grow = false;
		}
		else if (m_Grow == false && m_StartNumber != 3)
		{
			m_fSizeX = 150.f;
			m_fSizeY = 150.f;
		}
		if (m_StartNumber == 3)
		{
			m_fSizeX += (_float)_dTimeDelta;
			m_fSizeY += (_float)_dTimeDelta;
			if (m_fSizeX >= 350.f || m_fSizeY >= 250.f)
			{
				m_fSizeX = 300.f;
				m_fSizeY = 200.f;
			}
			m_fAlpha -= (_float)_dTimeDelta * 0.4f;
			if (m_fAlpha <= 0.f)
				m_fAlpha = 0.f;
		}
	}

	//끝났을때 카운트0
	if (m_Number  == 0 && Is_MyName(L"Mini_End") == true)
	{
		m_fAlpha += (_float)_dTimeDelta * 1.5f;
		if (m_fAlpha >= 1.f)
		{
			if (SoundPlay == false)
			{
				pGameInstance->Play_SoundW(TEXT("UI_Win2"), 0.5f);
				SoundPlay = true;
			}

			m_fAlpha = 1.f;
			Return_Rattan = true;

		}
	}

	Safe_Release(pGameInstance);
}

void CUI_MiniGame::Gauge(const _double& _dTimeDelta)
{

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*> Prototype_GameObjects = pGameInstance->Find_GameObjectList(LEVEL_MINIGAME, LAYER_UI);
	CUI_MiniGame* Rank = nullptr;
	for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
	{
		if (dynamic_cast<CUI_MiniGame*>(*it)->Is_MyName(L"Mini_Rank") == true)
		{
			Rank = dynamic_cast<CUI_MiniGame*>(*it);
			break;
		}
	}


	//게이지바 증가 계산
	if (100.f < m_NowGauge)
		m_NowGauge = 100.f; // 100보다 크면 100으로

	m_NowPercent = m_NowGauge / m_MaxGauge;

	//m_NowPercent에 따른 게이지바 텍스쳐 변경 
	if (Is_MyName(L"Mini_Bar") == true)
	{
		if (m_NowPercent > 0.f)
		{
			wsprintf(m_TextureName, TEXT("Mini_Bar0"));
			Rank->Set_Texture(L"Rank_0");
		}
		if (m_NowPercent > 0.28f)
		{
			wsprintf(m_TextureName, TEXT("Mini_Bar1"));
			Rank->Set_Texture(L"Rank_1");
		}
		if (m_NowPercent > 0.49f)
		{
			wsprintf(m_TextureName, TEXT("Mini_Bar2"));
			Rank->Set_Texture(L"Rank_2");
		}
		if (m_NowPercent > 0.64f)
		{
			wsprintf(m_TextureName, TEXT("Mini_Bar3"));
			Rank->Set_Texture(L"Rank_3");
		}
		if (m_NowPercent > 0.80f)
		{
			wsprintf(m_TextureName, TEXT("Mini_Bar4"));
			Rank->Set_Texture(L"Rank_4");
		}
	}


	Safe_Release(pGameInstance);
}

void CUI_MiniGame::KeyButton(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (Is_MyName(L"Mini_Start") == true)
	{
		if(m_Number != 60 && m_Number > 1)
		m_RandomKey += _dTimeDelta;

		_int RandCount = pGameInstance->Get_RandomCount() % 8;

		_int Key = 0;
		_int EffectKey = 0;

		if (RandCount == 0)
		{
			Key = CMiniGame_Note::KEY_Y;
		}
		else if (RandCount == 1)
		{
			Key = CMiniGame_Note::KEY_B;
		}
		else if (RandCount == 2)
		{
			Key = CMiniGame_Note::KEY_A;
		}
		else if (RandCount == 3)
		{
			Key = CMiniGame_Note::KEY_X;
		}
		//effect키 
		else if (RandCount == 4)
		{
			EffectKey = CMiniGame_Note::EFFECT_Y;
			Key = CMiniGame_Note::KEY_Y;
		}
		else if (RandCount == 5)
		{
			EffectKey = CMiniGame_Note::EFFECT_B;
			Key = CMiniGame_Note::KEY_B;
		}
		else if (RandCount == 6)
		{
			EffectKey = CMiniGame_Note::EFFECT_A;
			Key = CMiniGame_Note::KEY_A;
		}
		else if (RandCount == 7)
		{
			EffectKey = CMiniGame_Note::EFFECT_X;
			Key = CMiniGame_Note::KEY_X;
		}

		if (m_RandomKey > 0.3) //0.3초에 한번씩 생성 
		{
			if (RandCount <= 3)
			{
				pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
					LAYER_UI, PROTO_GOB_UI_NOTE, &Key);
				m_RandomKey = 0;
			}
			else if (RandCount > 3)
			{
				pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
					LAYER_UI, PROTO_GOB_UI_NOTE, &Key);
				pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
					LAYER_UI, PROTO_GOB_UI_NOTE, &EffectKey);
				m_RandomKey = 0;
			}
		}

		//일반키면 하나만 생성 
		//이펙트 키면 두개 생성
		//예를들어 랜덤값이 4이면 이펙트enum으로 하나 생성하고 일반enum으로 하나 생성
	}
	Safe_Release(pGameInstance);
}

void CUI_MiniGame::Result(_uint Result_type)
{
	if (Is_MyName(L"Mini_Result") == true) //나중에 효과주기 
	{
		switch (Result_type)
		{
		case Client::CUI_MiniGame::RESULT_GREAT:
		{
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);
			pGameInstance->Play_SoundW(TEXT("UI_Note"), 0.2f);
			Safe_Release(pGameInstance);

			m_fSizeX = 220.f;
			m_fSizeY = 150.f;
			Resulttype = RESULT_GREAT;
			wsprintf(m_TextureName, TEXT("Mini_Great"));
		}
			break;
		case Client::CUI_MiniGame::RESULT_GOOD:
		{
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);
			pGameInstance->Play_SoundW(TEXT("UI_Note"), 0.2f);
			Safe_Release(pGameInstance);

			m_fSizeX = 220.f;
			m_fSizeY = 150.f;
			Resulttype = RESULT_GOOD;
			wsprintf(m_TextureName, TEXT("Mini_Good"));
		}
			break;
		case Client::CUI_MiniGame::RESULT_BAD:
		{
			m_fSizeX = 230.f;
			m_fSizeY = 150.f;
			Resulttype = RESULT_BAD;
			wsprintf(m_TextureName, TEXT("Mini_Bad"));
		}
			break;
		case Client::CUI_MiniGame::RESULT_END:
			m_fAlpha = 0.f;
			break;

		}

		
	}

}

void CUI_MiniGame::Score_Plus(_float Gauge, _int score)
{
	m_NowGauge += Gauge;
	m_Score += score;
}

void CUI_MiniGame::Combo_Plus(_int combo)
{
	if (Is_MyName(L"Mini_Combo") == true)
	{
		if (combo == 0)
			m_Combo = 0;

		m_Combo += combo;

		if (m_Combo == 0)
		{
			m_fAlpha = 0.f;
			m_fSizeX = 120.f;
			m_fSizeY = 50.f;
		}
		else if (m_Combo == 1)
			m_fAlpha = 1.f;

		else if (m_Combo == 3)
		{
			m_fSizeX = 130.f;
			m_fSizeY = 60.f;
		}

		else if (m_Combo == 6)
		{
			m_fSizeX = 140.f;
			m_fSizeY = 70.f;
		}

		else if (m_Combo == 10)
		{
			m_fSizeX = 150.f;
			m_fSizeY = 80.f;
		}
	}
}

void CUI_MiniGame::Combo()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*> Prototype_GameObjects = pGameInstance->Find_GameObjectList(LEVEL_MINIGAME, LAYER_UI);
	CUI_MiniGame* Combo = nullptr;
	for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
	{
		if (dynamic_cast<CUI_MiniGame*>(*it)->Is_MyName(L"Mini_Combo") == true)
		{
			Combo = dynamic_cast<CUI_MiniGame*>(*it);
			break;
		}
	}
	ComboNum = Combo->Get_Combo();

	if (Is_MyName(L"Mini_ComboBase") == true)
	{
		if (ComboNum == 0)
		{
			m_fAlpha = 0.f;
			m_fSizeX = 130.f;
			m_fSizeY = 100.f;
		}
		else if (ComboNum == 1)
			m_fAlpha = 1.f;

		else if (ComboNum == 3)
		{
			m_fSizeX = 160.f;
			m_fSizeY = 130.f;
		}

		else if (ComboNum == 6)
		{
			m_fSizeX = 200.f;
			m_fSizeY = 170.f;
		}

		else if (ComboNum == 10)
		{
			m_fSizeX = 240.f;
			m_fSizeY = 210.f;
		}
	}

	if (Is_MyName(L"Mini_ComboNumber10") == true || Is_MyName(L"Mini_ComboNumber1") == true)
	{

		m_ComboNumber10 = ComboNum / 10;
		m_ComboNumber1 = ComboNum % 10;

		if (Is_MyName(L"Mini_ComboNumber1") == true) //1의 자리숫자
			wsprintf(m_TextureName, TEXT("Mini_Combo%d"), m_ComboNumber1);

		if (Is_MyName(L"Mini_ComboNumber10") == true)//10의 자리숫자 
			wsprintf(m_TextureName, TEXT("Mini_Combo%d"), m_ComboNumber10);
		
		if (ComboNum == 0)
		{
			m_fAlpha = 0.f;
			m_fSizeX = 60.f;
			m_fSizeY = 60.f;
		}
		else if (ComboNum == 1)
			m_fAlpha = 1.f;

		else if (ComboNum == 3)
		{
			m_fSizeX = 70.f;
			m_fSizeY = 70.f;
		}

		else if (ComboNum == 6)
		{
			m_fSizeX = 80.f;
			m_fSizeY = 80.f;
		}

		else if (ComboNum == 10)
		{
			m_fSizeX = 90.f;
			m_fSizeY = 90.f;
		}
	}
	
	Safe_Release(pGameInstance);
}

void CUI_MiniGame::Dialog(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (Is_MyName(L"Mini_Dia") == true)
	{
	DiaTime += _dTimeDelta;
	if (DiaTime > 1)
	{
		Dia++;
		DiaTime = 0;
	}

	if (Dia == 5 && m_Number != 0)
	{
		SoundPlay = false;
		DiaTexture = pGameInstance->Get_RandomCount() % 9;
		m_fAlpha = 1.f;
		Dia = 0;
	}
	if (Dia ==2)
		m_fAlpha = 0.f;

		wsprintf(m_TextureName, TEXT("MiniGame_Dia%d"), DiaTexture);

		//사운드 
		if (Is_MyTexture(L"MiniGame_Dia0") == true && SoundPlay == false)//힘내세요
		{
			pGameInstance->Play_SoundW(TEXT("UI_Fight"), 0.3f);
			SoundPlay = true;
		}
		else if (Is_MyTexture(L"MiniGame_Dia1") == true && SoundPlay == false)
		{
			pGameInstance->Play_SoundW(TEXT("UI_WIN1"), 0.3f);
			SoundPlay = true;
		}
		else if (Is_MyTexture(L"MiniGame_Dia2") == true && SoundPlay == false)//실망이에요
		{
			pGameInstance->Play_SoundW(TEXT("UI_Lose"), 0.3f);
			SoundPlay = true;
		}
		else if (Is_MyTexture(L"MiniGame_Dia3") == true && SoundPlay == false)
		{
			pGameInstance->Play_SoundW(TEXT("UI_Lose"), 0.3f);
			SoundPlay = true;
		}
		else if (Is_MyTexture(L"MiniGame_Dia4") == true && SoundPlay == false)
		{
			pGameInstance->Play_SoundW(TEXT("UI_Fight1"), 0.3f);
			SoundPlay = true;
		}
		else if (Is_MyTexture(L"MiniGame_Dia5") == true && SoundPlay == false)
		{
			pGameInstance->Play_SoundW(TEXT("UI_Lose1"), 0.3f);
			SoundPlay = true;
		}
		else if (Is_MyTexture(L"MiniGame_Dia6") == true && SoundPlay == false)
		{
			pGameInstance->Play_SoundW(TEXT("UI_Win2"), 0.3f);
			SoundPlay = true;
		}
		else if (Is_MyTexture(L"MiniGame_Dia7") == true && SoundPlay == false)
		{
			pGameInstance->Play_SoundW(TEXT("UI_Lose"), 0.3f);
			SoundPlay = true;
		}
		else if (Is_MyTexture(L"MiniGame_Dia8") == true && SoundPlay == false)
		{
			pGameInstance->Play_SoundW(TEXT("UI_WIN"), 0.3f);
			SoundPlay = true;
		}
		

	}


	Safe_Release(pGameInstance);
}

HRESULT CUI_MiniGame::SetUp_Components()
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

HRESULT CUI_MiniGame::SetUp_ConstantTable()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_MiniGaugePercent", &m_NowPercent, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_MiniGameRGB", &m_RGBColor, sizeof(_float3))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DiffuseTexture", m_TextureName)))
		return E_FAIL;

	return S_OK;
}

CUI_MiniGame* CUI_MiniGame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MiniGame* pInstance = new CUI_MiniGame(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_MiniGame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI_Base* CUI_MiniGame::Clone(void* pArg)
{
	CUI_MiniGame* pInstance = new CUI_MiniGame(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_MiniGame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_MiniGame::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
