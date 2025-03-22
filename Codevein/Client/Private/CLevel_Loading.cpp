#include "CLevel_Loading.h"
#include "CLoader.h"
#include"CGameInstance.h"
#include "CLevel_Logo.h"
#include "CLevel_GamePlay.h"
#include "CLevel_Tool.h"
#include "CUserInterface.h"
#include "CData_Manager.h"
#include "CUI_LoadImg.h"
#include "CLevel_GamePlay2.h"

CLevel_Loading::CLevel_Loading(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const LEVEL & _eLevel)
	: CLevel(_pDevice, _pContext), m_eNextLevel(_eLevel)
{
}

HRESULT CLevel_Loading::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	_bool bIsTrue = false;
	BEGIN_SINGLETON(CData_Manager, pDataManager)
	{
		if(nullptr != pDataManager)
		bIsTrue = pDataManager->Is_InitLoadData();
	}
	END_SINGLETON(pDataManager);

	if (true == bIsTrue)
	{
		BEGIN_SINGLETON(CGameInstance, pGameInstance)
		{
			m_pUserInterfaceTexture = static_cast<CTexture*>(pGameInstance->Clone_Component(LEVEL_STATIC,
				PROTO_COMP_TEXTURE_LOADING));

			m_pShaderCom = static_cast<CShader*>(pGameInstance->Clone_Component(LEVEL_STATIC,
				PROTO_COMP_SHADER_VTXTEX));

			CRectTransform::RECTDESC tRectDesc;
			tRectDesc.v2Pos = _float2(_float(g_iWinSizeX) / 2.f, _float(g_iWinSizeY) / 2.f);
			tRectDesc.v2Size = _float2(_float(g_iWinSizeX), _float(g_iWinSizeY));
			XMStoreFloat4x4(&tRectDesc.ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
			m_pRectTranform = CRectTransform::Create(m_pDevice, m_pContext);
			m_pRectTranform->Set_RectDesc(tRectDesc);

			m_pVIBufferBackRect = static_cast<CVIBuffer_Rect*>(pGameInstance->Clone_Component(LEVEL_STATIC,
				PROTO_COMP_VIBUFFER_RECT));

			m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);

			if (nullptr != m_pTransformCom && nullptr != m_pRectTranform)
				m_pRectTranform->Update_Rect(m_pTransformCom, g_iWinSizeX, g_iWinSizeY, 0.9f);
		}
		END_SINGLETON(pGameInstance);
	}

	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		_float2 v2Pos = _float2(1195.f, 640.f);
		_float2 v2Size = _float2(20.f, 20.f);
		for (_uint i = 0; i < 3; ++i)
		{
			for (_uint j = 0; j < 3; ++j)
			{
				if (i == 1)
					v2Pos.x -= 21.f;

				CUserInterface::UIDESC tUIDesc = CUserInterface::UIDESC(L"T_LoadWallpaper_UI_TGS",
					v2Pos, v2Size, 0.1f);

				CUI_LoadImg* pInstance = static_cast<CUI_LoadImg*>(pGameInstance->Clone_GameObject_Add(LEVEL_LOADING,
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

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(const _double & _dTimeDelta)
{
	__super::Tick(_dTimeDelta);

	if (9 != m_iInitIndex)
	{
		if (m_fDelayTime >= 0.1f)
		{
			m_LoadingImgList[m_iInitIndex++]->Set_Active(true);
			m_fDelayTime = 0.f;
		}
		m_fDelayTime += (_float)(_dTimeDelta);
	}
	else
	{
		m_iInitIndex = 0;
	}
}

void CLevel_Loading::Late_Tick(const _double & _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	if (true == m_pLoader->Is_Finished())
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CLevel* pLevel = nullptr;
		switch (m_eNextLevel)
		{
		case LEVEL_TOOL:
			g_iCurrentLevel = LEVEL_TOOL;
			pLevel = CLevel_Tool::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_LOGO:
			g_iCurrentLevel = LEVEL_LOGO;
			pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_GAMEPLAY1:
			g_iCurrentLevel = LEVEL_GAMEPLAY1;
			pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_GAMEPLAY2:
			g_iCurrentLevel = LEVEL_GAMEPLAY2;
			pLevel = CLevel_GamePlay2::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pLevel)
		{
			Safe_Release(pGameInstance);
			return;
		}
		if (FAILED(pGameInstance->Open_Level(m_eNextLevel, pLevel)))
		{
			Safe_Release(pGameInstance);
			return;
		}

		Safe_Release(pGameInstance);
	}
}

HRESULT CLevel_Loading::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (nullptr == m_pRectTranform || nullptr == m_pTransformCom)
		return E_FAIL;

	_float4x4 ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_pRectTranform->Get_RectDesc().ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pUserInterfaceTexture->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DiffuseTexture", L"T_LoadWallpaper_UI")))
		return E_FAIL;


	m_pShaderCom->Begin(0);
	m_pVIBufferBackRect->Render();
	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const LEVEL & _eNextLevel)
{
	CLevel_Loading* pInstance = new CLevel_Loading(_pDevice, _pContext, _eNextLevel);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CLevel_Loading::Create To Failed");
	}
	return pInstance;
}

void CLevel_Loading::Free()
{
	Safe_Release(m_pUserInterfaceTexture);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRectTranform);
	Safe_Release(m_pVIBufferBackRect);
	Safe_Release(m_pTransformCom);

	Safe_Release(m_pLoader);

	for (auto& iter : m_LoadingImgList)
		Safe_Release(iter);

	__super::Free();
}
