#include "MiniGame_Note.h"
#include "GameInstance.h"
#include "UI_MiniGame.h"
#include "MiniGame_Model.h"

CMiniGame_Note::CMiniGame_Note(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CUI_Base(_pDevice, _pContext)
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

CMiniGame_Note::CMiniGame_Note(const CMiniGame_Note& _rhs)
	: CUI_Base(_rhs)
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

HRESULT CMiniGame_Note::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CMiniGame_Note::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	type = *(_int*)pArg;

	m_fSizeX = 70.f;
	m_fSizeY = 70.f;

	m_RGBColor = _float3(1.f, 1.f, 0.488f);
	
	Setting_Positon(type);


	if (FAILED(SetUp_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CMiniGame_Note::Tick(const _double& _dTimeDelta)
{
	

	__super::Tick(_dTimeDelta);

	Tick_Event(_dTimeDelta);
	Bezier(_dTimeDelta);
	Great_Eff(_dTimeDelta);
	
	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_Pos(XMVectorSet(m_fX, m_fY, 0.0f, 1.f));

}

void CMiniGame_Note::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMiniGame_Note::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if(type == EFFECT_Y || type == EFFECT_B || type == EFFECT_A || type == EFFECT_X)
		m_pShaderCom->Begin(13);
	
	else
	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CMiniGame_Note::Setting_Positon(_uint Type)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//이펙트 이넘으로 생성될 경우 해당일반키와 Note값을 똑같이 주고 회전시킨다 
	switch (Type)
	{
	case Client::CMiniGame_Note::KEY_Y:
		wsprintf(m_TextureName, TEXT("Btn_0"));
		Note[0] = _float2(-378.2f, 124.4f);
		Note[1] = _float2(-168.7f, 250.f);
		Note[2] = _float2(0.1f, 121.9f);
		break;
	case Client::CMiniGame_Note::KEY_B:
		wsprintf(m_TextureName, TEXT("Btn_1"));
		Note[0] = _float2(426.9f, 185.7f);
		Note[1] = _float2(442.f, 20.f);
		Note[2] = _float2(120.7f, 0.8f);
		break;
	case Client::CMiniGame_Note::KEY_A:
		wsprintf(m_TextureName, TEXT("Btn_2"));
		Note[0] = _float2(342.8f, -206.5f);
		Note[1] = _float2(129.3f, -270.f);
		Note[2] = _float2(0.1f, -119.7f);
		break;
	case Client::CMiniGame_Note::KEY_X:
		wsprintf(m_TextureName, TEXT("Btn_3"));
		Note[0] = _float2(-440.7f, -154.2f);
		Note[1] = _float2(-261.6f, 52.7f);
		Note[2] = _float2(-120.f, 1.9f);
		break;


	case Client::CMiniGame_Note::EFFECT_Y:
		m_fSizeX = 180.f;
		m_fSizeY = 180.f;
		m_Depth = 2;
		wsprintf(m_TextureName, TEXT("Mini_NoteEff"));
		Note[0] = _float2(-378.2f, 124.4f);
		Note[1] = _float2(-168.7f, 250.f);
		Note[2] = _float2(0.1f, 121.9f);
		break;
	case Client::CMiniGame_Note::EFFECT_B:
		m_fSizeX = 180.f;
		m_fSizeY = 180.f;
		m_Depth = 2;
		wsprintf(m_TextureName, TEXT("Mini_NoteEff"));
		Note[0] = _float2(426.9f, 185.7f);
		Note[1] = _float2(442.f, 20.f);
		Note[2] = _float2(120.7f, 0.8f);
		break;
	case Client::CMiniGame_Note::EFFECT_A:
		m_fSizeX = 180.f;
		m_fSizeY = 180.f;
		m_Depth = 2;
		wsprintf(m_TextureName, TEXT("Mini_NoteEff"));
		Note[0] = _float2(342.8f, -206.5f);
		Note[1] = _float2(129.3f, -270.f);
		Note[2] = _float2(0.1f, -119.7f);
		break;
	case Client::CMiniGame_Note::EFFECT_X:
		m_fSizeX = 180.f;
		m_fSizeY = 180.f;
		m_Depth = 2;
		wsprintf(m_TextureName, TEXT("Mini_NoteEff"));
		Note[0] = _float2(-440.7f, -154.2f);
		Note[1] = _float2(-261.6f, 52.7f);
		Note[2] = _float2(-120.f, 1.9f);
		break;
	case Client::CMiniGame_Note::EFFECT_GREAT:
	{
		wsprintf(m_TextureName, TEXT("Mini_GreatEff"));
		m_RGBColor = _float3(1.f, 1.f, 0.488f); //수정 필요 
		m_fSizeX = 100.f;
		m_fSizeY = 100.f;
		m_Depth = 1;
		m_fAlpha = 0.f; //나중 0으로 바꿔야함///////////////////////////////
	}
		break;
	case Client::CMiniGame_Note::EFFECT_RING:
		wsprintf(m_TextureName, TEXT("Mini_GreatEff"));
		m_RGBColor = _float3(1.f, 1.f, 0.488f); //수정 필요 
		m_fSizeX = 50.f;
		m_fSizeY = 50.f;
		m_Depth = 1;
		m_fAlpha = 0.f; //나중 0으로 바꿔야함///////////////////////////////
		break;
	case Client::CMiniGame_Note::EFFECT_GOOD:
	{
		wsprintf(m_TextureName, TEXT("Mini_GoodEff"));
		m_fSizeX = 150.f;
		m_fSizeY = 150.f;
		m_Depth = 2;
		m_fAlpha = 1.f;
	}
		break;
	case Client::CMiniGame_Note::EFFECT_STAR:
		wsprintf(m_TextureName, TEXT("Mini_GoodEff"));
		m_fSizeX = 200.f;
		m_fSizeY = 200.f;
		m_Depth = 2;
		m_fAlpha = 1.f;
		break;
	case Client::CMiniGame_Note::KEY_END:
		break;
	}
	Safe_Release(pGameInstance);
}

void CMiniGame_Note::Tick_Event(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	
	if (type == EFFECT_Y || type == EFFECT_B || type == EFFECT_A || type == EFFECT_X)
		//회전 시킨다 
	{
		m_pTransformCom->Rotate_Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), _dTimeDelta, 7.f);
	}

	if (type == EFFECT_GOOD)
	{
		timeeff += _dTimeDelta;

		m_pTransformCom->Rotate_Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), _dTimeDelta, 5.f);
		if(timeeff > 0.15)
			pGameInstance->Add_DeleteObject(LEVEL_MINIGAME, LAYER_QUEST, this);
	}
	
	if (type == EFFECT_STAR)
	{
	timeeff += _dTimeDelta;

	m_pTransformCom->Rotate_Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), _dTimeDelta, 2.f);
	if (timeeff > 0.3)
		pGameInstance->Add_DeleteObject(LEVEL_MINIGAME, LAYER_QUEST, this);
	}



	Safe_Release(pGameInstance);
}

void CMiniGame_Note::Bezier(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (type != EFFECT_GREAT || type != EFFECT_RING)
	{
			//베지어 곡선 해 
		m_time += _dTimeDelta;

		if (m_time >= 0.005) //노트 속도  
		{
			m_t += 0.05f;
			m_time = 0;
		}

		if (m_t > 1.f)
			m_t = 1.f;
		else if (m_t < 0.f)
			m_t = 0.f;

		m_fX = (1 - m_t) * (1 - m_t) * Note[0].x + 2 * m_t * (1 - m_t) * Note[1].x + m_t * m_t * Note[2].x;
		m_fY = (1 - m_t) * (1 - m_t) * Note[0].y + 2 * m_t * (1 - m_t) * Note[1].y + m_t * m_t * Note[2].y;
	}

	list<CGameObject*> Prototype_GameObjects = pGameInstance->Find_GameObjectList(LEVEL_MINIGAME, LAYER_UI);
	
	CUI_MiniGame* Mini = nullptr;
	for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
	{
		if (dynamic_cast<CUI_MiniGame*>(*it)->Is_MyName(L"Mini_Result") == true)
		{
			Mini = dynamic_cast<CUI_MiniGame*>(*it);
			break;
		}
	}

	CUI_MiniGame* Gauge = nullptr;
	for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
	{
		if (dynamic_cast<CUI_MiniGame*>(*it)->Is_MyName(L"Mini_Bar") == true)
		{
			Gauge = dynamic_cast<CUI_MiniGame*>(*it);
			break;
		}
	}

	CUI_MiniGame* Combo = nullptr;
	for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
	{
		if (dynamic_cast<CUI_MiniGame*>(*it)->Is_MyName(L"Mini_Combo") == true)
		{
			Combo = dynamic_cast<CUI_MiniGame*>(*it);
			break;
		}
	}

	list<CGameObject*> Prototype_NoteObjects = pGameInstance->Find_GameObjectList(LEVEL_MINIGAME, LAYER_QUEST);

	CMiniGame_Note* Greateff = nullptr;
	for (auto it = Prototype_NoteObjects.begin(); it != Prototype_NoteObjects.end(); it++)
	{
		if (dynamic_cast<CMiniGame_Note*>(*it)->type == EFFECT_GREAT)
		{
			Greateff = dynamic_cast<CMiniGame_Note*>(*it);
			break;
		}
	}

	CMiniGame_Note* Ringeff = nullptr;
	for (auto it = Prototype_NoteObjects.begin(); it != Prototype_NoteObjects.end(); it++)
	{
		if (dynamic_cast<CMiniGame_Note*>(*it)->type == EFFECT_RING)
		{
			Ringeff = dynamic_cast<CMiniGame_Note*>(*it);
			break;
		}
	}

	//점수,판정,콤보 올려 
		if (type == KEY_Y)
		{
			if (pGameInstance->Get_DIKeyDown(DIK_UP))
			{
				Mini->Set_ResultEff(true);
				if (m_t == 1 && deletetime > 0.1)
				{
					Mini->Result(CUI_MiniGame::RESULT_GREAT);
					Gauge->Score_Plus(0.8f, 15);
					Combo->Combo_Plus(1);
					Greateff->Efftype = KEY_Y;
					Greateff->GreatEff = true;
					Greateff->m_fAlpha = 1.5f;
					Ringeff->Efftype = KEY_Y;
					Ringeff->GreatEff = true;
					Ringeff->m_fAlpha = 1.5f;
					CreatEff = true;
				}
				else if (1 >= m_t && m_t >= 0.8)
				{
					Mini->Result(CUI_MiniGame::RESULT_GOOD);
					Gauge->Score_Plus(0.5f,10);
					Combo->Combo_Plus(1);
					Greateff->Efftype = KEY_Y;
					Greateff->GreatEff = true;
					Greateff->m_fAlpha = 1.5f;
					Ringeff->Efftype = KEY_Y;
					Ringeff->GreatEff = true;
					Ringeff->m_fAlpha = 1.5f;
				}
				else //if(m_t <= 0.7)
				{
					Mini->Result(CUI_MiniGame::RESULT_BAD);
					Combo->Combo_Plus(0);
				}
			}
		}

		if (type == KEY_B)
		{
			if (pGameInstance->Get_DIKeyDown(DIK_RIGHT))
			{
				Mini->Set_ResultEff(true);
				if (m_t == 1 && deletetime > 0.1)
				{
					Mini->Result(CUI_MiniGame::RESULT_GREAT);
					Gauge->Score_Plus(0.8f, 15);
					Combo->Combo_Plus(1);
					Greateff->Efftype = KEY_B;
					Greateff->GreatEff = true;
					Greateff->m_fAlpha = 1.5f;
					Ringeff->Efftype = KEY_B;
					Ringeff->GreatEff = true;
					Ringeff->m_fAlpha = 1.5f;
					CreatEff = true;
				}

				else if (1 >= m_t && m_t >= 0.8)
				{
					Mini->Result(CUI_MiniGame::RESULT_GOOD);
					Gauge->Score_Plus(0.5f, 10);
					Combo->Combo_Plus(1);
					Greateff->Efftype = KEY_B;
					Greateff->GreatEff = true;
					Greateff->m_fAlpha = 1.5f;
					Ringeff->Efftype = KEY_B;
					Ringeff->GreatEff = true;
					Ringeff->m_fAlpha = 1.5f;
					
				}

				else //if (m_t <= 0.7)
				{
					Mini->Result(CUI_MiniGame::RESULT_BAD);
					Combo->Combo_Plus(0);
				}
			}
		}

		if (type == KEY_A)
		{
			if (pGameInstance->Get_DIKeyDown(DIK_DOWN))
			{
				Mini->Set_ResultEff(true);
				if (m_t == 1 && deletetime > 0.1)
				{
					Mini->Result(CUI_MiniGame::RESULT_GREAT);
					Gauge->Score_Plus(0.8f, 15);
					Combo->Combo_Plus(1);
					Greateff->Efftype = KEY_A;
					Greateff->GreatEff = true;
					Greateff->m_fAlpha = 1.5f;
					Ringeff->Efftype = KEY_A;
					Ringeff->GreatEff = true;
					Ringeff->m_fAlpha = 1.5f;
					CreatEff = true;
				}

				else if (1 >= m_t && m_t >= 0.8)
				{
					Mini->Result(CUI_MiniGame::RESULT_GOOD);
					Gauge->Score_Plus(0.5f, 10);
					Combo->Combo_Plus(1);
					Greateff->Efftype = KEY_A;
					Greateff->GreatEff = true;
					Greateff->m_fAlpha = 1.5f;
					Ringeff->Efftype = KEY_A;
					Ringeff->GreatEff = true;
					Ringeff->m_fAlpha = 1.5f;
				}

				else //if (m_t <= 0.7)
				{
					Mini->Result(CUI_MiniGame::RESULT_BAD);
					Combo->Combo_Plus(0);
				}
			}
		}

		if (type == KEY_X)
		{
			if (pGameInstance->Get_DIKeyDown(DIK_LEFT))
			{
				Mini->Set_ResultEff(true);
				if (m_t == 1 && deletetime > 0.1)
				{
					Mini->Result(CUI_MiniGame::RESULT_GREAT);
					Gauge->Score_Plus(0.8f, 15);
					Combo->Combo_Plus(1);
					Greateff->Efftype = KEY_X;
					Greateff->GreatEff = true;
					Greateff->m_fAlpha = 1.5f;
					Ringeff->Efftype = KEY_X;
					Ringeff->GreatEff = true;
					Ringeff->m_fAlpha = 1.5f;
					CreatEff = true;
				}

				else if (1 >= m_t && m_t >= 0.8)
				{
					Mini->Result(CUI_MiniGame::RESULT_GOOD);
					Gauge->Score_Plus(0.5f, 10);
					Combo->Combo_Plus(1);
					Greateff->Efftype = KEY_X;
					Greateff->GreatEff = true;
					Greateff->m_fAlpha = 1.5f;
					Ringeff->Efftype = KEY_X;
					Ringeff->GreatEff = true;
					Ringeff->m_fAlpha = 1.5f;
				}

				else //if (m_t <= 0.7)
				{
					Mini->Result(CUI_MiniGame::RESULT_BAD);
					Combo->Combo_Plus(0);
				}
			}
		}

		//이펙트 생성해 
		if (CreatEff == true)
		{
			_int GoodEff = CMiniGame_Note::EFFECT_GOOD;
			_int StarEff = CMiniGame_Note::EFFECT_STAR;
			pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
				LAYER_QUEST, PROTO_GOB_UI_NOTE, &GoodEff);
			pGameInstance->Clone_GameObject(LEVEL_MINIGAME,
				LAYER_QUEST, PROTO_GOB_UI_NOTE, &StarEff);
			CreatEff = false;
		}


	//삭제해 
		if (type != EFFECT_GREAT || type != EFFECT_RING)
		{
			if (m_t == 1)
				deletetime += _dTimeDelta;

			if (deletetime >= 0.2f)
				pGameInstance->Add_DeleteObject(LEVEL_MINIGAME, LAYER_UI, this);
		}

		Safe_Release(pGameInstance);
}

void CMiniGame_Note::Great_Eff(const _double& _dTimeDelta)
{ 

	if (GreatEff == true && type == EFFECT_GREAT)
	{
		m_bEffect = true;

		Grow_UI(2.0f, 150.f, 0.f, 0.f, _dTimeDelta);

		if (m_bEffect == false)
		{
				GreatEff = false;
			    m_fSizeX = 100.f;
				m_fSizeY = 100.f;
				m_fAlpha = 0.f;
		}

		switch (Efftype)
		{
		case Client::CMiniGame_Note::KEY_Y:
		{
			m_fX = 0.1f;
			m_fY = 121.9f;
			break;
		}
		case Client::CMiniGame_Note::KEY_B:
		{
			m_fX = 120.7f;
			m_fY = 0.8f;
			break;
		}
		case Client::CMiniGame_Note::KEY_A:
		{
			m_fX = 0.1f;
			m_fY = -119.7f;
			break;
		}
		case Client::CMiniGame_Note::KEY_X:
		{
			m_fX = -120.f;
			m_fY = 1.9f;
			break;
		}
		default:
			break;
		}
		
	}

	if (GreatEff == true && type == EFFECT_RING)
	{
		m_bEffect = true;

		Grow_UI(2.0f, 100.f, 0.f, 0.f, _dTimeDelta);

		if (m_bEffect == false)
		{
			GreatEff = false;
			m_fSizeX = 50.f;
			m_fSizeY = 50.f;
			m_fAlpha = 0.f;
		}

		switch (Efftype)
		{
		case Client::CMiniGame_Note::KEY_Y:
		{
			m_fX = 0.1f;
			m_fY = 121.9f;
			break;
		}
		case Client::CMiniGame_Note::KEY_B:
		{
			m_fX = 120.7f;
			m_fY = 0.8f;
			break;
		}
		case Client::CMiniGame_Note::KEY_A:
		{
			m_fX = 0.1f;
			m_fY = -119.7f;
			break;
		}
		case Client::CMiniGame_Note::KEY_X:
		{
			m_fX = -120.f;
			m_fY = 1.9f;
			break;
		}
		default:
			break;
		}

	}


}

HRESULT CMiniGame_Note::SetUp_Components()
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

HRESULT CMiniGame_Note::SetUp_ConstantTable()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_MiniGameRGB", &m_RGBColor, sizeof(_float3))))
		return E_FAIL;


	return S_OK;
}

CMiniGame_Note* CMiniGame_Note::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMiniGame_Note* pInstance = new CMiniGame_Note(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created : CMiniGame_Note");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI_Base* CMiniGame_Note::Clone(void* pArg)
{
	CMiniGame_Note* pInstance = new CMiniGame_Note(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMiniGame_Note");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMiniGame_Note::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
