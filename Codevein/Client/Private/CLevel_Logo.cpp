#include "CLevel_Logo.h"
#include"CGameInstance.h"
#include "CBackground.h"
#include "CText.h"
#include "CButton.h"
#include "CLevel_Loading.h"
#include "CData_Manager.h"
#include "CUI_LoadImg.h"

CLevel_Logo::CLevel_Logo(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Logo::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Text(LAYER_TEXT)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Button(LAYER_BUTTON)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Background(LAYER_BACKGROUND)))
		return E_FAIL;

	if (FAILED(Ready_LoadingImg()))
		return E_FAIL;

	g_iCurrentLevel = LEVEL_LOGO;
	return S_OK;
}

void CLevel_Logo::Tick(const _double & _dDeltaSeconds)
{
	_bool bIsTrue = false;
	BEGIN_SINGLETON(CData_Manager, pDataManager)
	{
		bIsTrue = pDataManager->Is_InitLoad_End();
	}
	END_SINGLETON(pDataManager);

	if (true == bIsTrue)
	{
		for (_uint i = 0; i < m_iInitIndex; ++i)
			m_LoadingImgList[i]->Set_Active(false);

		m_iInitIndex = 0;

		BEGIN_SINGLETON(CGameInstance, pGameInstance);
		{
			if (pGameInstance->Get_DIKeyDown(DIK_RETURN))
			{
				if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY1))))
				{
					END_SINGLETON(pGameInstance);
					return;
				}
			}
			else if (pGameInstance->Get_DIKeyDown(DIK_SPACE))
			{
				if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TOOL))))
				{
					END_SINGLETON(pGameInstance);
					return;
				}
			}
		}
		END_SINGLETON(pGameInstance);
	}
	else
	{
		if (9 > m_iInitIndex)
		{
			if (m_fDelayTime >= 0.1f)
			{
				m_LoadingImgList[m_iInitIndex++]->Set_Active(true);
				m_fDelayTime = 0.f;
			}
			m_fDelayTime += (_float)(_dDeltaSeconds);
		}
		else
		{
			m_iInitIndex = 0;
		}
	}
}

void CLevel_Logo::Late_Tick(const _double & _dDeltaSeconds)
{
}

HRESULT CLevel_Logo::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Background(const _tchar * _pLayerTag)
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		CUserInterface::UIDESC tUIDesc = CUserInterface::UIDESC(L"T_TitleWallpaper_UI", 
			_float2((_float)(g_iWinSizeX >> 1), (_float)(g_iWinSizeY >> 1)), _float2((_float)g_iWinSizeX, (_float)g_iWinSizeY), 0.99f);
		m_pBackground = pGameInstance->Clone_GameObject_Add(LEVEL_LOGO, _pLayerTag, PROTO_GOB_BACKGROUND, &tUIDesc);
		Safe_AddRef(m_pBackground);
	}
	END_SINGLETON(pGameInstance);

	if (nullptr == m_pBackground)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Button(const _tchar* _pLayerTag)
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		// Start
		{
			CButton::BUTTONDESC tButtonDesc;
			tButtonDesc.tUIDesc = CUserInterface::UIDESC(L"Start_Text",
				_float2((_float)(g_iWinSizeX >> 1), 400.f), _float2(120.f, 80.f), 0.1f);

			tButtonDesc.strOtherImgName = L"Start_Text_Transper";

			m_pStartButton = pGameInstance->Clone_GameObject_Add(LEVEL_LOGO, _pLayerTag, PROTO_GOB_BUTTON, &tButtonDesc);
			Safe_AddRef(m_pStartButton);
		}
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Text(const _tchar * _pLayerTag)
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		CUserInterface::UIDESC tUIDesc = CUserInterface::UIDESC(L"T_TitleLogo_UI",
			_float2((_float)(g_iWinSizeX >> 1), 200.f), _float2(900.f, 250.f), 0.1f);
		m_pTitleText = pGameInstance->Clone_GameObject_Add(LEVEL_LOGO, _pLayerTag, PROTO_GOB_TEXT, &tUIDesc);
		Safe_AddRef(m_pTitleText);
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

HRESULT CLevel_Logo::Ready_LoadingImg()
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		_float2 v2Pos = _float2(1195.f, 640.f);
		_float2 v2Size = _float2(20.f, 20.f);
		for (_uint i = 0; i < 3; ++i)
		{
			for (_uint j = 0; j< 3; ++j)
			{
				if (i == 1)
					v2Pos.x -= 21.f;

				CUserInterface::UIDESC tUIDesc = CUserInterface::UIDESC(L"T_LoadWallpaper_UI_TGS",
					v2Pos, v2Size, 0.1f);

				CUI_LoadImg* pInstance = static_cast<CUI_LoadImg*>(pGameInstance->Clone_GameObject_Add(LEVEL_LOGO,
					LAYER_UI, PROTO_GOB_UI_LOADINGIMG, &tUIDesc));
				pInstance->Set_Active(false);
				Safe_AddRef(pInstance);

				m_LoadingImgList.push_back(pInstance);

				if (i != 1)
					v2Pos.x += 21.f;
			}
			v2Pos.y += 21.f;
		}
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

void CLevel_Logo::Update_LoadImg(const _double& _dTimeDelta)
{

}

CLevel * CLevel_Logo::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CLevel_Logo::Create To Failed");
	}
	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();

	Safe_Release(m_pTitleText);
	Safe_Release(m_pStartButton);
	Safe_Release(m_pBackground);

	for (auto& iter : m_LoadingImgList)
		Safe_Release(iter);
}
