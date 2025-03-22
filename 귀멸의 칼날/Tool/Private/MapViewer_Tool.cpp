#include "MapViewer_Tool.h"
#include "GameInstance.h"
#include "MapGround_Tool.h"
#include "MapObj_Tool.h" 
#include "Monster_Tool.h"

CMapViewer_Tool::CMapViewer_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CImGuiClass_Tool(_pDevice, _pContext)
{
}

CMapViewer_Tool* CMapViewer_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMapViewer_Tool* pInstance = new CMapViewer_Tool(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CMapViewer_Tool::Create To Failed");
	}
	return pInstance;
}

HRESULT CMapViewer_Tool::NativeConstruct(void)
{
	m_pName = "MapView";
	m_tMenu = DOCKDESC("MapView", ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar, ImVec2(1.f, 1.f), ImVec2(500.f, 820.f), true);

	m_tMapToolView = DOCKDESC("MapTool", ImGuiWindowFlags_NoFocusOnAppearing);
	m_tNavigationView = DOCKDESC("NavigationTool", ImGuiWindowFlags_NoFocusOnAppearing);
	m_tMonsterView = DOCKDESC("MonsterTool", ImGuiWindowFlags_NoFocusOnAppearing);
	m_tColliderView = DOCKDESC("ColliderTool", ImGuiWindowFlags_NoFocusOnAppearing);

	m_PivotMatrix = XMMatrixIdentity();
	m_PivotMatrix = XMMatrixScaling(0.04f, 0.04f, 0.04f);

	m_MonsterPivotmatrix = XMMatrixIdentity();
	m_MonsterPivotmatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	return S_OK;
}

HRESULT CMapViewer_Tool::NativeConstruct_InnerValue(void)
{
	return S_OK;
}

HRESULT CMapViewer_Tool::Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Tick(_dTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapViewer_Tool::Late_Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Late_Tick(_dTimeDelta)))
		return E_FAIL;

	Show_Window();

	return S_OK;
}

HRESULT CMapViewer_Tool::Release_InnerValue(void)
{
	Safe_Release(m_pGroundModel);
	m_pGround = nullptr;
	m_pSky = nullptr;
	m_pModifyObj = nullptr;

	for (auto& iter : m_ObjModelMap)
	{
		Safe_Release(iter.second);
	}
	m_ObjModelMap.clear();
	// Modify모드에서 피킹으로 선택한 오브젝트 
	m_pModifyObj = nullptr;
	m_bLoadCheck = false;
	return S_OK;
}

HRESULT CMapViewer_Tool::Render(void)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

#ifdef _DEBUG
	// 드로잉박스 그리기 
	for (auto& iter : m_vecSemo)
	{
		_float3 fPoint1 = _float3{ iter.vertexPos[0].x, iter.vertexPos[0].y + 1.f, iter.vertexPos[0].z };
		_float3 fPoint2 = _float3{ iter.vertexPos[1].x, iter.vertexPos[1].y + 1.f, iter.vertexPos[1].z };
		_float3 fPoint3 = _float3{ iter.vertexPos[2].x, iter.vertexPos[2].y + 1.f, iter.vertexPos[2].z };

		BoundingBox bBox1 = BoundingBox(iter.vertexPos[0], _float3{ 1.f, 1.f, 1.f });
		pGameInstance->Draw(bBox1, CDrawing::VIEW_3D);
		BoundingBox bBox2 = BoundingBox(iter.vertexPos[1], _float3{ 1.f, 1.f, 1.f });
		pGameInstance->Draw(bBox2, CDrawing::VIEW_3D);
		BoundingBox bBox3 = BoundingBox(iter.vertexPos[2], _float3{ 1.f, 1.f, 1.f });
		pGameInstance->Draw(bBox3, CDrawing::VIEW_3D);

		pGameInstance->DrawTriangle(XMLoadFloat3(&fPoint1), XMLoadFloat3(&fPoint2), XMLoadFloat3(&fPoint3), CDrawing::VIEW_3D);
	}

	// 선택 오브젝트 
	if (nullptr != m_pModifyObj)
	{
		_float3 fPos = m_pModifyObj->Get_Transform()->Get_PosF3();
		_float3 fSize = m_pModifyObj->Get_Transform()->Get_Scale();
		BoundingBox bBox4 = BoundingBox(fPos, fSize * 10.f);
		pGameInstance->Draw(bBox4, CDrawing::VIEW_3D, XMVectorSet(0.f, 0.f, 1.f, 1.f));

		// 콜라이더 크기 
		if (true == m_bColliderCheck)
		{
			_float3 fPos = m_pModifyObj->Get_Transform()->Get_PosF3();
			_float3 fSize = _float3{ m_ColliderSize[0], m_ColliderSize[1], m_ColliderSize[2] };
			BoundingBox bBox5 = BoundingBox(_float3{ fPos.x, fPos.y + m_ColliderSize[1], fPos.z }, fSize);
			pGameInstance->Draw(bBox5, CDrawing::VIEW_3D, XMVectorSet(1.f, 0.f, 0.f, 1.f));
		}
	}
	// 선택 몬스터 
	if (nullptr != m_pModifyMonster)
	{
		_float3 fPos = m_pModifyMonster->Get_Transform()->Get_PosF3();
		_float3 fSize = m_pModifyMonster->Get_Transform()->Get_Scale();
		BoundingBox bBox4 = BoundingBox(fPos, fSize * 0.5f);
		pGameInstance->Draw(bBox4, CDrawing::VIEW_3D, XMVectorSet(0.f, 0.f, 1.f, 1.f));
	}
#endif

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CMapViewer_Tool::Show_Window(void)
{
	ImVec2 imVec2 = ImVec2(static_cast<_float>(g_iWinSizeX - 180.f), 10.f);
	ImGui::SetNextWindowPos(ImVec2(imVec2.x + m_tMenu.v2Pos.x, 5.f + imVec2.y + m_tMenu.v2Pos.y), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(m_tMenu.v2Size, ImGuiCond_Appearing);
	ImGui::Begin(m_tMenu.strName.c_str(), &m_tMenu.bEnable, m_tMenu.imGuiWindowFlags);
	{
		ImGui::SetNextWindowPos(ImVec2(imVec2.x + 10.f, 5.f + imVec2.y + 20.f), ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2(50.f, 50.f), ImGuiCond_Appearing);
		{
			m_tMenu.iID = ImGui::GetID(m_tMenu.strName.c_str());
			if (false == m_bInitSetting)
			{
				ImGui::DockBuilderRemoveNode(m_tMenu.iID);
				ImGui::DockBuilderAddNode(m_tMenu.iID, 0);

				ImGuiID dock_id_main = m_tMenu.iID;
				ImGuiID dock_id_MapTool = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Up, 0.5f, NULL, &dock_id_main);
				ImGuiID dock_id_Navigation = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Down, 0.5f, NULL, &dock_id_main);
				ImGuiID dock_id_MonsterTool = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Down, 0.5f, NULL, &dock_id_main);
				ImGuiID dock_id_ColliderTool = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Down, 0.5f, NULL, &dock_id_main);

				m_tMapToolView.iID = dock_id_MapTool;
				m_tNavigationView.iID = dock_id_Navigation;
				ImGui::DockBuilderDockWindow(m_tMapToolView.strName.c_str(), dock_id_MapTool);
				ImGui::DockBuilderDockWindow(m_tNavigationView.strName.c_str(), dock_id_Navigation);
				ImGui::DockBuilderDockWindow(m_tMonsterView.strName.c_str(), dock_id_MonsterTool);
				ImGui::DockBuilderDockWindow(m_tColliderView.strName.c_str(), dock_id_ColliderTool);
				ImGui::DockBuilderFinish(m_tMenu.iID);

				m_bInitSetting = true;
			}
			ImGui::DockSpace(m_tMenu.iID);
		}
		{
			ImGui::Begin(m_tMapToolView.strName.c_str(), &m_tMapToolView.bEnable, m_tMapToolView.imGuiWindowFlags);
			{
				// 맵툴 
				Show_Window_MapTool();
			}
			ImGui::End();
		}
		{
			ImGui::Begin(m_tNavigationView.strName.c_str(), &m_tNavigationView.bEnable, m_tNavigationView.imGuiWindowFlags);
			{
				// 네비게이션툴
				Show_Window_Navigation();
			}
			ImGui::End();
		}
		{
			ImGui::Begin(m_tMonsterView.strName.c_str(), &m_tMonsterView.bEnable, m_tMonsterView.imGuiWindowFlags);
			{
				// 몬스터 배치툴 
				Show_Window_MonsterTool();
			}
			ImGui::End();
		}
		{
			ImGui::Begin(m_tColliderView.strName.c_str(), &m_tColliderView.bEnable, m_tColliderView.imGuiWindowFlags);
			{
				Show_Window_ColliderTool();
			}
			ImGui::End();
		}
	}
	ImGui::End();
}

void CMapViewer_Tool::Load_File(void)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// urokodakim 
	PATHS Path = { L"Model", L"MapModel", L"102_urokodaki_iwa", L"FBX_Converter" };
	FILEGROUP* filegroup = pGameInstance->Find_Folder(&Path);
	for (auto& iter : filegroup->vecFileDataList)
	{
		CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, filegroup, &iter, m_PivotMatrix);
		if (nullptr == pModel)
			continue;
		m_ObjModelMap.emplace(iter.strFileName, pModel);
	}
	// hokusei 
	Path = { L"Model", L"MapModel", L"104_hokusei_vs", L"FBX_Converter" };
	filegroup = pGameInstance->Find_Folder(&Path);
	for (auto& iter : filegroup->vecFileDataList)
	{
		CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, filegroup, &iter, m_PivotMatrix);
		if (nullptr == pModel)
			continue;
		m_ObjModelMap.emplace(iter.strFileName, pModel);
	}
	// story map
	Path = { L"Model", L"MapModel", L"001_fujikasaneyama", L"FBX_Converter" };
	filegroup = pGameInstance->Find_Folder(&Path);
	for (auto& iter : filegroup->vecFileDataList)
	{
		CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, filegroup, &iter, m_PivotMatrix);
		if (nullptr == pModel)
			continue;
		m_ObjModelMap.emplace(iter.strFileName, pModel);
	}
	// 타마요진료소 
	Path = { L"Model", L"MapModel", L"106_shinryojo_niwa", L"FBX_Converter" };
	filegroup = pGameInstance->Find_Folder(&Path);
	for (auto& iter : filegroup->vecFileDataList)
	{
		CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, filegroup, &iter, m_PivotMatrix);
		if (nullptr == pModel)
			continue;
		m_ObjModelMap.emplace(iter.strFileName, pModel);
	}
	// 아사쿠사
	Path = { L"Model", L"MapModel", L"003_tokyofuasakusa", L"FBX_Converter" };
	filegroup = pGameInstance->Find_Folder(&Path);
	for (auto& iter : filegroup->vecFileDataList)
	{
		CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, filegroup, &iter, m_PivotMatrix);
		if (nullptr == pModel)
			continue;
		m_ObjModelMap.emplace(iter.strFileName, pModel);
	}

	// 몬스터모델 
	Path = { L"AnimationModel", L"OniCharacter", L"ZakoOni01", L"FBX_Converter" };
	filegroup = pGameInstance->Find_Folder(&Path);
	for (auto& iter : filegroup->vecFileDataList)
	{
		CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, filegroup, &iter, m_MonsterPivotmatrix);
		if (nullptr == pModel)
			continue;
		m_MonsterModelMap.emplace(iter.strFileName, pModel);
	}
	Path = { L"AnimationModel", L"OniCharacter", L"ZakoOni02", L"FBX_Converter" };
	filegroup = pGameInstance->Find_Folder(&Path);
	for (auto& iter : filegroup->vecFileDataList)
	{
		CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, filegroup, &iter, m_MonsterPivotmatrix);
		if (nullptr == pModel)
			continue;
		m_MonsterModelMap.emplace(iter.strFileName, pModel);
	}
	Path = { L"AnimationModel", L"OniCharacter", L"ZakoOni03", L"FBX_Converter" };
	filegroup = pGameInstance->Find_Folder(&Path);
	for (auto& iter : filegroup->vecFileDataList)
	{
		CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, filegroup, &iter, m_MonsterPivotmatrix);
		if (nullptr == pModel)
			continue;
		m_MonsterModelMap.emplace(iter.strFileName, pModel);
	}

	MSG_BOX("Load OK");
	RELEASE_INSTANCE(CGameInstance);
}

void CMapViewer_Tool::Show_Window_MapTool(void)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Select ToolMode");
	ImGui::RadioButton("Map", &iToolMode, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Navigation", &iToolMode, 1);
	//ImGui::SameLine();
	//ImGui::RadioButton("Monster", &iToolMode, 2);
	//ImGui::SameLine();
	//ImGui::RadioButton("Collider", &iToolMode, 3);

	//ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), u8"<맵툴 시작방법>");
	//ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), u8"");
	//ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), u8"지형만 띄우고 싶을 때");
	//ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), u8" - CreateGround버튼으로 지형을 생성할 수 있습니다");
	//ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), u8"");
	//ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), u8"맵을 만들고 싶을 때");
	//ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), u8" - MapLoad, CreateGround 두버튼을 눌러시작합니다");
	//ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), u8" - 라디오버튼으로 맵 선택후 Update를 누르면");
	//ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), u8"   지형과 스카이박스 변경이 가능합니다");
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), u8"");

	ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Create File");
	if (ImGui::Button("ModelLoad"))
	{
		if (0 == (_uint)m_ObjModelMap.size())
			Load_File();
	}

	ImGui::SameLine();
	if (ImGui::Button("CreateGround"))
	{
		// 그라운드 생성
		if (nullptr == m_pGround)
		{
			_matrix PivotMatrix = XMMatrixIdentity();
			PivotMatrix = XMMatrixScaling(0.04f, 0.04f, 0.04f);
			PATHS Path = { L"Model", L"MapModel", L"MapGround", L"FBX_Converter" };
			FILEGROUP* filegroup = pGameInstance->Find_Folder(&Path);
			lstrcpy(m_GroundName, PROTO_COMP_MODEL_UROKODAKIIWA_GROUND);
			m_pGroundModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, filegroup, m_GroundName, PivotMatrix);

			m_pGround = pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_MAPGROUND, PROTO_GOB_MAPGROUND);

			lstrcpy(m_SkyName, PROTO_COMP_MODEL_UROKODAKIIWA_SKY);
			m_pSky = pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_MAPSKY, PROTO_GOB_MAPSKY, m_SkyName);
		}
	}

	ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Select Map To Create");
	ImGui::RadioButton("Urokodaki", &iVIBuffer, 0);
	ImGui::SameLine();
	ImGui::RadioButton("StoryMap", &iVIBuffer, 1);
	//ImGui::SameLine();
	//ImGui::RadioButton("Asakusa", &iVIBuffer, 2);

	ImGui::RadioButton("Rattan", &iVIBuffer, 3);
	ImGui::SameLine();
	ImGui::RadioButton("TeOni", &iVIBuffer, 4);
	ImGui::SameLine();
	ImGui::RadioButton("Tamayo", &iVIBuffer, 5);

	switch (iVIBuffer)
	{
	case 0:
	{
		lstrcpy(m_GroundName, PROTO_COMP_MODEL_UROKODAKIIWA_GROUND);
		lstrcpy(m_SkyName, PROTO_COMP_MODEL_UROKODAKIIWA_SKY);
	}
	break;
	case 1:
	{
		lstrcpy(m_GroundName, PROTO_COMP_MODEL_STORY_GROUND);
		lstrcpy(m_SkyName, PROTO_COMP_MODEL_STORY_SKY);
	}
	break;
	case 2: // 아사쿠사 
	{
		lstrcpy(m_GroundName, PROTO_COMP_MODEL_ASAKUSA_GROUND);
		lstrcpy(m_SkyName, PROTO_COMP_MODEL_STORY_SKY);
	}
	break;
	case 3: // 등나무맵 rattan
	{
		lstrcpy(m_GroundName, PROTO_COMP_MODEL_RATTAN_GROUND);
		lstrcpy(m_SkyName, PROTO_COMP_MODEL_RATTAN_SKY);
	}
	break;
	case 4: // 테오니 
	{
		lstrcpy(m_GroundName, PROTO_COMP_MODEL_TEONI_GROUND);
		lstrcpy(m_SkyName, PROTO_COMP_MODEL_STORY_SKY);
	}
	break;
	case 5: // 타마요	
	{
		lstrcpy(m_GroundName, PROTO_COMP_MODEL_TAMAYO_GROUND);
		lstrcpy(m_SkyName, PROTO_COMP_MODEL_STORY_SKY);
	}
	break;
	}

	if (ImGui::Button("Update"))
	{
		dynamic_cast<CMapGround_Tool*>(m_pGround)->Delete_Self();
		dynamic_cast<CMapGround_Tool*>(m_pSky)->Delete_Self();
		m_pGround = nullptr;
		m_pSky = nullptr;

		_matrix PivotMatrix = XMMatrixIdentity();
		PivotMatrix = XMMatrixScaling(0.04f, 0.04f, 0.04f);
		PATHS Path = { L"Model", L"MapModel", L"MapGround", L"FBX_Converter" };
		FILEGROUP* filegroup = pGameInstance->Find_Folder(&Path);
		Safe_Release(m_pGroundModel);
		m_pGroundModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, filegroup, m_GroundName, PivotMatrix);
		m_pGround = pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_MAPGROUND, PROTO_GOB_MAPGROUND);
		m_pSky = pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_MAPSKY, PROTO_GOB_MAPSKY, m_SkyName);

		if (2 == iVIBuffer)
			m_pGround->Get_Transform()->Bind_WorldMatrix(m_pGround->Get_Transform()->Get_WorldMatrix() * XMMatrixRotationY(-90.f));
	}

	static int item_current_idx = 0;
	ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Select ModelList");

	_char chName[MAX_PATH] = { "" };
	strcpy_s(chName, m_strFindFolderFileName.c_str());
	_bool bIsInput = ImGui::InputText("Find Folder", chName, sizeof(_char) * MAX_PATH);
	m_strFindFolderFileName = chName;
	if (ImGui::BeginListBox("."))
	{
		_uint iIndex = 0;
		for (auto& iter : m_ObjModelMap)
		{
			_string str = CMethod::ConvertWToS(iter.first);
			if (true == CMethod::Is_Same_Text(str.c_str(), m_strFindFolderFileName.c_str()))
			{
				const bool is_selected = (m_iIndex == iIndex);
				if (ImGui::Selectable(str.c_str(), is_selected))
				{
					m_pSelectModel = iter.second;
					m_iIndex = iIndex;
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			iIndex++;
		}
		ImGui::EndListBox();
	}

	ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Select CreateMode");
	ImGui::RadioButton("Create", &iCreateMode, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Modify", &iCreateMode, 1);

	// 크리에이트 모드 + 컨트롤, 피킹이라면 오브젝트 생성 / 모델 넘겨줘서 생성할것 
	if (pGameInstance->Get_DIKeyPressing(DIK_LCONTROL) && 0 == iToolMode)
	{
		if (0 == iCreateMode) // Create
		{
			if (pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
			{
				CGameObject* pGame_Object = pGameInstance->Find_GameObject(LEVEL_TOOL, LAYER_MAPGROUND, 0);
				if (nullptr != pGame_Object)
				{
					CTransform* pTransform = dynamic_cast<CTransform*>(pGame_Object->Get_Component(COM_TRANSFORM));
					_uint iMeshNum = m_pGroundModel->Get_NumMeshContainers();
					vector<class CMeshContainer*> MeshContainers;
					for (_uint i = 0; i < iMeshNum; ++i)
					{
						MeshContainers.push_back(m_pGroundModel->Get_MeshContainers()[i]);
						if (nullptr != MeshContainers[i])
						{
							_float3 vOut;
							if (pGameInstance->Picking_Mesh(nullptr, pTransform, reinterpret_cast<CVIBuffer*>(MeshContainers[i]), &vOut))
							{
								if (nullptr != m_pSelectModel)
								{
									pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, m_pSelectModel->Get_FileData()->strFileName.c_str(), PROTO_GOB_MAPOBJECT, &vOut);
								}
							}
						}
					}
				}
			}
		}
		else if (1 == iCreateMode) // Modifiy
		{
			if (pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
			{
				list<CGameObject*> listObject = pGameInstance->Find_GameObjectList(LEVEL_TOOL, m_pSelectModel->Get_FileData()->strFileName.c_str());
				if (nullptr != &listObject)
				{
					for (auto& iter : listObject)
					{
						CTransform* pTransform = dynamic_cast<CTransform*>(iter->Get_Component(COM_TRANSFORM));
						CModel* pModel = dynamic_cast<CModel*>(iter->Get_Component(COM_MODEL));
						if (nullptr != pTransform && nullptr != pModel)
						{
							_uint i = 0;
							_uint iMeshNum = pModel->Get_NumMeshContainers();
							vector<class CMeshContainer*> MeshContainers;
							for (_uint i = 0; i < iMeshNum; ++i)
							{
								MeshContainers.push_back(pModel->Get_MeshContainers()[i]);
								if (nullptr != MeshContainers[i])
								{
									if (pGameInstance->Picking_Mesh(nullptr, pTransform, reinterpret_cast<CVIBuffer*>(MeshContainers[i])))
									{
										m_pModifyObj = iter;
										break;
									}
								}
							}
						}
					}
				}
			}

		}
	}

	ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Set Matrix");

	// size
	static float fSize[3]{ 1.f, 1.f, 1.f };
	if (nullptr != m_pModifyObj)
	{
		CComponent* pTransform = m_pModifyObj->Get_Component(COM_TRANSFORM);
		_float3 vSize = dynamic_cast<CTransform*>(pTransform)->Get_Scale();
		fSize[0] = vSize.x;
		fSize[1] = vSize.y;
		fSize[2] = vSize.z;
	}

	// 사이즈 버튼 
	if (ImGui::Button("Size +"))
	{
		CComponent* pTransform = m_pModifyObj->Get_Component(COM_TRANSFORM);
		_float3 vSize = dynamic_cast<CTransform*>(pTransform)->Get_Scale();
		fSize[0] = vSize.x + 0.5f;
		fSize[1] = vSize.y;
		fSize[2] = vSize.z;

		if (nullptr != m_pModifyObj)
		{
			if (0.1f >= fSize[0])
				fSize[0] = 0.1f;
			if (50.f <= fSize[0])
				fSize[0] = 50.f;
			CComponent* pTransform = m_pModifyObj->Get_Component(COM_TRANSFORM);
			dynamic_cast<CTransform*>(pTransform)->Set_Scale(_float3{ fSize[0], fSize[0], fSize[0] });

			fSize[1] = fSize[0];
			fSize[2] = fSize[0];
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Size -"))
	{
		CComponent* pTransform = m_pModifyObj->Get_Component(COM_TRANSFORM);
		_float3 vSize = dynamic_cast<CTransform*>(pTransform)->Get_Scale();
		fSize[0] = vSize.x - 0.5f;
		fSize[1] = vSize.y;
		fSize[2] = vSize.z;

		if (nullptr != m_pModifyObj)
		{
			if (0.1f >= fSize[0])
				fSize[0] = 0.1f;
			if (50.f <= fSize[0])
				fSize[0] = 50.f;
			CComponent* pTransform = m_pModifyObj->Get_Component(COM_TRANSFORM);
			dynamic_cast<CTransform*>(pTransform)->Set_Scale(_float3{ fSize[0], fSize[0], fSize[0] });

			fSize[1] = fSize[0];
			fSize[2] = fSize[0];
		}
	}

	if (ImGui::DragFloat3("Size", fSize, 0.1f))
	{
		if (nullptr != m_pModifyObj)
		{
			if (0.1 >= fSize[0])
				fSize[0] = 0.1f;
			CComponent* pTransform = m_pModifyObj->Get_Component(COM_TRANSFORM);
			dynamic_cast<CTransform*>(pTransform)->Set_Scale(_float3{ fSize[0], fSize[0], fSize[0] });

			fSize[1] = fSize[0];
			fSize[2] = fSize[0];
		}
	}

	// Rotation 
	static float fRotation[3]{};
	if (nullptr != m_pModifyObj)
	{
		fRotation[0] = dynamic_cast<CMapObj_Tool*>(m_pModifyObj)->Get_RotationXYZ()->x;
		fRotation[1] = dynamic_cast<CMapObj_Tool*>(m_pModifyObj)->Get_RotationXYZ()->y;
		fRotation[2] = dynamic_cast<CMapObj_Tool*>(m_pModifyObj)->Get_RotationXYZ()->z;
	}
	if (ImGui::DragFloat3("Rotation", fRotation, 1.f))
	{
		if (nullptr != m_pModifyObj)
		{
			CComponent* pTransform = m_pModifyObj->Get_Component(COM_TRANSFORM);
			_vector vRight = dynamic_cast<CTransform*>(pTransform)->Get_Right();
			_vector vUp = dynamic_cast<CTransform*>(pTransform)->Get_Up();
			_vector vLook = dynamic_cast<CTransform*>(pTransform)->Get_Look();
			_float3 vSize = dynamic_cast<CTransform*>(pTransform)->Get_Scale();

			_matrix RotationMatrix;
			RotationMatrix = XMMatrixRotationX(XMConvertToRadians(fRotation[0])) * XMMatrixRotationY(XMConvertToRadians(fRotation[1])) * XMMatrixRotationZ(XMConvertToRadians(fRotation[2]));

			dynamic_cast<CTransform*>(pTransform)->Set_Right(RotationMatrix.r[0]);
			dynamic_cast<CTransform*>(pTransform)->Set_Up(RotationMatrix.r[1]);
			dynamic_cast<CTransform*>(pTransform)->Set_Look(RotationMatrix.r[2]);
			dynamic_cast<CTransform*>(pTransform)->Set_Scale(vSize);

			dynamic_cast<CMapObj_Tool*>(m_pModifyObj)->Set_RotationXYZ(_float3{ fRotation[0], fRotation[1], fRotation[2] });
		}
	}

	// Position 
	static float fPosition[3];
	if (nullptr != m_pModifyObj)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pModifyObj->Get_Component(COM_TRANSFORM));
		_float3 vfPos{};
		_vector vPos = pTransform->Get_Pos();
		XMStoreFloat3((_float3*)&vfPos, vPos);
		fPosition[0] = vfPos.x;
		fPosition[1] = vfPos.y;
		fPosition[2] = vfPos.z;
	}

	if (ImGui::DragFloat3("Position", fPosition, 0.5f))
	{
		if (nullptr != m_pModifyObj)
		{
			CTransform* pTransform = dynamic_cast<CTransform*>(m_pModifyObj->Get_Component(COM_TRANSFORM));
			_vector vPos = XMVectorSet(fPosition[0], fPosition[1], fPosition[2], 1.f);
			pTransform->Set_Pos(vPos);
		}
	}

	// 드래그 위치 수정기능 여기에 추가 
	// L컨트롤, 모디파이모드 
	if (pGameInstance->Get_DIKeyPressing(DIK_LSHIFT) && 0 == iToolMode)
	{
		if (1 == iCreateMode && nullptr != m_pModifyObj) // Modifiy
		{
			if (pGameInstance->Get_DIMBtnPressing(CInput_Device::DIMB_L))
			{
				_long MouseMove;

				CTransform* pTransform = dynamic_cast<CTransform*>(m_pModifyObj->Get_Component(COM_TRANSFORM));
				CTransform* pCameraTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_TOOL, LAYER_CAM, COM_TRANSFORM));
				_float3 vfPos{};
				_vector vPos = pTransform->Get_Pos();
				XMStoreFloat3((_float3*)&vfPos, vPos);

				if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_X))
				{
					fPosition[0] = vfPos.x + (_float)MouseMove * pCameraTransform->Get_RightF3().x * 0.5f;
					fPosition[1] = vfPos.y;
					fPosition[2] = vfPos.z + (_float)MouseMove * pCameraTransform->Get_RightF3().z * 0.5f;
					_vector vPos = XMVectorSet(fPosition[0], fPosition[1], fPosition[2], 1.f);
					pTransform->Set_Pos(vPos);
				}
				if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_Y))
				{
					fPosition[0] = vfPos.x - (_float)MouseMove * pCameraTransform->Get_LookF3().x * 0.5f;
					fPosition[1] = vfPos.y;
					fPosition[2] = vfPos.z - (_float)MouseMove * pCameraTransform->Get_LookF3().z * 0.5f;
					//fPosition[2] = vfPos.z - (_float)MouseMove;
					_vector vPos = XMVectorSet(fPosition[0], fPosition[1], fPosition[2], 1.f);
					pTransform->Set_Pos(vPos);
				}
			}
		}
	}

	if (ImGui::Button("Delete"))
	{
		if (1 == iCreateMode)
		{
			if (nullptr != m_pModifyObj)
			{
				dynamic_cast<CMapObj_Tool*>(m_pModifyObj)->Delete_Self();
				m_pModifyObj = nullptr;
			}
		}
	}

	if (ImGui::Button("Copy"))
	{
		if (1 == iCreateMode)
		{
			if (nullptr != m_pModifyObj && nullptr != m_pSelectModel)
			{
				_matrix mat = m_pModifyObj->Get_Transform()->Get_WorldMatrix();
				_float3 vSize = m_pModifyObj->Get_Transform()->Get_Scale();
				_float3 vRot = *static_cast<CMapObj_Tool*>(m_pModifyObj)-> Get_RotationXYZ();
				_float3 vPos = m_pModifyObj->Get_Transform()->Get_PosF3();

				CGameObject * pObj = pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, m_pSelectModel->Get_FileData()->strFileName.c_str(), PROTO_GOB_MAPOBJECT);
				if (nullptr == pObj)
				{
					MSG_BOX("Create Failed");
					return;
				}

				CTransform* pTransform = dynamic_cast<CTransform*>(pObj->Get_Component(COM_TRANSFORM));
				(pTransform)->Bind_WorldMatrix(mat);
				(pTransform)->Set_Scale(vSize);
				dynamic_cast<CMapObj_Tool*>(pObj)->Set_RotationXYZ(vRot);

				(pTransform)->Set_PosF3(_float3{ vPos.x + 5.f, vPos.y, vPos.z + 5.f });

				m_pModifyObj = pObj;
			}
		}
	}

	ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Save File");
	if (ImGui::Button("MapSave"))
	{
		if (IDYES == MSG_BUTTONBOX("정말 정말 저장하시겠습니까?"))
		{
			if (0 != (_uint)m_ObjModelMap.size())
			{
				{
					_wstring strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\";
					_wmkdir(strFilePath.c_str());

					if (0 == iVIBuffer) // 큰바위앞
					{
						strFilePath.append(L"\\");
						strFilePath.append(L"MapObj01");
						strFilePath.append(L".bin");
					}
					else if (1 == iVIBuffer)
					{
						strFilePath.append(L"\\");
						strFilePath.append(L"MapObj02");
						strFilePath.append(L".bin");
					}
					else if (2 == iVIBuffer)
					{
						strFilePath.append(L"\\");
						strFilePath.append(L"MapObj03");
						strFilePath.append(L".bin");
					}
					else if (3 == iVIBuffer) // 등나무맵 rattan
					{
						strFilePath.append(L"\\");
						strFilePath.append(L"MapObj04");
						strFilePath.append(L".bin");
					}
					else if (4 == iVIBuffer) // 테오니 맵
					{
						strFilePath.append(L"\\");
						strFilePath.append(L"MapObj05");
						strFilePath.append(L".bin");
					}
					else if (5 == iVIBuffer) //타마요
					{
						strFilePath.append(L"\\");
						strFilePath.append(L"MapObj06");
						strFilePath.append(L".bin");
					}

					_ulong dwByte = 0;
					HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, NULL,
						NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

					if (INVALID_HANDLE_VALUE == hFile)
					{
						CloseHandle(hFile);
						return;
					}
					if (0 == hFile)
						return;

					for (auto& iter : m_ObjModelMap)
					{
						list<CGameObject*> listObject = pGameInstance->Find_GameObjectList(LEVEL_TOOL, iter.second->Get_FileData()->strFileName.c_str());
						if (nullptr != &listObject)
						{
							for (auto& iter2 : listObject)
							{
								CComponent* pTransfrom = dynamic_cast<CGameObject*>(iter2)->Get_Component(COM_TRANSFORM);
								_matrix Matrix = dynamic_cast<CTransform*>(pTransfrom)->Get_WorldMatrix();
								_float4x4 fMatrix;
								XMStoreFloat4x4(&fMatrix, Matrix);
								WriteFile(hFile, &fMatrix, sizeof(_float4x4), &dwByte, nullptr);
								_uint i = lstrlen(iter.second->Get_FileData()->strFileName.c_str());
								WriteFile(hFile, &i, sizeof(_uint), &dwByte, nullptr);
								WriteFile(hFile, iter.second->Get_FileData()->strFileName.c_str(), sizeof(_tchar) * i, &dwByte, nullptr);
								_float3 vSize = dynamic_cast<CTransform*>(pTransfrom)->Get_Scale();
								WriteFile(hFile, &vSize, sizeof(_float3), &dwByte, nullptr);
								_float3 vRot = *dynamic_cast<CMapObj_Tool*>(iter2)->Get_RotationXYZ();
								WriteFile(hFile, &vRot, sizeof(_float3), &dwByte, nullptr);
							}
						}
					}
					CloseHandle(hFile);
				}
				{
					_wstring strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\";
					_wmkdir(strFilePath.c_str());

					if (0 == iVIBuffer) // 큰바위앞
					{
						strFilePath.append(L"\\");
						strFilePath.append(L"MapGround01");
						strFilePath.append(L".bin");
					}
					else if (1 == iVIBuffer)
					{
						strFilePath.append(L"\\");
						strFilePath.append(L"MapGround02");
						strFilePath.append(L".bin");
					}
					else if (2 == iVIBuffer)
					{
						strFilePath.append(L"\\");
						strFilePath.append(L"MapGround03");
						strFilePath.append(L".bin");
					}
					else if (3 == iVIBuffer)
					{
						strFilePath.append(L"\\");
						strFilePath.append(L"MapGround04");
						strFilePath.append(L".bin");
					}
					else if (4 == iVIBuffer)
					{
						strFilePath.append(L"\\");
						strFilePath.append(L"MapGround05");
						strFilePath.append(L".bin");
					}
					else if (5 == iVIBuffer)
					{
						strFilePath.append(L"\\");
						strFilePath.append(L"MapGround06");
						strFilePath.append(L".bin");
					}

					_ulong dwByte = 0;
					HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, NULL,
						NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

					if (INVALID_HANDLE_VALUE == hFile)
					{
						CloseHandle(hFile);
						return;
					}
					if (0 == hFile)
						return;

					CComponent* pGroundModel = m_pGround->Get_Component(COM_MODEL);
					_uint i = lstrlen(dynamic_cast<CModel*>(pGroundModel)->Get_FileData()->strFileName.c_str());
					WriteFile(hFile, &i, sizeof(_uint), &dwByte, nullptr);
					WriteFile(hFile, dynamic_cast<CModel*>(pGroundModel)->Get_FileData()->strFileName.c_str(), sizeof(_tchar) * i, &dwByte, nullptr);

					CComponent* pSkyModel = m_pSky->Get_Component(COM_MODEL);
					i = lstrlen(dynamic_cast<CModel*>(pSkyModel)->Get_FileData()->strFileName.c_str());
					WriteFile(hFile, &i, sizeof(_uint), &dwByte, nullptr);
					WriteFile(hFile, dynamic_cast<CModel*>(pSkyModel)->Get_FileData()->strFileName.c_str(), sizeof(_tchar) * i, &dwByte, nullptr);

					CloseHandle(hFile);
				}
				//{
				//	// 클라이언트 스토리맵에서 쓰는 나무만 파일 
				//	_wstring strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\";
				//	_wmkdir(strFilePath.c_str());
				//	strFilePath.append(L"\\");
				//	strFilePath.append(L"MapObjInstanceTree");
				//	strFilePath.append(L".bin");

				//	_ulong dwByte = 0;
				//	HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, NULL,
				//		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

				//	if (INVALID_HANDLE_VALUE == hFile)
				//	{
				//		CloseHandle(hFile);
				//		return;
				//	}
				//	if (0 == hFile)
				//		return;

				//	for (auto& iter : m_ObjModelMap)
				//	{
				//		if (!lstrcmp(iter.second->Get_FileData()->strFileName.c_str(), PROTO_COMP_MODEL_TREE01)
				//			|| !lstrcmp(iter.second->Get_FileData()->strFileName.c_str(), PROTO_COMP_MODEL_TREE02))
				//		{
				//			list<CGameObject*> listObject = pGameInstance->Find_GameObjectList(LEVEL_TOOL, iter.second->Get_FileData()->strFileName.c_str());
				//			if (nullptr != &listObject)
				//			{
				//				for (auto& iter2 : listObject)
				//				{
				//					CComponent* pTransfrom = dynamic_cast<CGameObject*>(iter2)->Get_Component(COM_TRANSFORM);
				//					_matrix Matrix = dynamic_cast<CTransform*>(pTransfrom)->Get_WorldMatrix();
				//					_float4x4 fMatrix;
				//					XMStoreFloat4x4(&fMatrix, Matrix);
				//					WriteFile(hFile, &fMatrix, sizeof(_float4x4), &dwByte, nullptr);
				//					_uint i = lstrlen(iter.second->Get_FileData()->strFileName.c_str());
				//					WriteFile(hFile, &i, sizeof(_uint), &dwByte, nullptr);
				//					WriteFile(hFile, iter.second->Get_FileData()->strFileName.c_str(), sizeof(_tchar) * i, &dwByte, nullptr);
				//					_float3 vSize = dynamic_cast<CTransform*>(pTransfrom)->Get_Scale();
				//					WriteFile(hFile, &vSize, sizeof(_float3), &dwByte, nullptr);
				//					_float3 vRot = *dynamic_cast<CMapObj_Tool*>(iter2)->Get_RotationXYZ();
				//					WriteFile(hFile, &vRot, sizeof(_float3), &dwByte, nullptr);
				//				}
				//			}
				//		}
				//	}
				//	CloseHandle(hFile);
				//}

				MSG_BOX("Save OK");
			}
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("MapLoad"))
	{
		if (false == m_bLoadCheck)
		{
			dynamic_cast<CMapGround_Tool*>(m_pGround)->Delete_Self();
			dynamic_cast<CMapGround_Tool*>(m_pSky)->Delete_Self();
			m_pGround = nullptr;
			m_pSky = nullptr;

			{
				_wstring strFilePath;

				if (0 == iVIBuffer)
				{
					strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapObj01.bin";
				}
				else if (1 == iVIBuffer)
				{
					strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapObj02.bin";
				}
				else if (2 == iVIBuffer)
				{
					strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapObj03.bin";
				}
				else if (3 == iVIBuffer)
				{
					strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapObj04.bin";
				}
				else if (4 == iVIBuffer)
				{
					strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapObj05.bin";
				}
				else if (5 == iVIBuffer)
				{
					strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapObj06.bin";
				}

				HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, NULL,
					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

				if (INVALID_HANDLE_VALUE == hFile)
				{
					CloseHandle(hFile);
					return;
				}
				if (0 == hFile)
					return;

				DWORD dwByte = 0;
				while (true)
				{
					_float4x4 fMatrix;
					_uint i = 0;
					_tchar ModelName[MAX_PATH] = L"";
					_float3 vSize{};
					_float3 vRot{};
					ReadFile(hFile, &fMatrix, sizeof(_float4x4), &dwByte, nullptr);
					ReadFile(hFile, &i, sizeof(_uint), &dwByte, nullptr);
					ReadFile(hFile, ModelName, sizeof(_tchar) * i, &dwByte, nullptr);
					ReadFile(hFile, &vSize, sizeof(_float3), &dwByte, nullptr);
					ReadFile(hFile, &vRot, sizeof(_float3), &dwByte, nullptr);
					if (0 == dwByte)
						break;

					for (auto& iter : m_ObjModelMap)
					{
						if (0 == _tcscmp(iter.first.c_str(), ModelName))
						{
							m_pSelectModel = iter.second;
							break;
						}
					}
					CGameObject* pObj = pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, m_pSelectModel->Get_FileData()->strFileName.c_str(), PROTO_GOB_MAPOBJECT);
					if (nullptr == pObj)
					{
						MSG_BOX("Load Failed");
						return;
					}

					m_vecName.push_back(ModelName);
					CTransform* pTransform = dynamic_cast<CTransform*>(pObj->Get_Component(COM_TRANSFORM));
					(pTransform)->Bind_WorldMatrix(XMLoadFloat4x4(&fMatrix));
					(pTransform)->Set_Scale(vSize);
					dynamic_cast<CMapObj_Tool*>(pObj)->Set_RotationXYZ(vRot);

					//pTransform->Set_Scale(pTransform->Get_Scale() * 0.4f);
					//pTransform->Set_Pos(pTransform->Get_Pos() * 0.4f);
					////_vector vPos = XMVector3TransformCoord(pTransform->Get_Pos(), XMMatrixRotationY(-90.f));
					////pTransform->Set_Pos(vPos);
					//pTransform->Bind_WorldMatrix(pTransform->Get_WorldMatrix() * XMMatrixRotationY(-90.f));
					//// dynamic_cast<CTransform*>(pTransform)->Rotate_Turn(_float3{ 0.f, -180.f, 0.f });

				}
				CloseHandle(hFile);
			}

			{
				_wstring strFilePath;

				if (0 == iVIBuffer)
				{
					strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapGround01.bin";
				}
				else if (1 == iVIBuffer)
				{
					strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapGround02.bin";
				}
				else if (2 == iVIBuffer)
				{
					strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapGround03.bin";
				}
				else if (3 == iVIBuffer)
				{
					strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapGround04.bin";
				}
				else if (4 == iVIBuffer)
				{
					strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapGround05.bin";
				}
				else if (5 == iVIBuffer)
				{
					strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapGround06.bin";
				}

				HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, NULL,
					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

				if (INVALID_HANDLE_VALUE == hFile)
				{
					CloseHandle(hFile);
					return;
				}
				if (0 == hFile)
					return;

				DWORD dwByte = 0;
				_bool GroundCheck = false;
				while (true)
				{
					_uint i = 0;
					_tchar ModelName[MAX_PATH] = L"";
					ReadFile(hFile, &i, sizeof(_uint), &dwByte, nullptr);
					ReadFile(hFile, ModelName, sizeof(_tchar) * i, &dwByte, nullptr);
					if (0 == dwByte)
						break;

					//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					//lstrcpy(ModelName, PROTO_COMP_MODEL_STORY_GROUND);

					if (false == GroundCheck)
					{
						if (0 == _tcscmp(PROTO_COMP_MODEL_UROKODAKIIWA_GROUND, ModelName))
						{
							lstrcpy(m_GroundName, PROTO_COMP_MODEL_UROKODAKIIWA_GROUND);
						}
						else if (0 == _tcscmp(PROTO_COMP_MODEL_STORY_GROUND, ModelName))
						{
							lstrcpy(m_GroundName, PROTO_COMP_MODEL_STORY_GROUND);
						}
						else if (0 == _tcscmp(PROTO_COMP_MODEL_RATTAN_GROUND, ModelName))
						{
							lstrcpy(m_GroundName, PROTO_COMP_MODEL_RATTAN_GROUND);
						}
						else if (0 == _tcscmp(PROTO_COMP_MODEL_TEONI_GROUND, ModelName))
						{
							lstrcpy(m_GroundName, PROTO_COMP_MODEL_TEONI_GROUND);
						}

						m_pGround = pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_MAPGROUND, PROTO_GOB_MAPGROUND);
						if (nullptr == m_pGround)
						{
							MSG_BOX("Load Failed");
							return;
						}
						GroundCheck = true;
						if (0 == _tcscmp(PROTO_COMP_MODEL_STORY_GROUND, ModelName))
							m_pGround->Get_Transform()->Bind_WorldMatrix(m_pGround->Get_Transform()->Get_WorldMatrix() * XMMatrixRotationY(-90.f));
					}
					else
					{
						if (0 == _tcscmp(PROTO_COMP_MODEL_UROKODAKIIWA_SKY, ModelName))
						{
							lstrcpy(m_SkyName, PROTO_COMP_MODEL_UROKODAKIIWA_SKY);
						}
						else if (0 == _tcscmp(PROTO_COMP_MODEL_STORY_SKY, ModelName))
						{
							lstrcpy(m_SkyName, PROTO_COMP_MODEL_STORY_SKY);
						}
						else if (0 == _tcscmp(PROTO_COMP_MODEL_RATTAN_SKY, ModelName))
						{
							lstrcpy(m_SkyName, PROTO_COMP_MODEL_RATTAN_SKY);
						}

						m_pSky = pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_MAPSKY, PROTO_GOB_MAPSKY, m_SkyName);
						if (nullptr == m_pSky)
						{
							MSG_BOX("Load Failed");
							return;
						}
					}
				}
				CloseHandle(hFile);
			}
			MSG_BOX("Load OK");
			m_bLoadCheck = true;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CMapViewer_Tool::Show_Window_Navigation(void)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 라디오 버튼 
	ImGui::TextColored(ImVec4(0.6f, 0.9f, 1.0f, 1.0f), "Select NaviMode");
	ImGui::RadioButton("Ground", &iNaviMode, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Cliff", &iNaviMode, 1);

	ImGui::TextColored(ImVec4(0.6f, 0.9f, 1.0f, 1.0f), "Select MonsterNum");
	ImGui::RadioButton("Story", &iMonsterMode, 0);
	ImGui::RadioButton("Zako1", &iMonsterMode, 1);
	ImGui::SameLine();
	ImGui::RadioButton("Zako2", &iMonsterMode, 2);
	ImGui::SameLine();
	ImGui::RadioButton("Zako3", &iMonsterMode, 3);
	//ImGui::SameLine();
	//ImGui::RadioButton("Zako4", &iMonsterMode, 4);


	if (pGameInstance->Get_DIKeyPressing(DIK_LCONTROL) && 1 == iToolMode)
	{
		if (pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
		{
			// cliff 
			if (1 == iNaviMode)
			{
				list<CGameObject*> listObject = pGameInstance->Find_GameObjectList(LEVEL_TOOL, LAYER_CLIFF);
				if (nullptr != &listObject)
				{
					for (auto& iter : listObject)
					{
						CTransform* pTransform = dynamic_cast<CTransform*>(iter->Get_Component(COM_TRANSFORM));
						CModel* pModel = dynamic_cast<CModel*>(iter->Get_Component(COM_MODEL));
						if (nullptr != pTransform && nullptr != pModel)
						{
							_uint i = 0;
							_uint iMeshNum = pModel->Get_NumMeshContainers();
							vector<class CMeshContainer*> MeshContainers;
							for (_uint i = 0; i < iMeshNum; ++i)
							{
								MeshContainers.push_back(pModel->Get_MeshContainers()[i]);
								if (nullptr != MeshContainers[i])
								{
									_float3 vOut;
									if (pGameInstance->Picking_Mesh(nullptr, pTransform, reinterpret_cast<CVIBuffer*>(MeshContainers[i]), &vOut))
									{
										for (auto& Vertex3 : m_vecSemo)
										{
											// 비슷한곳을 눌렀을때 같은 점을 선택하도록
											for (_uint i = 0; i < 3; ++i)
											{
												_float fLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vOut) - XMLoadFloat3(&Vertex3.vertexPos[i])));
												if (10.f > fLength)
												{
													vOut = Vertex3.vertexPos[i];
												}
											}
										}
										static _float3 vPosition;
										memcpy(&vPosition, &vOut, sizeof(_float3));
										m_vertex3.vertexPos[m_iCount] = vPosition;
										m_iCount++;

										if (3 == m_iCount)
										{
											// 점 3개 시계방향으로 맞춰주고 넣어주기 
											_vector a = XMLoadFloat3(&m_vertex3.vertexPos[1]) - XMLoadFloat3(&m_vertex3.vertexPos[0]);
											_vector b = XMLoadFloat3(&m_vertex3.vertexPos[2]) - XMLoadFloat3(&m_vertex3.vertexPos[1]);
											_float3 temp{ 0.f, 0.f, 0.f };
											//_vector sour{ 0.f, 0.f, 0.f };

											if (0 > XMVectorGetY(XMVector3Cross(a, b))) // 외적값 음수라면, 시계방향이아니라면 
											{
												temp = m_vertex3.vertexPos[0];
												//sour = XMLoadFloat3(&m_vertex3.vertexPos[1]);
												m_vertex3.vertexPos[0] = m_vertex3.vertexPos[1];
												m_vertex3.vertexPos[1] = temp;
											}

											m_vecSemo.push_back(m_vertex3);
											m_iCount = 0;
										}
									}
								}
							}
						}
					}
				}
			}
			// ground
			if (0 == iNaviMode)
			{
				CGameObject* pGame_Object = pGameInstance->Find_GameObject(LEVEL_TOOL, LAYER_MAPGROUND, 0);
				if (nullptr != pGame_Object)
				{
					CTransform* pTransform = dynamic_cast<CTransform*>(pGame_Object->Get_Component(COM_TRANSFORM));
					_uint iMeshNum = m_pGroundModel->Get_NumMeshContainers();
					vector<class CMeshContainer*> MeshContainers;
					for (_uint i = 0; i < iMeshNum; ++i)
					{
						MeshContainers.push_back(m_pGroundModel->Get_MeshContainers()[i]);
						if (nullptr != MeshContainers[i])
						{
							_float3 vOut;
							if (pGameInstance->Picking_Mesh(nullptr, pTransform, reinterpret_cast<CVIBuffer*>(MeshContainers[i]), &vOut))
							{
								for (auto& Vertex3 : m_vecSemo)
								{
									// 비슷한곳을 눌렀을때 같은 점을 선택하도록
									for (_uint i = 0; i < 3; ++i)
									{
										_float fLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vOut) - XMLoadFloat3(&Vertex3.vertexPos[i])));
										if (10.f > fLength)
										{
											vOut = Vertex3.vertexPos[i];
										}
									}
								}
								static _float3 vPosition;
								memcpy(&vPosition, &vOut, sizeof(_float3));
								m_vertex3.vertexPos[m_iCount] = vPosition;
								m_iCount++;

								if (3 == m_iCount)
								{
									// 점 3개 시계방향으로 맞춰주고 넣어주기 
									_vector a = XMLoadFloat3(&m_vertex3.vertexPos[1]) - XMLoadFloat3(&m_vertex3.vertexPos[0]);
									_vector b = XMLoadFloat3(&m_vertex3.vertexPos[2]) - XMLoadFloat3(&m_vertex3.vertexPos[1]);
									_float3 temp{ 0.f, 0.f, 0.f };
									//_vector sour{ 0.f, 0.f, 0.f };

									if (0 > XMVectorGetY(XMVector3Cross(a, b))) // 외적값 음수라면, 시계방향이아니라면 
									{
										temp = m_vertex3.vertexPos[0];
										//sour = XMLoadFloat3(&m_vertex3.vertexPos[1]);
										m_vertex3.vertexPos[0] = m_vertex3.vertexPos[1];
										m_vertex3.vertexPos[1] = temp;
									}

									m_vecSemo.push_back(m_vertex3);
									m_iCount = 0;
								}
							}
						}
					}
				}
			}
		}
	}

	static int item_current_index = 0;
	ImGui::TextColored(ImVec4(0.6f, 0.9f, 1.0f, 1.0f), "Semo List");
	if (ImGui::BeginListBox("."))
	{
		for (int i = 0; i < m_vecSemo.size(); i++)
		{
			int x = i;
			char buf[256];
			sprintf_s(buf, "%d", x);
			const bool is_selected = (item_current_index == i);
			if (ImGui::Selectable(buf, is_selected))
			{
				item_current_index = i;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (m_vecSemo.size() != 0 && m_vecSemo.size() == item_current_index)
		item_current_index--;

	static float vecVertexPosition1[3] = { 0.f, 0.f, 0.f };
	static float vecVertexPosition2[3] = { 0.f, 0.f, 0.f };
	static float vecVertexPosition3[3] = { 0.f, 0.f, 0.f };
	if (false == m_vecSemo.empty())
	{
		vecVertexPosition1[0] = m_vecSemo[item_current_index].vertexPos[0].x;
		vecVertexPosition1[1] = m_vecSemo[item_current_index].vertexPos[0].y;
		vecVertexPosition1[2] = m_vecSemo[item_current_index].vertexPos[0].z;

		vecVertexPosition2[0] = m_vecSemo[item_current_index].vertexPos[1].x;
		vecVertexPosition2[1] = m_vecSemo[item_current_index].vertexPos[1].y;
		vecVertexPosition2[2] = m_vecSemo[item_current_index].vertexPos[1].z;

		vecVertexPosition3[0] = m_vecSemo[item_current_index].vertexPos[2].x;
		vecVertexPosition3[1] = m_vecSemo[item_current_index].vertexPos[2].y;
		vecVertexPosition3[2] = m_vecSemo[item_current_index].vertexPos[2].z;
	}
	if (ImGui::DragFloat3("Vertex1", vecVertexPosition1, 0.01f))
	{
		m_vecSemo[item_current_index].vertexPos[0].x = vecVertexPosition1[0];
		m_vecSemo[item_current_index].vertexPos[0].y = vecVertexPosition1[1];
		m_vecSemo[item_current_index].vertexPos[0].z = vecVertexPosition1[2];

	}
	if (ImGui::DragFloat3("Vertex2", vecVertexPosition2, 0.01f))
	{
		m_vecSemo[item_current_index].vertexPos[1].x = vecVertexPosition2[0];
		m_vecSemo[item_current_index].vertexPos[1].y = vecVertexPosition2[1];
		m_vecSemo[item_current_index].vertexPos[1].z = vecVertexPosition2[2];

	}
	if (ImGui::DragFloat3("Vertex3", vecVertexPosition3, 0.01f))
	{
		m_vecSemo[item_current_index].vertexPos[2].x = vecVertexPosition3[0];
		m_vecSemo[item_current_index].vertexPos[2].y = vecVertexPosition3[1];
		m_vecSemo[item_current_index].vertexPos[2].z = vecVertexPosition3[2];
	}

	if (ImGui::Button("DeleteSemo"))
	{
		if (false == m_vecSemo.empty())
			m_vecSemo.erase(m_vecSemo.begin() + item_current_index);
	}

	if (true == m_vecSemo.empty())
	{
		for (_uint i = 0; i < 3; ++i)
		{
			vecVertexPosition1[i] = 0.f;
			vecVertexPosition2[i] = 0.f;
			vecVertexPosition3[i] = 0.f;
		}
	}

	ImGui::TextColored(ImVec4(0.6f, 0.9f, 1.0f, 1.0f), "Save File");
	if (ImGui::Button("NavigationSave"))
	{
		if (0 != (_uint)m_vecSemo.size())
		{
			_wstring strFilePath = L"..\\..\\Resources\\MapData\\Navigation\\";
			_wmkdir(strFilePath.c_str());

			if (0 == iVIBuffer) // 큰바위앞
			{
				strFilePath.append(L"\\");
				strFilePath.append(L"Navigation01");
				strFilePath.append(L".bin");
			}
			else if (1 == iVIBuffer)
			{
				switch (iMonsterMode)
				{
				case 0:
				{
					strFilePath.append(L"\\");
					strFilePath.append(L"Navigation02");
					strFilePath.append(L".bin");
				}
				break;
				case 1:
				{
					strFilePath.append(L"\\");
					strFilePath.append(L"Monster1_Navigation02");
					strFilePath.append(L".bin");
				}
				break;
				case 2:
				{
					strFilePath.append(L"\\");
					strFilePath.append(L"Monster2_Navigation02");
					strFilePath.append(L".bin");
				}
				break;
				case 3:
				{
					strFilePath.append(L"\\");
					strFilePath.append(L"Monster3_Navigation02");
					strFilePath.append(L".bin");
				}
				break;
				case 4:
				{
					strFilePath.append(L"\\");
					strFilePath.append(L"Monster4_Navigation02");
					strFilePath.append(L".bin");
				}
				break;
				default:
					break;
				}
			}
			else if (2 == iVIBuffer)
			{
				strFilePath.append(L"\\");
				strFilePath.append(L"Navigation03");
				strFilePath.append(L".bin");
			}
			else if (3 == iVIBuffer)
			{
				strFilePath.append(L"\\");
				strFilePath.append(L"Navigation04");
				strFilePath.append(L".bin");
			}
			else if (4 == iVIBuffer)
			{
				strFilePath.append(L"\\");
				strFilePath.append(L"Navigation05");
				strFilePath.append(L".bin");
			}
			else if (5 == iVIBuffer)
			{
				strFilePath.append(L"\\");
				strFilePath.append(L"Navigation06");
				strFilePath.append(L".bin");
			}

			_ulong dwByte = 0;
			HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, NULL,
				NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			if (INVALID_HANDLE_VALUE == hFile)
			{
				CloseHandle(hFile);
				return;
			}
			if (0 == hFile)
				return;

			for (auto& Semo : m_vecSemo)
			{
				_float3 vPoint[3];
				ZeroMemory(vPoint, sizeof(_float3));
				vPoint[0] = Semo.vertexPos[0]; //_float3{ Semo.vertexPos[0].x, Semo.vertexPos[0].y + 1.f, Semo.vertexPos[0].z };//Semo.vertexPos[0];
				vPoint[1] = Semo.vertexPos[1]; //_float3{ Semo.vertexPos[1].x, Semo.vertexPos[1].y + 1.f, Semo.vertexPos[1].z };//Semo.vertexPos[1];
				vPoint[2] = Semo.vertexPos[2]; //_float3{ Semo.vertexPos[2].x, Semo.vertexPos[2].y + 1.f, Semo.vertexPos[2].z };//Semo.vertexPos[2];

				WriteFile(hFile, vPoint, sizeof(_float3) * 3, &dwByte, nullptr);
			}

			MSG_BOX("Save OK");
			CloseHandle(hFile);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("NavigationLoad"))
	{
		if (0 == (_uint)m_vecSemo.size())
		{
			if (false == m_bNaviLoadCheck)
			{
				{
					_wstring strFilePath;

					if (0 == iVIBuffer)
					{
						strFilePath = L"..\\..\\Resources\\MapData\\Navigation\\Navigation01.bin";
					}
					else if (1 == iVIBuffer)
					{
						switch (iMonsterMode)
						{
						case 0:
						{
							strFilePath = L"..\\..\\Resources\\MapData\\Navigation\\Navigation02.bin";
						}
						break;
						case 1:
						{
							strFilePath = L"..\\..\\Resources\\MapData\\Navigation\\Monster1_Navigation02.bin";
						}
						break;
						case 2:
						{
							strFilePath = L"..\\..\\Resources\\MapData\\Navigation\\Monster2_Navigation02.bin";
						}
						break;
						case 3:
						{
							strFilePath = L"..\\..\\Resources\\MapData\\Navigation\\Monster3_Navigation02.bin";
						}
						break;
						case 4:
						{
							strFilePath = L"..\\..\\Resources\\MapData\\Navigation\\Monster4_Navigation02.bin";
						}
						break;
						default:
							break;
						}
					}
					else if (2 == iVIBuffer)
					{
						strFilePath = L"..\\..\\Resources\\MapData\\Navigation\\Navigation03.bin";
					}
					else if (3 == iVIBuffer)
					{
						strFilePath = L"..\\..\\Resources\\MapData\\Navigation\\Navigation04.bin";
					}
					else if (4 == iVIBuffer)
					{
						strFilePath = L"..\\..\\Resources\\MapData\\Navigation\\Navigation05.bin";
					}
					else if (5 == iVIBuffer)
					{
						strFilePath = L"..\\..\\Resources\\MapData\\Navigation\\Navigation06.bin";
					}

					HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, NULL,
						NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

					if (INVALID_HANDLE_VALUE == hFile)
					{
						CloseHandle(hFile);
						return;
					}
					if (0 == hFile)
						return;

					DWORD dwByte = 0;
					while (true)
					{
						_float3 vPoint[3];
						ReadFile(hFile, vPoint, sizeof(_float3) * 3, &dwByte, nullptr);
						if (0 == dwByte)
							break;

						m_vertex3.vertexPos[0] = vPoint[0];
						m_vertex3.vertexPos[1] = vPoint[1];
						m_vertex3.vertexPos[2] = vPoint[2];
						m_vecSemo.push_back(m_vertex3);
					}
					CloseHandle(hFile);
				}
			}
			MSG_BOX("Load OK");
			m_bNaviLoadCheck = true;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CMapViewer_Tool::Show_Window_MonsterTool(void)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 몬스터 리스트 박스 (인스턴스 모델 똑같은 모델 로드해서 띄울것)
	ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.7f, 1.0f), "Select Monster");

	if (ImGui::BeginListBox("."))
	{
		_uint iIndex = 0;
		for (auto& iter : m_MonsterModelMap)
		{
			_string str = CMethod::ConvertWToS(iter.first);
			const bool is_selected = (m_iMonsterIndex == iIndex);
			if (ImGui::Selectable(str.c_str(), is_selected))
			{
				m_pSelectMonsterModel = iter.second;
				m_iMonsterIndex = iIndex;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();

			iIndex++;
		}
		ImGui::EndListBox();
	}

	static int iMonsterCreateMode = 0;
	ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.7f, 1.0f), "Select CreateMode");
	ImGui::RadioButton("Create", &iMonsterCreateMode, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Modify", &iMonsterCreateMode, 1);

	if (pGameInstance->Get_DIKeyPressing(DIK_LCONTROL) && 2 == iToolMode)
	{
		if (0 == iMonsterCreateMode) // Create
		{
			if (pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
			{
				CGameObject* pGame_Object = pGameInstance->Find_GameObject(LEVEL_TOOL, LAYER_MAPGROUND, 0);
				if (nullptr != pGame_Object)
				{
					CTransform* pTransform = dynamic_cast<CTransform*>(pGame_Object->Get_Component(COM_TRANSFORM));
					_uint iMeshNum = m_pGroundModel->Get_NumMeshContainers();
					vector<class CMeshContainer*> MeshContainers;
					for (_uint i = 0; i < iMeshNum; ++i)
					{
						MeshContainers.push_back(m_pGroundModel->Get_MeshContainers()[i]);
						if (nullptr != MeshContainers[i])
						{
							_float3 vOut;
							if (pGameInstance->Picking_Mesh(nullptr, pTransform, reinterpret_cast<CVIBuffer*>(MeshContainers[i]), &vOut))
							{
								if (nullptr != m_pSelectMonsterModel)
								{
									pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, m_pSelectMonsterModel->Get_FileData()->strFileName.c_str(), PROTO_GOB_MONSTER, &vOut);
								}
							}
						}
					}
				}
			}
		}
		else if (1 == iMonsterCreateMode) // Modifiy
		{
			if (pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
			{
				list<CGameObject*> listObject = pGameInstance->Find_GameObjectList(LEVEL_TOOL, m_pSelectMonsterModel->Get_FileData()->strFileName.c_str());
				if (nullptr != &listObject)
				{
					for (auto& iter : listObject)
					{
						CTransform* pTransform = dynamic_cast<CTransform*>(iter->Get_Component(COM_TRANSFORM));
						CModel* pModel = dynamic_cast<CModel*>(iter->Get_Component(COM_MODEL));
						if (nullptr != pTransform && nullptr != pModel)
						{
							_uint i = 0;
							_uint iMeshNum = pModel->Get_NumMeshContainers();
							vector<class CMeshContainer*> MeshContainers;
							for (_uint i = 0; i < iMeshNum; ++i)
							{
								MeshContainers.push_back(pModel->Get_MeshContainers()[i]);
								if (nullptr != MeshContainers[i])
								{
									if (pGameInstance->Picking_Mesh(nullptr, pTransform, reinterpret_cast<CVIBuffer*>(MeshContainers[i])))
									{
										m_pModifyMonster = iter;
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.7f, 1.0f), "Set Position");
	// Position 
	static float fPosition[3];
	if (nullptr != m_pModifyMonster)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pModifyMonster->Get_Component(COM_TRANSFORM));
		_float3 vfPos{};
		_vector vPos = pTransform->Get_Pos();
		XMStoreFloat3((_float3*)&vfPos, vPos);
		fPosition[0] = vfPos.x;
		fPosition[1] = vfPos.y;
		fPosition[2] = vfPos.z;
	}

	if (ImGui::DragFloat3("Position", fPosition, 0.5f))
	{
		if (nullptr != m_pModifyMonster)
		{
			CTransform* pTransform = dynamic_cast<CTransform*>(m_pModifyMonster->Get_Component(COM_TRANSFORM));
			_vector vPos = XMVectorSet(fPosition[0], fPosition[1], fPosition[2], 1.f);
			pTransform->Set_Pos(vPos);
		}
	}

	if (pGameInstance->Get_DIKeyPressing(DIK_LSHIFT) && 2 == iToolMode)
	{
		if (1 == iMonsterCreateMode && nullptr != m_pModifyMonster) // Modifiy
		{
			if (pGameInstance->Get_DIMBtnPressing(CInput_Device::DIMB_L))
			{
				_long MouseMove;

				CTransform* pTransform = dynamic_cast<CTransform*>(m_pModifyMonster->Get_Component(COM_TRANSFORM));
				CTransform* pCameraTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_TOOL, LAYER_CAM, COM_TRANSFORM));
				_float3 vfPos{};
				_vector vPos = pTransform->Get_Pos();
				XMStoreFloat3((_float3*)&vfPos, vPos);

				if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_X))
				{
					fPosition[0] = vfPos.x + (_float)MouseMove * pCameraTransform->Get_RightF3().x * 0.5f;
					fPosition[1] = vfPos.y;
					fPosition[2] = vfPos.z + (_float)MouseMove * pCameraTransform->Get_RightF3().z * 0.5f;
					_vector vPos = XMVectorSet(fPosition[0], fPosition[1], fPosition[2], 1.f);
					pTransform->Set_Pos(vPos);
				}
				if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_Y))
				{
					fPosition[0] = vfPos.x - (_float)MouseMove * pCameraTransform->Get_LookF3().x * 0.5f;
					fPosition[1] = vfPos.y;
					fPosition[2] = vfPos.z - (_float)MouseMove * pCameraTransform->Get_LookF3().z * 0.5f;
					//fPosition[2] = vfPos.z - (_float)MouseMove;
					_vector vPos = XMVectorSet(fPosition[0], fPosition[1], fPosition[2], 1.f);
					pTransform->Set_Pos(vPos);
				}
			}
		}
	}

	if (ImGui::Button("Delete"))
	{
		if (1 == iMonsterCreateMode)
		{
			if (nullptr != m_pModifyMonster)
			{
				dynamic_cast<CMonster_Tool*>(m_pModifyMonster)->Delete_Self();
				m_pModifyMonster = nullptr;
			}
		}
	}

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Save File");
	if (ImGui::Button("MonsterSave"))
	{
		if (0 != (_uint)m_MonsterModelMap.size())
		{
			{
				_wstring strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\";
				_wmkdir(strFilePath.c_str());

				if (0 == iVIBuffer) // 큰바위앞
				{
					strFilePath.append(L"\\");
					strFilePath.append(L"MapMonster01");
					strFilePath.append(L".bin");
				}
				else if (1 == iVIBuffer)
				{
					strFilePath.append(L"\\");
					strFilePath.append(L"MapMonster02");
					strFilePath.append(L".bin");
				}
				else if (2 == iVIBuffer)
				{
					strFilePath.append(L"\\");
					strFilePath.append(L"MapMonster02");
					strFilePath.append(L".bin");
				}

				_ulong dwByte = 0;
				HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, NULL,
					NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

				if (INVALID_HANDLE_VALUE == hFile)
				{
					CloseHandle(hFile);
					return;
				}
				if (0 == hFile)
					return;

				for (auto& iter : m_MonsterModelMap)
				{
					list<CGameObject*> listObject = pGameInstance->Find_GameObjectList(LEVEL_TOOL, iter.second->Get_FileData()->strFileName.c_str());
					if (nullptr != &listObject)
					{
						for (auto& iter2 : listObject)
						{
							CComponent* pTransfrom = dynamic_cast<CGameObject*>(iter2)->Get_Component(COM_TRANSFORM);
							_float3 vPos = dynamic_cast<CTransform*>(pTransfrom)->Get_PosF3();
							WriteFile(hFile, &vPos, sizeof(_float3), &dwByte, nullptr);
							_uint i = lstrlen(iter.second->Get_FileData()->strFileName.c_str());
							WriteFile(hFile, &i, sizeof(_uint), &dwByte, nullptr);
							WriteFile(hFile, iter.second->Get_FileData()->strFileName.c_str(), sizeof(_tchar) * i, &dwByte, nullptr);
						}
					}

				}
				CloseHandle(hFile);
			}
		}
		MSG_BOX("Save OK");
	}

	ImGui::SameLine();
	if (ImGui::Button("MonsterLoad"))
	{
		if (false == m_bMonsterLoadCheck)
		{
			{
				_wstring strFilePath;

				if (0 == iVIBuffer)
				{
					strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapMonster01.bin";
				}
				else if (1 == iVIBuffer)
				{
					strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapMonster02.bin";
				}
				else if (2 == iVIBuffer)
				{
					strFilePath = L"..\\..\\Resources\\MapData\\MapObj\\MapMonster03.bin";
				}

				HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, NULL,
					NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

				if (INVALID_HANDLE_VALUE == hFile)
				{
					CloseHandle(hFile);
					return;
				}
				if (0 == hFile)
					return;

				DWORD dwByte = 0;
				while (true)
				{
					_float3 vPos{};
					_uint i = 0;
					_tchar ModelName[MAX_PATH] = L"";
					ReadFile(hFile, &vPos, sizeof(_float3), &dwByte, nullptr);
					ReadFile(hFile, &i, sizeof(_uint), &dwByte, nullptr);
					ReadFile(hFile, ModelName, sizeof(_tchar) * i, &dwByte, nullptr);
					if (0 == dwByte)
						break;

					for (auto& iter : m_MonsterModelMap)
					{
						if (0 == _tcscmp(iter.first.c_str(), ModelName))
						{
							m_pSelectMonsterModel = iter.second;
							break;
						}
					}
					CGameObject* pObj = pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, m_pSelectMonsterModel->Get_FileData()->strFileName.c_str(), PROTO_GOB_MONSTER);
					if (nullptr == pObj)
					{
						MSG_BOX("Load Failed");
						return;
					}

					m_vecMonsterName.push_back(ModelName);
					CTransform* pTransform = dynamic_cast<CTransform*>(pObj->Get_Component(COM_TRANSFORM));
					(pTransform)->Set_PosF3(vPos);
				}
				CloseHandle(hFile);
			}
			MSG_BOX("Load OK");
			m_bMonsterLoadCheck = true;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CMapViewer_Tool::Show_Window_ColliderTool(void)
{
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.7f, 1.0f), "Set Size");

	static float fSize[3]{};

	if (ImGui::DragFloat3("Size", fSize, 0.1f))
	{
		if (nullptr != m_pModifyObj)
		{
			m_ColliderSize[0] = fSize[0];
			m_ColliderSize[1] = fSize[1];
			m_ColliderSize[2] = fSize[2];
		}

		for (_uint i = 0; i < 3; ++i)
		{
			if (0.01f > fSize[i])
				fSize[i] = 0.01f;
		}
	}

	//ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.7f, 1.0f), "Set Y Position");

	//static float fY = 0;

	//if (ImGui::DragFloat("Y Pos", &fY, 0.1f))
	//{
	//	if (nullptr != m_pModifyObj)
	//	{
	//		m_fColliderYPos = fY;
	//	}
	//}

	if (ImGui::Button("CreateCollider"))
	{
		m_bColliderCheck = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("DeleteCollider"))
	{
		fSize[0] = 0;
		fSize[1] = 0;
		fSize[2] = 0;

		//fY = 0;

		m_bColliderCheck = false;
	}
}

void CMapViewer_Tool::Free(void)
{
	__super::Free();

	Safe_Release(m_pGroundModel);
	m_pGround = nullptr;
	m_pSky = nullptr;
	m_pModifyObj = nullptr;
	m_pModifyMonster = nullptr;

	for (auto& iter : m_ObjModelMap)
	{
		Safe_Release(iter.second);
	}
	m_ObjModelMap.clear();

	for (auto& iter : m_MonsterModelMap)
	{
		Safe_Release(iter.second);
	}
	m_MonsterModelMap.clear();
}