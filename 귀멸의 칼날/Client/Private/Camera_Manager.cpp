#include "Camera_Manager.h"
#include "Camera.h"

IMPLEMENT_SINGLETON(CCamera_Manager);

CCamera_Manager::CCamera_Manager()
{
}

CCamera* CCamera_Manager::Get_ActiveCamera()
{
	for (auto& iter : m_CameraArr)
	{
		if (true == iter.second->Is_Active())
			return iter.second;
	}

	return nullptr;
}

HRESULT CCamera_Manager::Add_Camera(const _tchar* tagCamera, CCamera* pCamera)
{
	if (nullptr != Find_Camera(tagCamera))
		return E_FAIL;

	m_CameraArr.emplace(tagCamera, pCamera);
	Safe_AddRef(pCamera);
	return S_OK;
}

void CCamera_Manager::Set_ActiveCamera(const _tchar* tagCamera)
{
	CCamera* pCamera = Find_Camera(tagCamera);
	if (nullptr == pCamera)
		return;

	for (auto& iter : m_CameraArr)
	{
		if (iter.second == pCamera)
			pCamera->Set_Active(true);
		else
			iter.second->Set_Active(false);
	}
}

void CCamera_Manager::Change_Level()
{
	for (auto& iter : m_CameraArr)
		Safe_Release(iter.second);
	m_CameraArr.clear();
}

CCamera* CCamera_Manager::Find_Camera(const _tchar* tagCamera)
{
	auto& iter = find_if(m_CameraArr.begin(), m_CameraArr.end(), CTag_Finder(tagCamera));

	if (iter == m_CameraArr.end())
		return nullptr;

	return iter->second;
}

void CCamera_Manager::Free()
{
	for (auto& iter : m_CameraArr)
		Safe_Release(iter.second);
	m_CameraArr.clear();
}
