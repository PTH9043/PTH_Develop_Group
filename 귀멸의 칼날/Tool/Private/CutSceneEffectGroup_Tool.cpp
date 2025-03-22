#include "CutSceneEffectGroup_Tool.h"
#include "GameInstance.h"
#include "CutSceneEffect_Tool.h"

CCutSceneEffectGroup_Tool::CCutSceneEffectGroup_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
}

HRESULT CCutSceneEffectGroup_Tool::NativeConstruct(VOIDDATAS* _pDatas)
{
	return S_OK;
}

HRESULT CCutSceneEffectGroup_Tool::Render_EffectVector()
{
	return S_OK;
}

void CCutSceneEffectGroup_Tool::Extern_Value(_wstring _strSelectEffectName, const _uint& _iIndex)
{

	if (ImGui::Button("Add_CutSceneEffect"))
	{
		VOIDDATAS tDatas = { &_strSelectEffectName };
		CCutSceneEffect_Tool* pInstance = CCutSceneEffect_Tool::Create(m_pDevice, m_pContext, &tDatas);
		m_CutSceneEffectVector.push_back(pInstance);
	}

	if (m_CutSceneEffectVector.size() > 0)
	{
		if (ImGui::BeginListBox("CutScene_EffectListBox", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			_uint iIndex = 0;
			for (auto& iter : m_CutSceneEffectVector)
			{
				_wstring str = CMethod::Combine_Int(iIndex, L"");
				str.append(L". ");
				str.append(iter->Get_Name());
				_string ConvertString = CMethod::ConvertWToS(str);
				if (ImGui::Selectable(ConvertString.c_str()))
				{
					m_pCutSceneEffectData = iter;
				}
			}
			ImGui::EndListBox();
		}

		if (nullptr != m_pCutSceneEffectData)
			m_pCutSceneEffectData->Extern_EffectValue();
	}
}

void CCutSceneEffectGroup_Tool::Save_CutSceneTex(HANDLE& _hFile, DWORD* _pByte)
{
	_uint iSize = (_uint)m_CutSceneEffectVector.size();
	WriteFile(_hFile, &iSize, sizeof(_uint), _pByte, nullptr);
	for (auto& iter : m_CutSceneEffectVector)
		iter->Save_CutSceneTex(_hFile, _pByte);
}

void CCutSceneEffectGroup_Tool::Load_CutSceneTex(HANDLE& _hFile, DWORD* _pByte)
{
	_uint iSize = 0;
	ReadFile(_hFile, &iSize, sizeof(_uint), _pByte, nullptr);
	for (_uint i = 0; i < iSize; ++i)
	{
		CCutSceneEffect_Tool* pInstance = CCutSceneEffect_Tool::Create(m_pDevice, m_pContext);
		pInstance->Load_CutSceneTex(_hFile, _pByte);
		m_CutSceneEffectVector.push_back(pInstance);
	}
}

CCutSceneEffectGroup_Tool* CCutSceneEffectGroup_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, VOIDDATAS* _pDatas)
{
	CCutSceneEffectGroup_Tool* pInstance = new CCutSceneEffectGroup_Tool(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct(_pDatas)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CCutSceneEffectGroup::Create To Failed");
#endif
	}
	return pInstance;
}

void CCutSceneEffectGroup_Tool::Free()
{
	for (auto& iter : m_CutSceneEffectVector)
		Safe_Release(iter);

	__super::Free();
}
