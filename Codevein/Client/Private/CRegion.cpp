#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CRegion.h"
#include "CGameInstance.h"
#include "CCell.h"
#include "CWorldObj.h"
#include "CCubeObj.h"

#ifdef _DEBUG
void CRegion::tagCubeObjs::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CCell* _pCell)
{		
	// Cube Add
#ifdef _DEBUG
	
	_uint iCount = 0;
	pObj1 = CCubeObj::Create(_pDevice, _pContext, _pCell->Get_Point(CCell::POINT_A), (iCount++));
	pObj2 = CCubeObj::Create(_pDevice, _pContext, _pCell->Get_Point(CCell::POINT_B), (iCount++));
	pObj3 = CCubeObj::Create(_pDevice, _pContext, _pCell->Get_Point(CCell::POINT_C), (iCount++));

	Safe_AddRef(_pCell);
	pCell = _pCell;
#endif
}

void CRegion::tagCubeObjs::Rebalance()
{
	if (nullptr == pCell)
		return;

	if (nullptr == pObj1 || nullptr == pObj2 || nullptr == pObj3)
		return;

	pObj1->Set_Position(pCell->Get_Point(CCell::POINT_A));
	pObj2->Set_Position(pCell->Get_Point(CCell::POINT_B));
	pObj3->Set_Position(pCell->Get_Point(CCell::POINT_C));
	pCell->ReBuffer();
}

void CRegion::tagCubeObjs::Delete(CCell* _pCell)
{
	if (_pCell == pCell)
	{
		Release();
	}
}

void CRegion::tagCubeObjs::Release()
{
	Safe_Release(pCell);
	Safe_Release(pObj1);
	Safe_Release(pObj2);
	Safe_Release(pObj3);
}

void CRegion::tagCubeObjs::Render_Objs(CRenderer* _pRenderer)
{
	if (nullptr != pObj1)
		pObj1->Add_Render(_pRenderer);

	if (nullptr != pObj2)
		pObj2->Add_Render(_pRenderer);

	if (nullptr != pObj3)
		pObj3->Add_Render(_pRenderer);
}

#endif

CRegion::CRegion(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRegion::NativeConstruct()
{
	if (nullptr == m_pNavigation)
		m_pNavigation = CNavigation::Create(m_pDevice, m_pContext);


#ifdef _DEBUG
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		m_pRenderer = static_cast<CRenderer*>(pGameInstance->Clone_Component(LEVEL_STATIC,
			PROTO_COMP_RENDERER));
	}
	END_SINGLETON(pGameInstance);
#endif
	return S_OK;
}

#ifdef _DEBUG
HRESULT CRegion::Add_Render()
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	for (auto& iter : m_CubeObjList)
		iter.Render_Objs(m_pRenderer);

	m_pRenderer->Add_Components(m_pNavigation);

	return S_OK;
}

#endif

HRESULT CRegion::Add_Cell(CCell* _pCell)
{
	if (nullptr != m_pNavigation)
	{
		m_pNavigation->Add_Cells(_pCell);
#ifdef _DEBUG
		CUBOBJS tObjs;
		tObjs.Create(m_pDevice, m_pContext, _pCell);
		m_CubeObjList.push_back(tObjs);
#endif
	}
	return S_OK;
}

HRESULT CRegion::Modify_Cells()
{
	if (nullptr == m_pNavigation)
		return E_FAIL;

	if (ImGui::TreeNodeEx("Region_Modfiy_Cells", ImGuiTreeNodeFlags_DefaultOpen))
	{
		vector<CCell*>* pCells = m_pNavigation->Get_Cells();
		for (vector<CCell*>::iterator it = pCells->begin(); it != pCells->end(); ++it)
		{
			char TEXT[MAX_PATH] = {};
			sprintf_s(TEXT, "Cell: %d", (*it)->Get_Index());
			if (ImGui::TreeNodeEx(TEXT, ImGuiTreeNodeFlags_DefaultOpen))
			{
#ifdef _DEBUG
				_bool bTrue1 = false, bTrue2 = false, bTrue3 = false;
#endif
				_float3 v3Pos1, v3Pos2, v3Pos3;
				{
					v3Pos1 = (*it)->Get_PointV3(CCell::POINT_A);
					_float PosArr[3] = { v3Pos1.x, v3Pos1.y, v3Pos1.z };
#ifdef _DEBUG
					bTrue1 = ImGui::InputFloat3("P1", PosArr, "%.2f");
#else
					ImGui::InputFloat3("P1", PosArr, "%.2f");
#endif

					v3Pos1 = _float3(PosArr[0], PosArr[1], PosArr[2]);
					(*it)->Set_PointV3(CCell::POINT_A, v3Pos1);
				}
				{
					v3Pos2 = (*it)->Get_PointV3(CCell::POINT_B);
					_float PosArr[3] = { v3Pos2.x, v3Pos2.y, v3Pos2.z };
#ifdef _DEBUG
					bTrue2 = ImGui::InputFloat3("P2", PosArr, "%.2f");
#else
					ImGui::InputFloat3("P2", PosArr, "%.2f");
#endif
					v3Pos2 = _float3(PosArr[0], PosArr[1], PosArr[2]);
					(*it)->Set_PointV3(CCell::POINT_B, v3Pos2);
				}
				{
					v3Pos3 = (*it)->Get_PointV3(CCell::POINT_C);
					_float PosArr[3] = { v3Pos3.x, v3Pos3.y, v3Pos3.z };
#ifdef _DEBUG
					bTrue3 = ImGui::InputFloat3("P3", PosArr, "%.2f");
#else
					ImGui::InputFloat3("P3", PosArr, "%.2f");
#endif
					v3Pos3 = _float3(PosArr[0], PosArr[1], PosArr[2]);
					(*it)->Set_PointV3(CCell::POINT_C, v3Pos3);
				}

#ifdef _DEBUG
				if (true == bTrue1 || true == bTrue2 || true == bTrue3)
				{
					for (auto& iter : m_CubeObjList)
						iter.Rebalance();
				}
#endif
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	return S_OK;
}

HRESULT CRegion::Show_Cells()
{
	if (nullptr == m_pNavigation)
		return E_FAIL;
	if (ImGui::TreeNodeEx("Region_Show_Cells", ImGuiTreeNodeFlags_DefaultOpen))
	{
		vector<CCell*>* pCells = m_pNavigation->Get_Cells();
		_uint iIndex = 0;
		for (vector<CCell*>::iterator it = pCells->begin(); it != pCells->end(); ++it)
		{
			char TEXT[MAX_PATH] = {};
			sprintf_s(TEXT, "Cell: %d", iIndex++);
			if (ImGui::TreeNodeEx(TEXT, ImGuiTreeNodeFlags_DefaultOpen))
			{
				{
					_float3 v3Pos = (*it)->Get_PointV3(CCell::POINT_A);
					sprintf_s(TEXT, "PA: (%.2f, %.2f, %.2f)", v3Pos.x, v3Pos.y, v3Pos.z);
					ImGui::Text(TEXT);
				}
				{
					_float3 v3Pos = (*it)->Get_PointV3(CCell::POINT_B);
					sprintf_s(TEXT, "PB: (%.2f, %.2f, %.2f)", v3Pos.x, v3Pos.y, v3Pos.z);
					ImGui::Text(TEXT);
				}
				{
					_float3 v3Pos = (*it)->Get_PointV3(CCell::POINT_C);
					sprintf_s(TEXT, "PC: (%.2f, %.2f, %.2f)", v3Pos.x, v3Pos.y, v3Pos.z);
					ImGui::Text(TEXT);
				}

				if (ImGui::Button("Delete"))
				{
#ifdef _DEBUG
					for (list<CUBOBJS>::iterator v = m_CubeObjList.begin(); v != m_CubeObjList.end(); ++v)
					{
						if ((*v).pCell == (*it))
						{
							(*v).Delete(*it);
							m_CubeObjList.erase(v);
							break;
						}
					}
#endif
					Safe_Release((*it));
					pCells->erase(it);
					ImGui::TreePop();
					break;
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	return S_OK;
}

HRESULT CRegion::Clear_Cell()
{
	return S_OK;
}

HRESULT CRegion::LoadFile(const _wstring& _strFile)
{
	if (nullptr == m_pNavigation)
		return E_FAIL;

	m_pNavigation->Load_File(_strFile.c_str());
#ifdef _DEBUG
	for (auto* iter : *m_pNavigation->Get_Cells())
	{
		CUBOBJS tObjs;
		tObjs.Create(m_pDevice, m_pContext, iter);
		m_CubeObjList.push_back(tObjs);
	}
#endif

	return S_OK;
}

HRESULT CRegion::SaveFile(const _wstring& _strFile)
{
	if (nullptr == m_pNavigation)
		return E_FAIL;

	m_pNavigation->Save_File(_strFile.c_str(), m_iIndex);
	return S_OK;
}

_bool CRegion::Is_Collision(CCollider* _pCollider)
{
	if (nullptr == _pCollider || nullptr == m_pNavigation)
		return false;

	return m_pNavigation->Is_Collision(_pCollider);
}

void CRegion::Control_Collider()
{
	if (nullptr == m_pNavigation)
		return;

	if (nullptr != m_pNavigation->Get_Collider())
	{
		if (ImGui::TreeNodeEx("ControlCollider", ImGuiTreeNodeFlags_DefaultOpen))
		{
			CCollider::COLLIDERDESC tCollData;
			tCollData.vScale = m_pNavigation->Get_Collider()->Get_Scale();
			tCollData.vTranslation = m_pNavigation->Get_Collider()->Get_Translate();

			_float Translate[3] = { tCollData.vTranslation.x,  tCollData.vTranslation.y, tCollData.vTranslation.z };
			_float CollSclae[3] = { tCollData.vScale.x ,  tCollData.vScale.y , tCollData.vScale.z };

			if (ImGui::InputFloat3("FindT", Translate, "%.4f"))
			{
				m_pNavigation->Get_Collider()->Set_Translate(_float3(Translate[0], Translate[1], Translate[2]));
			}
			if (ImGui::InputFloat3("TindS", CollSclae, "%.4f"))
			{
				m_pNavigation->Get_Collider()->Set_Scale(_float3(CollSclae[0], CollSclae[1], CollSclae[2]));
			}
			ImGui::TreePop();
		}
	}
}

void CRegion::Add_NeighborRegion(CRegion* _pRegion)
{
	if (nullptr == _pRegion  || nullptr == m_pNavigation)
		return;

	if (true == m_pNavigation->Is_Collision(_pRegion->Get_Navigation()->Get_Collider()))
	{
		for (auto& iter : m_NeighborRegion)
		{
			if (iter == _pRegion)
				return;
		}
		m_NeighborRegion.push_back(_pRegion);
	}
}

CRegion* CRegion::Is_Collision_OtherRegion(CCollider* _pCollider)
{
	if (nullptr == _pCollider || nullptr == m_pNavigation)
		return nullptr;

	for (auto& iter : m_NeighborRegion)
	{
		if (true == m_pNavigation->Is_Collision(_pCollider))
		{
			return iter;
		}
	}
	return nullptr;
}

void CRegion::Update_WorldObjects()
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		list<CGameObject*> List = pGameInstance->Find_GameObjectList(g_iCurrentLevel, LAYER_WORLDOBJ);

		if (List.size() != m_WorldObjList.size() || 0 == m_WorldObjList.size())
		{
			m_WorldObjList.clear();
			for (auto& iter : List)
			{
				CWorldObj* pWorldObj = static_cast<CWorldObj*>(iter);
				if (nullptr == pWorldObj)
					break;
				CCollider* _pCollider = static_cast<CCollider*>(pWorldObj->Get_Component(COM_COLLCOLLIDER));
				if (nullptr == _pCollider)
					break;

				if (true == _pCollider->Is_Collision(m_pNavigation->Get_Collider()))
					m_WorldObjList.push_back(pWorldObj);
			}
		}
	}
	END_SINGLETON(pGameInstance);
}

CRegion* CRegion::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CRegion* pInstance = new CRegion(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CStage::Create To Failed");
	}
	return pInstance;
}

void CRegion::Free()
{
#ifdef _DEBUG
	for (auto& iter : m_CubeObjList)
		iter.Release();

	Safe_Release(m_pRenderer);
#endif

	Safe_Release(m_pNavigation);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}