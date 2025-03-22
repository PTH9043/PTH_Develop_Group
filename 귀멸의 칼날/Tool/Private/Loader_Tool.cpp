#include "Loader_Tool.h"
#include "GameInstance.h"

// jinju 
#include "MapGround_Tool.h"
#include "MapObj_Tool.h"
// kang
#include "AnimViewerModel.h"
#include "Animator.h"

// Effect
#include "EffectObject.h"
#include "ImageEffect.h"
#include "StaticMeshEffect.h"
#include "AnimMeshEffect.h"
#include "Test_Instance.h"

CLoader_Tool::CLoader_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
{
	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);
}

HRESULT CLoader_Tool::NativeConstruct(const LEVEL& _eNextLevel)
{
	m_eNextLevel = _eNextLevel;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Run_Thread, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}
_uint CLoader_Tool::Run_Thread(void* _pArg)
{
	CLoader_Tool* pLoader = (CLoader_Tool*)_pArg;
	pLoader->Running();
	return 0;
}

void CLoader_Tool::Running()
{
	EnterCriticalSection(&m_CriticalSection);
	switch (m_eNextLevel)
	{
	case LEVEL_TOOL:
	case LEVEL_REMOVE:
		Loading_ForToolLevel();
		break;
	}
	LeaveCriticalSection(&m_CriticalSection);
}

void CLoader_Tool::Add_Thread(void* _pArg)
{
	CLoader_Tool* pInstance = static_cast<CLoader_Tool*>(_pArg);
	if (nullptr != pInstance)
		pInstance->Loading_Data();
}

void CLoader_Tool::Loading_Data()
{
	if (0 == m_iThreadCount)
	{
		++m_iThreadCount;
		Load_1();
	}
	else if (1 == m_iThreadCount)
	{
		++m_iThreadCount;
		Load_2();
	}
}

void CLoader_Tool::Load_1()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// mapground sky
	{
		_matrix PivotMatrix = XMMatrixIdentity();
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		PATHS Path = { L"Model", L"MapModel", L"MapSky", L"FBX_Converter" };
		FILEGROUP* filegroup = pGameInstance->Find_Folder(&Path);
		{
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_UROKODAKIIWA_SKY, CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, filegroup, PROTO_COMP_MODEL_UROKODAKIIWA_SKY, PivotMatrix))))
				return;
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_HOKUSEIVS_SKY, CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, filegroup, PROTO_COMP_MODEL_HOKUSEIVS_SKY, PivotMatrix))))
				return;
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_STORY_SKY, CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, filegroup, PROTO_COMP_MODEL_STORY_SKY, PivotMatrix))))
				return;
			if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_RATTAN_SKY, CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, filegroup, PROTO_COMP_MODEL_RATTAN_SKY, PivotMatrix))))
				return;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CLoader_Tool::Load_2()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//{
	//	_matrix PivotMatrix = XMMatrixIdentity();
	//	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	//	PATHS Path = { L"Model", L"MapModel", L"MapSky", L"FBX_Converter" };
	//	FILEGROUP* filegroup = pGameInstance->Find_Folder(&Path);
	//	{
	//		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_MODEL_STORY_SKY, CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, filegroup, PROTO_COMP_MODEL_STORY_SKY, PivotMatrix))))
	//			return;
	//		// AnimViewerPrototype Animator Component
	//		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_TOOL, PROTO_COMP_ANIMATOR, CAnimator::Create(m_pDevice, m_pContext))))
	//			return;
	//	}
	//}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CLoader_Tool::Loading_ForToolLevel()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		pGameInstance->Add_Thread(Add_Thread, this);
		pGameInstance->Add_Thread(Add_Thread, this);
		pGameInstance->Join_Thread();
		pGameInstance->End_Threads();
	}
	RELEASE_INSTANCE(CGameInstance);
	m_bFinished = true;
	return S_OK;
}

CLoader_Tool* CLoader_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LEVEL& _eNextLevel)
{
	CLoader_Tool* pInstance = new CLoader_Tool(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct(_eNextLevel)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CLoader::Create To Failed");
	}
	return pInstance;
}

void CLoader_Tool::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	DeleteCriticalSection(&m_CriticalSection);
	CloseHandle(m_hThread);
	m_hThread = NULL;

	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pContext);
}