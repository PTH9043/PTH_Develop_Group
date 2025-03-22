#include "EngineDefine.h"
#include "UMeshFilter.h"
#include "UMethod.h"
#include "UMeshContainer.h"

UMeshFilter::UMeshFilter(CSHPTRREF<UDevice> _spDevice) : 
	UController(_spDevice)
{
}

UMeshFilter::UMeshFilter(const UMeshFilter& _rhs) : 
	UController(_rhs), m_MeshFilterInfoContainer{ _rhs.m_MeshFilterInfoContainer }
{
}

HRESULT UMeshFilter::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UMeshFilter::NativeConstruct(const _wstring& _wstrPath, const MESHCONTAINERS& _meshContainer)
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);
	_wstring str = _wstrPath;
	str.append(L"\\Filter");
	if (0 != _wmkdir(str))
	{
		str.append(L"\\");
		str.append(L"MeshFilter");
		str.append(DEFAULT_OUTFOLDEREXTENSION);
		if (false == Load(str))
		{
			m_MeshFilterInfoContainer.reserve(_meshContainer.size());
			for (auto& iter : _meshContainer)
			{
				MESHFILTERINFO MeshFilterInfo;
				MeshFilterInfo.iCurMeshIndex = iter->GetMeshIndex();
				MeshFilterInfo.iMeshGroupIndex = 0;
				m_MeshFilterInfoContainer.push_back(MeshFilterInfo);
			}
		}
	}
	return S_OK;
}

HRESULT UMeshFilter::NativeConstructClone(const VOIDDATAS& _tDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_tDatas), E_FAIL);
	return S_OK;
}

void UMeshFilter::Tick(const _double& _dTimeDelta)
{
}

_bool UMeshFilter::IsShowEnable(const _int _iMeshIndex)
{
	if (false == m_MeshFilterInfoContainer.size())
		return true;

	return m_MeshFilterInfoContainer[_iMeshIndex].isMeshFilterEnable;
}

void UMeshFilter::GroupEnable(const _int _iGroupIndex)
{
	for (auto& iter : m_MeshFilterInfoContainer)
	{
		if (iter.iCurMeshIndex == _iGroupIndex)
			iter.isMeshFilterEnable = true;
	}
}

void UMeshFilter::GroupDisable(const _int _iGroupIndex)
{
	for (auto& iter : m_MeshFilterInfoContainer)
	{
		if (iter.iCurMeshIndex == _iGroupIndex)
			iter.isMeshFilterEnable = false;
	}
}

_bool UMeshFilter::Load(const _wstring& _wstrPath)
{
	std::ifstream read{ _wstrPath, std::ios::binary };
	// 읽기가 실패했을 경우
	if (false == read.is_open())
	{
		return false;
	}
	_int size = 0;
	read.read((_char*)&size, sizeof(_int));
	if (0 == size)
		return false;

	m_MeshFilterInfoContainer.resize(size);
	for (_int i = 0; i < size; ++i)
	{
		read.read((_char*)&m_MeshFilterInfoContainer[i], sizeof(MESHFILTERINFO));
	}
	return true;
}

void UMeshFilter::Save(const _wstring& _wstrPath)
{
	_wstring str = _wstrPath;
	str.append(L"\\Filter");
	if (0 != _wmkdir(str))
	{
		str.append(L"\\");
		str.append(L"MeshFilter");
		str.append(DEFAULT_OUTFOLDEREXTENSION);

		std::ofstream save{ str, std::ios::binary };
		assert(save);

		_int size = static_cast<_int>(m_MeshFilterInfoContainer.size());
		save.write((_char*)&size, sizeof(_int));
		for (_int i = 0; i < size; ++i)
		{
			save.write((_char*)&m_MeshFilterInfoContainer[i], sizeof(MESHFILTERINFO));
		}
	}
}

void UMeshFilter::Free()
{
}
