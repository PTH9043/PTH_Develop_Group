#include "EngineDefine.h"
#include "URegion.h"
#include "UGameInstance.h"
#include "UCell.h"
#include "UTransform.h"
#include "UDefaultDebugging.h"
#include "UCollider.h"
#include "UNavigation.h"
#include "UMethod.h"
#include "UDefaultCell.h"

#ifdef _EDIT_NAVI
void URegion::tagCubeObjs::Create(SHPTRREF<UCell> _pCell)
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	UDefaultDebugging::DEBUGTYPE eDebugType{ UDefaultDebugging::DEBUGTYPE::DEBUG_END };
	eDebugType = UDefaultDebugging::DEBUGTYPE::DEBUG_CUBE;
	// Clone Actor

	VOIDDATAS vecDatas;
	vecDatas.push_back(&eDebugType);

	spCube1 = static_pointer_cast<UDefaultDebugging>(spGameInstance->CloneActorAdd(
		PROTO_ACTOR_DEUBGGINGDEFAULTOBJECT, vecDatas));;
	spCube1->GetTransform()->SetPos(_pCell->GetPoint(UCell::POINT_A));
	
	spCube2 = static_pointer_cast<UDefaultDebugging>(spGameInstance->CloneActorAdd(
		PROTO_ACTOR_DEUBGGINGDEFAULTOBJECT, vecDatas));;
	spCube2->GetTransform()->SetPos(_pCell->GetPoint(UCell::POINT_B));
	
	spCube3 = static_pointer_cast<UDefaultDebugging>(spGameInstance->CloneActorAdd(
		PROTO_ACTOR_DEUBGGINGDEFAULTOBJECT, vecDatas));;
	spCube3->GetTransform()->SetPos(_pCell->GetPoint(UCell::POINT_C));
	

	spCell = _pCell;
}

void URegion::tagCubeObjs::Rebalance()
{
	if (nullptr == spCell)
		return;

	if (nullptr == spCube1 || nullptr == spCube2 || nullptr == spCube3)
		return;

	spCube1->GetTransform()->SetPos(spCell->GetPoint(UCell::POINT_A));
	spCube2->GetTransform()->SetPos(spCell->GetPoint(UCell::POINT_B));
	spCube3->GetTransform()->SetPos(spCell->GetPoint(UCell::POINT_C));

	spCell->ReBuffer();
}

void URegion::tagCubeObjs::AddCubesRenderGroup()
{
	if (nullptr != spCube1)
		spCube1->AddRenderGroup(RENDERID::RI_NONALPHA_LAST);

	if (nullptr != spCube2)
		spCube2->AddRenderGroup(RENDERID::RI_NONALPHA_LAST);

	if (nullptr != spCube3)
		spCube3->AddRenderGroup(RENDERID::RI_NONALPHA_LAST);
}

HRESULT URegion::AddAllCellsRenderGroup()
{
	for (auto& iter : m_CubeObjList)
		iter.AddCubesRenderGroup();

	RETURN_CHECK_FAILED(nullptr == m_spNavigation, E_FAIL);

	for (SHPTR<UCell> iter : *m_spNavigation->GetCells())
	{
		iter->AddCellRenderGroup();
	}
	return S_OK;
}

HRESULT URegion::AddCellsRenderGroup(_int& begin, _int& end)
{
	RETURN_CHECK_FAILED(nullptr == m_spNavigation, E_FAIL);

	auto cells = m_spNavigation->GetCells();
	if (!cells) {
		return E_FAIL;
	}
	_int cellCount = static_cast<_int>(cells->size());
	if (begin >= cellCount || end >= cellCount || begin > end) {
		return E_INVALIDARG;
	}

	auto cubeIter = m_CubeObjList.begin();
	std::advance(cubeIter, begin);
	for (_int i = begin; i <= end; ++i, ++cubeIter) {
		cubeIter->AddCubesRenderGroup();
	}

	for (_int i = begin; i <= end; ++i) {
		(*cells)[i]->AddCellRenderGroup();
	}

	return S_OK;
}
#endif

URegion::URegion(CSHPTRREF<UDevice> _spDevice)
	: UComponent(_spDevice),
	m_f3Color{0.6f, 0.f, 0.f},
	m_spNavigation{ nullptr },
	m_iIndex{ 0 },
	m_NeighborRegion{},
	m_wsRegionName{ "name" },
	m_bEditName{false}
#ifdef _EDIT_NAVI
	,m_DeleteCellsList{ nullptr },
	m_bDeletionEnabled{ false },
	m_CubeObjList{},
	m_DeleteCubesList{ nullptr }
#endif
{
}

URegion::URegion(const URegion& _rhs)
	: UComponent(_rhs),
	m_f3Color{ 0.6f, 0.f, 0.f },
	m_spNavigation{ nullptr },
	m_iIndex{ 0 },
	m_NeighborRegion{},
	m_wsRegionName{"name"},
	m_bEditName{false}
#ifdef _EDIT_NAVI
	,m_DeleteCellsList{ nullptr },
	m_bDeletionEnabled{ false }
	,m_CubeObjList{},
	m_DeleteCubesList{nullptr}
#endif
{
}

void URegion::Free()
{
}

HRESULT URegion::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	if (nullptr == m_spNavigation)
		m_spNavigation = CreateConstructorNative<UNavigation>(spGameInstance->GetDevice());
	return S_OK;
}

HRESULT URegion::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);
	


	return S_OK;
}


#ifdef _EDIT_NAVI
HRESULT URegion::AddCell(SHPTR<UCell>& _pCell)
{
	if (nullptr != m_spNavigation)
	{
		m_spNavigation->AddCell(_pCell);
#ifdef _EDIT_NAVI
		CUBOBJS tObjs;
		tObjs.Create(_pCell);
		m_CubeObjList.push_back(tObjs);
#endif
	}
	return S_OK;
}


HRESULT URegion::ShowCells()
{
	RETURN_CHECK_FAILED(nullptr == m_spNavigation, E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	if (ImGui::TreeNodeEx("Region_Show_Cells", ImGuiTreeNodeFlags_DefaultOpen))
	{
		SHPTR<CELLCONTAINER> pCells = m_spNavigation->GetCells();
		_uint iIndex = 0;

		for (CELLCONTAINER::iterator it = pCells->begin(); it != pCells->end(); ++it)
		{
			char TEXT[MAX_PATH] = {};
			sprintf_s(TEXT, "Cell: %d", iIndex++);
			if (ImGui::TreeNodeEx(TEXT, ImGuiTreeNodeFlags_DefaultOpen))
			{
				{
					_float3 v3Pos = (*it)->GetPoint(UCell::POINT_A);
					sprintf_s(TEXT, "PA: (%.2f, %.2f, %.2f)", v3Pos.x, v3Pos.y, v3Pos.z);
					ImGui::Text(TEXT);
				}
				{
					_float3 v3Pos = (*it)->GetPoint(UCell::POINT_B);
					sprintf_s(TEXT, "PB: (%.2f, %.2f, %.2f)", v3Pos.x, v3Pos.y, v3Pos.z);
					ImGui::Text(TEXT);
				}
				{
					_float3 v3Pos = (*it)->GetPoint(UCell::POINT_C);
					sprintf_s(TEXT, "PC: (%.2f, %.2f, %.2f)", v3Pos.x, v3Pos.y, v3Pos.z);
					ImGui::Text(TEXT);
				}

				if (ImGui::Button("Delete"))
				{
					for (LIST<CUBOBJS>::iterator v = m_CubeObjList.begin(); v != m_CubeObjList.end(); ++v)
					{
						if ((*v).spCell == (*it))
						{
							m_DeleteCubesList.insert((*v).spCube1);
							m_DeleteCubesList.insert((*v).spCube2);
							m_DeleteCubesList.insert((*v).spCube3);
							break;
						}
					}
					m_DeleteCellsList.insert((*it));
					m_bDeletionEnabled = true;
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

HRESULT URegion::RearrageCells()
{
	RETURN_CHECK_FAILED(nullptr == m_spNavigation, E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	SHPTR<CELLCONTAINER> pCells = m_spNavigation->GetCells();
	int i = 0;
	for (CELLCONTAINER::iterator iter = pCells->begin(); iter != pCells->end();)
	{
		//셀들의 인덱스 재설정
		(*iter)->SetIndex(i++);
		++iter;
	}
	m_spNavigation->ReadyNeighbor();
	return S_OK;
}


HRESULT URegion::ClearCell()
{
	return S_OK;
}


HRESULT URegion::SetColor()
{
	RETURN_CHECK(nullptr == m_spNavigation, E_FAIL)

	if (ImGui::TreeNodeEx("Edit_Region_Color", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::BeginGroup();
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::PushItemWidth(200);
		ImGui::ColorEdit4("##Pick_Region_Color", (float*)&m_f3Color,
			ImGuiColorEditFlags_None | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoSidePreview);	
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}

	return S_OK;
}

HRESULT URegion::SetName()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	RETURN_CHECK(nullptr == m_spNavigation, E_FAIL);
		
	if (ImGui::TreeNodeEx("Edit_Region_Name", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::BeginGroup();
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::PushItemWidth(200);
		// _wstring을 출력
		ImGui::Text("Region Name: %ls", m_wsRegionName.c_str());

		ImGui::Checkbox("Edit Region Name", &m_bEditName);
		if (m_bEditName)
		{
			static char regionNameBuffer[256];
			ImGui::InputText("##RegionName", regionNameBuffer, IM_ARRAYSIZE(regionNameBuffer));
			ImGui::SameLine();
			if (ImGui::Button("Confirm"))
			{
				m_wsRegionName = regionNameBuffer;
				m_bEditName = false; // Exit edit mode
			}
		}
	
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}
	return S_OK;
}

HRESULT URegion::DeleteLatestCell()
{
	SHPTR<CELLCONTAINER> pCells = m_spNavigation->GetCells();
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	if (!pCells->empty()) {
		CELLCONTAINER::iterator it = --pCells->end();

		for (LIST<CUBOBJS>::iterator v = m_CubeObjList.begin(); v != m_CubeObjList.end(); ++v)
		{
			if ((*v).spCell == (*it))
			{
				m_DeleteCubesList.insert((*v).spCube1);
				m_DeleteCubesList.insert((*v).spCube2);
				m_DeleteCubesList.insert((*v).spCube3);
				break;
			}
		}
		m_DeleteCellsList.insert((*it));
		m_bDeletionEnabled = true;
	}
	return S_OK;
}

HRESULT URegion::DeleteCell(const _uint& _iIndex)
{
	SHPTR<CELLCONTAINER> pCells = m_spNavigation->GetCells();
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	if (_iIndex < pCells->size())
	{
		CELLCONTAINER::iterator it = pCells->begin();
		std::advance(it, _iIndex);

		for (LIST<CUBOBJS>::iterator v = m_CubeObjList.begin(); v != m_CubeObjList.end(); ++v)
		{
			if ((*v).spCell == (*it))
			{
				m_DeleteCubesList.insert((*v).spCube1);
				m_DeleteCubesList.insert((*v).spCube2);
				m_DeleteCubesList.insert((*v).spCube3);
				break;
			}
		}
		m_DeleteCellsList.insert((*it));
		m_bDeletionEnabled = true;
		return S_OK;
	}
	else
	{
		return E_INVALIDARG;
	}
}


void URegion::FlushDeleteCells()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	SHPTR<CELLCONTAINER> pCells = m_spNavigation->GetCells();

	for (LIST<CUBOBJS>::iterator v = m_CubeObjList.begin(); v != m_CubeObjList.end();)
	{
		if (m_DeleteCubesList.find(v->spCube1) != m_DeleteCubesList.end()
			|| m_DeleteCubesList.find(v->spCube2) != m_DeleteCubesList.end()
			|| m_DeleteCubesList.find(v->spCube3) != m_DeleteCubesList.end())
		{
			spGameInstance->RemoveActor(v->spCube1);
			spGameInstance->RemoveActor(v->spCube2);
			spGameInstance->RemoveActor(v->spCube3);

			v->spCube1.reset();
			v->spCube2.reset();
			v->spCube3.reset();
			v = m_CubeObjList.erase(v);
		}
		else
			++v;
	}
	m_DeleteCubesList.clear();

	int i = 0;
	for (CELLCONTAINER::iterator iter = pCells->begin(); iter != pCells->end();)
	{
		if (m_DeleteCellsList.find((*iter)) != m_DeleteCellsList.end())
		{
			spGameInstance->RemoveActor((*iter)->GetCellPawn());
			(*iter).reset();
			iter = pCells->erase(iter);
		}
		else
		{
			iter++;
		}
	}
	RearrageCells();
	m_spNavigation->ReadyNeighbor();

	m_DeleteCellsList.clear();
	m_bDeletionEnabled = false;
}
#endif

_bool URegion::Load(const _wstring& _wstrPath)
{
	RETURN_CHECK(nullptr == m_spNavigation, false)
	m_spNavigation->Load(_wstrPath);
#ifdef _EDIT_NAVI
	for (SHPTR<UCell> iter : *m_spNavigation->GetCells())
	{
		CUBOBJS tObjs;
		tObjs.Create(iter);		
		m_CubeObjList.push_back(tObjs);
	}
#endif
	return S_OK;
}

_bool URegion::Save(const _wstring& _wstrPath)
{
	RETURN_CHECK(nullptr == m_spNavigation, false)
	m_spNavigation->Save(_wstrPath);
	return S_OK;
}

_bool URegion::Is_Collision(SHPTR<UCollider>& _pCollider)
{
	if (nullptr == _pCollider || nullptr == m_spNavigation)
		return false;

	return m_spNavigation->IsCollision(_pCollider);
}


void URegion::Add_NeighborRegion(CSHPTRREF<URegion> _pRegion)
{
	if (nullptr == _pRegion || nullptr == m_spNavigation)
		return;

	SHPTR<UCollider> pCollider = _pRegion->GetNavigation()->GetCollider();
	
	if (true == m_spNavigation->IsCollision(pCollider))
	{
		for (auto& iter : m_NeighborRegion)
		{
			if (iter.lock() == _pRegion)
				return;
		}
		m_NeighborRegion.push_back(_pRegion);
	}
}
