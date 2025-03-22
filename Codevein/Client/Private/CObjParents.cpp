#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CObjParents.h"

void CObjParents::Add_Child(CGameObject* _pGameObject)
{
	if (nullptr == _pGameObject)
		return;

	auto iter = find_if(m_ChildObj.begin(), m_ChildObj.end(), [&](auto& pair)
	{
		if (_pGameObject == pair)
			return true;

		return false;
	});

	if (m_ChildObj.end() != iter)
		return;

	m_ChildObj.push_back(_pGameObject);
}

HRESULT CObjParents::NativeConstruct()
{
	return S_OK;
}

void CObjParents::Render_List()
{
	
}

CObjParents* CObjParents::Create()
{
	CObjParents* pInstance = new CObjParents();
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CObjParents::Create");
	}
	return pInstance;
}

void CObjParents::Free()
{
	for (auto& iter : m_ChildObj)
		Safe_AddRef(iter);
}
