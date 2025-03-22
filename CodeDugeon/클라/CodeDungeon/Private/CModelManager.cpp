#include "ClientDefines.h"
#include "CModelManager.h"
#include "UGameInstance.h"
#include "UModel.h"
#include "UDevice.h"
#include "UCommand.h"

CModelManager::CModelManager()
{
}

void CModelManager::Free()
{
}


HRESULT CModelManager::CreateModelProtos(CSHPTRREF<UGameInstance> _spGameInstance, CSHPTRREF<UDevice> _spDevice)
{
	SHPTR<FILEGROUP> ModelFolder = _spGameInstance->FindFolder(L"Model");

	for (const FOLDERPAIR& Folder : ModelFolder->UnderFileGroupList)
	{
		AddModelProtosFromFile(_spGameInstance, _spDevice, Folder.second);
	}

	SHPTR<FILEGROUP> MapFolder = _spGameInstance->FindFolder(L"Map");
	for (const FOLDERPAIR& Folder : MapFolder->UnderFileGroupList)
	{
		AddModelProtosFromFile(_spGameInstance, _spDevice, Folder.second);
	}

	return S_OK;
}

HRESULT CModelManager::AddModelProtosFromFile(CSHPTRREF<UGameInstance> _spGameInstance, CSHPTRREF<UDevice> _spDevice, CSHPTRREF<FILEGROUP> _spFolder)
{
	SHPTR<FILEGROUP> ConvertFolder = _spFolder->FindGroup(L"Convert");
	if (nullptr != ConvertFolder && 0 <= ConvertFolder->FileDataList.size())
	{
		for (const FILEPAIR& File : ConvertFolder->FileDataList)
		{
			_wstring ProtoTag = L"Proto_Res_Model_";
			_wstring FileName = File.second->wstrfileName;

			ProtoTag.append(FileName);
			_spGameInstance->AddPrototype(ProtoTag, CLONETYPE::CLONE_STATIC,
				CreateConstructorNative<UModel>(_spDevice, File.second->wstrfilePath));
		}
	}
	else
	{
		// Folders 
		for (const FOLDERPAIR& Folder : _spFolder->UnderFileGroupList)
		{
			AddModelProtosFromFile(_spGameInstance, _spDevice, Folder.second);
		}
	}
	return S_OK;
}
