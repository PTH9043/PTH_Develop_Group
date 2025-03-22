#include "MainView_Tool.h"
#include "GameObject_Tool.h"
#include "GameInstance.h"
#include "ImGuiClass_Manager_Tool.h"
#include "MapViewer_Tool.h"
#include "FilePathViewer_Tool.h"
#include "ImGui_UI_Tool.h"
#include "Effect_Tool.h"
#include "AnimViewer_Tool.h"
#include "CamViewer_Tool.h"
#include "ShaderView_Tool.h"
#include "Level_Main_Tool.h"
#include "Level_Loading_Tool.h"
#include "CutSceneViewer_Tool.h"

CMainView_Tool::CMainView_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CImGuiClass_Tool(_pDevice, _pContext)
{
}

HRESULT CMainView_Tool::NativeConstruct()
{
	m_tMainView = DOCKDESC("Main", ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar, ImVec2(200.f, 1.f), ImVec2(500.f, 720.f), true);
	m_tPerspective = DOCKDESC("Perspective", ImGuiWindowFlags_NoFocusOnAppearing);
	m_tHierarchy = DOCKDESC("Hierarchy", ImGuiWindowFlags_NoFocusOnAppearing);
	// Name
	m_pName = "MainView";
	if (FAILED(Ready_ImGuiClass()))
		return E_FAIL;

	m_bOpen = true;
	return S_OK;
}

HRESULT CMainView_Tool::NativeConstruct_InnerValue()
{
	return S_OK;
}

HRESULT CMainView_Tool::Tick(const _double& _dTimeDelta)
{
	Reset_ToolFunction();

	for (auto& iter : m_ImGuiList)
		iter->Tick(_dTimeDelta);
	return S_OK;
}

HRESULT CMainView_Tool::Late_Tick(const _double& _dTimeDelta)
{
	for (auto& iter : m_ImGuiList)
	{
		iter->Late_Tick(_dTimeDelta);
	}

	Show_Window();
	return S_OK;
}

HRESULT CMainView_Tool::Render()
{
	for (auto& iter : m_ImGuiList)
		iter->Render();
	return S_OK;
}

HRESULT CMainView_Tool::Release_InnerValue()
{
	return S_OK;
}

HRESULT CMainView_Tool::Ready_ImGuiClass()
{
	CImGuiClass_Manager_Tool* pInstance = GET_INSTANCE(CImGuiClass_Manager_Tool);
	{
		// PathViewer
		{
			CImGuiClass_Tool* pImGuiClass = CFilePathViewer_Tool::Create(m_pDevice, m_pContext);
			pInstance->Insert_ImGuiClass(IMGCLASS_FILEPATH, pImGuiClass);
			m_ImGuiList.push_back(pImGuiClass);
			Safe_AddRef(pImGuiClass);
		}
		//// Insert ImGui Class
		{
			CImGuiClass_Tool* pImGuiClass = CMapViewer_Tool::Create(m_pDevice, m_pContext);
			pInstance->Insert_ImGuiClass(IMGCLASS_MAPVIEWER, pImGuiClass);
			m_ImGuiList.push_back(pImGuiClass);
			Safe_AddRef(pImGuiClass);
		}
		// AnimViewer
       {
           CImGuiClass_Tool* pImGuiClass = CAnimViewer_Tool::Create(m_pDevice, m_pContext);
           pInstance->Insert_ImGuiClass(IMGCLASS_ANIMVIEWER, pImGuiClass);
           m_ImGuiList.push_back(pImGuiClass);
           Safe_AddRef(pImGuiClass);
       }
		// UI
		{
			CImGuiClass_Tool* pImGuiClass = CImGui_UI_Tool::Create(m_pDevice, m_pContext);
			pInstance->Insert_ImGuiClass(IMGCLASS_UI, pImGuiClass);
			m_ImGuiList.push_back(pImGuiClass);
			Safe_AddRef(pImGuiClass);
		}
		// Effect
		{
			CImGuiClass_Tool* pImGuiClass = CEffect_Tool::Create(m_pDevice, m_pContext);
			pInstance->Insert_ImGuiClass(IMGCLASS_EFFECT, pImGuiClass);
			m_ImGuiList.push_back(pImGuiClass);
			Safe_AddRef(pImGuiClass);
		}
		// CamViewer
		{
			CImGuiClass_Tool* pImGuiClass = CCamViewer_Tool::Create(m_pDevice, m_pContext);
			pInstance->Insert_ImGuiClass(IMGCLASS_CAMVIEWER, pImGuiClass);
			m_ImGuiList.push_back(pImGuiClass);
			Safe_AddRef(pImGuiClass);
		}
		// Shader Viewer
		{
			CImGuiClass_Tool* pImGuiClass = CShaderView_Tool::Create(m_pDevice, m_pContext);
			pInstance->Insert_ImGuiClass(IMGCLASS_SHADERVIEW, pImGuiClass);
			m_ImGuiList.push_back(pImGuiClass);
			Safe_AddRef(pImGuiClass);
		}
		// Shader Viewer
		{
			CImGuiClass_Tool* pImGuiClass = CCutSceneViewer_Tool::Create(m_pDevice, m_pContext);
			pInstance->Insert_ImGuiClass(IMGCLASS_CUTSCENE, pImGuiClass);
			m_ImGuiList.push_back(pImGuiClass);
			Safe_AddRef(pImGuiClass);
		}
	}
	RELEASE_INSTANCE(CImGuiClass_Manager_Tool);

	return S_OK;
}

void CMainView_Tool::Picking_Clones()
{
	return;
}

void CMainView_Tool::Show_Window()
{
	ImVec2 imVec2 = ImVec2(static_cast<_float>(g_iWinSizeX - 180.f), 10.f);
	ImGui::SetNextWindowPos(ImVec2(imVec2.x + m_tMainView.v2Pos.x, 5.f + imVec2.y + m_tMainView.v2Pos.y), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(m_tMainView.v2Size, ImGuiCond_Appearing);
	{
		m_tMainView.bEnable = m_bOpen;
		if (false == m_bOpen)
			return;
		ImGui::Begin(m_tMainView.strName.c_str(), &m_tMainView.bEnable, m_tMainView.imGuiWindowFlags);
		{
			// Basic info
			ImGuiContext& g = *GImGui;
			ImGuiIO& io = g.IO;
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

			// Setting MenuBar 
			{
				ImGui::BeginMenuBar();
				{
					// Open ImGuiClass
					{
						Open_ImguiClass();
					}
					// Game Control 
					{
						Game_Control();
					}

				}
				ImGui::EndMenuBar();
			}
			// MainView Setting (DockBuild Setting)
			{
				m_tMainView.iID = ImGui::GetID(m_tMainView.strName.c_str());
				if (false == m_bInitSetting)
				{
					ImGui::DockBuilderRemoveNode(m_tMainView.iID); // Clear out existing layout
					ImGui::DockBuilderAddNode(m_tMainView.iID, 0); // Add empty node

					ImGuiID dock_main_id = m_tMainView.iID; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
					ImGuiID dock_id_prop = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.5f, NULL, &dock_main_id);
					ImGuiID dock_id_Hier = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.5f, NULL, &dock_main_id);

					m_tPerspective.iID = dock_id_prop;
					m_tHierarchy.iID = dock_id_Hier;
					// Perspective , Heirecky
					ImGui::DockBuilderDockWindow(m_tPerspective.strName.c_str(), dock_id_prop);
					ImGui::DockBuilderDockWindow(m_tHierarchy.strName.c_str(), dock_id_Hier);
					ImGui::DockBuilderFinish(m_tMainView.iID);

					m_bInitSetting = true;
				}
				ImGui::DockSpace(m_tMainView.iID);
			}
			// Hierachy
			{
				// Hierachy
				ImGui::Begin(m_tHierarchy.strName.c_str(), &m_tHierarchy.bEnable, m_tHierarchy.imGuiWindowFlags);
				{
					Hierachy_Control();
				}
				ImGui::End();
			}
			// Properties
			{
				// Properties
				ImGui::Begin(m_tPerspective.strName.c_str(), &m_tPerspective.bEnable, m_tPerspective.imGuiWindowFlags);
				{
					Perspective_Control();
				}
				ImGui::End();
			}
		}
		ImGui::End();
	}
}

void CMainView_Tool::Open_ImguiClass()
{
	if (ImGui::BeginMenu("Open_ImGuiClass"))
	{
		for (auto& iter : m_ImGuiList)
		{
			if (nullptr != iter)
			{
				bool bIsOpen = iter->Is_Open();
				ImGui::MenuItem(iter->Get_Name(), NULL, &bIsOpen);
				iter->Set_Open(bIsOpen);
			}
		}
		ImGui::EndMenu();
	}
}

void CMainView_Tool::Game_Control()
{
	if (ImGui::BeginMenu("Game"))
	{
		ImGui::MenuItem("Play", 0, &m_bPlay);
		ImGui::MenuItem("Pause", 0, &m_bPause);
		ImGui::MenuItem("Stop", 0, &m_bStop);
		ImGui::MenuItem("Reset", 0, &m_bReset);
		ImGui::EndMenu();
	}
}

void CMainView_Tool::Hierachy_Control()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		Show_Clones_Update(pGameInstance);
		for (auto& iter : m_ShowCloneMap)
		{
			if (ImGui::TreeNode(iter.first.c_str()))
			{
				for (auto& value : iter.second)
				{
					if (ImGui::TreeNode(value.first.c_str()))
					{
						for (auto& Objects : value.second)
						{
							if (true == ImGui::Selectable(Objects.strName.c_str()))
							{
								m_tSelectDesc = Objects;
							}
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CMainView_Tool::Perspective_Control()
{
	if (nullptr != m_tSelectDesc.pGameObject)
	{
		if (ImGui::TreeNodeEx(m_tSelectDesc.strName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			m_tSelectDesc.pGameObject->Extern_HierakeyView(m_tSelectDesc.strName);

			ImGui::TreePop();
		}
	}
}

void CMainView_Tool::Show_Clones_Update(CGameInstance* _pGameInstance)
{
#ifdef _DEBUG
	if (m_ClonesLayerStr.size() != _pGameInstance->Cur_LayerSize(LEVEL_TOOL))
	{
		m_ClonesLayerStr.clear();
		_pGameInstance->Out_LayerTags(LEVEL_TOOL, &m_ClonesLayerStr);
	}
#endif
	_uint iCheck = 0;
	for (auto& Name : m_ClonesLayerStr)
	{
		list<CGameObject*> List = _pGameInstance->Find_GameObjectList(LEVEL_TOOL, Name);
		iCheck += (_uint)List.size();
	}

	if (iCheck <= 0)
		return;

	if (iCheck != m_iShowSize)
	{
		m_ShowCloneMap.clear();
		m_iShowSize = (_uint)iCheck;
		for (auto& Name : m_ClonesLayerStr)
		{
			list<CGameObject*> List = _pGameInstance->Find_GameObjectList(LEVEL_TOOL, Name);
			if (List.size() > 0)
			{
				CGameObject_Tool* Check = dynamic_cast<CGameObject_Tool*>(List.front());
				if (nullptr != Check)
				{
					map<_string, list<SELECTDESC>> TooObj;
					_uint iIndex = 0;
					for (auto& iter : List)
					{
						CGameObject_Tool* pObject = static_cast<CGameObject_Tool*>(iter);
						if (nullptr != pObject)
						{
							_bool bIsTrue = false;
							for (auto& value : TooObj)
							{
								if (!strcmp(value.first.c_str(), pObject->Get_Name()))
								{
									SELECTDESC tDesc;
									tDesc.pGameObject = pObject;
									tDesc.strName = CMethod::Combine_Int(iIndex, pObject->Get_Name());
									value.second.push_back(tDesc);
									bIsTrue = true;
									break;
								}
							}
							if (false == bIsTrue)
							{
								list<SELECTDESC> pGameObjectList;
								SELECTDESC tDesc;
								tDesc.pGameObject = pObject;
								tDesc.strName = CMethod::Combine_Int(iIndex, pObject->Get_Name());
								pGameObjectList.push_back(tDesc);
								TooObj.emplace(pObject->Get_Name(), pGameObjectList);
							}
						}

						++iIndex;
					}
					string tag = CMethod::ConvertWToS(Name);
					m_ShowCloneMap.emplace(tag, TooObj);
				}
			}
		}
	}
}

void CMainView_Tool::Reset_ToolFunction()
{
	if (false == m_bReset)
		return;

	for (auto& iter : m_ImGuiList)
		if (nullptr != iter)
			iter->Release_InnerValue();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading_Tool::Create(m_pDevice, m_pContext, LEVEL_REMOVE))))
			return ;
	}
	RELEASE_INSTANCE(CGameInstance);

	m_bReset = false;
}

CMainView_Tool* CMainView_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMainView_Tool* pInstance = new CMainView_Tool(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("MainView_Tool::Create To Failed");
	}
	return pInstance;
}

void CMainView_Tool::Free()
{
	for (auto& iter : m_ImGuiList)
		Safe_Release(iter);

	__super::Free();
}
