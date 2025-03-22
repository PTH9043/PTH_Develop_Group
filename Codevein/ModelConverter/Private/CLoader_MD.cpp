#include "CLoader_MD.h"
#include "CGameInstance.h"
#include "CData_Manager_MD.h"

CLoader_MD::CLoader_MD(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLoader_MD::NativeConstruct(const LEVEL & _eNextLevel)
{
	m_eNextLevel = _eNextLevel;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Run_Thread, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

_uint APIENTRY CLoader_MD::Run_Thread(void * _pArg)
{
	CLoader_MD*		pLoader = (CLoader_MD*)_pArg;
	pLoader->Running();
	return 0;
}

void CLoader_MD::Running()
{	/* 자원로드할꺼야. */

	EnterCriticalSection(&m_CriticalSection);
	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		Loading_ForLogoLevel();
		break;
	}
	LeaveCriticalSection(&m_CriticalSection);
}

HRESULT CLoader_MD::Loading_ForLogoLevel()
{
#ifdef _DEBUG
	cout << "Loading For Logo Level" << endl;
#endif

	BEGIN_SINGLETON(CData_Manager_MD, pDataManager)
	{
		pDataManager->Load_Data_FBX();
	//	pDataManager->Load_Data_NonAnim();
		
	}
	END_SINGLETON(pDataManager);
	m_bFinished = true;
	return S_OK;
}

CLoader_MD * CLoader_MD::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const LEVEL & _eNextLevel)
{
	CLoader_MD* pInstance = new CLoader_MD(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct(_eNextLevel)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CLoader::Create To Failed");
	}
	return pInstance;
}

void CLoader_MD::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	DeleteCriticalSection(&m_CriticalSection);
	CloseHandle(m_hThread);
	m_hThread = NULL;

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
