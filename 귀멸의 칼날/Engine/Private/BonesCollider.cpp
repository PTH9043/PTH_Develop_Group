#include "BonesCollider.h"
#include "GameInstance.h"
#include "Collider.h"
#include "HierarchyNode.h"

CBonesCollider::CBonesCollider()
{
}

void CBonesCollider::Add_Colliders(CModel* _pModel, const _char* _pNodeNames, const _uint& _iLevel, const _tchar* _pTag, void* _pArg)
{
	if (nullptr == _pModel)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		m_pHierachyNode = _pModel->Get_HierarchyNodes(_pNodeNames);
		Safe_AddRef(m_pHierachyNode);

		m_pCollider = static_cast<CCollider*>(pGameInstance->Clone_Component(_iLevel, _pTag, _pArg));
		if (m_strTag == L"")
			m_strTag = _pTag;
	}
	RELEASE_INSTANCE(CGameInstance);
}

_matrix CBonesCollider::MultiplyCollider(_matrix WorldMatrix)
{
	if (nullptr == m_pCollider)
		return XMMatrixIdentity();

	_matrix MultipleWorld = XMMatrixScaling(0.1f, 0.1f, 0.1f)* 
		m_pHierachyNode->Get_CombinedTransformationMatrix() * XMMatrixRotationY(XMConvertToRadians(180.0f)) * WorldMatrix;

	_vector vCurPosition = MultipleWorld.r[3];

	m_vMoveDirection = XMVector3Normalize(MultipleWorld.r[3] - m_vPrePosition.Get_Vector());

	m_pCollider->Tick(MultipleWorld);
	XMStoreFloat3(&m_vPrePosition, MultipleWorld.r[3]);

	return MultipleWorld;
}

_bool CBonesCollider::Is_Collision(CCollider* _pCollider)
{
	if (nullptr == m_pCollider || nullptr == _pCollider)
		return false;

	if (false == m_bIsCollision)
		return false;

	if (m_pCollider->Is_Collision(_pCollider))
		return true;

	return false;
}

_bool CBonesCollider::Is_Collision(const char* _pName, CCollider* _pCollider)
{
	if (nullptr == m_pCollider || nullptr == _pCollider || nullptr == m_pHierachyNode)
		return false;

	if (false == m_bIsCollision)
		return false;

	if (!strcmp(m_pHierachyNode->Get_Name().c_str(), _pName))
	{
		if (true == m_bIsCollision)
			return true;
	}

	return false;
}

// 원 충돌체끼리 충돌할때 충돌체끼리 겹치는 만큼을 반환하는 함수
// 충돌이 안되거나 원 충돌이 아닐경우 false 반환
_bool CBonesCollider::Compute_Distance_Sphere(CBonesCollider* _pBoneCollider, _float& fDistance)
{
	if (nullptr == _pBoneCollider)
		return false;
	
	CCollider* pCollider = _pBoneCollider->Get_Collider();
	if (CCollider::TYPE_SPHERE != pCollider->Get_Type() || CCollider::TYPE_SPHERE != m_pCollider->Get_Type())
		return false;
	
	_float r1 = m_pCollider->Get_Scale().x;
	_float r2 = pCollider->Get_Scale().x;
	_float r = XMVectorGetX(XMVector3Length(XMVectorSetY(m_pCollider->Get_CurPos(), 0.f) -  XMVectorSetY(pCollider->Get_CurPos(), 0.f)));

	fDistance = r1 + r2 - r;

	if (fDistance < 0.f)
		return false;

	_pBoneCollider->m_bIsCollision = true;
	return true;
}


HRESULT CBonesCollider::Save(CModel* _pModel, FILEGROUP* _pFileGroup)
{
	if (nullptr == _pFileGroup || nullptr == _pModel)
		return E_FAIL;

	_wstring str = CMethod::ConvertSToW(m_pHierachyNode->Get_Name());

	_wstring Path = _pFileGroup->strPath;
	_wstring FolderPath = Path;
	FolderPath.append(L"\\BonesCollider");
	_wmkdir(FolderPath.c_str());

	FolderPath.append(L"\\");
	FolderPath.append(str);
	FolderPath.append(L".dat");

	HANDLE hFile = CreateFile(FolderPath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	DWORD dwByte = 0;
	{
		_uint iLength = ((_uint)strlen(m_pHierachyNode->Get_Name().c_str()) + 1) * sizeof(_char);
		WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, m_pHierachyNode->Get_Name().c_str(), iLength, &dwByte, nullptr);
	}
	CMethod::Save_File_WString(hFile, dwByte, m_strTag);
	WriteFile(hFile, &m_pCollider->Get_Type(), sizeof(CCollider::TYPE), &dwByte, nullptr);
	WriteFile(hFile, &m_pCollider->Get_Scale(), sizeof(_float3), &dwByte, nullptr);
	WriteFile(hFile, &m_pCollider->Get_Translate(), sizeof(_float3), &dwByte, nullptr);

	WriteFile(hFile, &m_bIsCollision, sizeof(_bool), &dwByte, nullptr);
	CloseHandle(hFile);
	return S_OK;
}

HRESULT CBonesCollider::Load(CModel* _pModel, FILEDATA* _pFileData, const _uint& _iLevel)
{
	if (nullptr == _pModel || nullptr == _pFileData)
		return E_FAIL;

	HANDLE hFile = CreateFile(_pFileData->strFilePath.c_str(),
		GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return E_FAIL;
	}
	string strName = "";
	DWORD dwByte = 0;
	strName = CMethod::Load_File_String(hFile, dwByte);
	m_strTag = CMethod::Load_File_WString(hFile, dwByte);
	CCollider::TYPE eType;
	_float3 v3Scale;
	_float3 v3Translate;
	ReadFile(hFile, &eType, sizeof(CCollider::TYPE), &dwByte, nullptr);
	ReadFile(hFile, &v3Scale, sizeof(_float3), &dwByte, nullptr);
	ReadFile(hFile, &v3Translate, sizeof(_float3), &dwByte, nullptr);
	ReadFile(hFile, &m_bIsCollision, sizeof(_bool), &dwByte, nullptr);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		CCollider::COLLIDERDESC tCollDesc;
		tCollDesc.vScale = v3Scale;
		tCollDesc.vTranslation = v3Translate;
		Add_Colliders(_pModel, strName.c_str(), _iLevel, m_strTag.c_str(), &tCollDesc);
	}
	RELEASE_INSTANCE(CGameInstance);

	CloseHandle(hFile);
	return S_OK;
}
#ifdef  _DEBUG
void CBonesCollider::Render()
{
	if (nullptr == m_pCollider)
		return;

	m_pCollider->Render_Debug();
}
#endif //  _DEBUG
void CBonesCollider::Release_Data()
{
}

CBonesCollider* CBonesCollider::Create()
{
	CBonesCollider* pInstance = new CBonesCollider();
	return pInstance;
}

void CBonesCollider::Free()
{
	Safe_Release(m_pCollider);
	Safe_Release(m_pHierachyNode);
	m_strTag.clear();
}
