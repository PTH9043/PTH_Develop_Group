#include "UI_Boss.h"
#include "GameInstance.h"
#include <cmath> 
#include "Battle_Manager.h"

CUI_Boss::CUI_Boss(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CUI_Base(_pDevice, _pContext)
{
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

CUI_Boss::CUI_Boss(const CUI_Boss& _rhs)
    : CUI_Base(_rhs)
{
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

HRESULT CUI_Boss::NativeConstruct_Prototype()
{
    return S_OK;
}

HRESULT CUI_Boss::NativeConstruct_Clone(void* pArg)
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

	if (Is_MyName(L"O_HP_Life") == true)
	{
		m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);
	}

	if (Is_MyName(L"O_HP_Damage") == true)
	{
		m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);
	}

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CUI_Boss::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_Pos(XMVectorSet(m_fX, m_fY, 0.0f, 1.f));

	//ÇÃ·¹ÀÌ¾î
	CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager);
	{
		m_pPlayerObject = (CGameObject*)pInstance->Get_BattleCharacter(CBattle_Manager::CHAR_PLAYER);
		m_pOniObject = (CGameObject*)pInstance->Get_BattleCharacter(CBattle_Manager::CHAR_AI);
	}
	RELEASE_INSTANCE(CBattle_Manager);

	pPlayer = (CPlayer*)m_pPlayerObject;
	pOni = (CPlayer*)m_pOniObject;

	if (pPlayer == nullptr || pOni == nullptr)
		return;

	CPlayer::BATTLEDATA PlayerData = pPlayer->Get_BattleData();
	m_fPlayerHP = PlayerData.fCurHp;
	m_fNowSkil = PlayerData.fChargeValue;

	CPlayer::BATTLEDATA AIData = pOni->Get_BattleData();
	m_fOniHP = AIData.fCurHp;

	TickFrame += (_float)_dTimeDelta;

	if (TickFrame >= 0.03f)
	{
		EffDir.x *= -1.f;
		EffDir.y *= -1.f;
		TickFrame = 0.f;
	}
	
	HP(_dTimeDelta);
	Skil(_dTimeDelta);
}

void CUI_Boss::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	HP_Effect(_dTimeDelta);
}

HRESULT CUI_Boss::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (Is_MyName(L"P_HP_Life") == true || Is_MyName(L"O_HP_Life") == true ||
		Is_MyName(L"P_HP_Damage") == true || Is_MyName(L"O_HP_Damage") == true)
		m_pShaderCom->Begin(3);

	else if (Is_MyName(L"Skil_Gauge") == true)
		m_pShaderCom->Begin(4);

	else
		m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CUI_Boss::HP(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (Is_MyName(L"P_HP_Life") == true)
	{
		m_dDamageTime += _dTimeDelta;

		if (m_dDamageTime >= 1.0)
		{
			m_fDamageEnd = m_fPlayerHP / m_fMaxHP;
			m_dDamageTime = 0.0;
		}
	}
	
	if (Is_MyName(L"O_HP_Life") == true)
	{
		m_dDamageTime += _dTimeDelta;

		if (m_dDamageTime >= 1.0)
		{
			m_fDamageEnd = m_fOniHP / m_fMaxHP;
			m_dDamageTime = 0.0;
		}
	}

	_vector total = XMVectorLerp(XMLoadFloat(&m_fDamagePercent), XMLoadFloat(&m_fDamageEnd), (_float)_dTimeDelta);
	XMStoreFloat(&m_fDamagePercent, total);

	//ÇÃ·¹ÀÌ¾î 
	if (Is_MyName(L"P_HP_Life") == true || Is_MyName(L"P_Portrait") == true)
	{
		if (m_fPlayerOriginHP == m_fPlayerHP)
		{
			Safe_Release(pGameInstance);
			return;
		}
		else
		{
			if (Is_MyName(L"P_HP_Life") == true)
			{
				m_bPlayerEff = true;
				m_fHPPercent = m_fPlayerHP / m_fMaxHP; //ÃÊ·Ï
			}

			if (Is_MyName(L"P_Portrait") == true)
			{
				if (m_fPlayerOriginHP != m_fPlayerHP)
				{
					m_bPlayerEff = true;
					OrginPos.x = m_fX;
					OrginPos.y = m_fY;
				}
			}
			m_fPlayerOriginHP = m_fPlayerHP;
		}
	}

	//º¸½º
	if (Is_MyName(L"O_HP_Life") == true || Is_MyName(L"O_Portrait") == true)
	{
		if (m_fOniOriginHP == m_fOniHP)
		{
			Safe_Release(pGameInstance);
			return;
		}
		else
		{
			if (Is_MyName(L"O_HP_Life") == true)
			{
				m_bOniEff = true;
				m_fHPPercent = m_fOniHP / m_fMaxHP; //ÃÊ·Ï
			}
			if (Is_MyName(L"O_Portrait") == true)
			{
				if (m_fOniOriginHP != m_fOniHP)
				{
					m_bOniEff = true;
					OrginPos.x = m_fX;
					OrginPos.y = m_fY;
				}
			}
			m_fOniOriginHP = m_fOniHP;
		}
	}

	Safe_Release(pGameInstance);
}

void CUI_Boss::HP_Effect(const _double& _dTimeDelta)
{
	/*if (Is_MyName(L"P_Portrait") == true)
	{
		if (m_bPlayerEff == true)
		{
			EffTime += (_float)_dTimeDelta;

			m_fX += EffDir.x;
			m_fY += EffDir.y;

			if (EffTime >= 0.3f)
			{
				m_bPlayerEff = false;
				EffTime = 0.f;

				m_fX = OrginPos.x;
				m_fY = OrginPos.y;
			}
		}
	}

	if (Is_MyName(L"O_Portrait") == true)
	{
		if (m_bOniEff == true)
		{
			EffTime += (_float)_dTimeDelta;

			m_fX += EffDir.x;
			m_fY += EffDir.y;

			if (EffTime >= 0.3f)
			{
				m_bOniEff = false;
				EffTime = 0.f;

				m_fX = OrginPos.x;
				m_fY = OrginPos.y;
			}
		}
	}*/

	if (Is_MyName(L"P_HP_Life") == true)
	{
		if (m_bPlayerEff == true)
		{
			m_fAlpha = 1.6f;
			AlphaTime += (_float)_dTimeDelta;
			if (AlphaTime >= 0.3f)
			{
				m_bPlayerEff = false;
				AlphaTime = 0.f;
				m_fAlpha = 1.0f;
			}
		}
	}

	if (Is_MyName(L"O_HP_Life") == true)
	{
		if (m_bOniEff == true)
		{
			m_fAlpha = 1.6f;
			AlphaTime += (_float)_dTimeDelta;
			if (AlphaTime >= 0.3f)
			{
				m_bOniEff = false;
				AlphaTime = 0.f;
				m_fAlpha = 1.0f;
			}
		}
	}
}

void CUI_Boss::Skil(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_Delay += _dTimeDelta;

	if (m_Delay >= 3.0) //½Ã°£ÃÊ Áà¼­ ¸Ø­Ÿ´Ù°¡ Â÷¿À¸£°Ô
		m_fNowSkil += (_float)(10.0 * _dTimeDelta);

	if (m_fMaxSkil < m_fNowSkil)
		m_fNowSkil = m_fMaxSkil; // ÃÖ´ë ½ºÅ³ °ÔÀÌÁöº¸´Ù Å©¸é ¾È ³Ñ¾î°¡°Ô ÇÏ±â
	if (0.f > m_fNowSkil)
		m_fNowSkil = 0.f; // 0º¸´Ù ÀÛÀ¸¸é 0À¸·Î

	if (Is_MyName(L"Skil_Gauge") == true)
	{
		if (pGameInstance->Get_DIKeyDown(DIK_1)) //Å×½ºÆ® ¿ë
		{
			m_fNowSkil -= m_fSkilCost;
			m_Delay = 0.0;
		}
		m_fSkilPercent = m_fNowSkil / m_fMaxSkil;
	}
	Safe_Release(pGameInstance);
}

HRESULT CUI_Boss::SetUp_Components()
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

HRESULT CUI_Boss::SetUp_ConstantTable()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_HPPercent", &m_fHPPercent, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_DamagePercent", &m_fDamagePercent, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_SkilPercent", &m_fSkilPercent, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_HPTexture", L"HP_P_Life")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DamageTexture", L"HP_P_Damage")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DiffuseTexture", m_TextureName)))
		return E_FAIL;


	return S_OK;
}

CUI_Boss* CUI_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Boss* pInstance = new CUI_Boss(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Boss");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI_Base* CUI_Boss::Clone(void* pArg)
{
	CUI_Boss* pInstance = new CUI_Boss(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Boss");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
