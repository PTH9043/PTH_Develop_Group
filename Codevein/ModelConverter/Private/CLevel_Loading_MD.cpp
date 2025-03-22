#include "CLevel_Loading_MD.h"
#include "CLoader_MD.h"
#include"CGameInstance.h"
#include "CLevel_Logo_MD.h"


CLevel_Loading_MD::CLevel_Loading_MD(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const LEVEL & _eLevel)
	: CLevel(_pDevice, _pContext), m_eNextLevel(_eLevel)
{
}

HRESULT CLevel_Loading_MD::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	m_pLoader = CLoader_MD::Create(m_pDevice, m_pContext, m_eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading_MD::Tick(const _double & _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CLevel_Loading_MD::Late_Tick(const _double & _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	if (true == m_pLoader->Is_Finished())
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		CLevel* pLevel = nullptr;

		switch (m_eNextLevel)
		{
		case LEVEL_LOGO:
			pLevel = CLevel_Logo_MD::Create(m_pDevice, m_pContext);
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

HRESULT CLevel_Loading_MD::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	return S_OK;
}

CLevel_Loading_MD * CLevel_Loading_MD::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const LEVEL & _eNextLevel)
{
	CLevel_Loading_MD* pInstance = new CLevel_Loading_MD(_pDevice, _pContext, _eNextLevel);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CLevel_Loading::Create To Failed");
	}
	return pInstance;
}

void CLevel_Loading_MD::Free()
{
	Safe_Release(m_pLoader);
	__super::Free();
}
