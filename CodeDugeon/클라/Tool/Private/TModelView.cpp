#include "ToolDefines.h"
#include "TModelView.h"
#include "UGameInstance.h"
#include "UMethod.h"
#include "UPawn.h"
#include "TAssimpModel.h"
#include "UModel.h"
#include "UAnimModel.h"
#include "TShowAnimModelObject.h"
#include "TShowModelObject.h"
#include "UAnimation.h"
#include "TImGuiManager.h"
#include "UTransform.h"
#include "TUnityModelLoader.h"
#include "TGuizmoManager.h"
#include "UPicking.h"
#include "UMapLayout.h"
#include "UVIBuffer.h"
#include "UMeshContainer.h"
#include "UCollider.h"


TModelView::TModelView(CSHPTRREF<UDevice> _spDevice) :
	TImGuiView(_spDevice, "ModelView"),
	m_stMainDesc{},
	m_stModelDockDesc{},
	m_stAnimModelDockDesc{},
	m_stTransformEditorDesc{},
	m_ModelsContainer{},
	m_AnimModelContainer{},
	m_DeleteModelsContainer{},
	m_DeleteAnimModelsContainer{},
	m_spModelFileFolder{ nullptr },
	m_spAnimModelFileFolder{ nullptr },
	m_vModelScale{},
	m_vAnimModelScale{},
	m_iFileIndex{ 0 },
	m_isAllConverter{ false },
	m_spShowAnimModelObject{ nullptr },
	m_spShowModelObject{ nullptr },
	m_isInitSetting{ false },
	m_isResetModel{ false },
	m_isResetAnimModel{ false },
	m_spGuizmoManager{nullptr},
	m_bSelectedhasAnim{false},
	m_bColliderActive{false},
	m_ShowAnimModelsContainer{},
	m_ShowModelsContainer{},
	m_spSelectedModel{nullptr},
	m_SelectedModelName{},
	m_spCopiedModel{nullptr},
	m_CopiedModelName{},
	m_spMapLayout{nullptr},
	m_bshowLayoutAddPopup{false},
	m_bLayoutSavePopup{false},
	m_bLayoutLoadPopup{false},
	m_bErrorPopup{},
	m_bEditPosByPicking{false},
	m_bisModelsLoaded{false},
	m_bisAnimModelsLoaded{false}
{
}

void TModelView::Free()
{
}

HRESULT TModelView::NativeConstruct()
{
	m_stMainDesc = MAINDESC(
		ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse, ImGuiDockNodeFlags_None,
		ImVec2{ (_float)WINDOW_WIDTH, 0.f }, ImVec2{ 500.f, (_float)WINDOW_HEIGHT });

	m_stModelDockDesc = DOCKDESC("ModelViewer", ImGuiWindowFlags_None,
		ImGuiDockNodeFlags_CentralNode);
	m_stAnimModelDockDesc = DOCKDESC("AnimModelViewer", ImGuiWindowFlags_None,
		ImGuiDockNodeFlags_CentralNode);
	m_stTransformEditorDesc = DOCKDESC("ModelEditor", ImGuiWindowFlags_NoFocusOnAppearing,
		ImGuiDockNodeFlags_CentralNode);

	m_spGuizmoManager = Create<TGuizmoManager>();
	m_spMapLayout = CreateConstructorNativeNotMsg<UMapLayout>(GetGameInstance()->GetDevice());

	return S_OK;
}

HRESULT TModelView::LoadResource()
{
	//// AnimModelPivotScale
	//m_vAnimModelPivotScale = m_spShowAnimModelObject->GetTransform()->GetScale();
	return S_OK;
}

HRESULT TModelView::ReleaseResource()
{
	m_AnimModelContainer.clear();
	m_ModelsContainer.clear();
	m_ShowModelsContainer.clear();
	m_ShowAnimModelsContainer.clear();

	m_spModelFileFolder = nullptr;
	m_spAnimModelFileFolder = nullptr;

	m_spShowModelObject.reset();
	m_spShowAnimModelObject.reset();
	m_spSelectedModel.reset();

	m_DeleteModelsContainer.clear();
	m_DeleteAnimModelsContainer.clear();

	GetGameInstance()->RemoveActor(m_spShowModelObject);
	GetGameInstance()->RemoveActor(m_spShowAnimModelObject);

	ActiveResetSceneData();
	return S_OK;
}

void TModelView::TickActive(const _double& _dTimeDelta)
{
	for (auto& model : m_DeleteModelsContainer)
	{
		auto it = m_ShowModelsContainer.find(model);

		if (it != m_ShowModelsContainer.end())
		{
			m_ShowModelsContainer.erase(it);
		}
	}
	m_DeleteModelsContainer.clear();

	for (auto& model : m_DeleteAnimModelsContainer)
	{
		auto it = m_ShowAnimModelsContainer.find(model);

		if (it != m_ShowAnimModelsContainer.end())
		{
			m_ShowAnimModelsContainer.erase(it);
		}
	}
	m_DeleteAnimModelsContainer.clear();

}

void TModelView::LateTickActive(const _double& _dTimeDetla)
{
	if(m_spSelectedModel != nullptr)
	{
		if(m_bColliderActive)
		{
			for (auto& Colliders : m_spSelectedModel->GetColliderContainer())
				Colliders.second->AddRenderer(RENDERID::RI_NONALPHA_LAST);
		}
	}

	//for (auto& ShowModel : m_ShowModelsContainer)
	//{
	//	//픽킹이 가능하도록 픽킹목록에 추가
	//	SHPTR<UModel> spModel = ShowModel.second->GetShowModel();
	//	for (auto& Mesh : spModel->GetMeshContainers())
	//	{
	//		SHPTR<UVIBuffer> spVIbuffer = static_pointer_cast<UVIBuffer>(Mesh);
	//		SHPTR<UPawn> pawnModel = static_pointer_cast<UPawn>(ShowModel.second);
	//		GetGameInstance()->AddPickingObject(pawnModel, spVIbuffer);
	//	}
	//}

	//for (auto& ShowAnimModel : m_ShowAnimModelsContainer)
	//{
	//	//픽킹이 가능하도록 픽킹목록에 추가
	//	SHPTR<UModel> spModel = ShowAnimModel.second->GetAnimModel();
	//	for (auto& Mesh : spModel->GetMeshContainers())
	//	{
	//		SHPTR<UVIBuffer> spVIbuffer = static_pointer_cast<UVIBuffer>(Mesh);
	//		SHPTR<UPawn> pawnModel = static_pointer_cast<UPawn>(ShowAnimModel.second);
	//		GetGameInstance()->AddPickingObject(pawnModel, spVIbuffer);
	//	}
	//}
}

void TModelView::RenderActive()
{
	ImGui::SetNextWindowPos(m_stMainDesc.vPos, ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(m_stMainDesc.vSize, ImGuiCond_Appearing);
	// Name, OpenPointer, MainDesc 
	ImGui::Begin(GetName().c_str(), GetOpenPointer(), m_stMainDesc.imgWindowFlags);
	{
		m_stMainDesc.iDockSpaceID = ImGui::GetID(GetName().c_str());
		DockBuildInitSetting();
		ImGui::DockSpace(m_stMainDesc.iDockSpaceID, ImVec2{}, m_stMainDesc.imgDockNodeFlags);
		// Show Model
		ImGui::NewLine();
		ShowModels();
		MouseInput();
		KeyboardInput();
		AddModelstoMapLayout();
		EditModel();
	}
	ImGui::End();

}

void TModelView::DockBuildInitSetting()
{
	// 초기 설정이 이미 완료되었다면 중단
	RETURN_CHECK(true == m_isInitSetting, ;);

	{
		ImGuiID dock_main_id = m_stMainDesc.iDockSpaceID;

		// 기존 DockSpace 제거 및 새로 추가
		ImGui::DockBuilderRemoveNode(m_stMainDesc.iDockSpaceID);
		ImGui::DockBuilderAddNode(m_stMainDesc.iDockSpaceID);

		// 기존 노드에 윈도우를 모두 탭 형태로 도킹
		ImGui::DockBuilderDockWindow(m_stModelDockDesc.strName.c_str(), dock_main_id);
		ImGui::DockBuilderDockWindow(m_stAnimModelDockDesc.strName.c_str(), dock_main_id);

		m_stTransformEditorDesc.iDockSpaceID = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.3f, NULL, &dock_main_id); // 새로운 노드 생성
		ImGui::DockBuilderDockWindow(m_stTransformEditorDesc.strName.c_str(), m_stTransformEditorDesc.iDockSpaceID); // 새 노드에 도킹
		
		// Dock 빌더 완료
		ImGui::DockBuilderFinish(m_stMainDesc.iDockSpaceID);
	}

	m_isInitSetting = true; // 초기화 완료 플래그 설정
}

void TModelView::EditModel()
{
	// EditModel은 독에서 렌더링되므로 별도 ImGui::Begin 호출이 필요 없음
	if (ImGui::Begin(m_stTransformEditorDesc.strName.c_str()))
	{
		if (m_spSelectedModel != nullptr)
		{
			m_spGuizmoManager->SetSelectedActor(m_spSelectedModel);

			ImGui::Text("Selected Model: %s", m_SelectedModelName.c_str());
			m_spGuizmoManager->EditTransformViaGuizmo();
			ImGui::Checkbox("Show Collider", &m_bColliderActive);
			ImGui::Checkbox("Edit Position by Picking", &m_bEditPosByPicking);
		}
		else
		{
			ImGui::Text("Selected Model: None, Select Current Model to edit transform");
		}
	}
	ImGui::End();
}


void TModelView::ShowModels()
{
	static bool Modelfirstrun = true;
	static bool AnimModelfirstrun = true;
	static std::future<void> loadModelFuture; // 비동기 로딩 상태 추적용
	static std::future<void> loadAnimModelFuture; // 비동기 로딩 상태 추적용
	static bool isModelLoadComplete = false;     // 일반 모델 로딩 완료 여부

	// 일반 모델 표시
	ImGui::Begin(m_stModelDockDesc.strName.c_str(), GetOpenPointer(), m_stModelDockDesc.imgWindowFlags);
	{
		if (ImGui::TreeNodeEx("ShowModel", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// 일반 모델 로딩 상태 처리
			if (!m_bisModelsLoaded)
			{
				ImGui::OpenPopup("Loading##ModelLoadingDialog");
				ImGui::SetNextWindowPos(ImVec2(WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
				if (ImGui::BeginPopupModal("Loading##ModelLoadingDialog", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("Models are loading. Please wait...");
					ImGui::Separator();

					{
						// 로딩 작업 확인
						if (loadModelFuture.valid() && loadModelFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
						{
							
							m_bisModelsLoaded = true;   // 로딩 완료
							
							loadModelFuture.get();           // 비동기 결과 가져오기 (예외 발생 시 처리)
						}
					}

					ImGui::EndPopup();
				}
			}

			// 로딩 트리거
			if (!m_bisModelsLoaded && !Modelfirstrun && !loadModelFuture.valid())
			{
				loadModelFuture = std::async(std::launch::async, [this]()
					{
						ConvertModels(); // 동기적으로 호출		
						isModelLoadComplete = true; // 일반 모델 로딩 완료
					});
			}


			// 로딩 완료 시 모델 표시
			if (m_bisModelsLoaded)
			{
				ResetModels();
				ShowModelList();
			}

			ImGui::TreePop();
			Modelfirstrun = false;
		}
	}
	ImGui::End();

	// 애니메이션 모델 표시 (일반 모델 로딩이 완료된 이후에만 실행)
	if (isModelLoadComplete)
	{
		ImGui::Begin(m_stAnimModelDockDesc.strName.c_str(), GetOpenPointer(), m_stAnimModelDockDesc.imgWindowFlags);
		{
			if (ImGui::TreeNodeEx("ShowAnimModel", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (!m_bisAnimModelsLoaded)
				{
					ImGui::OpenPopup("Loading##AnimModelLoadingDialog");
					ImGui::SetNextWindowPos(ImVec2(WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
					if (ImGui::BeginPopupModal("Loading##AnimModelLoadingDialog", NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::Text("Animation models are loading. Please wait...");
						ImGui::Separator();

						// 로딩 작업 확인
						if (loadAnimModelFuture.valid() && loadAnimModelFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
						{
							m_bisAnimModelsLoaded = true; // 로딩 완료
							loadAnimModelFuture.get();
						}
						ImGui::EndPopup();
					}
				}

				// 로딩 트리거
				if (!m_bisAnimModelsLoaded && !AnimModelfirstrun && !loadAnimModelFuture.valid())
				{
					loadAnimModelFuture = std::async(std::launch::async, [this]()
						{
							ConvertAnimModels(); // 동기적으로 호출
						});
				}

				// 로딩 완료 시 애니메이션 모델 표시
				if (m_bisAnimModelsLoaded)
				{
					ResetAnimModels();
					ShowAnimModelList();
				}
				ImGui::TreePop();
				AnimModelfirstrun = false;
			}
		}
		ImGui::End();
		
	}
}



void TModelView::ShowModelList()
{
	if (ImGui::TreeNodeEx("Add ShowModel", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginListBox("Model List to Add", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (const MODELPAIR& Model : m_ModelsContainer)
			{
				_bool isTrue{ false };
				if (ImGui::Selectable(Model.first.c_str(), &isTrue))
				{
					_string ModelName = Model.first;
					_string uniqueName = ModelName;
					int counter = 1;

					// 중복된 이름 처리
					while (m_ShowModelsContainer.find(uniqueName) != m_ShowModelsContainer.end())
					{
						uniqueName = ModelName + "_" + std::to_string(counter);
						counter++;
					}

					//ShowModel을 컨테이너에 추가
					SHPTR<TShowModelObject> newModel = std::static_pointer_cast<TShowModelObject>(GetGameInstance()->CloneActorAdd(PROTO_ACTOR_SHOWMODELOBJECT));
					newModel->SetShowModel(Model.second);
					newModel->SetOutline(true);
					for (auto& Containers : newModel->GetColliderContainer())
					{
						Containers.second->SetTranslate(newModel->GetShowModel()->GetCenterPos());
						Containers.second->SetScaleToFitModel(newModel->GetShowModel()->GetMinVertexPos(), newModel->GetShowModel()->GetMaxVertexPos());
						Containers.second->SetTransform(newModel->GetTransform());
					}

					m_ShowModelsContainer.emplace(uniqueName, newModel);
				}
			}
			ImGui::EndListBox();
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("Current ShowModels", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginListBox("Current Models Shown", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (auto& Model : m_ShowModelsContainer)
			{
				_bool isTrue{ false };
				if (ImGui::Selectable(Model.first.c_str(), &isTrue))
				{
					m_spSelectedModel = static_pointer_cast<UPawn>(Model.second);
					m_SelectedModelName = Model.first;
					m_bSelectedhasAnim = false;
				}
			}
			ImGui::EndListBox();
		}
		if (ImGui::Button("Clear All ShowModels"))
			ClearAllShowModels();

		if (ImGui::Button("Add Current ShowModels to MapLayout"))
		{
			m_bshowLayoutAddPopup = true;
			ImGui::OpenPopup("Add to MapLayouts");
		}
		AddModelstoMapLayout();

		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("MapLayouts", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Save"))
		{
			m_bLayoutSavePopup = true;
			ImGui::OpenPopup("Save MapLayouts");
		}
		SaveCurrentMapLayouts();
		ImGui::SameLine();

		if (ImGui::Button("Load"))
		{
			m_bLayoutLoadPopup = true;
			ImGui::OpenPopup("Load MapLayouts");
		}
		LoadObjectLayoutsFromFile();

		if (ImGui::BeginListBox("MapLayouts", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (const auto& maplayouts : (*m_spMapLayout->GetMapObjectsContainer().get()))
			{
				_bool isTrue{ false };
				if (ImGui::Selectable(maplayouts.first.c_str(), &isTrue))
				{
					if (m_ModelsContainer.size() > 0)
					{
						/*ClearAllShowModels();*/
						for (const auto& layoutObjects : maplayouts.second)
						{
							auto it = m_ModelsContainer.find(layoutObjects._sModelName);

							_string ModelName = it->first;
							_string uniqueName = ModelName;
							int counter = 1;

							// 중복된 이름 처리
							while (m_ShowModelsContainer.find(uniqueName) != m_ShowModelsContainer.end())
							{
								uniqueName = ModelName + "_" + std::to_string(counter);
								counter++;
							}


							//ShowModel을 컨테이너에 추가
							SHPTR<TShowModelObject> newModel = std::static_pointer_cast<TShowModelObject>(GetGameInstance()->CloneActorAdd(PROTO_ACTOR_SHOWMODELOBJECT));
							newModel->SetShowModel(it->second);
							newModel->GetTransform()->SetNewWorldMtx(layoutObjects._mWorldMatrix);
							newModel->SetOutline(true);
							for (auto& Containers : newModel->GetColliderContainer())
							{
								Containers.second->SetTranslate(newModel->GetShowModel()->GetCenterPos());
								Containers.second->SetScaleToFitModel(newModel->GetShowModel()->GetMinVertexPos(), newModel->GetShowModel()->GetMaxVertexPos());
								Containers.second->SetTransform(newModel->GetTransform());
							}

							m_ShowModelsContainer.emplace(uniqueName, newModel);
						}
					}
					else
					{
						m_bErrorPopup = true;
						ImGui::OpenPopup("Error Message");
						if (m_bErrorPopup && ImGui::BeginPopupModal("Error Message", NULL, ImGuiWindowFlags_AlwaysAutoResize))
						{
							ImGui::Text("Please load models by pressing Convert button above before loading Maplayout!");
							if (ImGui::Button("Ok", ImVec2(120, 0)))
							{
								ImGui::CloseCurrentPopup();
								m_bErrorPopup = false;
							}
							ImGui::EndPopup();
						}
					}
				}
			}
			ImGui::EndListBox();
		}
		if (ImGui::Button("Add All Maplayouts"))
		{
			for (const auto& maplayouts : (*m_spMapLayout->GetMapObjectsContainer().get()))
			{
				if (m_ModelsContainer.size() > 0)
				{
					/*ClearAllShowModels();*/
					for (const auto& layoutObjects : maplayouts.second)
					{
						auto it = m_ModelsContainer.find(layoutObjects._sModelName);

						_string ModelName = it->first;
						_string uniqueName = ModelName;
						int counter = 1;

						// 중복된 이름 처리
						while (m_ShowModelsContainer.find(uniqueName) != m_ShowModelsContainer.end())
						{
							uniqueName = ModelName + "_" + std::to_string(counter);
							counter++;
						}


						//ShowModel을 컨테이너에 추가
						SHPTR<TShowModelObject> newModel = std::static_pointer_cast<TShowModelObject>(GetGameInstance()->CloneActorAdd(PROTO_ACTOR_SHOWMODELOBJECT));
						newModel->SetShowModel(it->second);
						newModel->GetTransform()->SetNewWorldMtx(layoutObjects._mWorldMatrix);
						newModel->SetOutline(true);
						for (auto& Containers : newModel->GetColliderContainer())
						{
							Containers.second->SetTranslate(newModel->GetShowModel()->GetCenterPos());
							Containers.second->SetScaleToFitModel(newModel->GetShowModel()->GetMinVertexPos(), newModel->GetShowModel()->GetMaxVertexPos());
							Containers.second->SetTransform(newModel->GetTransform());
					
						}

						m_ShowModelsContainer.emplace(uniqueName, newModel);
					}
				}
				else
				{
					m_bErrorPopup = true;
					ImGui::OpenPopup("Error Message");
					if (m_bErrorPopup && ImGui::BeginPopupModal("Error Message", NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::Text("Please load models by pressing Convert button above before loading Maplayout!");
						if (ImGui::Button("Ok", ImVec2(120, 0)))
						{
							ImGui::CloseCurrentPopup();
							m_bErrorPopup = false;
						}
						ImGui::EndPopup();
					}
				}
			}
		}


		ImGui::TreePop();
	}
}


void TModelView::LoadObjectLayoutsFromFile()
{
	if (m_bLayoutLoadPopup && ImGui::BeginPopupModal("Load MapLayouts", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Your current Maplayouts can be deleted. Continue?");
		if (ImGui::Button("Ok", ImVec2(120, 0)))
		{
			if(m_spMapLayout->LoadMapObjects())
			{
				ImGui::CloseCurrentPopup();
				m_bLayoutLoadPopup = false;
			}
			else
			{
				ImGui::Text("Load Failed");
				if (ImGui::Button("Ok", ImVec2(120, 0)))
				{
					ImGui::CloseCurrentPopup();
					m_bLayoutLoadPopup = false;
				}
			}
		}
		ImGui::SameLine();

		if (ImGui::Button("No", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			m_bLayoutLoadPopup = false;
		}
		ImGui::EndPopup();
	}
}

void TModelView::LoadMobsLayoutsFromFile()
{
	if (m_bLayoutLoadPopup && ImGui::BeginPopupModal("Load MapLayouts", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Your current Maplayouts can be deleted. Continue?");
		if (ImGui::Button("Ok", ImVec2(120, 0)))
		{
			if (m_spMapLayout->LoadMapMobs())
			{
				ImGui::CloseCurrentPopup();
				m_bLayoutLoadPopup = false;
			}
			else
			{
				ImGui::Text("Load Failed");
				if (ImGui::Button("Ok", ImVec2(120, 0)))
				{
					ImGui::CloseCurrentPopup();
					m_bLayoutLoadPopup = false;
				}
			}
		}
		ImGui::SameLine();

		if (ImGui::Button("No", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			m_bLayoutLoadPopup = false;
		}
		ImGui::EndPopup();
	}
}



void TModelView::ClearCurrentModel()
{
	auto it = m_ShowModelsContainer.begin();
	while (it != m_ShowModelsContainer.end())
	{
		if (it->second == m_spSelectedModel)
		{
			GetGameInstance()->RemoveActor(it->second);
			m_DeleteModelsContainer.push_back(it->first);
			break;
		}
		else
		{
			++it;
		}
	}
	m_spSelectedModel.reset();
}

void TModelView::ClearAllShowModels()
{
	auto it = m_ShowModelsContainer.begin();
	while (it != m_ShowModelsContainer.end())
	{
		GetGameInstance()->RemoveActor(it->second);
		m_DeleteModelsContainer.push_back(it->first);
		it++;
	}
	if (m_spSelectedModel != nullptr)
		m_spSelectedModel.reset();
}


void TModelView::ClearCurrentAnimModel()
{
	auto it = m_ShowAnimModelsContainer.begin();
	while (it != m_ShowAnimModelsContainer.end())
	{
		if (it->second == m_spSelectedModel)
		{
			GetGameInstance()->RemoveActor(it->second);
			m_DeleteAnimModelsContainer.push_back(it->first);
			break;
		}
		else
		{
			++it;
		}
	}
	m_spSelectedModel.reset();
}

void TModelView::MouseInput()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	_float2 MousePos = spGameInstance->GetMousePosition();

	if (MousePos.x <= 0 || MousePos.x >= WINDOW_WIDTH)
		return;

	if (MousePos.y <= 0 || MousePos.y >= WINDOW_HEIGHT)
		return;

	if (!spGameInstance->GetDIMBtnDown(DIMOUSEBUTTON::DIMB_L))
		return;

	PICKINGDESC tDesc = spGameInstance->GetPickDesc();
	if (!tDesc.bPickingSuccess)
		return;

	if (!m_bEditPosByPicking)
	{
		for (auto& ShowModel : m_ShowModelsContainer)
		{
			if (ShowModel.second == tDesc.spPawn)
			{
				m_spSelectedModel = tDesc.spPawn;
				m_SelectedModelName = ShowModel.first;
				m_bSelectedhasAnim = false;
				return;
			}
		}

		for (auto& ShowModel : m_ShowAnimModelsContainer)
		{
			if (ShowModel.second == tDesc.spPawn)
			{
				m_spSelectedModel = tDesc.spPawn;
				m_SelectedModelName = ShowModel.first;
				m_bSelectedhasAnim = true;
				return;
			}
		}
	}
	else
	{
		if (m_spSelectedModel != nullptr)
		{
			m_spSelectedModel->GetTransform()->SetPos(tDesc.vPickPos);
		}
	}

}

void TModelView::KeyboardInput()
{
	if (GetGameInstance()->GetDIKeyPressing(DIK_LCONTROL))
		if (GetGameInstance()->GetDIKeyDown(DIK_C))
			CopyCurrentModel();
	if (GetGameInstance()->GetDIKeyDown(DIK_DELETE))
		if (!m_bSelectedhasAnim)
			ClearCurrentModel();
		else
			ClearCurrentAnimModel();
}

HRESULT TModelView::CopyCurrentModel()
{
	RETURN_CHECK(m_spSelectedModel == nullptr, E_FAIL)

		if (m_spCopiedModel != nullptr)
			m_spCopiedModel.reset();

	if (!m_bSelectedhasAnim)
	{
		m_spCopiedModel = std::static_pointer_cast<TShowModelObject>(GetGameInstance()->CloneActorAdd(PROTO_ACTOR_SHOWMODELOBJECT));
		SHPTR<TShowModelObject> _CopiedModel = std::static_pointer_cast<TShowModelObject>(m_spCopiedModel);

		_CopiedModel->SetShowModel(std::static_pointer_cast<TShowModelObject>(m_spSelectedModel)->GetShowModel());

		for (auto& Containers : m_spCopiedModel->GetColliderContainer())
		{
			Containers.second->SetTranslate(_CopiedModel->GetShowModel()->GetCenterPos());
			Containers.second->SetScaleToFitModel(_CopiedModel->GetShowModel()->GetMinVertexPos(), _CopiedModel->GetShowModel()->GetMaxVertexPos());
			Containers.second->SetTransform(_CopiedModel->GetTransform());
		}
	}
	else
	{
		m_spCopiedModel = std::static_pointer_cast<TShowAnimModelObject>(GetGameInstance()->CloneActorAdd(PROTO_ACTOR_SHOWANIMMODELOBJECT));
		SHPTR<TShowAnimModelObject> _CopiedModel = std::static_pointer_cast<TShowAnimModelObject>(m_spCopiedModel);
		_CopiedModel->SetShowModel(std::static_pointer_cast<TShowAnimModelObject>(m_spSelectedModel)->GetAnimModel());

		for (auto& Containers : m_spCopiedModel->GetColliderContainer())
		{
			Containers.second->SetTranslate(_CopiedModel->GetAnimModel()->GetCenterPos());
			Containers.second->SetScaleToFitModel(_CopiedModel->GetAnimModel()->GetMinVertexPos(), _CopiedModel->GetAnimModel()->GetMaxVertexPos());
			Containers.second->SetTransform(_CopiedModel->GetTransform());
		}
	}

	m_spCopiedModel->GetTransform()->SetNewWorldMtx(m_spSelectedModel->GetTransform()->GetWorldMatrix());

	m_CopiedModelName = m_SelectedModelName;

	int count = 1;

	// 중복된 이름 처리
	while (m_ShowModelsContainer.find(m_CopiedModelName) != m_ShowModelsContainer.end() || m_ShowAnimModelsContainer.find(m_CopiedModelName) != m_ShowAnimModelsContainer.end())
	{
		m_CopiedModelName = m_SelectedModelName + "_Clone" + std::to_string(count);
		++count;
	}

	if (m_bSelectedhasAnim)
		m_ShowAnimModelsContainer.emplace(m_CopiedModelName, std::dynamic_pointer_cast<TShowAnimModelObject>(m_spCopiedModel));
	else
		m_ShowModelsContainer.emplace(m_CopiedModelName, std::dynamic_pointer_cast<TShowModelObject>(m_spCopiedModel));

	m_spSelectedModel = m_spCopiedModel;


	return S_OK;
}



void TModelView::ConvertModels()
{
	SHPTR<FILEGROUP> ModelFolder = GetGameInstance()->FindFolder(L"Model");
	// Folders 
	for (const FOLDERPAIR& Folder : ModelFolder->UnderFileGroupList)
	{
		LoadAssimpModelDatas(Folder.second);
	}
	// Load 
	GetGameInstance()->LoadFirstFolder(FIRST_RESOURCE_FOLDER);
	m_spModelFileFolder = GetGameInstance()->FindFolder(L"Model");
}

void TModelView::ConvertAnimModels()
{
	SHPTR<FILEGROUP> ModelFolder = GetGameInstance()->FindFolder(L"AnimModel");
	m_AnimModelContainer.clear();
	// Folders 
	for (const FOLDERPAIR& Folder : ModelFolder->UnderFileGroupList)
	{
		LoadAnimModelData(Folder.second);
	}
	// Load 
	GetGameInstance()->LoadFirstFolder(FIRST_RESOURCE_FOLDER);
	m_spAnimModelFileFolder = GetGameInstance()->FindFolder(L"AnimModel");
}

void TModelView::LoadAssimpModelDatas(CSHPTRREF<FILEGROUP> _spFolder)
{
	RETURN_CHECK(nullptr == _spFolder, ;);
	SHPTR<FILEGROUP> ConvertFolder = _spFolder->FindGroup(L"Convert");
	if (nullptr != ConvertFolder && 0 <= ConvertFolder->FileDataList.size())
	{
		for (const FILEPAIR& File : ConvertFolder->FileDataList)
		{
			SHPTR<UModel> spModel = CreateConstructorNativeNotMsg<UModel>(GetDevice(), File.second->wstrfilePath);
			m_ModelsContainer.insert(std::pair<_string, SHPTR<UModel>>(UMethod::ConvertWToS(File.second->wstrfileName), spModel));
		}
	}
	else
	{
		// FileData
		for (const FILEPAIR& File : _spFolder->FileDataList)
		{
			// Assimp 
			SHPTR<TAssimpModel> spAssimpModel = CreateConstructorNativeNotMsg<TAssimpModel>(GetDevice(),
				TAssimpModel::TYPE::NONANIM, _spFolder, File.second);

			// Save Non Anim Model
			if (nullptr != spAssimpModel)
			{
				_wstring wstrPath;
				spAssimpModel->SaveNonAnimModel(_spFolder->wstrPath, wstrPath);
				SHPTR<UModel> spModel = CreateConstructorNativeNotMsg<UModel>(GetDevice(), wstrPath);
				m_ModelsContainer.insert(std::pair<_string, SHPTR<UModel>>(UMethod::ConvertWToS(File.second->wstrfileName), spModel));
			}
		}
		// Folders 
		for (const FOLDERPAIR& Folder : _spFolder->UnderFileGroupList)
		{
			LoadAssimpModelDatas(Folder.second);
		}
	}
}

void TModelView::LoadAnimModelData(CSHPTRREF<FILEGROUP> _spFolder)
{
	RETURN_CHECK(nullptr == _spFolder, ;);
	RETURN_CHECK(_spFolder->wstrFolderName == L"Anim", ;);
	RETURN_CHECK(_spFolder->wstrFolderName == L"AnimFBX", ;);

	SHPTR<FILEGROUP> UnityFBXFolder = _spFolder->FindGroup(L"UnityFBX");
	if (nullptr != UnityFBXFolder)
	{
		// Convert Folder가 존재하는 경우 다시 만들지 않음 
		for (const FILEPAIR& File : UnityFBXFolder->FileDataList)
		{
			_wstring wstrConvertLoadPath = L"";
			TUnityModelLoader::LoadUnityAnimFBXData(UnityFBXFolder, File.second, wstrConvertLoadPath);
			SHPTR<UAnimModel> spModel = CreateConstructorNativeNotMsg<UAnimModel>(GetDevice(), wstrConvertLoadPath);
			m_AnimModelContainer.insert(std::pair<_string, SHPTR<UAnimModel>>(UMethod::ConvertWToS(File.second->wstrfileName), spModel));
		}
	}
	else
	{
		SHPTR<FILEGROUP> ConvertFolder = _spFolder->FindGroup(L"Convert");
		if (nullptr != ConvertFolder && 0 <= ConvertFolder->FileDataList.size())
		{
			// Convert Folder가 존재하는 경우 다시 만들지 않음 
			for (const FILEPAIR& File : ConvertFolder->FileDataList)
			{
				SHPTR<UAnimModel> spModel = CreateConstructorNativeNotMsg<UAnimModel>(GetDevice(), File.second->wstrfilePath);
				m_AnimModelContainer.insert(std::pair<_string, SHPTR<UAnimModel>>(UMethod::ConvertWToS(File.second->wstrfileName), spModel));
			}
		}
		else
		{
			SHPTR<FILEGROUP> spAnimFBX = _spFolder->FindGroup(L"AnimFBX");
			SHPTR<FILEGROUP> spAnim = _spFolder->FindGroup(L"Anim");
			// FileData
			for (const FILEPAIR& File : _spFolder->FileDataList)
			{
				// Assimp 
				SHPTR<TAssimpModel> spAssimpModel = CreateConstructorNativeNotMsg<TAssimpModel>(GetDevice(),
					TAssimpModel::TYPE::ANIM, _spFolder, File.second);

				if (nullptr != spAnimFBX)
				{
					for (auto& iter : spAnimFBX->FileDataList)
					{
						spAssimpModel->LoadAnimationFBX(iter.second->wstrfilePath);
					}
				}
				if (nullptr != spAnim)
				{
					for (auto& iter : spAnim->FileDataList)
					{
						spAssimpModel->LoadAnimation(iter.second->wstrfilePath);
					}
				}
				// Save Anim Model
				if (nullptr != spAssimpModel)
				{
					_wstring wstrPath;
					spAssimpModel->SaveAnimModel(_spFolder->wstrPath, wstrPath);
					SHPTR<UAnimModel> spModel = CreateConstructorNativeNotMsg<UAnimModel>(GetDevice(), wstrPath);

					m_AnimModelContainer.insert(std::pair<_string, SHPTR<UAnimModel>>(UMethod::ConvertWToS(File.second->wstrfileName), spModel));
				}
			}
			// Folders 
			for (const FOLDERPAIR& Folder : _spFolder->UnderFileGroupList)
			{
				LoadAnimModelData(Folder.second);
			}
		}
	}
}

void TModelView::ResetModels()
{
	ImGui::SameLine();
	m_isResetModel = ImGui::Button("Reset Models");
	if (true == m_isResetModel)
	{
		m_ModelsContainer.clear();
		m_spModelFileFolder = nullptr;
	}
}

void TModelView::ResetAnimModels()
{
	ImGui::SameLine();
	m_isResetAnimModel = ImGui::Button("Reset AnimModels");
	if (true == m_isResetAnimModel)
	{
		m_AnimModelContainer.clear();
		m_spAnimModelFileFolder = nullptr;
	}
}


void TModelView::SaveCurrentMapLayouts()
{
	if (m_bLayoutSavePopup && ImGui::BeginPopupModal("Save MapLayouts", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if ((*m_spMapLayout->GetMapObjectsContainer().get()).size() > 0)
		{
			if(m_spMapLayout->SaveMapObjects(FIRST_RESOURCE_FOLDER))
			{
				ImGui::Text("Save Complete");
				if (ImGui::Button("Ok", ImVec2(120, 0)))
				{
					ImGui::CloseCurrentPopup();
					m_bLayoutSavePopup = false;
				}
			}
		}
		else
		{
			ImGui::Text("Save Failed");
			if (ImGui::Button("Ok", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				m_bLayoutSavePopup = false;
			}
		}
		ImGui::EndPopup();
	}
}

void TModelView::SaveCurrentMobsLayouts()
{
	if (m_bLayoutSavePopup && ImGui::BeginPopupModal("Save MapLayouts", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if ((*m_spMapLayout->GetMapMobsContainer().get()).size() > 0)
		{
			if (m_spMapLayout->SaveMapMobs(FIRST_RESOURCE_FOLDER))
			{
				ImGui::Text("Save Complete");
				if (ImGui::Button("Ok", ImVec2(120, 0)))
				{
					ImGui::CloseCurrentPopup();
					m_bLayoutSavePopup = false;
				}
			}
		}
		else
		{
			ImGui::Text("Save Failed");
			if (ImGui::Button("Ok", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				m_bLayoutSavePopup = false;
			}
		}
		ImGui::EndPopup();
	}
}

void TModelView::AddModelstoMobsLayout()
{
	static char nameBuffer[256] = "";

	if (m_bshowLayoutAddPopup && ImGui::BeginPopupModal("Add to MapLayouts", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (m_ShowAnimModelsContainer.size() > 0)
		{
			ImGui::Text("Enter a name for the Models to be placed in:");
			ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer));
			if (ImGui::Button("Add", ImVec2(120, 0)))
			{
				if (nameBuffer[0] != '\0')
				{
					VECTOR<UMapLayout::MOBDESC> ObjDataVector{};
					for (auto& Model : m_ShowAnimModelsContainer)
					{
						UMapLayout::MOBDESC objDesc{};
						objDesc._sAnimModelName = UMethod::ConvertWToS(Model.second->GetAnimModel()->GetModelName());
						objDesc._sAnimName = UMethod::ConvertWToS(Model.second->GetAnimModel()->GetCurrentAnimation()->GetAnimName());
						objDesc._mWorldMatrix = Model.second->GetTransform()->GetWorldMatrix();
						ObjDataVector.push_back(objDesc);
					}
					m_spMapLayout->AddtoMobsContainer(nameBuffer, ObjDataVector);
					ImGui::OpenPopup("Successfully Added");
				}
				ImGui::CloseCurrentPopup();
				m_bshowLayoutAddPopup = false;
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				m_bshowLayoutAddPopup = false;
			}
		}
		else
		{
			ImGui::Text("There are no models to add!");
			if (ImGui::Button("Ok", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				m_bshowLayoutAddPopup = false;
			}
		}

		ImGui::EndPopup();
	}
}

void TModelView::AddModelstoMapLayout()
{
	static char nameBuffer[256] = "";

	if (m_bshowLayoutAddPopup &&ImGui::BeginPopupModal("Add to MapLayouts", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if(m_ShowModelsContainer.size() > 0)
		{
			ImGui::Text("Enter a name for the Models to be placed in:");
			ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer));
			if (ImGui::Button("Add", ImVec2(120, 0)))
			{
				if (nameBuffer[0] != '\0')
				{
					VECTOR<UMapLayout::OBJDESC> ObjDataVector{};
					for (auto& Model : m_ShowModelsContainer)
					{
						UMapLayout::OBJDESC objDesc{};
						objDesc._sModelName = UMethod::ConvertWToS(Model.second->GetShowModel()->GetModelName());
						objDesc._mWorldMatrix = Model.second->GetTransform()->GetWorldMatrix();
						ObjDataVector.push_back(objDesc);
					}
					m_spMapLayout->AddtoMapContainer(nameBuffer, ObjDataVector);
					ImGui::OpenPopup("Successfully Added");
				}
				ImGui::CloseCurrentPopup();
				m_bshowLayoutAddPopup = false;
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				m_bshowLayoutAddPopup = false;
			}
		}
		else
		{
			ImGui::Text("There are no models to add!");
			if (ImGui::Button("Ok", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				m_bshowLayoutAddPopup = false;
			}
		}

		ImGui::EndPopup();
	}
}

void TModelView::ShowAnimModelList()
{
	if (ImGui::TreeNodeEx("Add AnimShowModel", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginListBox("AnimModel List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (const ANIMMODELPAIR& Model : m_AnimModelContainer)
			{
				_bool isTrue = false;
				if (ImGui::Selectable(Model.first.c_str(), &isTrue, ImGuiTreeNodeFlags_Selected))
				{
					_string ModelName = Model.first;
					_string uniqueName = ModelName;
					int counter = 1;

					// Handle duplicate names
					while (m_ShowAnimModelsContainer.find(uniqueName) != m_ShowAnimModelsContainer.end())
					{
						uniqueName = ModelName + "_" + std::to_string(counter);
						counter++;
					}
					// Add ShowModel to container
					auto newAnimModel = std::static_pointer_cast<TShowAnimModelObject>(GetGameInstance()->CloneActorAdd(PROTO_ACTOR_SHOWANIMMODELOBJECT));
					newAnimModel->SetShowModel(Model.second);

					for (auto& Containers : newAnimModel->GetColliderContainer())
					{
						Containers.second->SetTranslate(newAnimModel->GetAnimModel()->GetCenterPos());
						Containers.second->SetScaleToFitModel(newAnimModel->GetAnimModel()->GetMinVertexPos(), newAnimModel->GetAnimModel()->GetMaxVertexPos());
						Containers.second->SetTransform(newAnimModel->GetTransform());
					}

					m_ShowAnimModelsContainer.emplace(uniqueName, newAnimModel);
				}
			}
			ImGui::EndListBox();
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Current AnimShowModels", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginListBox("Current AnimModels Shown", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (auto& Model : m_ShowAnimModelsContainer)
			{
				_bool isTrue = false;
				if (ImGui::Selectable(Model.first.c_str(), &isTrue))
				{
					m_spSelectedModel = std::static_pointer_cast<UPawn>(Model.second);
					m_SelectedModelName = Model.first;
					m_bSelectedhasAnim = true;
				}
			}
			ImGui::EndListBox();
		}

		if (ImGui::Button("Clear Current AnimShowModels"))
		{
			ClearCurrentAnimModel();
		}

		if (ImGui::Button("Add Current AnimShowModels to MapLayout"))
		{
			m_bshowLayoutAddPopup = true;
			ImGui::OpenPopup("Add to MapLayouts");
		}
		AddModelstoMobsLayout();

		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("MapLayouts", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Save"))
		{
			m_bLayoutSavePopup = true;
			ImGui::OpenPopup("Save MapLayouts");
		}
		SaveCurrentMobsLayouts();
		ImGui::SameLine();

		if (ImGui::Button("Load"))
		{
			m_bLayoutLoadPopup = true;
			ImGui::OpenPopup("Load MapLayouts");
		}
		LoadMobsLayoutsFromFile();

		if (ImGui::BeginListBox("MapLayouts", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (const auto& maplayouts : (*m_spMapLayout->GetMapMobsContainer().get()))
			{
				_bool isTrue = false;
				if (ImGui::Selectable(maplayouts.first.c_str(), &isTrue))
				{
					if (m_AnimModelContainer.size() > 0)
					{
						for (const auto& layoutObjects : maplayouts.second)
						{
							auto it = m_AnimModelContainer.find(layoutObjects._sAnimModelName);
							if (it != m_AnimModelContainer.end())
							{
								_string ModelName = it->first;
								_string uniqueName = ModelName;
								int counter = 1;

								while (m_ShowAnimModelsContainer.find(uniqueName) != m_ShowAnimModelsContainer.end())
								{
									uniqueName = ModelName + "_" + std::to_string(counter);
									counter++;
								}

								auto newModel = std::static_pointer_cast<TShowAnimModelObject>(GetGameInstance()->CloneActorAdd(PROTO_ACTOR_SHOWANIMMODELOBJECT));
								newModel->SetShowModel(it->second);
								newModel->GetTransform()->SetNewWorldMtx(layoutObjects._mWorldMatrix);
								newModel->GetAnimModel()->SetAnimation(UMethod::ConvertSToW(layoutObjects._sAnimName));

								for (auto& Containers : newModel->GetColliderContainer())
								{
									Containers.second->SetTranslate(newModel->GetAnimModel()->GetCenterPos());
									Containers.second->SetScaleToFitModel(newModel->GetAnimModel()->GetMinVertexPos(), newModel->GetAnimModel()->GetMaxVertexPos());
									Containers.second->SetTransform(newModel->GetTransform());
								}

								m_ShowAnimModelsContainer.emplace(uniqueName, newModel);
							}
						}
					}
					else
					{
						m_bErrorPopup = true;
						ImGui::OpenPopup("Error Message");
						if (m_bErrorPopup && ImGui::BeginPopupModal("Error Message", NULL, ImGuiWindowFlags_AlwaysAutoResize))
						{
							ImGui::Text("Please load models by pressing Convert button above before loading Maplayout!");
							if (ImGui::Button("Ok", ImVec2(120, 0)))
							{
								ImGui::CloseCurrentPopup();
								m_bErrorPopup = false;
							}
							ImGui::EndPopup();
						}
					}
				}
			}
			ImGui::EndListBox();
		}

		if (ImGui::Button("Add All Maplayouts"))
		{
			for (const auto& maplayouts : (*m_spMapLayout->GetMapMobsContainer().get()))
			{
				if (m_AnimModelContainer.size() > 0)
				{
					for (const auto& layoutObjects : maplayouts.second)
					{
						auto it = m_AnimModelContainer.find(layoutObjects._sAnimModelName);
						if (it != m_AnimModelContainer.end())
						{
							_string ModelName = it->first;
							_string uniqueName = ModelName;
							int counter = 1;

							while (m_ShowAnimModelsContainer.find(uniqueName) != m_ShowAnimModelsContainer.end())
							{
								uniqueName = ModelName + "_" + std::to_string(counter);
								counter++;
							}

							auto newModel = std::static_pointer_cast<TShowAnimModelObject>(GetGameInstance()->CloneActorAdd(PROTO_ACTOR_SHOWANIMMODELOBJECT));
							newModel->SetShowModel(it->second);
							newModel->GetTransform()->SetNewWorldMtx(layoutObjects._mWorldMatrix);
							newModel->GetAnimModel()->SetAnimation(UMethod::ConvertSToW(layoutObjects._sAnimName));

							for (auto& Containers : newModel->GetColliderContainer())
							{
								Containers.second->SetTranslate(newModel->GetAnimModel()->GetCenterPos());
								Containers.second->SetScaleToFitModel(newModel->GetAnimModel()->GetMinVertexPos(), newModel->GetAnimModel()->GetMaxVertexPos());
								Containers.second->SetTransform(newModel->GetTransform());
							}


							m_ShowAnimModelsContainer.emplace(uniqueName, newModel);
						}
					}
				}
				else
				{
					m_bErrorPopup = true;
					ImGui::OpenPopup("Error Message");
					if (m_bErrorPopup && ImGui::BeginPopupModal("Error Message", NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::Text("Please load models by pressing Convert button above before loading Maplayout!");
						if (ImGui::Button("Ok", ImVec2(120, 0)))
						{
							ImGui::CloseCurrentPopup();
							m_bErrorPopup = false;
						}
						ImGui::EndPopup();
					}
				}
			}
		}

		ImGui::TreePop();
	}

	if (m_bSelectedhasAnim)
	{
		if (ImGui::TreeNodeEx("Select Animation", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::BeginListBox("Select Animation", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				if (m_spSelectedModel != nullptr)
				{
					auto spModel = std::dynamic_pointer_cast<TShowAnimModelObject>(m_spSelectedModel)->GetAnimModel();
					if (spModel != nullptr)
					{
						for (auto& Animation : spModel->GetAnimations())
						{
							_string strName = UMethod::ConvertWToS(Animation->GetAnimName());
							_bool isTrue = false;
							if (ImGui::Selectable(strName.c_str(), &isTrue))
							{
								spModel->SetAnimation(Animation->GetAnimName());
							}
						}
					}
				}
				ImGui::EndListBox();
			}
			ImGui::TreePop();
		}
	}
}


