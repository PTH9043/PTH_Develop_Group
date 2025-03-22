#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CEffectGroup.h"
#include "CGameInstance.h"


CEffectGroup::CEffectGroup(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CEffectGroup::CEffectGroup(const CEffectGroup& _rhs)
	: CGameObject(_rhs), m_strPath(_rhs.m_strPath)
{
}

void CEffectGroup::Active(const _bool& _bIsActive)
{
	m_bActive = _bIsActive;

	for (auto& iter : m_EffectList)
	{
		iter->Set_Active(_bIsActive);
		iter->Reset_Data();
	}
}

void CEffectGroup::Update_Pos(_fvector _vPos)
{
	if (nullptr != m_pTransformCom)
		m_pTransformCom->Set_Pos(_vPos);

	for (auto& iter : m_EffectList)
		iter->Reset_Data();
}

void CEffectGroup::Update_Rot(const _float3& _v3Rot)
{
	if (nullptr != m_pTransformCom)
		m_pTransformCom->Rotate_Turn(_v3Rot);
}

void CEffectGroup::Update_Scale(const _float3& _v3Scale)
{
	if (nullptr != m_pTransformCom)
		m_pTransformCom->Set_Scale(_v3Scale);
}

void CEffectGroup::Update_Look(_fvector _vLook)
{
	if (nullptr != m_pTransformCom)
		m_pTransformCom->LookAt(_vLook + m_pTransformCom->Get_Pos());
}

void CEffectGroup::Get_Collider(void** _ppOut)
{
	if (nullptr != _ppOut)
	{
		list<CCollider*> mColliders;
		for (auto& iter : m_EffectList)
		{
			CCollider* pCollider = static_cast<CCollider*>(iter->Get_Component(COM_COLLCOLLIDER));
			mColliders.push_back(pCollider);
		}
		*_ppOut = (void*)&mColliders;
	}
}

void CEffectGroup::Get_Collider(list<CCollider*>* _pOut)
{
	if (nullptr != _pOut)
	{
		for (auto& iter : m_EffectList)
		{
			CCollider* pCollider = static_cast<CCollider*>(iter->Get_Component(COM_COLLCOLLIDER));
			_pOut->push_back(pCollider);
		}
	}
}

HRESULT CEffectGroup::NativeConstruct_Prototype()
{
	CEffect* pEffect = CEffect::Create(m_pDevice, m_pContext);
	if (nullptr != pEffect)
		m_EffectList.push_back(pEffect);

	if (FAILED(Ready_Transform(COM_TRANSFORM)))
		return E_FAIL;

	m_bActive = true;
	return S_OK;
}

HRESULT CEffectGroup::NativeConstruct_Prototype(const _wstring& _strFilePath)
{
	m_strPath = _strFilePath;
	if (LEVEL_TOOL == g_iCurrentLevel)
	{
		if (FAILED(Ready_Transform(COM_TRANSFORM)))
			return E_FAIL;

		Load(m_strPath);

		for (auto& iter : m_EffectList)
			iter->Set_OwnerTransform(m_pTransformCom);

		m_bActive = true;
	}
	return S_OK;
}

HRESULT CEffectGroup::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(Ready_Transform(COM_TRANSFORM)))
		return E_FAIL;

	Load(m_strPath);

	for (auto& iter : m_EffectList)
		iter->Set_OwnerTransform(m_pTransformCom);

	m_bActive = true;
	return S_OK;
}

void CEffectGroup::Tick(const _double& _dTimeDelta)
{
	if (false == m_bActive)
	{
		for (auto& iter : m_EffectList)
			iter->Set_Active(m_bActive);
		return;
	}

	for (auto& iter : m_EffectList)
		iter->Tick(_dTimeDelta);
}

void CEffectGroup::Late_Tick(const _double& _dTimeDelta)
{
	if (false == m_bActive)
	{
		for (auto& iter : m_EffectList)
			iter->Set_Active(m_bActive);
		return;
	}

	for (auto& iter : m_EffectList)
		iter->Late_Tick(_dTimeDelta);

	if (false == m_bLoop)
	{
		if (true == m_tLifeTimer.Is_Over(_dTimeDelta))
		{
			for (auto& iter : m_EffectList)
				iter->Reset_Data();

			if (true == g_bGameRun)
				m_bActive = false;

			m_tLifeTimer.Reset_Timer();
		}
	}
}

HRESULT CEffectGroup::Render()
{
	return S_OK;
}

void CEffectGroup::Modify_Value(vector<CModel*>* _pModel, int* _pCount, int* _pIndex, CParticle* _pParticle)
{
	string str;
	str.assign(m_strEffectName.begin(), m_strEffectName.end());
	_char Effect[MAX_PATH] = {};
	strcpy_s(Effect, sizeof(_char) * MAX_PATH, str.c_str());
	ImGui::InputText("Effect Name", Effect, sizeof(_char) * MAX_PATH);
	str = Effect;
	m_strEffectName.assign(str.begin(), str.end());

	if (ImGui::DragFloat("LifeTime: ", &m_fLifeTime))
	{
		for (auto& iter : m_EffectList)
			iter->Reset_Data();
		m_tLifeTimer = CUSTIMER(m_fLifeTime);
		m_tLifeTimer.Reset_Timer();
	}

	ImGui::Checkbox("Loop", &m_bLoop);

	ImGui::DragInt("Effect Num Instance Count", _pCount);
	ImGui::DragInt("Effect Select Index", _pCount);

	if (ImGui::Button("Plus Effect Instance"))
	{
		m_EffectList.push_back(CEffect::Create(m_pDevice, m_pContext));

		for (auto& iter : m_EffectList)
			iter->Set_OwnerTransform(m_pTransformCom);
	}

	if (ImGui::Button("Create Many Instance"))
	{
		_uint iSize = *_pCount - (_uint)m_EffectList.size();
		if (iSize > 0)
		{
			for (_uint i = 0; i < iSize; ++i)
			{
				m_EffectList.push_back(CEffect::Create(m_pDevice, m_pContext));
			}

			for (auto& iter : m_EffectList)
				iter->Set_OwnerTransform(m_pTransformCom);
		}
	}

	if (ImGui::Button("Save Effect Instance"))
	{
		Save();
	}

	if (ImGui::TreeNode("Effect Modify Tool"))
	{
		_char TEXT[MAX_PATH] = "";
		_uint i = 0;
		for (auto& iter : m_EffectList)
		{
			sprintf_s(TEXT, "Effect Piece: %d", i++);
			if (ImGui::TreeNodeEx(TEXT))
			{
				iter->Modify_Value(_pModel, _pParticle);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::Button("Delete Effect Instance"))
	{
		_uint iIndex = 0;
		for (vector<CEffect*>::iterator it = m_EffectList.begin(); it != m_EffectList.end(); ++it)
		{
			if (iIndex++ == *_pIndex)
			{
				Safe_Release(*it);
				it = m_EffectList.erase(it);
				break;
			}
		}
	}

	if (ImGui::Button("Clear Effect Instnace"))
	{
		_uint iIndex = 0;
		for (vector<CEffect*>::iterator it = m_EffectList.begin(); it != m_EffectList.end();)
			Safe_Release(*it);

		m_EffectList.clear();
	}
}

void CEffectGroup::Save()
{
	_wstring strFilePath = L"..\\..\\Resource\\Json\\Effect\\";
	strFilePath.append(m_strEffectName);
	strFilePath.append(L".bin");
	HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		MSG_BOX("CModel::Load_Data_NonAnim To Failed");
		return;
	}
	DWORD dwByte = 0;
	{
		{
			_uint Length = (_uint)lstrlen(m_strEffectName.c_str()) + 1;
			WriteFile(hFile, &Length, sizeof(_uint), &dwByte, 0);
			WriteFile(hFile, m_strEffectName.c_str(), sizeof(_tchar) * Length, &dwByte, 0);
		}
		_uint iSize = static_cast<_uint>(m_EffectList.size());
		WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
		for (auto& iter : m_EffectList)
		{
			iter->Save(hFile, &dwByte);
		}
		WriteFile(hFile, &m_fLifeTime, sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &m_bLoop, sizeof(_bool), &dwByte, nullptr);
	}

	CloseHandle(hFile);
}

void CEffectGroup::Load(const _wstring& _strPath)
{
	HANDLE hFile = CreateFile(_strPath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		MSG_BOX("CEffect::Load To Failed");
		return;
	}
	DWORD dwByte;
	_uint iLength = 0;
	{
		ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
		_tchar* pName = new _tchar[iLength];
		ReadFile(hFile, pName, sizeof(_tchar) * iLength, &dwByte, nullptr);
		m_strEffectName = pName;
		Safe_Delete_Array(pName);
	}
	_uint iSize = 0;
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iSize; ++i)
	{
		CEffect* pInstance = CEffect::Create(m_pDevice, m_pContext);
		pInstance->Load(hFile, &dwByte);
		pInstance->Set_OwnerTransform(m_pTransformCom);
		m_EffectList.push_back(pInstance);
	}
	ReadFile(hFile, &m_fLifeTime, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &m_bLoop, sizeof(_bool), &dwByte, nullptr);

	m_tLifeTimer = CUSTIMER(m_fLifeTime);

	CloseHandle(hFile);
}

CEffectGroup* CEffectGroup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffectGroup* pInstance = new CEffectGroup(pDevice, pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CEffectGroup::Create To Failed");
	}
	return pInstance;
}

CEffectGroup* CEffectGroup::Create_LoadFile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& _strFilePath)
{
	CEffectGroup* pInstance = new CEffectGroup(pDevice, pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(_strFilePath)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CEffectGroup::Create To Failed");
	}
	return pInstance;
}

CGameObject* CEffectGroup::Clone(void* pArg)
{
	CEffectGroup* pInstance = new CEffectGroup(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CEffectGroup::Clone To Failed");
	}
	return pInstance;
}

void CEffectGroup::Free()
{
	for (auto& iter : m_EffectList)
		Safe_Release(iter);

	__super::Free();
}
