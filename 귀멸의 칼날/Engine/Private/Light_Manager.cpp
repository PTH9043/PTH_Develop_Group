#include "Light_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLight_Manager);

CLight_Manager::CLight_Manager()
{
}

const LIGHTDESC* CLight_Manager::Get_LightDesc(const _uint& _iIndex)
{
	list<CLight*>::iterator it = m_Lights.begin();
	for (_uint i = 0; i < _iIndex; ++i)
		++it;

	if ((it) == m_Lights.end())
		return nullptr;

	return ((*it)->Get_LightDesc());
}

HRESULT CLight_Manager::Add_Light(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LIGHTDESC& _tLightDesc, CLight** _ppOut)
{
	CLight* pLight = CLight::Create(_pDevice, _pContext, _tLightDesc);
	if (nullptr == pLight)
		return E_FAIL;
	pLight->Set_Index((_uint)(m_Lights.size()));
	m_Lights.push_back(pLight);
	if (nullptr != _ppOut)
		*_ppOut = pLight;
	return S_OK;
}

HRESULT CLight_Manager::Modify_Light(const LIGHTDESC& _tLightDesc, const _uint& _iIndex)
{
	_uint iIndex = 0;
	for (list<CLight*>::iterator it = m_Lights.begin(); it != m_Lights.end(); ++it)
	{
		if (iIndex++ == _iIndex)
		{
			(*it)->Set_LightDesc(_tLightDesc);
			break;
		}
	}
	return S_OK;
}

HRESULT CLight_Manager::Active_Light(const _uint& _iIndex, const _bool& _bIsControl)
{
	_uint iIndex = 0;
	for (list<CLight*>::iterator it = m_Lights.begin(); it != m_Lights.end(); ++it)
	{
		if (iIndex++ == _iIndex)
		{
			(*it)->Set_NotCaluclation(!_bIsControl);
			break;
		}
	}
	return S_OK;
}

HRESULT CLight_Manager::Delete_Light(const _uint& _iIndex)
{
	_uint iIndex = 0;
	for (list<CLight*>::iterator it = m_Lights.begin(); it != m_Lights.end(); ++it)
	{
		if (iIndex++ == _iIndex)
		{
			Safe_Release(*it);
			m_Lights.erase(it);
			break;
		}
	}

	iIndex = 0;
	for (auto& iter : m_Lights)
	{
		iter->Set_Index(iIndex++);
	}

	return S_OK;
}

HRESULT CLight_Manager::Clear_Light()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();

	return S_OK;
}

HRESULT CLight_Manager::Render_Lights(CShader* _pShader, CVIBuffer_Rect* _pRect)
{
	if (nullptr == _pShader)
		return E_FAIL;
	for (auto& iter : m_Lights)
	{
		if (nullptr != iter)
		{
			if (FAILED(iter->Render(_pShader, _pRect)))
				return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT	CLight_Manager::Save_LightsData(const _wstring& _strName)
{
	_wstring Path = L"..\\..\\Resources\\Data\\Light\\";

	Path.append(_strName.c_str());
	Path.append(L".bin");

	_ulong dwByte = 0;
	HANDLE hFile = CreateFile(Path.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	for (auto& iter : m_Lights)
	{
		LIGHTDESC tLightDesc = *iter->Get_LightDesc();

		WriteFile(hFile, &tLightDesc, sizeof(LIGHTDESC), &dwByte, nullptr);
	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT	CLight_Manager::Load_LightsData(const _wstring& _strName, ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	_wstring Path = L"..\\..\\Resources\\Data\\Light\\";

	Path.append(_strName.c_str());
	Path.append(L".bin");

	HANDLE hFile = CreateFile(Path.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	if (FAILED(Clear_Light()))
		return E_FAIL;

	_ulong dwByte = 0;

	LIGHTDESC tLightDesc;
	ZeroMemory(&tLightDesc, sizeof(LIGHTDESC));
	while (true)
	{
		ReadFile(hFile, &tLightDesc, sizeof(LIGHTDESC), &dwByte, nullptr);

		if (0 == dwByte)
			break;
		Add_Light(_pDevice, _pContext, tLightDesc, nullptr);
	}
	CloseHandle(hFile);

	return S_OK;
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}
