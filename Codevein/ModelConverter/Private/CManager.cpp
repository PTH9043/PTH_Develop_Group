#include "CManager.h"

CManager::CManager()
{
}

CManager::CManager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef(_pDevice);
	Safe_AddRef(_pContext);
}

void CManager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
