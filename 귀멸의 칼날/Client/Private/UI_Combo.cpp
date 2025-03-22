#include "UI_Combo.h"
#include "GameInstance.h"
#include "Battle_Manager.h"
#include "Player.h"

CUI_Combo::CUI_Combo(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CUI_Base(_pDevice, _pContext)
{
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

CUI_Combo::CUI_Combo(const CUI_Combo& _rhs)
    : CUI_Base(_rhs)
{
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}
HRESULT CUI_Combo::NativeConstruct_Prototype()
{
    return S_OK;
}

HRESULT CUI_Combo::NativeConstruct_Clone(void* pArg)
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

	return S_OK;
}

void CUI_Combo::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_Pos(XMVectorSet(m_fX, m_fY, 0.0f, 1.f));
	
	CBattle_Manager* pBattleInstance = GET_INSTANCE(CBattle_Manager);

	//if (CBattle_Manager::Get_Instance()->Get_BattleStart() == true || pBattleInstance->Get_BattleInit() == true)
	if (pBattleInstance->Get_BattleInfo().bBattleStart == true || pBattleInstance->Get_BattleInfo().bBattleInit == true)
		m_fAlpha = 0.f;

	RELEASE_INSTANCE(CBattle_Manager);

	Combo_Gauge(_dTimeDelta);
}

void CUI_Combo::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if(m_fAlpha != 0.f)
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	//플레이어
	CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager);
	{
		m_pPlayerObject = (CGameObject*)pInstance->Get_BattleCharacter(CBattle_Manager::CHAR_PLAYER);
		m_p2PlayerObject = (CGameObject*)pInstance->Get_BattleCharacter(CBattle_Manager::CHAR_AI);
	}
	RELEASE_INSTANCE(CBattle_Manager);

	CPlayer* pPlayer = (CPlayer*)m_pPlayerObject;
	CPlayer* p2Player = (CPlayer*)m_p2PlayerObject;

	if (pPlayer == nullptr || p2Player == nullptr)
		return;


	CPlayer::BATTLEDATA PlayerData = pPlayer->Get_BattleData();
	m_Number = PlayerData.iHitCount;

	CPlayer::BATTLEDATA AIData = p2Player->Get_BattleData();
	m_2PNumber = AIData.iHitCount;

	Combo_Number(_dTimeDelta);
}

HRESULT CUI_Combo::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (Is_MyName(L"Combo_GaugeBase") == true || Is_MyName(L"2P_Combo_GaugeBase") == true)
		m_pShaderCom->Begin(6);
		
	else
		m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CUI_Combo::Combo_Number(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (Is_MyName(L"Combo_Base") == true || Is_MyName(L"Combo_Hit") == true || Is_MyName(L"Combo_Number") == true
		|| Is_MyName(L"Combo_GaugeBase") == true || Is_MyName(L"Combo_HUD") == true || Is_MyName(L"Combo_Number10") == true)
	{
		if (m_Number != 0 && Is_MyName(L"Combo_Number10") != true)
			m_fAlpha = 1.f;

		m_Number10 = m_Number / 10;
		m_Number1 = m_Number % 10;

		if (m_OriginNumber == m_Number)
		{
			Safe_Release(pGameInstance);
			return;
		}

		else
		{
			m_ComboGauge = true;
			m_NowAngle = 179.f;

			if (Is_MyName(L"Combo_Number") == true)
			{
				if (m_Number1 != 0)
					m_fAlpha = 1.f;
				else if (m_Number1 == 0)
					m_fAlpha = 0.f;

				m_Grow = true;
				m_fSizeX = 40.f;
				m_fSizeY = 40.f;
			}

			if (Is_MyName(L"Combo_Number10") == true)
			{
				if (m_Number10 != 0)
					m_fAlpha = 1.f;
				else if (m_Number10 == 0)
					m_fAlpha = 0.f;

				m_Grow = true;
				m_fSizeX = 40.f;
				m_fSizeY = 40.f;
			}

			m_OriginNumber = m_Number;
		}
	}

	//================================
	if (Is_MyName(L"2P_Combo_Base") == true || Is_MyName(L"2P_Combo_Hit") == true || Is_MyName(L"2P_Combo_Number") == true
		|| Is_MyName(L"2P_Combo_GaugeBase") == true || Is_MyName(L"2P_Combo_HUD") == true || Is_MyName(L"2P_Combo_Number10") == true)
	{
		if (m_2PNumber != 0 && Is_MyName(L"2P_Combo_Number10") != true)
			m_fAlpha = 1.f;

		m_Number10 = m_2PNumber / 10;
		m_Number1 = m_2PNumber % 10;


		if (m_2POriginNumber == m_2PNumber)
		{
			Safe_Release(pGameInstance);
			return;
		}

		else
		{

			m_ComboGauge = true;
			m_NowAngle = 179.f;

			if (Is_MyName(L"2P_Combo_Number") == true)
			{
				if (m_Number1 != 0)
					m_fAlpha = 1.f;
				else if (m_Number1 == 0)
					m_fAlpha = 0.f;

				m_Grow = true;
				m_fSizeX = 40.f;
				m_fSizeY = 40.f;
			}

			if (Is_MyName(L"2P_Combo_Number10") == true)
			{
				if (m_Number10 != 0)
					m_fAlpha = 1.f;
				else if (m_Number10 == 0)
					m_fAlpha = 0.f;

				m_Grow = true;
				m_fSizeX = 40.f;
				m_fSizeY = 40.f;
			}

			m_2POriginNumber = m_2PNumber;
		}
	}
	Safe_Release(pGameInstance);
}

void CUI_Combo::Combo_Gauge(const _double& _dTimeDelta)
{
	if (Is_MyName(L"Combo_Base") == true || Is_MyName(L"Combo_Hit") == true || Is_MyName(L"Combo_Number") == true
		|| Is_MyName(L"Combo_GaugeBase") == true || Is_MyName(L"Combo_HUD") == true || Is_MyName(L"Combo_Number10") == true)
	{
		if (Is_MyName(L"Combo_Number") == true)
		{
			if (m_Grow == true)
			{
				m_bEffect = true;
				Grow_Combo(5.f, 80.f, 0.f, 0.f, _dTimeDelta);
				if (m_fSizeX >= 80.f)
					m_Grow = false;
			}
			else
			{
				m_fSizeX = 50.f;
				m_fSizeY = 50.f;
			}

			wsprintf(m_TextureName, TEXT("Num_Combo%d"), m_Number1);
		}

		if (Is_MyName(L"Combo_Number10") == true)
		{

			if (m_Grow == true)
			{
				m_bEffect = true;
				Grow_Combo(5.f, 80.f, 0.f, 0.f, _dTimeDelta);
				if (m_fSizeX >= 80.f)
					m_Grow = false;
			}
			else
			{
				m_fSizeX = 50.f;
				m_fSizeY = 50.f;
			}

			wsprintf(m_TextureName, TEXT("Num_Combo%d"), m_Number10);
		}

		if (m_ComboGauge == true && m_NowAngle >= m_AngleMax)
		{
			m_Angle = XMConvertToRadians(m_NowAngle);
			m_NowAngle -= (_float)_dTimeDelta * 360.f;
		}
		
		else
		{
			m_ComboGauge = false;
			Minus_Alpha(1.0f, 0.f, 1.0f, _dTimeDelta);
			m_Number = 0;
			m_NowAngle = 179.f;
		}
	}

	//====================================================
	if (Is_MyName(L"2P_Combo_Base") == true || Is_MyName(L"2P_Combo_Hit") == true || Is_MyName(L"2P_Combo_Number") == true
		|| Is_MyName(L"2P_Combo_GaugeBase") == true || Is_MyName(L"2P_Combo_HUD") == true || Is_MyName(L"2P_Combo_Number10") == true)
	{
		if (Is_MyName(L"2P_Combo_Number") == true)
		{
			if (m_Grow == true)
			{
				m_bEffect = true;
				Grow_Combo(5.f, 80.f, 0.f, 0.f, _dTimeDelta);
				if (m_fSizeX >= 80.f)
					m_Grow = false;
			}
			else
			{
				m_fSizeX = 50.f;
				m_fSizeY = 50.f;
			}

			wsprintf(m_TextureName, TEXT("Num_Combo%d"), m_Number1);
		}

		if (Is_MyName(L"2P_Combo_Number10") == true)
		{

			if (m_Grow == true)
			{
				m_bEffect = true;
				Grow_Combo(5.f, 80.f, 0.f, 0.f, _dTimeDelta);
				if (m_fSizeX >= 80.f)
					m_Grow = false;
			}
			else
			{
				m_fSizeX = 50.f;
				m_fSizeY = 50.f;
			}

			wsprintf(m_TextureName, TEXT("Num_Combo%d"), m_Number10);
		}

		if (m_ComboGauge == true && m_NowAngle >= m_AngleMax)
		{
			m_Angle = XMConvertToRadians(m_NowAngle);
			m_NowAngle -= (_float)_dTimeDelta * 360.f;
		}

		else
		{
			m_ComboGauge = false;
			Minus_Alpha(1.0f, 0.f, 1.0f, _dTimeDelta);
			m_2PNumber = 0;
			m_NowAngle = 179.f;
		}
	}
}


HRESULT CUI_Combo::SetUp_Components()
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

HRESULT CUI_Combo::SetUp_ConstantTable()
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
	
//	if (Is_MyName(L"Combo_Base") == true || Is_MyName(L"Combo_Hit") == true || Is_MyName(L"Combo_Number") == true
	//	|| Is_MyName(L"Combo_GaugeBase") == true || Is_MyName(L"Combo_HUD") == true || Is_MyName(L"Combo_Number10") == true)
	//{
		if (FAILED(m_pShaderCom->Set_RawValue("g_Angle", &m_Angle, sizeof(_float))))
			return E_FAIL;
	//}

	//if (Is_MyName(L"2P_Combo_Base") == true || Is_MyName(L"2P_Combo_Hit") == true || Is_MyName(L"2P_Combo_Number") == true
	//	|| Is_MyName(L"2P_Combo_GaugeBase") == true || Is_MyName(L"2P_Combo_HUD") == true || Is_MyName(L"2P_Combo_Number10") == true)
	//{
	//	if (FAILED(m_pShaderCom->Set_RawValue("g_Angle", &m_2PAngle, sizeof(_float))))
	//		return E_FAIL;
	//}
	
	if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DiffuseTexture", m_TextureName)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_ColorTexture", L"Combo_Color")))
		return E_FAIL;


	return S_OK;
}

CUI_Combo* CUI_Combo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Combo* pInstance = new CUI_Combo(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Combo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI_Base* CUI_Combo::Clone(void* pArg)
{
	CUI_Combo* pInstance = new CUI_Combo(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Combo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Combo::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
