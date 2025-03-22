#include "ConsolDebugActor.h"

CConsolDebugActor::CConsolDebugActor()
{
}

HRESULT CConsolDebugActor::NativeConstruct()
{
	return S_OK;
}

CConsolDebugActor* CConsolDebugActor::Create()
{
	CConsolDebugActor* pInstance = new CConsolDebugActor();
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CConsolDebugActor::Create To Failed");
#endif
	}
	return pInstance;
}

void CConsolDebugActor::Free()
{
	__super::Free();
}
