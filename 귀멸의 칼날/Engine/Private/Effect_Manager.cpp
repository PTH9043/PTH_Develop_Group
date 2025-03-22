#include "Effect_Manager.h"
#include "GameInstance.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CEffect_Manager)

CEffect_Manager::CEffect_Manager()
{
}

HRESULT CEffect_Manager::NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	m_pDevice = _pDevice;
	m_pContext = _pContext;

	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);

	return S_OK;
}

HRESULT CEffect_Manager::Add_Effect(_uint iLevelIndex, wstring pTag, CGameObject* pEffect)
{
	CGameObject* pEffectObject = Fine_Effect(iLevelIndex, pTag);
	if (nullptr != pEffectObject)
		return E_FAIL;

	m_mapEffects.emplace(pTag, pEffect);
	Safe_AddRef(pEffect);

	return S_OK;
}

CGameObject* CEffect_Manager::Fine_Effect(_uint iLevelIndex, wstring pTag)
{
	auto& Pair = find_if(m_mapEffects.begin(), m_mapEffects.end(), CTag_Finder_WS(pTag.c_str()));
	if (Pair == m_mapEffects.end())
		return nullptr;

	return Pair->second;
}

CGameObject* CEffect_Manager::Get_Effect(_uint iLevelIndex, wstring pTag)
{
	CGameObject* pEffectObject = Fine_Effect(iLevelIndex, pTag);
	if (nullptr == pEffectObject)
		return nullptr;

	return pEffectObject;
}

HRESULT CEffect_Manager::Load_Effect()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	HANDLE		hFile = nullptr;

	PATHS vecPath = { L"Data",L"Effect" };
	FILEGROUP* pEffectFolder = pGameInstance->Find_Folder(&vecPath);

	_int iCount = 3;

	for (auto& iter : pEffectFolder->vecFileDataList)
	{
		EFFECTDESC	tEffectObjectDesc;
		EFFECTDESC	tEffectDesc;
		INSTANCEDESC tInstanceEffectDesc;
		ZeroMemory(&tEffectObjectDesc, sizeof(EFFECTDESC));
		ZeroMemory(&tEffectDesc, sizeof(EFFECTDESC));
		ZeroMemory(&tInstanceEffectDesc, sizeof(INSTANCEDESC));

		_tchar szFileName[MAX_PATH] = L"";
		_int iFileNameSize = 0;

		_uint iEffectObjectCount = 0;
		_int iEffectCount = 0;
		_int iEffectType = CEffect::EFFECT_END;

		_tchar* pFullPath = nullptr;
		_tchar szFullPath[MAX_PATH] = L"";
		_int iPathSize = 0;
		_int iTextureIndex = 0;

		_tchar* pNoiseFullPath = nullptr;
		_tchar szNoiseFullPath[MAX_PATH] = L"";
		_int iNoisePathSize = 0;
		_int iNoiseIndex = 0;

		_tchar* pMeshFullPath = nullptr;
		_tchar szMeshFullPath[MAX_PATH] = L"";
		_int iMeshPathSize = 0;
		_int iMeshIndex = 0;


		_wstring strFileName = iter.strFileName;

		_wstring strFilePath = L"../../Resources/Data/Effect/";
		strFilePath += strFileName;
		strFilePath += L".bin";

		//Release();		// 기존에 있던 타일 정보를 소멸하고 불러들이는 데이터로 재생성하기 위함

		hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (nullptr == hFile)
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		DWORD	dwByte = 0;

		// 파일 이름 사이즈, 파일 이름 불러오기
		ReadFile(hFile, &iFileNameSize, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &szFileName, iFileNameSize * 2, &dwByte, nullptr);

		// 이펙트 오브젝트 안의 이펙트들 갯수를 로드
		ReadFile(hFile, &iEffectCount, sizeof(_int), &dwByte, nullptr);

		// 이펙트 오브젝트의 데이터 로드
		ReadFile(hFile, &tEffectObjectDesc, sizeof(EFFECTDESC), &dwByte, nullptr);

		// 클라이언트에서 로드 할때는 Play 를 바로 실행할 수 있도록 true 로 설정 해준다.
		tEffectObjectDesc.bPlay = true;

		// 이펙트 오브젝트 객체 생성
		CGameObject* pEffectObject = pGameInstance->Clone_GameObject_Add(0, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectObjectDesc);
		if (nullptr == pEffectObject)
		{
			MSG_BOX("이펙트 오브젝트 생성 실패");
			return E_FAIL;
		}

		// 프로젝트 타입을 설정해준다.
		static_cast<CEffectObject*>(pEffectObject)->Set_ProjectType(CEffectObject::PROJECT_CLIENT);
		// 클라이언트에서 이펙트를 불러오기 위해서 이름을 지정해준다.
		static_cast<CEffectObject*>(pEffectObject)->Set_Name(strFileName);

		for (_int i = 0; i < iEffectCount; ++i)
		{
			// 이펙트 데이터 로드
			ReadFile(hFile, &tEffectDesc, sizeof(EFFECTDESC), &dwByte, nullptr);
			// 이펙트 타입 로드
			ReadFile(hFile, &iEffectType, sizeof(_int), &dwByte, nullptr);

			if (CEffect::EFFECT_IMAGE == iEffectType)
			{
				ReadFile(hFile, &iPathSize, sizeof(_int), &dwByte, nullptr);
				ReadFile(hFile, &szFullPath, iPathSize * 2, &dwByte, nullptr);
				ReadFile(hFile, &iTextureIndex, sizeof(_int), &dwByte, nullptr);
				//pFullPath = new _tchar[iPathSize];
				//pFullPath = szFullPath;
				//ReadFile(hFile, &pFullPath, iPathSize * 2, &dwByte, nullptr);
				//m_StrList.push_back(pFullPath);

				CEffect* pEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(0, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Image"), &tEffectDesc);
				if (nullptr != pEffect)
				{
					//pEffect->Client_Init_Texture(CEffect::TEXTURE_DIFFUSE, szFullPath);
					pEffect->Init_Texture();
					pEffect->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, iTextureIndex);

					if (tEffectDesc.bNoise)
					{
						ReadFile(hFile, &iNoisePathSize, sizeof(_int), &dwByte, nullptr);
						ReadFile(hFile, &szNoiseFullPath, iNoisePathSize * 2, &dwByte, nullptr);
						ReadFile(hFile, &iNoiseIndex, sizeof(_int), &dwByte, nullptr);
						//pEffect->Client_Init_Texture(CEffect::TEXTURE_NOISE, szNoiseFullPath);
						pEffect->Set_TextureIndex(CEffect::TEXTURE_NOISE, iNoiseIndex);
					}

					static_cast<CEffectObject*>(pEffectObject)->Add_Effect(pEffect);
				}
			}
			else if (CEffect::EFFECT_STATICMESH == iEffectType)
			{
				// 텍스쳐 데이터
				ReadFile(hFile, &iPathSize, sizeof(_int), &dwByte, nullptr);
				ReadFile(hFile, &szFullPath, iPathSize * 2, &dwByte, nullptr);
				ReadFile(hFile, &iTextureIndex, sizeof(_int), &dwByte, nullptr);

				CEffect* pEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(0, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_StaticMesh"), &tEffectDesc);
				if (nullptr != pEffect)
				{
					//pEffect->Client_Init_Texture(CEffect::TEXTURE_DIFFUSE, szFullPath);
					pEffect->Init_Texture();
					pEffect->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, iTextureIndex);

					if (tEffectDesc.bNoise)
					{
						ReadFile(hFile, &iNoisePathSize, sizeof(_int), &dwByte, nullptr);
						ReadFile(hFile, &szNoiseFullPath, iNoisePathSize * 2, &dwByte, nullptr);
						ReadFile(hFile, &iNoiseIndex, sizeof(_int), &dwByte, nullptr);
						//pEffect->Client_Init_Texture(CEffect::TEXTURE_NOISE, szNoiseFullPath);
						pEffect->Set_TextureIndex(CEffect::TEXTURE_NOISE, iNoiseIndex);
					}

					// 메쉬 데이터
					ReadFile(hFile, &iMeshPathSize, sizeof(_int), &dwByte, nullptr);
					ReadFile(hFile, &szMeshFullPath, iMeshPathSize * 2, &dwByte, nullptr);
					ReadFile(hFile, &iMeshIndex, sizeof(_int), &dwByte, nullptr);

					_matrix PivotMatrix = XMMatrixIdentity();
					PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f);
					_wstring strProtoTag = L"Proto_Component_Model_";
					strProtoTag += iter.strFileName;
					CModel* pModel = (CModel*)pGameInstance->Clone_Component(0, strProtoTag.c_str());
					//Safe_AddRef(pModel);
					//CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szMeshFullPath, PivotMatrix);
					if (nullptr != pModel)
					{
						static_cast<CStaticMeshEffect*>(pEffect)->Set_Model(pModel, 0);
						static_cast<CEffectObject*>(pEffectObject)->Add_Effect(pEffect);
					}
				}
			}
			else if (CEffect::EFFECT_ANIMMESH == iEffectType)
			{
				ReadFile(hFile, &iMeshPathSize, sizeof(_int), &dwByte, nullptr);
				ReadFile(hFile, &szMeshFullPath, iMeshPathSize * 2, &dwByte, nullptr);
				ReadFile(hFile, &iMeshIndex, sizeof(_int), &dwByte, nullptr);
				//pMeshFullPath = new _tchar[iMeshPathSize * 2];
				//pMeshFullPath = szMeshFullPath;
				//ReadFile(hFile, &pMeshFullPath, iMeshPathSize, &dwByte, nullptr);
				//m_StrList.push_back(pMeshFullPath);

				CEffect* pEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(0, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_AnimMesh"), &tEffectDesc);
				if (nullptr != pEffect)
				{
					_matrix PivotMatrix = XMMatrixIdentity();
					PivotMatrix *= XMMatrixScaling(0.001f, 0.001f, 0.001f);
					CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, szMeshFullPath, PivotMatrix);
					if (nullptr != pModel)
					{
						static_cast<CAnimMeshEffect*>(pEffect)->Set_Model(pModel, 0);
						static_cast<CEffectObject*>(pEffectObject)->Add_Effect(pEffect);
					}
				}
			}
			else
			{
				ReadFile(hFile, &tInstanceEffectDesc, sizeof(INSTANCEDESC), &dwByte, nullptr);

				ReadFile(hFile, &iPathSize, sizeof(_int), &dwByte, nullptr);
				ReadFile(hFile, &szFullPath, iPathSize * 2, &dwByte, nullptr);
				ReadFile(hFile, &iTextureIndex, sizeof(_int), &dwByte, nullptr);

				CEffect* pEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(0, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Instance"), &tEffectDesc);
				if (nullptr != pEffect)
				{
					//pEffect->Client_Init_Texture(CEffect::TEXTURE_DIFFUSE, szFullPath);
					pEffect->Init_Texture();
					pEffect->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, iTextureIndex);

					if (tEffectDesc.bNoise)
					{
						ReadFile(hFile, &iNoisePathSize, sizeof(_int), &dwByte, nullptr);
						ReadFile(hFile, &szNoiseFullPath, iNoisePathSize * 2, &dwByte, nullptr);
						ReadFile(hFile, &iNoiseIndex, sizeof(_int), &dwByte, nullptr);
						//pEffect->Client_Init_Texture(CEffect::TEXTURE_NOISE, szNoiseFullPath);
						pEffect->Set_TextureIndex(CEffect::TEXTURE_NOISE, iNoiseIndex);
					}

					static_cast<CInstanceEffect*>(pEffect)->ChangeInstanceCount(tEffectDesc.iInstanceCount);
					// 로드한 인스턴스 이펙트 데이터를 인스턴스 버퍼의 데이터로 넘김
					static_cast<CInstanceEffect*>(pEffect)->Set_InstanceEffectDesc(tInstanceEffectDesc);

					static_cast<CEffectObject*>(pEffectObject)->Add_Effect(pEffect);
				}
			}
		}

		Add_Effect(0, iter.strFileName, pEffectObject);
	}

	CloseHandle(hFile);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CGameObject* CEffect_Manager::Load_Effect2(const _tchar* _FileName, _uint iSelect)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	HANDLE		hFile = nullptr;

	PATHS vecTexturePath = { L"Model",L"EffectModel",L"Textures" };
	FILEGROUP* pTextureFolder = pGameInstance->Find_Folder(&vecTexturePath);

	PATHS vecNoisePath = { L"NoiseTexture" };
	FILEGROUP* pNoiseFolder = pGameInstance->Find_Folder(&vecNoisePath);

	EFFECTDESC	tEffectObjectDesc;
	EFFECTDESC	tEffectDesc;
	INSTANCEDESC tInstanceEffectDesc;
	ZeroMemory(&tEffectObjectDesc, sizeof(EFFECTDESC));
	ZeroMemory(&tEffectDesc, sizeof(EFFECTDESC));
	ZeroMemory(&tInstanceEffectDesc, sizeof(INSTANCEDESC));

	_tchar szFileName[MAX_PATH] = L"";
	_int iFileNameSize = 0;

	_uint iEffectObjectCount = 0;
	_int iEffectCount = 0;
	_int iEffectType = CEffect::EFFECT_END;

	_tchar* pFullPath = nullptr;
	_tchar szFullPath[MAX_PATH] = L"";
	_int iPathSize = 0;
	_int iTextureIndex = 0;

	_tchar* pNoiseFullPath = nullptr;
	_tchar szNoiseFullPath[MAX_PATH] = L"";
	_int iNoisePathSize = 0;
	_int iNoiseIndex = 0;

	_tchar* pMeshFullPath = nullptr;
	_tchar szMeshFullPath[MAX_PATH] = L"";
	_int iMeshPathSize = 0;
	_int iMeshIndex = 0;

	_wstring strFileName = _FileName;

	/*if (0 == iOption)
		strFileName = _tFileData.strFileName;
	else if (1 == iOption)
		strFileName = strName;*/

	_wstring strFilePath = L"../../Resources/Data/Effect/";
	strFilePath += strFileName;
	strFilePath += L".bin";

	//Release();		// 기존에 있던 타일 정보를 소멸하고 불러들이는 데이터로 재생성하기 위함

	hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (nullptr == hFile)
	{
		RELEASE_INSTANCE(CGameInstance);
		return nullptr;
	}

	DWORD	dwByte = 0;

	// 파일 이름 사이즈, 파일 이름 불러오기
	ReadFile(hFile, &iFileNameSize, sizeof(_int), &dwByte, nullptr);
	ReadFile(hFile, &szFileName, iFileNameSize * 2, &dwByte, nullptr);

	// 이펙트 오브젝트 안의 이펙트들 갯수를 로드
	ReadFile(hFile, &iEffectCount, sizeof(_int), &dwByte, nullptr);

	// 이펙트 오브젝트의 데이터 로드
	ReadFile(hFile, &tEffectObjectDesc, sizeof(EFFECTDESC), &dwByte, nullptr);

	// 클라이언트에서 로드 할때는 Play 를 바로 실행할 수 있도록 true 로 설정 해준다.
	if (0 == iSelect || 1 == iSelect)
		tEffectObjectDesc.bPlay = true;
	else
		tEffectObjectDesc.bPlay = false;

	// 이펙트 오브젝트 객체 생성
	CGameObject* pEffectObject = pGameInstance->Clone_GameObject_Add(0, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &tEffectObjectDesc);
	if (nullptr == pEffectObject)
	{
		MSG_BOX("이펙트 오브젝트 생성 실패");
		return nullptr;
	}

	static_cast<CEffectObject*>(pEffectObject)->Set_FileName(_FileName);

	// 프로젝트 타입을 설정해준다.
	if (0 == iSelect)
		static_cast<CEffectObject*>(pEffectObject)->Set_ProjectType(CEffectObject::PROJECT_CLIENT);
	else if (1 == iSelect)
		static_cast<CEffectObject*>(pEffectObject)->Set_ProjectType(CEffectObject::PROJECT_ANIMTOOL);
	else
		static_cast<CEffectObject*>(pEffectObject)->Set_ProjectType(CEffectObject::PROJECT_TOOL);

	// 클라이언트에서 이펙트를 불러오기 위해서 이름을 지정해준다.
	static_cast<CEffectObject*>(pEffectObject)->Set_Name(strFileName);
	static_cast<CEffectObject*>(pEffectObject)->Set_Play(true);
	for (_int i = 0; i < iEffectCount; ++i)
	{
		// 이펙트 데이터 로드
		ReadFile(hFile, &tEffectDesc, sizeof(EFFECTDESC), &dwByte, nullptr);
		// 이펙트 타입 로드
		ReadFile(hFile, &iEffectType, sizeof(_int), &dwByte, nullptr);

		if (CEffect::EFFECT_IMAGE == iEffectType)
		{
			ReadFile(hFile, &iPathSize, sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &szFullPath, iPathSize * 2, &dwByte, nullptr);
			ReadFile(hFile, &iTextureIndex, sizeof(_int), &dwByte, nullptr);
			//pFullPath = new _tchar[iPathSize];
			//pFullPath = szFullPath;
			//ReadFile(hFile, &pFullPath, iPathSize * 2, &dwByte, nullptr);
			//m_StrList.push_back(pFullPath);

			CEffect* pEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(0, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Image"), &tEffectDesc);
			if (nullptr != pEffect)
			{
				//pEffect->Client_Init_Texture(CEffect::TEXTURE_DIFFUSE, szFullPath);
				pEffect->Init_Texture();
				//pEffect->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, iTextureIndex);
				for (int i = 0; i < pTextureFolder->vecFileDataList.size(); ++i)
				{
					if (!lstrcmp(pTextureFolder->vecFileDataList[i].strFilePath.c_str(), szFullPath))
					{
						pEffect->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, i);
						break;
					}
				}

				if (tEffectDesc.bNoise)
				{
					ReadFile(hFile, &iNoisePathSize, sizeof(_int), &dwByte, nullptr);
					ReadFile(hFile, &szNoiseFullPath, iNoisePathSize * 2, &dwByte, nullptr);
					ReadFile(hFile, &iNoiseIndex, sizeof(_int), &dwByte, nullptr);
					//pEffect->Client_Init_Texture(CEffect::TEXTURE_NOISE, szNoiseFullPath);
					//pEffect->Set_TextureIndex(CEffect::TEXTURE_NOISE, iNoiseIndex);

					for (int i = 0; i < pNoiseFolder->vecFileDataList.size(); ++i)
					{
						if (!lstrcmp(pNoiseFolder->vecFileDataList[i].strFilePath.c_str(), szNoiseFullPath))
						{
							pEffect->Set_TextureIndex(CEffect::TEXTURE_NOISE, i);
							break;
						}
					}
				}

				static_cast<CEffectObject*>(pEffectObject)->Add_Effect(pEffect);
			}
		}
		else if (CEffect::EFFECT_STATICMESH == iEffectType)
		{
			// 텍스쳐 데이터
			ReadFile(hFile, &iPathSize, sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &szFullPath, iPathSize * 2, &dwByte, nullptr);
			ReadFile(hFile, &iTextureIndex, sizeof(_int), &dwByte, nullptr);

			CEffect* pEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(0, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_StaticMesh"), &tEffectDesc);
			if (nullptr != pEffect)
			{
				//pEffect->Client_Init_Texture(CEffect::TEXTURE_DIFFUSE, szFullPath);
				pEffect->Init_Texture();
				//pEffect->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, iTextureIndex);
				for (int i = 0; i < pTextureFolder->vecFileDataList.size(); ++i)
				{
					if (!lstrcmp(pTextureFolder->vecFileDataList[i].strFilePath.c_str(), szFullPath))
					{
						pEffect->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, i);
						break;
					}
				}

				if (tEffectDesc.bNoise)
				{
					ReadFile(hFile, &iNoisePathSize, sizeof(_int), &dwByte, nullptr);
					ReadFile(hFile, &szNoiseFullPath, iNoisePathSize * 2, &dwByte, nullptr);
					ReadFile(hFile, &iNoiseIndex, sizeof(_int), &dwByte, nullptr);
					//pEffect->Client_Init_Texture(CEffect::TEXTURE_NOISE, szNoiseFullPath);
					//pEffect->Set_TextureIndex(CEffect::TEXTURE_NOISE, iNoiseIndex);
					for (int i = 0; i < pNoiseFolder->vecFileDataList.size(); ++i)
					{
						if (!lstrcmp(pNoiseFolder->vecFileDataList[i].strFilePath.c_str(), szNoiseFullPath))
						{
							pEffect->Set_TextureIndex(CEffect::TEXTURE_NOISE, i);
							break;
						}
					}
				}

				// 메쉬 데이터
				ReadFile(hFile, &iMeshPathSize, sizeof(_int), &dwByte, nullptr);
				ReadFile(hFile, &szMeshFullPath, iMeshPathSize * 2, &dwByte, nullptr);
				ReadFile(hFile, &iMeshIndex, sizeof(_int), &dwByte, nullptr);

				PATHS vecPath = CMethod::Split_Paths(szMeshFullPath, L"Resources\\Model\\EffectModel\\FBX_Converter");
				_matrix PivotMatrix = XMMatrixIdentity();
				if (!lstrcmp(L"PlaneCustomX.bin", vecPath[0].c_str()))
					PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(1.27f, 0.f, 0.f);
				else if (!lstrcmp(L"PlaneCustomY.bin", vecPath[0].c_str()))
					PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(0.f, 1.27f, 0.f);
				else
					PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f);


				size_t lastindex = vecPath[0].find_last_of(L".");
				wstring strName = vecPath[0].substr(0, lastindex);

				_wstring strProtoTag = L"Prototype_Component_Model_";
				strProtoTag += strName;
				CModel* pModel = (CModel*)pGameInstance->Clone_Component(0, strProtoTag.c_str());
				//CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szMeshFullPath, PivotMatrix);
				if (nullptr != pModel)
				{
					static_cast<CStaticMeshEffect*>(pEffect)->Set_Model(pModel, 0);
					static_cast<CEffectObject*>(pEffectObject)->Add_Effect(pEffect);
				}
			}
		}
		else if (CEffect::EFFECT_ANIMMESH == iEffectType)
		{
			ReadFile(hFile, &iMeshPathSize, sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &szMeshFullPath, iMeshPathSize * 2, &dwByte, nullptr);
			ReadFile(hFile, &iMeshIndex, sizeof(_int), &dwByte, nullptr);
			//pMeshFullPath = new _tchar[iMeshPathSize * 2];
			//pMeshFullPath = szMeshFullPath;
			//ReadFile(hFile, &pMeshFullPath, iMeshPathSize, &dwByte, nullptr);
			//m_StrList.push_back(pMeshFullPath);

			CEffect* pEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(0, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_AnimMesh"), &tEffectDesc);
			if (nullptr != pEffect)
			{
				_matrix PivotMatrix = XMMatrixIdentity();
				PivotMatrix *= XMMatrixScaling(0.001f, 0.001f, 0.001f);
				CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, szMeshFullPath, PivotMatrix);
				if (nullptr != pModel)
				{
					static_cast<CAnimMeshEffect*>(pEffect)->Set_Model(pModel, 0);
					static_cast<CEffectObject*>(pEffectObject)->Add_Effect(pEffect);
				}
			}
		}
		else
		{
			ReadFile(hFile, &tInstanceEffectDesc, sizeof(INSTANCEDESC), &dwByte, nullptr);

			ReadFile(hFile, &iPathSize, sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &szFullPath, iPathSize * 2, &dwByte, nullptr);
			ReadFile(hFile, &iTextureIndex, sizeof(_int), &dwByte, nullptr);

			CEffect* pEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(0, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Instance"), &tEffectDesc);
			if (nullptr != pEffect)
			{
				//pEffect->Client_Init_Texture(CEffect::TEXTURE_DIFFUSE, szFullPath);
				pEffect->Init_Texture();
				//pEffect->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, iTextureIndex);
				for (int i = 0; i < pTextureFolder->vecFileDataList.size(); ++i)
				{
					if (!lstrcmp(pTextureFolder->vecFileDataList[i].strFilePath.c_str(), szFullPath))
					{
						pEffect->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, i);
						break;
					}
				}

				if (tEffectDesc.bNoise)
				{
					ReadFile(hFile, &iNoisePathSize, sizeof(_int), &dwByte, nullptr);
					ReadFile(hFile, &szNoiseFullPath, iNoisePathSize * 2, &dwByte, nullptr);
					ReadFile(hFile, &iNoiseIndex, sizeof(_int), &dwByte, nullptr);
					//pEffect->Client_Init_Texture(CEffect::TEXTURE_NOISE, szNoiseFullPath);
					//pEffect->Set_TextureIndex(CEffect::TEXTURE_NOISE, iNoiseIndex);
					for (int i = 0; i < pNoiseFolder->vecFileDataList.size(); ++i)
					{
						if (!lstrcmp(pNoiseFolder->vecFileDataList[i].strFilePath.c_str(), szNoiseFullPath))
						{
							pEffect->Set_TextureIndex(CEffect::TEXTURE_NOISE, i);
							break;
						}
					}
				}

				static_cast<CInstanceEffect*>(pEffect)->ChangeInstanceCount(tEffectDesc.iInstanceCount);
				// 로드한 인스턴스 이펙트 데이터를 인스턴스 버퍼의 데이터로 넘김
				static_cast<CInstanceEffect*>(pEffect)->Set_InstanceEffectDesc(tInstanceEffectDesc);

				static_cast<CEffectObject*>(pEffectObject)->Add_Effect(pEffect);
			}
		}
	}

	CloseHandle(hFile);
	RELEASE_INSTANCE(CGameInstance);
	return pEffectObject;
}

HRESULT CEffect_Manager::Init_Effects(_uint iSelect)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	PATHS vecPath = { L"Data",L"Effect" };
	FILEGROUP* pEffectFolder = pGameInstance->Find_Folder(&vecPath);

	_uint iCount = 0;
	
	for (auto& iter : pEffectFolder->vecFileDataList)
	{
		if (!lstrcmp(L"Zen_StepLine", iter.strFileName.c_str()) || !lstrcmp(L"FootDust", iter.strFileName.c_str())
			|| !lstrcmp(L"StepParticle", iter.strFileName.c_str()) || !lstrcmp(L"StepParticle_Aka", iter.strFileName.c_str())
			|| !lstrcmp(L"StepParticle_Sab", iter.strFileName.c_str()) || !lstrcmp(L"StepParticle_Tan", iter.strFileName.c_str())
			|| !lstrcmp(L"TEONI_JUMP_G", iter.strFileName.c_str()) || !lstrcmp(L"TEONI_STONE_5", iter.strFileName.c_str())
			|| !lstrcmp(L"TEONI_SJUMP_G", iter.strFileName.c_str()) || !lstrcmp(L"ZAKO2_ATTACK_G", iter.strFileName.c_str())
			|| !lstrcmp(L"StepLine", iter.strFileName.c_str()) || !lstrcmp(L"StepDistortion", iter.strFileName.c_str())
			|| !lstrcmp(L"Zen_StepDistortion", iter.strFileName.c_str()) || !lstrcmp(L"Aka_ShockWaveAura", iter.strFileName.c_str())
			|| !lstrcmp(L"Aka_ShockWaveAura2", iter.strFileName.c_str()) || !lstrcmp(L"Aka_ShockWaveFist", iter.strFileName.c_str())
			|| !lstrcmp(L"Aka_WindBall2", iter.strFileName.c_str()) || !lstrcmp(L"Aka_WindBallHit", iter.strFileName.c_str()))
			iCount = 7;
		else if (!lstrcmp(L"ZAKO1_SKILL1_ATTACK_4_L_P", iter.strFileName.c_str()))
			iCount = 10;
		else if (!lstrcmp(L"StepDistortion1_1", iter.strFileName.c_str()) || !lstrcmp(L"StepDistortion1_2", iter.strFileName.c_str()))
			iCount = 6;
		else if (!lstrcmp(L"Aka_FistSkill1_1", iter.strFileName.c_str()) || !lstrcmp(L"Aka_Attack5_1", iter.strFileName.c_str()))
			iCount = 30;
		else if (!lstrcmp(L"Aka_ChargeAttack1_1", iter.strFileName.c_str()))
			iCount = 20;
		else
			iCount = 1;

		if (FAILED(pGameInstance->Add_ObjectPool(iter.strFileName.c_str(), 0, iCount, iter.strFileName.c_str(), iSelect)))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CEffect_Manager::Free()
{
	for (auto& Pair : m_mapEffects)
	{
		if (nullptr != Pair.second)
			Safe_Release(Pair.second);

		//delete[] Pair.first;
		//Safe_Delete_Array(Pair.first);
	}

	Safe_Release_DV(m_pContext);
	Safe_Release_DV(m_pDevice);
}
