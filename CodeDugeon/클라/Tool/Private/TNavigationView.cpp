#include "ToolDefines.h"
#include "UGameInstance.h"
#include "TNavigationView.h"
#include "UStageManager.h"
#include "UStage.h"
#include "UInputManager.h"
#include "UPicking.h"
#include "UNavigation.h"
#include "URegion.h"
#include "UDefaultDebugging.h"
#include "UTransform.h"
#include "UCell.h"
#include "TMainScene.h"
#include "UGrid.h"
#include "UPawn.h"
#include "UDefaultCell.h"
#include "UVIBufferCell.h"

TNavigationView::TNavigationView(CSHPTRREF<UDevice> _spDevice) :
	TImGuiView(_spDevice, "NavigationView"),
	m_stMainDesc{},
	m_stNavigationView{},
	m_isInitSetting{ false },
	m_bNavigationModify{ false },
	m_bAllRender{ false },
	m_bRenderWireFrame{ false },
	m_bNavigationDebugColor{ false },
	m_dShowDeltaTime{ 0.0 },
	m_iCreateRegionIndex{ 0 },
	m_iRegionIndex{ 0 },
	m_spStageManager{ nullptr },
	m_vecPosList{},
	m_iSelIndex{ SEL_1 },
	m_bSelEnd{ false },
	m_iCellIndex{ 0 },
	m_bOnWindow{false},
	m_bPickOnCell{ false },
	m_spSelectedCell{nullptr}, 
	m_iWillRenderCellIndexBegin{0},
	m_iWillRenderCellIndexEnd{ 0 }
{
}

void TNavigationView::Free()
{
}

HRESULT TNavigationView::NativeConstruct()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_stMainDesc = MAINDESC(ImGuiWindowFlags_NoBackground, ImGuiDockNodeFlags_None,
		ImVec2{ (_float)WINDOW_WIDTH, 0.f }, ImVec2{ 500.f, (_float)WINDOW_HEIGHT });
	m_stNavigationView = DOCKDESC("NavigationViewer", ImGuiWindowFlags_NoFocusOnAppearing,
		ImGuiDockNodeFlags_CentralNode);

	m_spStageManager = Create<UStageManager>();


#ifdef _USE_DEBUGGING
	UDefaultDebugging::DEBUGTYPE eDebugType{ UDefaultDebugging::DEBUGTYPE::DEBUG_END };
	eDebugType = UDefaultDebugging::DEBUGTYPE::DEBUG_CUBE_WHITE;
	// Clone Actor

	VOIDDATAS vecDatas;
	vecDatas.push_back(&eDebugType); 
	for (_uint i = 0; i < SEL_END; i++)
	{
		m_spCubePosArr[i] = static_pointer_cast<UDefaultDebugging>(spGameInstance->CloneActorAddAndNotInLayer(
			PROTO_ACTOR_DEUBGGINGDEFAULTOBJECT, vecDatas));
		m_spCubePosArr[i]->GetTransform()->SetPos(_float3(0.f, 0.f, 0.f));
	}
#endif

	return S_OK;
}

HRESULT TNavigationView::LoadResource()
{
	return E_NOTIMPL;
}

HRESULT TNavigationView::ReleaseResource()
{
	return E_NOTIMPL;
}

void TNavigationView::TickActive(const _double& _dTimeDelta)
{
#ifdef _EDIT_NAVI
	if (nullptr != m_spStageManager->GetStage())
		m_spStageManager->GetStage()->FlushDeleteCells();

	if(m_bPickOnCell)
	{
		if (nullptr != m_spStageManager->GetStage())
		{
			for (auto& Regions : *m_spStageManager->GetStage()->GetRegionList())
			{
				CELLCONTAINER Cells = *Regions->GetNavigation()->GetCells();
				for (auto& cell : Cells)
				{

					SHPTR<UPawn> cellPawn = static_pointer_cast<UPawn>(cell->GetCellPawn());
					SHPTR<UVIBuffer> cellVIBuffer = static_pointer_cast<UVIBuffer>(cell->GetCellVIBuffer());

					GetGameInstance()->AddPickingObject(cellPawn, cellVIBuffer);
				}
			}
		}
	}
#endif 
}

void TNavigationView::LateTickActive(const _double& _dTimeDetla)
{
#ifdef _EDIT_NAVI
	if (nullptr != m_spStageManager->GetStage())
		m_spStageManager->GetStage()->UpdateRegion();

	if (false == m_bAllRender)
	{
		if (nullptr != m_spStageManager->GetStage())
			m_spStageManager->GetStage()->AddRender(m_iRegionIndex, m_iWillRenderCellIndexBegin, m_iWillRenderCellIndexEnd);
	}
	else
	{
		if (nullptr != m_spStageManager->GetStage())
			m_spStageManager->GetStage()->AddRenderAll(m_iRegionIndex);
	}

	for (_uint i = 0; i < SEL_END; i++)
		m_spCubePosArr[i]->AddRenderer(RENDERID::RI_NONALPHA_LAST);
#endif

}

void TNavigationView::RenderActive()
{
	ImGui::SetNextWindowPos(m_stMainDesc.vPos, ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(m_stMainDesc.vSize, ImGuiCond_Appearing);
	// Name, OpenPointer, MainDesc 
	ImGui::Begin(GetName().c_str(), GetOpenPointer(), m_stMainDesc.imgWindowFlags);
	{
		m_stMainDesc.iDockSpaceID = ImGui::GetID(GetName().c_str());
		DockBuildInitSetting();
		ImGui::DockSpace(m_stMainDesc.iDockSpaceID, ImVec2{}, m_stMainDesc.imgDockNodeFlags);
		NavigationView();
	}
	ImGui::End(); 
}


void TNavigationView::RenderMenu()
{
}

void TNavigationView::DockBuildInitSetting()
{
	RETURN_CHECK(true == m_isInitSetting, ;);
	{
		ImGuiID dock_main_id = m_stMainDesc.iDockSpaceID;
		ImGui::DockBuilderRemoveNode(m_stMainDesc.iDockSpaceID);
		ImGui::DockBuilderAddNode(m_stMainDesc.iDockSpaceID);
		// Docking Build 
		m_stNavigationView.iDockSpaceID = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 1.f, NULL, &dock_main_id);
		// DockBuild
		ImGui::DockBuilderDockWindow(m_stNavigationView.strName.c_str(), m_stNavigationView.iDockSpaceID);
		ImGui::DockBuilderFinish(m_stMainDesc.iDockSpaceID);
	}
	m_isInitSetting = true;
}

void TNavigationView::ModifyNavigation(CSHPTRREF<URegion> _spRegion)
{
#ifdef _EDIT_NAVI
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	if (nullptr == _spRegion)
		return;

	_float2 MousePos = spGameInstance->GetMousePosition();
	if (MousePos.x > 0 && MousePos.x < WINDOW_WIDTH)
	{
		if (MousePos.y > 0 && MousePos.y < WINDOW_HEIGHT)
		{
			if (true == spGameInstance->GetDIMBtnDown(DIMOUSEBUTTON::DIMB_L))
			{
				_float3 v3Pos;
				SHPTR<UNavigation> _spNav = _spRegion->GetNavigation();
				PICKINGDESC tDesc = spGameInstance->GetPickDesc();
				CELLCONTAINER Cells;
				if (nullptr != _spNav)
					Cells = *_spNav->GetCells();
				REGIONLIST Regions = (*m_spStageManager->GetStage()->GetRegionList());
				if (!tDesc.bPickingSuccess)
					return;
				v3Pos = tDesc.vPickPos;

				if (!m_bPickOnCell)
				{
					m_iCellIndex = static_cast<_uint>(Cells.size());
					for (auto& iter : Cells)
					{
						{
							for (_uint i = 0; i < UCell::POINT_END; ++i)
							{
								_float3 Point = iter->GetPoint((UCell::POINT)i);

								_float approxNum = 1.5f;
								if (v3Pos.y >= Point.y - approxNum && v3Pos.y <= Point.y + approxNum)
								{
									if (v3Pos.x >= Point.x - approxNum && v3Pos.x <= Point.x + approxNum)
									{
										if (v3Pos.z >= Point.z - approxNum && v3Pos.z <= Point.z + approxNum)
										{
											v3Pos = Point;
										}
									}
								}
							}
						}
					}
					m_vecPosList.push_back(v3Pos);

					m_spCubePosArr[m_iSelIndex++]->GetTransform()->SetPos(XMLoadFloat3(&v3Pos));
					if ((_uint)m_vecPosList.size() == 3)
					{
						ARRAY<_float3, 3> vPos = { m_vecPosList[SEL_1], m_vecPosList[SEL_2], m_vecPosList[SEL_3] };
						SHPTR<UCell> NewCell = CreateConstructorNative<UCell>(spGameInstance->GetDevice(), vPos, m_iCellIndex);
						_spRegion->AddCell(NewCell);
						m_vecPosList.clear();
						m_iSelIndex = SEL_1;
						for (int i = 0; i < 3; i++)
							m_spCubePosArr[i]->GetTransform()->SetPos(_float3(0.f, 0.f, 0.f));
					}
				}
				else
				{
					m_spSelectedCell = _spNav->FindCell(v3Pos);
				}
			}
			// Rollback if backspace key is pressed
			else if (spGameInstance->GetDIKeyDown(DIK_BACK))
			{
				if (!m_vecPosList.empty())
				{
					m_vecPosList.pop_back();
					m_iSelIndex--;
					if (m_iSelIndex < SEL_1)
						m_iSelIndex = SEL_1;
					m_spCubePosArr[m_iSelIndex]->GetTransform()->SetPos(_float3(0.f, 0.f, 0.f));
				}
				else
				{
					_spRegion->DeleteLatestCell();
				}
			}
		}
	}
#endif
}

void TNavigationView::SaveCurrentRegions()
{
	if (ImGui::Button("Save Current Regions"))
	{
		if (m_spStageManager->Save(FIRST_RESOURCE_FOLDER))
		{
			ImGui::OpenPopup("Save Success");
		}
		else
			ImGui::OpenPopup("Save Failed");
	}

	ImGui::SetNextWindowPos(ImVec2(WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT));

	if (ImGui::BeginPopupModal("Save Success", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{	
		ImGui::Text("Current Regions Successfully Saved At //Resource//Navigation");
		m_bOnWindow = true;
		if (ImGui::Button("OK", ImVec2(120, 0))) 
		{
			ImGui::CloseCurrentPopup(); 
			m_bOnWindow = false;
		}
		ImGui::EndPopup();
	}
	if (ImGui::BeginPopupModal("Save Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Save Failed");
		m_bOnWindow = true;
		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			m_bOnWindow = false;
		}
		ImGui::EndPopup();
	}

}

void TNavigationView::LoadRegionsFromFile()
{
	static bool showConfirmationDialog = false;
	if (ImGui::Button("Load Regions from File"))
	{
		showConfirmationDialog = true;
	}

	if (showConfirmationDialog)
	{
		ImGui::OpenPopup("Confirmation##LoadRegionsDialog");
		ImGui::SetNextWindowPos(ImVec2(WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT));
		if (ImGui::BeginPopupModal("Confirmation##LoadRegionsDialog", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			m_bOnWindow = true;
			ImGui::Text("Your current proccess may get overwritten. Continue?");
			ImGui::Separator();

			if (ImGui::Button("Yes", ImVec2(120, 0)))
			{
				m_spStageManager->Load();
				ImGui::CloseCurrentPopup(); 
				showConfirmationDialog = false; 
				m_bOnWindow = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("No", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup(); 
				showConfirmationDialog = false;
				m_bOnWindow = false;
			}
			ImGui::EndPopup();
		}
	}

}


void TNavigationView::NavigationView()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	ImGui::Begin(m_stNavigationView.strName.c_str(), GetOpenPointer(), m_stNavigationView.imgWindowFlags);
	{
		// Help window
		if (ImGui::Button("Help"))
		{
			ImGui::OpenPopup("Help Popup");
		}
#ifdef _EDIT_NAVI
		if (ImGui::BeginPopup("Help Popup"))
		{
			ImGui::Text("Controls:");
			ImGui::BulletText("Check 'Modify Navigation' to enable navigation modification.");
			ImGui::BulletText("Check 'Render_All_Regions' to render all regions.");
			ImGui::BulletText("Input an integer to set new region's index.");
			ImGui::BulletText("Click 'Create Region' to create a new region with the specified index.");
			ImGui::BulletText("Select a region from the list to modify its navigation.");
			ImGui::BulletText("Move the mouse within the window to modify navigation.");
			ImGui::BulletText("Click on the screen to set new cell's positions.");
			ImGui::BulletText("Use Backspace key to undo latest action.");
			ImGui::EndPopup();
		}

		ImGui::Checkbox("Modify Navigation", &m_bNavigationModify);
		if (true == m_bNavigationModify)
		{
			ImGui::Checkbox("Render_All_Regions", &m_bAllRender);

			if(!m_bAllRender)
			{
				ImGui::InputInt("Render Begin", &m_iWillRenderCellIndexBegin);
				ImGui::InputInt("Render End", &m_iWillRenderCellIndexEnd);
			}


			LoadRegionsFromFile();

			m_spStageManager->GetStage()->CreateRegion();
			SHPTR<REGIONLIST> Regionlist = m_spStageManager->GetStage()->GetRegionList();

			if (Regionlist->size() > 0)
			{
				SaveCurrentRegions();
				_uint iSelect = m_spStageManager->GetStage()->SelectRegion();
				if (INVALID_MINUS_STAGEVALUE != iSelect)
					m_iRegionIndex = iSelect;

				ImGui::Checkbox("Pick_On_Cell", &m_bPickOnCell);
				if(m_bPickOnCell)
				{
					if (ImGui::TreeNodeEx("Selected Cell", ImGuiTreeNodeFlags_DefaultOpen))
					{
						if (m_spSelectedCell == nullptr)
						{
							ImGui::Text("Select Cell by picking");
						}
						else
						{
							auto stage = m_spStageManager->GetStage();
							auto region = stage->GetRegion(m_iRegionIndex);
							auto navigation = region->GetNavigation();
							auto cellsContainer = navigation->GetCells();
							const std::vector<SHPTR<UCell>>& cells = *cellsContainer;
							_int selectedIndex = m_spSelectedCell->GetIndex();
							ARRAY<_int, 3> selectedNeighbors = m_spSelectedCell->GetNeighbor();
							auto cell = cells[selectedIndex];

							ImGui::Text("Selected Cell Index: %d", selectedIndex);
							ImGui::Text("Selected Cell Jumpable : %s", cell->GetJumpableState() ? "true" : "false");
							ImGui::Text("Selected Cell Neighbors : %d, %d, %d", selectedNeighbors[0], selectedNeighbors[1], selectedNeighbors[2]);
							if (ImGui::Button("Delete"))
							{
								region->DeleteCell(selectedIndex);
								m_spSelectedCell.reset();
							}
							if (ImGui::Button("Set Jumpable"))
							{
								if (selectedIndex >= 0 && selectedIndex < cells.size()) {
									cell->SetJumpableState(true);
								}
							}
							if (ImGui::Button("Unset Jumpable"))
							{
								if (selectedIndex >= 0 && selectedIndex < cells.size()) {
									cell->SetJumpableState(false);
								}
							}
						}
						ImGui::TreePop();
					}
				}

				if(spGameInstance->IsMouseInWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT) && !m_bOnWindow)
					ModifyNavigation(m_spStageManager->GetStage()->GetRegion(m_iRegionIndex));
				m_spStageManager->GetStage()->SetRegionName(m_iRegionIndex);
				if (ImGui::Button("Rearrange_Cells"))
				{
					m_spStageManager->GetStage()->RearrangeCells(m_iRegionIndex);
				}
				m_spStageManager->GetStage()->SetColor(m_iRegionIndex);
				m_spStageManager->GetStage()->ShowCells(m_iRegionIndex);
				m_spStageManager->GetStage()->Delete_Region(m_iRegionIndex);
			}
		}
#else
		if (ImGui::BeginPopup("Help Popup"))
		{
			ImGui::Text("Enable _EDIT_NAVI from UCell.cpp to Edit Navigation:");
			ImGui::EndPopup();
		}
#endif
		ImGui::End();
	}
}
