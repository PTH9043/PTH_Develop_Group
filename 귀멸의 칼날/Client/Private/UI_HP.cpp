#include "../Public/UI_HP.h"
#include "GameInstance.h"
#include <cmath> 
#include "Battle_Manager.h"
#include "CutScene_Manager.h"
#include "UI_CharSelect.h"

CUI_HP::CUI_HP(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CUI_Base(_pDevice, _pContext)
{
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());

}

CUI_HP::CUI_HP(const CUI_HP& _rhs)
    : CUI_Base(_rhs)
{
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

HRESULT CUI_HP::NativeConstruct_Prototype()
{
    return S_OK;
}

HRESULT CUI_HP::NativeConstruct_Clone(void* pArg)
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


	if (Is_MyName(L"HP_A_Life") == true)
	{
		m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);
	}

	if (Is_MyName(L"HP_A_Damage") == true)
	{
		m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);
	}

	if (Is_MyName(L"Battle_Text") == true)
		m_fAlpha = 0.f;

	if (Is_MyName(L"Round") == true)
	{
		m_fAlpha = 0.f;
		Set_Size(_float2(100.f, 100.f));
	}

	Load_SelectIndex();

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CUI_HP::Tick(const _double& _dTimeDelta)
{

	__super::Tick(_dTimeDelta);
	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_Pos(XMVectorSet(m_fX, m_fY, 0.0f, 1.f));


	//ÇÃ·¹ÀÌ¾î
	CBattle_Manager* pBattleInstance = GET_INSTANCE(CBattle_Manager);
	{
		m_pPlayerObject = (CGameObject*)pBattleInstance->Get_BattleCharacter(CBattle_Manager::CHAR_PLAYER);
		m_pAIObject = (CGameObject*)pBattleInstance->Get_BattleCharacter(CBattle_Manager::CHAR_AI);
		StartBattle = pBattleInstance->Get_BattleInfo().bBattleInit;// Get_BattleInit(); //ÇÑ ÇÁ·¹ÀÓ¸¸ true 
		CUSTIMER BattleTimer = pBattleInstance->Get_BattleInfo().tBattleTimer;//Get_BattleTimer();
		m_Number = 99-(_int)BattleTimer.fTimer;
	}
	
	RELEASE_INSTANCE(CBattle_Manager);

	pPlayer = (CPlayer*)m_pPlayerObject;
	pAI = (CPlayer*)m_pAIObject;

	if (pPlayer == nullptr || pAI == nullptr)
		return;

	CPlayer::BATTLEDATA PlayerData = pPlayer->Get_BattleData();
	fPlayerHP = PlayerData.fCurHp;
	m_fNowSkil = PlayerData.fChargeValue;

	CPlayer::BATTLEDATA AIData = pAI->Get_BattleData();
	fAIHP = AIData.fCurHp;
	m_f2PNowSkil = AIData.fChargeValue;

	m_fMaxHP = PlayerData.fMaxHp;

	//ÇÑ ÇÁ·¹ÀÓ¸¸ true

	if (StartBattle == true)
		Shack = false;
	
	else if (CBattle_Manager::Get_Instance()->Get_BattleInfo().bBattleStart/*Get_BattleStart()*/ == true)
		Shack = true;



	TickFrame += (_float)_dTimeDelta;
	
	if (TickFrame >= 0.03f)
	{
		fAIOriginHP = fAIHP;
		fPlayerOriginHP = fPlayerHP;
		EffDir.x *= -1.f;
		EffDir.y *= -1.f;
		TickFrame = 0.f;
	}
	else if (Shack == false)
	{
		if (Is_MyName(L"Battle_P_Portrait") == true)
			Set_Pos(_float2(-491.3f, 315.1f));
		
		if (Is_MyName(L"Battle_A_Portrait") == true)
			Set_Pos(_float2(477.7f, 315.4f));
		
		if (Is_MyName(L"Battle_P_Portrait") == true || Is_MyName(L"Battle_A_Portrait") == true)
		{
			EffDir = _float2(-3.f, 3.f);
			m_bPlayerEff = false;
			m_bAIEff = false;
		}

	}
	
	HP(_dTimeDelta);
	Skil(_dTimeDelta);
	Skil_2P(_dTimeDelta);
	Timer(_dTimeDelta);
	Round(_dTimeDelta); 
	//Text(_dTimeDelta);
	HP_Effect(_dTimeDelta);
	Reset_Round(_dTimeDelta);
}

void CUI_HP::Late_Tick(const _double& _dTimeDelta)
{

	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	

	
}

HRESULT CUI_HP::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (Is_MyName(L"HP_P_Life") == true || Is_MyName(L"HP_A_Life") == true ||
		Is_MyName(L"HP_P_Damage") == true || Is_MyName(L"HP_A_Damage") == true)
		m_pShaderCom->Begin(3);

	else if (Is_MyName(L"Skil_Bar") == true || Is_MyName(L"2P_Skil_Bar") == true)
		m_pShaderCom->Begin(4);
	
	else
		m_pShaderCom->Begin(1);
	
	m_pVIBufferCom->Render();

	return S_OK;
}

void CUI_HP::Extern_ImGuiClass()
{
	static int a = 10;
	ImGui::DragInt("a", &a);
	
}

void CUI_HP::HP(const _double& _dTimeDelta)
{

	//ÇÃ·¹ÀÌ¾î 
	if (Is_MyName(L"HP_P_Life") == true || Is_MyName(L"Battle_P_Portrait") == true)
	{
		if (Is_MyName(L"HP_P_Life") == true)
		{
			if (fPlayerOriginHP != fPlayerHP)
				m_bPlayerEff = true;
			
			m_fHPPercent = fPlayerHP / m_fMaxHP; //ÃÊ·Ï

			if (m_bMinusDamage == true)
				m_fDamageTime += (_float)_dTimeDelta;

			if (m_fDamageTime >= 1.f)
			{
				m_fDamageEnd = fPlayerHP / m_fMaxHP; //»¡°­ 
				m_fDamageTime = 0.f;
			}
		}
		
		if (Is_MyName(L"Battle_P_Portrait") == true)
		{
			if (fPlayerOriginHP != fPlayerHP)
			{
				m_bPlayerEff = true;
				OrginPos.x = m_fX;
				OrginPos.y = m_fY;
			}
		}
	}

	//AI
	if (Is_MyName(L"HP_A_Life") == true || Is_MyName(L"Battle_A_Portrait") == true)
	{
		if (Is_MyName(L"HP_A_Life") == true)
		{
			if (fAIOriginHP != fAIHP )
				m_bAIEff = true;

			m_fHPPercent = fAIHP / m_fMaxHP; //ÃÊ·Ï

			if (m_bMinusDamage == true)
				m_fDamageTime += (_float)_dTimeDelta;

			if (m_fDamageTime >= 1.f)
			{
				m_fDamageEnd = fAIHP / m_fMaxHP; //»¡°­ 
				m_fDamageTime = 0.f;
			}
		}

		if (Is_MyName(L"Battle_A_Portrait") == true)
		{
			if (fAIOriginHP != fAIHP)
			{
				m_bAIEff = true;
				OrginPos.x = m_fX;
				OrginPos.y = m_fY;
			}
		}
	}
	_vector total = XMVectorLerp(XMLoadFloat(&m_fDamagePercent), XMLoadFloat(&m_fDamageEnd), (_float)_dTimeDelta );
	XMStoreFloat(&m_fDamagePercent, total);
	
#ifdef ¿ø·¡ÄÚµå
	//if (Is_MyName(L"HP_P_Life") == true || Is_MyName(L"Battle_P_Portrait") == true)
	//{
	//	if (pGameInstance->Get_DIKeyDown(DIK_1))
	//	{
	//		m_fDamage += 10.f;
	//		m_bPlayerEff = true;
	//		OrginPos.x = m_fX;
	//		OrginPos.y = m_fY;
	//		m_fDamageMax += 10.f;
	//		m_fDamageTime = 0.f;
	//	} 
	//	    m_fHPPercent = (m_fHP - m_fDamage) / m_fMaxHP; //ÃÊ·Ï
	//		
	//		if (m_bMinusDamage == true)
	//			m_fDamageTime += (_float)_dTimeDelta;
	//		
	//		if(m_fDamageTime >= 2.f)
	//		m_fDamageEnd = (m_fHP - m_fDamageMax) / m_fMaxHP; //»¡°­ 
	//}

	//if (Is_MyName(L"HP_A_Life") == true || Is_MyName(L"Battle_A_Portrait") == true)
	//{
	//	if (pGameInstance->Get_DIKeyDown(DIK_3))
	//	{
	//		m_fDamage += 10.f;
	//		m_bAIEff = true;
	//		OrginPos.x = m_fX;
	//		OrginPos.y = m_fY;
	//		m_fDamageMax += 10.f;
	//		m_fDamageTime = 0.f;
	//	}
	//    m_fHPPercent = (m_fHP - m_fDamage) / m_fMaxHP;
	//	
	//	if (m_bMinusDamage == true)
	//		m_fDamageTime += (_float)_dTimeDelta;

	//	if (m_fDamageTime >= 2.f)
	//		m_fDamageEnd = (m_fHP - m_fDamageMax) / m_fMaxHP; //»¡°­ 
	//}

	//
	//_vector total = XMVectorLerp(XMLoadFloat(&m_fDamagePercent), XMLoadFloat(&m_fDamageEnd), (_float)_dTimeDelta );
	//XMStoreFloat(&m_fDamagePercent, total);

#endif

}  

void CUI_HP::Skil(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_Delay += _dTimeDelta;

	if(m_Delay >= 3.0) //½Ã°£ÃÊ Áà¼­ ¸Ø­Ÿ´Ù°¡ Â÷¿À¸£°Ô
	m_fNowSkil += (_float)(10.0 * _dTimeDelta);

	if (m_fMaxSkil < m_fNowSkil) 
		m_fNowSkil = m_fMaxSkil; // ÃÖ´ë ½ºÅ³ °ÔÀÌÁöº¸´Ù Å©¸é ¾È ³Ñ¾î°¡°Ô ÇÏ±â
	if (0.f > m_fNowSkil) 
		m_fNowSkil = 0.f; // 0º¸´Ù ÀÛÀ¸¸é 0À¸·Î

	if (Is_MyName(L"Skil_Bar") == true)
	{
		if (pGameInstance->Get_DIKeyDown(DIK_5))
		{
			m_fNowSkil -= m_fSkilCost;
			m_Delay = 0.0;
		}
		m_fSkilPercent = m_fNowSkil / m_fMaxSkil;
	}
	Safe_Release(pGameInstance);
}

void CUI_HP::Skil_2P(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	m_Delay += _dTimeDelta;

	if (m_Delay >= 3.0) //½Ã°£ÃÊ Áà¼­ ¸Ø­Ÿ´Ù°¡ Â÷¿À¸£°Ô
		m_f2PNowSkil += (_float)(10.0 * _dTimeDelta);

	if (m_fMaxSkil < m_f2PNowSkil)
		m_f2PNowSkil = m_fMaxSkil; // ÃÖ´ë ½ºÅ³ °ÔÀÌÁöº¸´Ù Å©¸é ¾È ³Ñ¾î°¡°Ô ÇÏ±â
	if (0.f > m_f2PNowSkil)
		m_f2PNowSkil = 0.f; // 0º¸´Ù ÀÛÀ¸¸é 0À¸·Î

	if (Is_MyName(L"2P_Skil_Bar") == true)
	{
		if (pGameInstance->Get_DIKeyDown(DIK_5))
		{
			m_f2PNowSkil -= m_fSkilCost;
			m_Delay = 0.0;
		}
		m_fSkilPercent = m_f2PNowSkil / m_fMaxSkil;
	}
	Safe_Release(pGameInstance);
}

void CUI_HP::HP_Effect(const _double& _dTimeDelta)
{
	/*if (Is_MyName(L"Battle_P_Portrait") == true)  // ¿­¹Þ´Â´Ù ¤Ñ¤Ñ
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
	
	if (Is_MyName(L"Battle_A_Portrait") == true)
	{
		if (m_bAIEff == true)
		{
			EffTime += (_float)_dTimeDelta;

			m_fX += EffDir.x;
			m_fY += EffDir.y;

			if (EffTime >= 0.3f)
			{
				m_bAIEff = false;
				EffTime = 0.f;

				m_fX = OrginPos.x;
				m_fY = OrginPos.y;
			}
		}
	}*/

	if (Is_MyName(L"HP_P_Life") == true)
	{
		if (m_bPlayerEff == true)
		{
			m_fAlpha = 1.6f;
			AlphaTime += (_float)_dTimeDelta;
			if (AlphaTime >= 0.2f)
			{
				m_bPlayerEff = false;
				AlphaTime = 0.f;
				m_fAlpha = 1.0f;
			}
		}
	}

	if (Is_MyName(L"HP_A_Life") == true)
	{
		if (m_bAIEff == true)
		{
			m_fAlpha = 1.6f;
			AlphaTime += (_float)_dTimeDelta;
			if (AlphaTime >= 0.2f)
			{
				m_bAIEff = false;
				AlphaTime = 0.f;
				m_fAlpha = 1.0f;
			}
		}
	}
}

void CUI_HP::Timer(const _double& _dTimeDelta)
{
	//ÃÑ Å¸ÀÌ¸Ó ½Ã°£°ú ½ÃÀÛÄ«¿îÆ®¸¦ °è»êÇÏ¿© ÅØ½ºÃÄ¸¦ ¹Ù²Û´Ù 

//Å¸ÀÌ¸Ó

	m_CountNumber10 = m_Number / 10;
	m_CountNumber1 = m_Number % 10;

	if (Is_MyName(L"Timer_Num1") == true) //1ÀÇ ÀÚ¸®¼ýÀÚ
		wsprintf(m_TextureName, TEXT("Num_Timer%d"), m_CountNumber1);


	if (Is_MyName(L"Timer_Num10") == true)//10ÀÇ ÀÚ¸®¼ýÀÚ 
	{
		if (m_CountNumber10 == 0)
			m_fAlpha = 0.f;
		else
			m_fAlpha = 1.f;

		wsprintf(m_TextureName, TEXT("Num_Timer%d"), m_CountNumber10);
	}

}

void CUI_HP::Round(const _double& _dTimeDelta)
{ 
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CPlayer::BATTLEDATA PlayerData = pPlayer->Get_BattleData();
	CPlayer::BATTLEDATA AIData = pAI->Get_BattleData();

	_int PlayerWin = PlayerData.iWinCount;
	_int AIWin = AIData.iWinCount;

	if (CBattle_Manager::Get_Instance()->Get_BattleInfo().bSabitoAI == false)
	{
		if (Is_MyName(L"Round") == true)
		{
			if (m_fSizeX == 600.f && Is_MyTexture(L"Round4") != true && Is_MyTexture(L"Round5") != true)
			{
				m_fAlpha = 0.f;
				//Set_Size(_float2(1.f, 1.f));
			}


			if ((PlayerWin >= 2 || AIWin >= 2) && EndRound == false)
			{
				pGameInstance->Play_SoundW(TEXT("UI_BattleStop"), 0.3f);
				wsprintf(m_TextureName, TEXT("Round1"));//°Å±â±îÁö 
				Set_Size(_float2(100.f, 100.f));
				m_fAlpha = 1.f;
				EndRound = true;
				Safe_Release(pGameInstance);
				return;
			}

			else if (StartBattle == true && EndRound == false)
			{
				m_fAlpha = 0.f;
				//Set_Size(_float2(100.f, 100.f));

				if (RoundCount == 0)
				{
					pGameInstance->Play_SoundW(TEXT("UI_Round1"), 0.3f);
					wsprintf(m_TextureName, TEXT("Round6"));//1Â÷Àü
				}
				else if (RoundCount == 1)
				{
					pGameInstance->Play_SoundW(TEXT("UI_BattleStop"), 0.3f);
					wsprintf(m_TextureName, TEXT("Round1"));//°Å±â±îÁö 
				}
				else if (RoundCount == 2)
				{
					pGameInstance->Play_SoundW(TEXT("UI_BattleStop"), 0.3f);
					wsprintf(m_TextureName, TEXT("Round1"));//°Å±â±îÁö 
				}
				else if (RoundCount == 3)
				{
					pGameInstance->Play_SoundW(TEXT("UI_BattleStop"), 0.3f);
					wsprintf(m_TextureName, TEXT("Round1"));//°Å±â±îÁö 
				}

				RoundCount++;
			}

			if (Is_MyTexture(L"Round6") == true || Is_MyTexture(L"Round7") == true || Is_MyTexture(L"Round8") == true)
			{
				if (m_fSizeX >= 600.f)
				{
					m_fAlpha = 0.f;
					Set_Size(_float2(1.f, 1.f));
					RoundSize = true;
				}
				else if (m_fSizeX < 600.f && RoundSize == false)
				{
					m_bEffect = true;
					m_fAlpha = 1.f;
					Grow_UI(5.f, 600.f, 0.f, 0.f, _dTimeDelta);
				}
			}

			if (Is_MyTexture(L"Round1") == true)
			{
				if (m_fSizeX >= 600.f)
				{
					m_fAlpha = 0.f;

					if (CBattle_Manager::Get_Instance()->Get_BattleInfo().bBattleStart == true)
					{
						if (2 <= PlayerData.iWinCount || 2 <= AIData.iWinCount)
						{
							pGameInstance->Play_SoundW(TEXT("UI_BattleEnd"), 0.3f);

							Set_Size(_float2(100.f, 100.f));
							wsprintf(m_TextureName, TEXT("Round2"));//½ÂºÎ°áÁ¤ 
							Safe_Release(pGameInstance);
							return;
						}

						if (RoundCount == 2)
						{
							pGameInstance->Play_SoundW(TEXT("UI_Round2"), 0.3f);

							wsprintf(m_TextureName, TEXT("Round7"));//2Â÷Àü 
							RoundSize = false;
						}
						else if (RoundCount == 3)
						{
							pGameInstance->Play_SoundW(TEXT("UI_Round3"), 0.3f);

							wsprintf(m_TextureName, TEXT("Round8"));//3Â÷Àü
							RoundSize = false;
						}
						else if (RoundCount == 4)
						{
							pGameInstance->Play_SoundW(TEXT("UI_BattleEnd"), 0.3f);

							wsprintf(m_TextureName, TEXT("UI_Round2"));//½ÂºÎ°áÁ¤ 
						}
						Set_Size(_float2(100.f, 100.f));
					}
				}
				else if (m_fSizeX < 600.f)
				{
					m_bEffect = true;
					m_fAlpha = 1.f;
					Grow_UI(5.f, 600.f, 0.f, 0.f, _dTimeDelta);
				}
			}

			if (Is_MyTexture(L"Round2") == true)
			{
				if (m_fSizeX >= 600.f)
				{
					if (AIData.iWinCount < PlayerData.iWinCount) //ÇÃ·¹ÀÌ¾î°¡ ÀÌ°åÀ»¶§
					{
						pGameInstance->Play_SoundW(TEXT("UI_BattleEndCmn"), 0.3f);
						wsprintf(m_TextureName, TEXT("Round4"));
					}

					else if (AIData.iWinCount > PlayerData.iWinCount)  //AI°¡ ÀÌ°åÀ»¶§
					{
						pGameInstance->Play_SoundW(TEXT("UI_BattleEndCmn"), 0.3f);
						wsprintf(m_TextureName, TEXT("Round5"));
					}
					m_fAlpha = 1.f;
				}
				else if (m_fSizeX < 600.f)
				{
					m_bEffect = true;
					m_fAlpha = 1.f;
					Grow_UI(5.f, 600.f, 0.f, 0.f, _dTimeDelta);
				}

			}

			if (Is_MyTexture(L"Round4") == true || Is_MyTexture(L"Round5") == true)
			{
				TextTimeAcc += _dTimeDelta;

				Set_Size(_float2(600.f, 600.f));
				Set_Pos(_float2(0.f, 0.f));

				if (TextTimeAcc > 3.0)
				{
					CBattle_Manager::Get_Instance()->Set_BattleEnd(true);
					m_fAlpha = 0.f;
				}

			}

		}
	}
	else
	{
		if (Is_MyName(L"Round") == true)
		{
			if (StartBattle == true)
				RoundRender = true;

				if (RoundRender == true && m_fSizeX >= 600.f && RoundSize == false)
				{
					
					m_fAlpha = 0.f;
					Set_Size(_float2(1.f, 1.f));
					wsprintf(m_TextureName, TEXT("Round1"));//°Å±â±îÁö 
					RoundSize = true;
				}
				 if (RoundRender == true && m_fSizeX < 600.f && RoundSize == false)
				{
					 if (SoundPlay2 == false)
					 {
						 pGameInstance->Play_SoundW(TEXT("UI_BattleStart"), 0.3f);
						 SoundPlay2 = true;
					 }
					m_bEffect = true;
					m_fAlpha = 1.f;
					Grow_UI(5.f, 600.f, 0.f, 0.f, _dTimeDelta);
				}
				 if ((PlayerWin != 0 || AIWin != 0) && RoundSize == true)
				{
					 if (SoundPlay2 == true)
					 {
						 pGameInstance->Play_SoundW(TEXT("UI_BattleStop"), 0.3f);
						 SoundPlay2 = false;

					 }
					m_bEffect = true;
					m_fAlpha = 1.f;
					Grow_UI(5.f, 600.f, 0.f, 0.f, _dTimeDelta);
					if(m_fAlpha == 0.f)
						CBattle_Manager::Get_Instance()->Set_BattleEnd(true);
				}
			
		}
	}

	//==========================================
	if (g_iCurrentLevel == LEVEL_BATTLE && StartBattle == true && SoundPlay == false)
	{
		CBattle_Manager::BATTLEMAP_TYPE mapType = CBattle_Manager::Get_Instance()->Get_BattleInfo().eBattleMap;
		//»ç¿îµå(BGM)
		if (CBattle_Manager::MAP_UROKODAKI == mapType)
			pGameInstance->Play_BGM(TEXT("UI_Ston"), 0.2f);
		else if (CBattle_Manager::MAP_HUJIKASANE == mapType)
			pGameInstance->Play_BGM(TEXT("UI_Mountain"), 0.2f);
		else if (CBattle_Manager::MAP_TAMAYO == mapType)
			pGameInstance->Play_BGM(TEXT("UI_Tamayo"), 0.2f);

		SoundPlay = true;
	}
	Safe_Release(pGameInstance);
}


void CUI_HP::Reset_Round(const _double& _dTimeDelta)
{
	//½ÂºÎ Ç¥½Ä 
	CPlayer::BATTLEDATA PlayerData = pPlayer->Get_BattleData();
	CPlayer::BATTLEDATA AIData = pAI->Get_BattleData();
	
	_int PlayerWin = PlayerData.iWinCount;
	_int AIWin = AIData.iWinCount;

	if(PlayerWin == 1 && Is_MyName(L"1P_Round0") == true)
		wsprintf(m_TextureName, TEXT("Icon_RoundOn"));
	else if (PlayerWin == 2 && Is_MyName(L"1P_Round1") == true)
		wsprintf(m_TextureName, TEXT("Icon_RoundOn"));

	if (AIWin == 1 && Is_MyName(L"2P_Round0") == true)
		wsprintf(m_TextureName, TEXT("Icon_RoundOn"));
	else if (AIWin == 2 && Is_MyName(L"2P_Round1") == true)
		wsprintf(m_TextureName, TEXT("Icon_RoundOn"));
}


void CUI_HP::Load_SelectIndex()
{
	SelectInfo info;

	//HP,½ºÅ³ °ÔÀÌÁö µî

	HANDLE    hFile = CreateFile(TEXT("../../Resources/Data/Select_Index.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return;


	DWORD    dwByte = 0;

	while (true)
	{

		ReadFile(hFile, &info, sizeof(SelectInfo), &dwByte, nullptr);

		if (0 == dwByte)
		{
			break;
		}

		PlayerIndex = info.PlayerIndex;
		AIIndex = info.AIIndex;
	}

	CloseHandle(hFile);

	CBattle_Manager* pBattleInstance = GET_INSTANCE(CBattle_Manager);
	{
		if (true == pBattleInstance->Get_BattleInfo().bSabitoAI)
		{
			PlayerIndex = 0;
			AIIndex = 2;
		}

	}
	RELEASE_INSTANCE(CBattle_Manager);

	if (Is_MyName(L"Battle_P_Name") == true)
		wsprintf(m_TextureName, TEXT("Char_Name%d"), PlayerIndex);

	if (Is_MyName(L"Battle_A_Name") == true)
		wsprintf(m_TextureName, TEXT("Char_Name%d"), AIIndex);

	if (Is_MyName(L"Battle_P_Portrait") == true)
		wsprintf(m_TextureName, TEXT("Portrait%d"), PlayerIndex);

	if (Is_MyName(L"Battle_A_Portrait") == true)
		wsprintf(m_TextureName, TEXT("Portrait%d"), AIIndex);

}

HRESULT CUI_HP::SetUp_Components()
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

HRESULT CUI_HP::SetUp_ConstantTable()
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

		/*if (FAILED(m_pShaderCom->Set_RawValue("g_TotalSkil", &m_TotalSkil, sizeof(_float))))
			return E_FAIL;*/

	    if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_HPTexture", L"HP_P_Life")))
	     	return E_FAIL;

	    if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DamageTexture", L"HP_P_Damage")))
		    return E_FAIL;
	
	if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DiffuseTexture", m_TextureName)))
		return E_FAIL;


	return S_OK;
}

CUI_HP* CUI_HP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_HP* pInstance = new CUI_HP(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_HP");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI_Base* CUI_HP::Clone(void* pArg)
{
	CUI_HP* pInstance = new CUI_HP(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_HP");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HP::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
