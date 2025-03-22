#include "Texture.h"
#include "Shader.h"
#include "FilePath_Manager.h"

CTexture::CTexture(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CTexture::CTexture(const CTexture& _rhs)
	: CComponent(_rhs),
	m_SRVs(_rhs.m_SRVs), m_TexNames(_rhs.m_TexNames)
{
	for (auto& iter : m_SRVs)
		Safe_AddRef_DV(iter);
}

ID3D11ShaderResourceView* CTexture::Find_Texture(const _tchar* _pTextureName)
{
	if (lstrcmp(_pTextureName, m_strFileName.c_str()))
	{
		auto iter = find_if(m_TexNames.begin(), m_TexNames.end(), [&](auto& Pair)->bool
		{
			if (!lstrcmp(Pair.first.c_str(), _pTextureName))
				return true;

			return false;
		});
		if (m_TexNames.end() == iter)
			return nullptr;

		return m_SRVs[m_iTextureIndex];
	}
	return m_pCurTexture;
}

ID3D11ShaderResourceView* CTexture::Find_Texture(const _uint& _iIndex)
{
	if (m_SRVs.size() <= _iIndex)
		return nullptr;

	if (m_SRVs.size() <= 0)
		return nullptr;

	return m_SRVs[_iIndex];
}

_wstring CTexture::Get_TextureName(const _uint& _iIndex)
{
	_wstring str = L"";

	if (m_SRVs.size() <= _iIndex)
		return L"";

	if (m_SRVs.size() <= 0)
		return L"";

	for (auto& iter : m_TexNames)
	{
		if (_iIndex == iter.second)
		{
			str = iter.first;
			break;
		}
	}
	return str;
}

HRESULT CTexture::NativeConstruct_ProtoType(FILEGROUP* _pFileDesc)
{
	if (nullptr == _pFileDesc)
		return E_FAIL;

	Load_Texture(_pFileDesc);
	m_pFileGroup = _pFileDesc;
	if (m_SRVs.size() <= 0)
		return E_FAIL;


	return S_OK;
}

HRESULT CTexture::NativeConstruct_ProtoType(PATHS* _pFolderList)
{
	FILEGROUP* pFileDesc = nullptr;
	CFilePath_Manager* pInstance = GET_INSTANCE(CFilePath_Manager);
	{
		pFileDesc = pInstance->Find_Folder(_pFolderList);
	}
	RELEASE_INSTANCE(CFilePath_Manager);
	if (nullptr == pFileDesc)
	{
		MSG_BOX("CTexture::NativeConstruct_ProtoType Load To Texture Failed");
		return E_FAIL;
	}
	m_pFileGroup = pFileDesc;
	Load_Texture(pFileDesc);

	if (m_SRVs.size() <= 0)
		return E_FAIL;

	return S_OK;
}

HRESULT CTexture::NativeConstruct_ProtoType(const _wstring& _strPath)
{
	ID3D11ShaderResourceView* _pSRV = nullptr;
	HRESULT			hResult = 0;

	_tchar			szExt[MAX_PATH] = TEXT("");
	_wsplitpath_s(_strPath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
	if (false == lstrcmp(szExt, L".dds"))
		hResult = DirectX::CreateDDSTextureFromFile(m_pDevice, _strPath.c_str(), nullptr, &_pSRV);
	else
		hResult = DirectX::CreateWICTextureFromFile(m_pDevice, _strPath.c_str(), nullptr, &_pSRV);

	if (FAILED(hResult))
		return E_FAIL;

	m_TexNames.emplace(pair<_wstring, _uint>(_strPath, 0));
	m_SRVs.push_back(_pSRV);
	return S_OK;
}

HRESULT CTexture::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(_pArg)))
		return E_FAIL;

	m_strFileName = L"DDDDDDDDDDDDDDDDD";
	m_bCloned = true;
	return S_OK;
}

HRESULT CTexture::SetUp_ConstantTable_Index(CShader* _pShader, const char* _pConstantName, const _uint& _iTextureIndex)
{
	if (nullptr == _pShader)
		return E_FAIL;
	m_pCurTexture = m_SRVs[_iTextureIndex];
	return _pShader->Set_SRV(_pConstantName, m_SRVs[_iTextureIndex]);
}

HRESULT CTexture::SetUp_ConstantTable_FileName(CShader* _pShader, const char* _pConstantName, const _tchar* _pFileName)
{
	if (nullptr == _pShader)
		return E_FAIL;

	if (lstrcmp(_pFileName, m_strFileName.c_str()))
	{
		auto iter = find_if(m_TexNames.begin(), m_TexNames.end(), [&](auto& Pair)->bool
		{
			if (!lstrcmp(Pair.first.c_str(), _pFileName))
				return true;

			return false;
		});
		if (m_TexNames.end() == iter)
			return E_FAIL;
		m_iTextureIndex = iter->second;
		m_strFileName = _pFileName;
	}
	m_pCurTexture = m_SRVs[m_iTextureIndex];
	return _pShader->Set_SRV(_pConstantName, m_SRVs[m_iTextureIndex]);
}

void CTexture::Load_Texture(FILEGROUP* _pFileGroup)
{
	if (nullptr == _pFileGroup)
		return;

	for (auto& iter : _pFileGroup->vecFileDataList)
	{
		_tchar			szExt[MAX_PATH] = TEXT("");
		_wsplitpath_s(iter.strFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
		// Path
		ID3D11ShaderResourceView* _pSRV = nullptr;
		HRESULT			hResult = 0;

		if (false == lstrcmp(szExt, L".dds"))
			hResult = DirectX::CreateDDSTextureFromFile(m_pDevice, iter.strFilePath.c_str(), nullptr, &_pSRV);
		else
			hResult = DirectX::CreateWICTextureFromFile(m_pDevice, iter.strFilePath.c_str(), nullptr, &_pSRV);

		if (FAILED(hResult))
			continue;

		m_TexNames.insert(pair<_wstring, _uint>(iter.strFileName, (_uint)(m_SRVs.size())));
		m_SRVs.push_back(_pSRV);
	}

	for (auto& iter : _pFileGroup->lsUnderList)
	{
		Load_Texture(iter);
	}
}

CTexture* CTexture::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, FILEGROUP* _pFileDesc)
{
	CTexture* pInstance = new CTexture(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_ProtoType(_pFileDesc)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CTexture::Create to Failed, FILEDESC");
	}
	return pInstance;
}

CTexture* CTexture::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, PATHS* _pFolderList)
{
	CTexture* pInstance = new CTexture(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_ProtoType(_pFolderList)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CTexture::Create to Failed, const _tchar");
	}
	return pInstance;
}

CTexture* CTexture::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _wstring& _strPath)
{
	CTexture* pInstance = new CTexture(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_ProtoType(_strPath)))
	{
		Safe_Release(pInstance);
		//MSG_BOX("CTexture::Create to Failed, Path");
	}
	return pInstance;
}

CComponent* CTexture::Clone(void* _pArg)
{
	CTexture* pInstance = new CTexture(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CTexture::Clone to Failed");
	}
	return pInstance;
}

void CTexture::Free()
{
	for (auto& iter : m_SRVs)
		Safe_Release_DV(iter);

	__super::Free();
}
