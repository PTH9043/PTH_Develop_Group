#include "UI_Ultimate.h"
#include "GameInstance.h"
#include "Battle_Manager.h"
#include "Player.h"

CUI_Ultimate::CUI_Ultimate(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CUI_Base(_pDevice, _pContext)
{
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

CUI_Ultimate::CUI_Ultimate(const CUI_Ultimate& _rhs)
    : CUI_Base(_rhs)
{
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}
HRESULT CUI_Ultimate::NativeConstruct_Prototype()
{
    return S_OK;
}

HRESULT CUI_Ultimate::NativeConstruct_Clone(void* pArg)
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

	if (Is_MyName(L"Ult_FrameEff2") == true || Is_MyName(L"2P_Ult_FireEff") == true)
	{
		m_fDepthValue = -1.f;
		m_fAlpha = 0.f;
	}

	if (Is_MyName(L"2P_Ult_GaugeBase") == true || Is_MyName(L"2P_Ult_FireEff") == true)
		m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);

	return S_OK;
}

void CUI_Ultimate::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_Pos(XMVectorSet(m_fX, m_fY, 0.0f, 1.f));

	//플레이어
	CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager);
	{
		m_pPlayerObject = (CGameObject*)pInstance->Get_BattleCharacter(CBattle_Manager::CHAR_PLAYER);
		m_p2PlayerObject = (CGameObject*)pInstance->Get_BattleCharacter(CBattle_Manager::CHAR_AI);
	}
	RELEASE_INSTANCE(CBattle_Manager);

	CPlayer* pPlayer = (CPlayer*)m_pPlayerObject;
	CPlayer* p2Player = (CPlayer*)m_p2PlayerObject;



	CPlayer::BATTLEDATA PlayerData = pPlayer->Get_BattleData();
	CPlayer::BATTLEDATA AIData = p2Player->Get_BattleData();

	_bool Awake = PlayerData.bIsAwake;
	_bool Awake2P = AIData.bIsAwake;
	
	m_ComboNumber = PlayerData.iHitCount;
	// 개방 게이지 카운트를 받아온다.
	m_Number = PlayerData.iUltimateCount;

	m_2PComboNumber = AIData.iHitCount;
	// 개방 게이지 카운트를 받아온다.
	m_2PNumber = AIData.iUltimateCount;
	
	// 개방 게이지를 받아온다.
	if (Awake == false)
	{
		m_NowGauge = PlayerData.fUltimateValue;
	}
	else
		m_NowGauge = PlayerData.fAwakeValue;

	// 개방 게이지를 받아온다.
	if (Awake2P == false)
	{
		m_2PNowGauge = AIData.fUltimateValue;
	}
	else
		m_2PNowGauge = AIData.fAwakeValue;

	TickFrame += (_float)_dTimeDelta;
	
	if (TickFrame >= 0.05f)
	{
		//m_OriginNumber = m_ComboNumber;
		//m_2POriginNumber = m_2PComboNumber;
		TickFrame = 0.f;
	}

	Ultimate_Gauge(_dTimeDelta);
	Ultimate_Number();
	Ultimate_FireEff(_dTimeDelta);
	Ultimate_FrameEff(_dTimeDelta);

	if (Is_MyName(L"Ult_FrameEff2") == true || Is_MyName(L"2P_Ult_FrameEff2") == true)
		m_fAlpha = 0.f;
}

void CUI_Ultimate::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	
	if (Is_MyName(L"Ult_FireEff") == true || Is_MyName(L"2P_Ult_FireEff") == true)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	
	else
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Ultimate::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (Is_MyName(L"Ult_Gauge") == true || Is_MyName(L"2P_Ult_Gauge") == true)
		m_pShaderCom->Begin(7);

	else if (Is_MyName(L"Ult_FireEff") == true || Is_MyName(L"2P_Ult_FireEff") == true)
		m_pShaderCom->Begin(14);

	else
		m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}



void CUI_Ultimate::Ultimate_Gauge(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	if (Is_MyName(L"Ult_Gauge") == true || Is_MyName(L"Ult_Number") == true
		|| Is_MyName(L"Ult_FrameEff") == true || Is_MyName(L"Ult_FrameEff2") == true
		|| Is_MyName(L"Ult_FireEff") == true || Is_MyName(L"Ult_GaugeBase") == true || Is_MyName(L"Ult_Frame") == true
		|| Is_MyName(L"Ult_InBase") == true || Is_MyName(L"Ult_GaugeFrame") == true)
	{

		if (pGameInstance->Get_DIKeyDown(DIK_8))
		{
			m_Minus_NowGauge = true;
			m_NumberEffect = true;
		}
		m_NowPercent = m_NowGauge / m_MaxGauge;
		
		if (m_Minus_NowGauge == true && m_NowGauge >= 0.f)
			m_NowGauge -= (_float)_dTimeDelta * 40.f;

	}

	//===============================================================
	if (Is_MyName(L"2P_Ult_Gauge") == true || Is_MyName(L"2P_Ult_Number") == true
	|| Is_MyName(L"2P_Ult_FrameEff") == true || Is_MyName(L"2P_Ult_FrameEff2") == true
		|| Is_MyName(L"2P_Ult_FireEff") == true || Is_MyName(L"2P_Ult_GaugeBase") == true || Is_MyName(L"2P_Ult_Frame") == true
		|| Is_MyName(L"2P_Ult_InBase") == true || Is_MyName(L"2P_Ult_GaugeFrame") == true)
	{

		 if (pGameInstance->Get_DIKeyDown(DIK_8))
		 {
 			m_Minus_NowGauge = true;
 			m_NumberEffect = true;
		 }
		 m_NowPercent = m_2PNowGauge / m_MaxGauge;
  
		 if (m_Minus_NowGauge == true && m_2PNowGauge >= 0.f)
 			m_2PNowGauge -= (_float)_dTimeDelta * 40.f;
  
	}

	Safe_Release(pGameInstance);
}

void CUI_Ultimate::Ultimate_Number()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (Is_MyName(L"Ult_Gauge") == true || Is_MyName(L"Ult_Number") == true
		|| Is_MyName(L"Ult_FrameEff") == true || Is_MyName(L"Ult_FrameEff2") == true
		|| Is_MyName(L"Ult_FireEff") == true || Is_MyName(L"Ult_GaugeBase") == true || Is_MyName(L"Ult_Frame") == true
		|| Is_MyName(L"Ult_InBase") == true || Is_MyName(L"Ult_GaugeFrame") == true)
	{
		if (m_MaxGauge <= m_NowGauge && m_Number < 3) //4
		{
			m_NowGauge = 0.f; // 최대 게이지보다 크면 
			m_Number++;
			m_FrameEffect = true;

			if (m_Number <= 3)
				m_NumberEffect = true;

		}

		if (pGameInstance->Get_DIKeyDown(DIK_8) && m_Number > 0)
			m_Number--;


		if (m_Number > 3)
		{
			m_Number = 3;
			m_NowGauge = 100.f;
		}

		if (m_iPrevNumber < m_Number)
		{
			m_FrameEffect = true;
			m_NumberEffect = true;
		}


		if (Is_MyName(L"Ult_Number") == true)
			wsprintf(m_TextureName, TEXT("Ult_Num%d"), m_Number);

		m_iPrevNumber = m_Number;
	}
	
	
	//===========================================================
	

	if (Is_MyName(L"2P_Ult_Gauge") == true || Is_MyName(L"2P_Ult_Number") == true
		|| Is_MyName(L"2P_Ult_FrameEff") == true || Is_MyName(L"2P_Ult_FrameEff2") == true
		|| Is_MyName(L"2P_Ult_FireEff") == true || Is_MyName(L"2P_Ult_GaugeBase") == true || Is_MyName(L"2P_Ult_Frame") == true
		|| Is_MyName(L"2P_Ult_InBase") == true || Is_MyName(L"2P_Ult_GaugeFrame") == true)
	{
		if (m_MaxGauge <= m_2PNowGauge && m_2PNumber < 3) //4
		{
			m_2PNowGauge = 0.f; // 최대 게이지보다 크면 
			m_2PNumber++;
			m_FrameEffect = true;

			if (m_2PNumber <= 3)
				m_NumberEffect = true;

		}

		if (pGameInstance->Get_DIKeyDown(DIK_8) && m_2PNumber > 0)
			m_2PNumber--;


		if (m_2PNumber > 3)
		{
			m_2PNumber = 3;
			m_2PNowGauge = 100.f;
		}

		if (m_iPrevNumber < m_2PNumber)
		{
			m_FrameEffect = true;
			m_NumberEffect = true;
		}


		if (Is_MyName(L"2P_Ult_Number") == true)
			wsprintf(m_TextureName, TEXT("Ult_Num%d"), m_2PNumber);

		m_iPrevNumber = m_2PNumber;
	}

	Safe_Release(pGameInstance);
}

void CUI_Ultimate::Ultimate_FireEff(const _double& _dTimeDelta)
{
	m_fFrame += (_float)_dTimeDelta * 8.0f;

	if (6.0f <= m_fFrame)
		m_fFrame = 0.f;
	
	if (Is_MyName(L"Ult_FireEff") == true )//|| Is_MyName(L"2P_Ult_FireEff") == true)
	{
		if(m_Number == 1)
		wsprintf(m_TextureName, TEXT("Ult_Eff1_%d"), (_int)m_fFrame);

		if (m_Number == 2)
		wsprintf(m_TextureName, TEXT("Ult_Eff2_%d"), (_int)m_fFrame);

		if (m_Number == 3)
			wsprintf(m_TextureName, TEXT("Ult_Eff3_%d"), (_int)m_fFrame);
		
		if(m_Number == 0)
			m_fAlpha = 0.f;
		else
			m_fAlpha = 1.f;
	}

	//====================================
	if (Is_MyName(L"2P_Ult_FireEff") == true)
	{
		if (m_2PNumber == 1)
			wsprintf(m_TextureName, TEXT("Ult_Eff1_%d"), (_int)m_fFrame);

		if (m_2PNumber == 2)
			wsprintf(m_TextureName, TEXT("Ult_Eff2_%d"), (_int)m_fFrame);

		if (m_2PNumber == 3)
			wsprintf(m_TextureName, TEXT("Ult_Eff3_%d"), (_int)m_fFrame);

		if (m_2PNumber == 0)
			m_fAlpha = 0.f;
		else
			m_fAlpha = 1.f;
	}
}

void CUI_Ultimate::Ultimate_FrameEff(const _double& _dTimeDelta)
{
	if (Is_MyName(L"Ult_FrameEff") == true && m_FrameEffect == true)
	{
		m_fAlpha -= (_float)_dTimeDelta;
		m_bEffect = true;
		Grow_Combo(2.0f, 100.f, 0.f, 0.f, _dTimeDelta );
		if (m_fAlpha <= 0.f)
			m_bAlpha = false;
		if (m_bEffect == false && m_bAlpha == false)
		{
			if(m_Number != 3)
			m_FrameEffect = false;
			m_fSizeX = 65.f;
			m_fSizeY = 65.f;
			m_fAlpha = 1.0f;

		}
	}

	//=====================
	if(Is_MyName(L"2P_Ult_FrameEff") == true && m_FrameEffect == true)
	{
		m_fAlpha -= (_float)_dTimeDelta;
		m_bEffect = true;
		Grow_Combo(2.0f, 100.f, 0.f, 0.f, _dTimeDelta);
		if (m_fAlpha <= 0.f)
			m_bAlpha = false;
		if (m_bEffect == false && m_bAlpha == false)
		{
			if (m_2PNumber != 3)
				m_FrameEffect = false;
			m_fSizeX = 65.f;
			m_fSizeY = 65.f;
			m_fAlpha = 1.0f;

		}
	}

	//==========================
	if ((Is_MyName(L"Ult_Number") == true && m_NumberEffect == true )|| (Is_MyName(L"2P_Ult_Number") == true && m_NumberEffect == true))
	{
		m_bEffect = true;
		Grow_Combo(5.f, 65.f, 0.f, 0.f, _dTimeDelta *0.7f);
		if (m_fSizeX >= 65.f)
		{
			m_NumberEffect = false;
			m_fSizeX = 48.f;
			m_fSizeY = 48.f;
		}
	}
}

HRESULT CUI_Ultimate::SetUp_Components()
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

HRESULT CUI_Ultimate::SetUp_ConstantTable()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_UltPercent", &m_NowPercent, sizeof(_float))))
		return E_FAIL;
	
		if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DiffuseTexture", m_TextureName)))
			return E_FAIL;
	

	return S_OK;
}

CUI_Ultimate* CUI_Ultimate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Ultimate* pInstance = new CUI_Ultimate(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Ultimate");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI_Base* CUI_Ultimate::Clone(void* pArg)
{
	CUI_Ultimate* pInstance = new CUI_Ultimate(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Ultimate");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Ultimate::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
