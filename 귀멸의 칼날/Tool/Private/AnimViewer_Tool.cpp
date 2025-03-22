#include "../../Tool/Public/AnimViewer_Tool.h"
#include "GameInstance.h"
#include "AnimViewerModel.h"
#include "Animation.h"
#include "HierarchyNode.h"
#include "MeshContainer.h"
#include "Channel.h"
#include "AnimationClip.h"
#include "Animator.h"
#include "Item_Tool.h"
#include "HierarchyNode.h"

const ImGuiTableSortSpecs* CAnimViewer_Tool::MyItem::s_current_sort_specs = NULL;

CAnimViewer_Tool::CAnimViewer_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CImGuiClass_Tool(_pDevice, _pContext)
{
}

HRESULT CAnimViewer_Tool::NativeConstruct(void)
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	m_pName = "AnimViewer";
	m_tWinDesc = WINDESC("AnimViewer", ImGuiWindowFlags_MenuBar, ImVec2(5.f, 5.f), ImVec2(600.f, 720.f), true);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 캐릭터 폴더가 있는 폴더까지의 경로를 담는 구조체 변수
	PATHS paths = { L"AnimationModel", L"PlayerCharacter" };
	// 캐릭터 별 폴더가 있는 폴더
	FILEGROUP* pCharacterFolder = pGameInstance->Find_Folder(&paths);
	if (nullptr == pCharacterFolder)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	//// 캐릭터 별 폴더 리스트
	list<FILEGROUP*> pUnderFolderList = pCharacterFolder->lsUnderList;

	paths = { L"AnimationModel", L"OniCharacter" };
	// 캐릭터 별 폴더가 있는 폴더
	FILEGROUP* pOniCharacter = pGameInstance->Find_Folder(&paths);
	if (nullptr == pCharacterFolder)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	for (auto& pFile : pOniCharacter->lsUnderList)
		pUnderFolderList.push_back(pFile);

	//// 보스 폴더 리스트
	paths = { L"AnimationModel", L"BossCharacter" };
	// 캐릭터 별 폴더가 있는 폴더
	FILEGROUP* pBossCharacter = pGameInstance->Find_Folder(&paths);
	if (nullptr == pCharacterFolder)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	for (auto& pFile : pBossCharacter->lsUnderList)
		pUnderFolderList.push_back(pFile);


	// 하위 폴더 리스트에서 바이너리 파일들을 찾아온다.
	if (0 != pUnderFolderList.size())
	{
		for (auto& iter : pUnderFolderList)
		{
			// 바이너리 폴더를 받아온다.
			FILEGROUP* pBinaryFolder = iter->Find_Group(L"FBX_Converter");
			// 바이너리 폴더가 없다면 continue;
			if (nullptr == pBinaryFolder)
				continue;

			// 바이너리 파일들을 binaryList에 담아준다.
			if (0 != pBinaryFolder->vecFileDataList.size())
			{
				for (auto& pBinaryFile : pBinaryFolder->vecFileDataList)
				{
					// 바이너리 파일들을 보관한다.
					m_BinaryList.push_back(pBinaryFile);
					// 피벗 매트릭스를 만들어준다.
					_matrix		PivotMatrix = XMMatrixIdentity();
					PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
					// 모델을 만든다.
					CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, pBinaryFolder, &pBinaryFile, PivotMatrix);
					if (nullptr == pModel)
						return E_FAIL;
					// 만들어진 모델을 리스트에 넣어준다 ( 프로토타입으로 사용함 )
					m_ModelList.push_back(pModel);
				}
			}
		}
	}

	// 사운드
	{
		map<const _tchar*, FMOD_SOUND*>* SoundMap = pGameInstance->Get_SoundMap();
		if (nullptr != SoundMap)
		{
			for (auto& Pair : *SoundMap)
			{
				// 담긴 사운드들의 이름을 담는다.
				_string strSoundName = CMethod::ConvertWToS(Pair.first);

				m_SoundNameList.push_back(strSoundName);
			}
		}
	}


	RELEASE_INSTANCE(CGameInstance);

	{
		paths = { L"Model", L"Item" };
		FILEGROUP* ItemFileGroup = pGameInstance->Find_Folder(&paths);
		for (auto& iter : ItemFileGroup->lsUnderList)
		{
			// 바이너리 폴더를 받아온다.
			FILEGROUP* pBinaryFolder = iter->Find_Group(L"FBX_Converter");
			// 바이너리 폴더가 없다면 continue;
			if (nullptr == pBinaryFolder)
				continue;

			for (auto& Data : pBinaryFolder->vecFileDataList)
			{
				CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pBinaryFolder, &Data,
					XMMatrixScaling(0.1f, 0.1f, 0.1f));
				m_ItemModelList.push_back(pModel);
			}
		}
	}

	{
		paths = { L"Data", L"Animator" };
		FILEGROUP* AnimatorFileGroup = pGameInstance->Find_Folder(&paths);
		for (auto& iter : AnimatorFileGroup->lsUnderList)
		{
			// 바이너리 파일을 넣어준다.
			for (auto& file : iter->vecFileDataList)
				m_SavedAnimatorList.push_back(file);
		}
	}

	return S_OK;
}

HRESULT CAnimViewer_Tool::NativeConstruct_InnerValue(void)
{
	if (FAILED(__super::NativeConstruct_InnerValue()))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		m_pSwordTool = static_cast<CItem_Tool*>(pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_TEST, PROTO_GOB_TEST_ITEM));
		Safe_AddRef(m_pSwordTool);
	}

	{
		m_pShelteTool = static_cast<CItem_Tool*>(pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_TEST, PROTO_GOB_TEST_ITEM));
		Safe_AddRef(m_pShelteTool);
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CAnimViewer_Tool::Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Tick(_dTimeDelta)))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr != m_pAnimatorCom && nullptr != m_pAnimatorCom->Get_CurrentAnimationClip())
	{
		if (pGameInstance->Get_DIKeyDown(DIK_SPACE))
			m_pAnimatorCom->Set_Play(!m_pAnimatorCom->Is_Play());
	}


	/*if (pGameInstance->Get_DIKeyDown(DIK_COMMA))
		m_pAnimatorCom->Play_OneFrame(-_dTimeDelta);

	if (pGameInstance->Get_DIKeyDown(DIK_PERIOD))
		m_pAnimatorCom->Play_OneFrame(_dTimeDelta);*/


	RELEASE_INSTANCE(CGameInstance);

	if (nullptr != m_pSwordTool)
		m_pSwordTool->Tick(_dTimeDelta);

	if (nullptr != m_pShelteTool)
		m_pShelteTool->Tick(_dTimeDelta);

	return S_OK;
}

HRESULT CAnimViewer_Tool::Late_Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Late_Tick(_dTimeDelta)))
		return E_FAIL;

	ImVec2 imVec2 = ImVec2(static_cast<_float>(g_iWinSizeX - 180.f), 10.f);
	ImGui::SetNextWindowPos(ImVec2(imVec2.x + m_tWinDesc.v2Pos.x, 5.f + imVec2.y + m_tWinDesc.v2Pos.y), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(m_tWinDesc.v2Size, ImGuiCond_Appearing);

	ImGui::Begin(m_tWinDesc.strName.c_str(), &m_tWinDesc.bEnable, m_tWinDesc.imGuiWindowFlags);

	Show_Window();

	ImGui::End();

	if (nullptr != m_pSwordTool)
		m_pSwordTool->Late_Tick(_dTimeDelta);

	if (nullptr != m_pShelteTool)
		m_pShelteTool->Late_Tick(_dTimeDelta);

	return S_OK;
}

HRESULT CAnimViewer_Tool::Release_InnerValue(void)
{
	if (FAILED(__super::Release_InnerValue()))
		return E_FAIL;

	// 모델을 적용하던 객체의 레퍼런스 카운트를 감소한다.
	Safe_Release(m_pAnimObject);
	Safe_Release(m_pSwordTool);
	Safe_Release(m_pShelteTool);
	// 테스트용 Animator 컴포넌트 객체의 레퍼런스 카운트를 감소시킨다.
	Safe_Release(m_pAnimatorCom);
	// 테스트용 Animation 클립을 없앤다.
	Safe_Release(m_pAnimationClip);

	m_pAnimObject = nullptr;
	m_pSwordTool = nullptr;
	m_pShelteTool = nullptr;
	m_pAnimatorCom = nullptr;
	m_pAnimationClip = nullptr;
	// 내가 선택한 애니메이션 모델 인덱스 번호 -> 아무것도 선택되지 않았다면 -1로 초기화
	m_iModelIndex = -1;
	// 내가 선택한 모델의 현재 애니메이션 인덱스 번호 -> 아무것도 선택되지 않았다면 -1로 초기화
	m_iAnimationIndex = -1;
	// 현재 재생되고있는 애니메이션이 갖고있는 Channel중 내가 선택한 채널 -> 아무것도 선택되지 않았다면 -1로 초기화
	m_iChannelIndex = -1;
	// 내가 선택한 모델의 현재 Bone 인덱스 번호 -> 아무것도 선택되지 않았다면 -1로 초기화
	m_iBoneIndex = -1;
	// 애니메이션 툴 모델이 들고있는 이벤트 목록중 내가 선택한 이벤트 인덱스
	m_iEventIndex = -1;
	// 현재 선택된 트랜지션 인덱스
	m_iTransitionIndex = -1;
	// Channel중 KeyFrame이 2인 경우를 제외할 때 사용하기위한 bool 변수
	m_bIsFiltered = false;

	return S_OK;
}

void CAnimViewer_Tool::Show_Window(void)
{
	__super::Show_Window();

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("AnimViewer TabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Object"))
		{
			// 애니메이션 모델 만드는 버튼
			if (nullptr == m_pAnimObject)
			{
				if (ImGui::Button("Create Animation Model"))
				{
					if (FAILED(Create_AnimObject()))
						return;
				}
			}
			// 애니메이션 모델이 만들어졌다면 위치 조정이나 무기를 붙이는등의 작업을 할 수 있음.
			else
			{
				ImGui::Text("Animation Model List");
				// 바이너리화 된 애니메이션 캐릭터 모델들을 보여주는 리스트를 띄운다. ( 이 단계에서 나오는 이름은 단순히 파일명일뿐임 )
				if (ImGui::BeginListBox("Animation Model List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
				{
					for (int i = 0; i < m_BinaryList.size(); i++)
					{
						const bool is_selected = (m_iModelIndex == i);
						if (ImGui::Selectable(CMethod::ConvertWToS(m_BinaryList[i].strFileName).c_str(), is_selected))
						{
							m_iModelIndex = i;

							// 모델을 설정한다 ( 테스트 모델의 애니메이터 컴포넌트에도 모델을 넣어준다. )
							if (FAILED(Set_Model()))
								return;

							//// 애니메이션 테스트 모델의 애니메이터를 받아온다.
							//m_pAnimatorCom = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_Animator();
							//if (nullptr == m_pAnimatorCom)
							//	return;

							// 애님모델이 가진 애니메이터에 담긴 모델의 애니메이션을 가져온다.
							vector<CAnimation*> AnimationList = m_pAnimatorCom->Get_Model()->Get_Animations();

							// 애니메이터 컴포넌트의 CurrentClip을 Test로 바꾼다.
							if (nullptr == m_pAnimatorCom->Get_AnimationClip("Test"))
							{
								// 빈 애니메이션 클립 하나를 생성한다.
								CAnimationClip* pClip = CAnimationClip::Create();

								// 이름을 Test로 지정한다.
								pClip->Set_Name("Test");

								// 애니메이터 컴포넌트에게 애니메이션 클립을 넣어준다.
								if (nullptr != pClip)
									m_pAnimatorCom->Add_AnimationClip(pClip);
							}
							m_pAnimatorCom->Set_CurrentAnimationClip("Test");

							// 현재 애니메이션 클립의 메인 애니메이션이 없다면 제일 기본으로 설정해준다.
							if (nullptr == m_pAnimatorCom->Get_CurrentAnimationClip()->Get_MainAnimation())
							{
								m_pAnimatorCom->Get_CurrentAnimationClip()->Set_MainAnimation(AnimationList[0]);
							}

							// 애님모델이 가진 애니메이터에 현재 클립에 애니메이션을 담아준다.
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_MainAnimation(AnimationList[0]);

							// 뼈값을 업데이트한다.
							m_pAnimatorCom->Update_InitBoneState();
						}
					}
					ImGui::EndListBox();
				}
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Animation Clip"))
		{
			// 애니메이션 모델이 없다면 없다는 문구를 띄운다.
			if (nullptr == m_pAnimObject)
				ImGui::Text("Please Select Model");
			else
			{
				// 생성된 애니메이션 모델의 ModelCom을 채워주지 않았다면 채워달라는 문구를 띄운다.
				if (-1 == m_iModelIndex)
					ImGui::Text("Please Set Animation Model");
				// 리스트에서 애니메이션 모델이 선택됐다면 애니메이션 모델이 가진 애니매이션 목록을 보여준다.
				else
				{
					vector<CAnimation*> AnimationList = m_pAnimatorCom->Get_Model()->Get_Animations();

					// 애니메이터 컴포넌트가 Test라는 클립을 갖고있지 않다면 만들어준다.
					if (nullptr == m_pAnimatorCom->Get_AnimationClip("Test"))
					{
						// 빈 애니메이션 클립 하나를 생성한다.
						CAnimationClip* pClip = CAnimationClip::Create();

						// 이름을 Test로 지정한다.
						pClip->Set_Name("Test");

						// 애니메이터 컴포넌트에게 애니메이션 클립을 넣어준다.
						if (nullptr != pClip)
							m_pAnimatorCom->Add_AnimationClip(pClip);
					}
					// 애니메이터의 현재 클립이 Test 클립과 같지 않다면 Test로 설정해준다.			
					if (m_pAnimatorCom->Get_AnimationClip("Test") != m_pAnimatorCom->Get_CurrentAnimationClip())
						m_pAnimatorCom->Set_CurrentAnimationClip("Test");

					if (nullptr == m_pAnimatorCom->Get_CurrentAnimationClip()->Get_MainAnimation())
					{
						m_pAnimatorCom->Get_CurrentAnimationClip()->Set_MainAnimation(AnimationList[0]);
					}

					// 현재 AnimViewerModel 객체의 실행되고있는 애니메이션 인덱스 번호
					string strCurAnimIndex = "Current Playing Animation Index : ";
					if (-1 == m_iAnimationIndex)
						strCurAnimIndex += "Please Select Animation";
					else
						strCurAnimIndex += CMethod::Convert_Number_S(m_iAnimationIndex);
					ImGui::Text(strCurAnimIndex.c_str());

					CAnimator* pAnimatorCom = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_Animator();

					// 현재 AnimViewerModel 객체의 실해되고있는 애니메이션의 이름
					string strCurAnimName = "Current Playing Animation Name : ";

					_int iStart = (_int)m_pAnimatorCom->Get_MainAnimationName().find("(", 0) + 1;
					_int iEnd = (_int)m_pAnimatorCom->Get_MainAnimationName().find(")", 0) - iStart;
					strCurAnimName += m_pAnimatorCom->Get_MainAnimationName().substr(iStart, iEnd);
					ImGui::Text(strCurAnimName.c_str());

					// 애니메이션의 현재 플레이 타임
					string strTimeAcc = "TimeAcc : ";
					strTimeAcc += CMethod::Convert_Number_S(m_pAnimatorCom->Get_TimeAcc(), 2);
					ImGui::PushItemWidth(1.f);
					ImGui::Text(strTimeAcc.c_str());
					ImGui::PopItemWidth();

					ImGui::SameLine();

					// 애니메이션의 길이
					string strDuration = "Duration : ";
					strDuration += CMethod::Convert_Number_S(m_pAnimatorCom->Get_Duration(), 2);
					ImGui::Text(strDuration.c_str());

					ImGui::SameLine();

					// 애니메이션의 TickPerSecond
					string strTickPerSecond = "Tps : ";
					strTickPerSecond += CMethod::Convert_Number_S(m_pAnimatorCom->Get_TickPerSecond(), 2);
					ImGui::Text(strTickPerSecond.c_str());

					ImGui::SameLine();

					_string strAnimSpeed = "Speed : ";
					strAnimSpeed += CMethod::Convert_Number_S(m_pAnimatorCom->Get_AnimationSpeed(), 2);

					ImGui::Text(strAnimSpeed.c_str());

					ImGui::SameLine();

					// 애니메이션 속도값
					if (ImGui::SmallButton("Set"))
					{
						ImGui::OpenPopup("Set");
					}

					if (ImGui::BeginPopup("Set"))
					{
						ImGui::Text("Set Speed");

						ImGui::SameLine();

						if (ImGui::SmallButton("Reset"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(1.f);

						ImGui::Separator();

						if (ImGui::Selectable("0.5x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(0.5f);

						if (ImGui::Selectable("2x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(2.f);

						if (ImGui::Selectable("3x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(3.f);

						if (ImGui::Selectable("4x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(4.f);

						if (ImGui::Selectable("5x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(5.f);

						if (ImGui::Selectable("6x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(6.f);

						_float fSpeed = m_pAnimatorCom->Get_CurrentAnimationClip()->Get_MainAnimation()->Get_FastValue();

						if (ImGui::DragFloat("Current Speed", &fSpeed, 0.1f, 0.f, 10.f))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(fSpeed);

						ImGui::EndPopup();
					}

#pragma region 애니메이션 재생관련 기능
					// 애니메이션의 진행도를 받아온다.
					_float fRatio = (_float)m_pAnimatorCom->Get_Ratio();
					if (ImGui::SliderFloat("Animation Progress Ratio", &fRatio, 0.0f, 1.0f))
					{
						// 애니메이션의 진행도를 변경한다.
						static_cast<CAnimViewerModel*>(m_pAnimObject)->Set_ProgressRatio(fRatio);
					}

					// 플레이 중일때는 Pause버튼 활성화, 플레이하고있지 않는 경우에는 Play 버튼 활성화
					if (true == m_pAnimatorCom->Is_Play())
					{
						if (ImGui::Button("Pause"))
						{
							m_pAnimatorCom->Set_Play(false);
						}
					}
					else
					{
						if (ImGui::Button("Play"))
						{
							m_pAnimatorCom->Set_Play(true);
						}
					}

					ImGui::SameLine();
					// Stop 버튼을 누르면 Play를 false로 설정하고 애니메이션을 현재 애니메이션으로 다시 넣어준다.
					if (ImGui::Button("Stop"))
					{
						m_pAnimatorCom->Set_Stop();
					}

					ImGui::SameLine();

					// 루프여부에 대한 체크를한다.
					_bool isLoop = m_pAnimatorCom->Get_CurrentAnimationClip()->Is_Loop();
					if (ImGui::Checkbox("Loop", &isLoop))
					{
						m_pAnimatorCom->Get_CurrentAnimationClip()->Set_Loop(isLoop);
					}

					ImGui::SameLine();

					// RootBone에 대한 체크를한다.
					_bool isRootBone = m_pAnimatorCom->Is_RootBone();
					if (ImGui::Checkbox("RootBone", &isRootBone))
					{
						m_pAnimatorCom->Set_RootBone(isRootBone);
					}
#pragma endregion

#pragma region 애니메이션 리스트
					if (ImGui::TreeNode("Animation List"))
					{
						const int COLUMNS_COUNT = 5;
						// 애니메이션 개수를 글자로 쓰기위한 string 변수
						string strAnimSize = "Animation Count : ";
						strAnimSize += CMethod::Convert_Number_S((_uint)AnimationList.size());
						ImGui::Text(strAnimSize.c_str());

						static ImVector<MyItem> items;
						// 아이템이 비어있다면 아이템을 만들어준다.
						if (items.Size == 0)
						{
							// 사이즈를 재설정한다.
							items.resize((_int)AnimationList.size(), MyItem());
							for (int n = 0; n < items.Size; n++)
							{
								MyItem& item = items[n];
								// 인덱스 정보 기입
								item.Index = n;
								// ( ) 안에 있는 글자만 받아오기
								iStart = (_int)AnimationList[n]->Get_Name().find("(", 0) + 1;
								iEnd = (_int)AnimationList[n]->Get_Name().find(")", 0) - iStart;
								_string szFileName = AnimationList[n]->Get_Name().substr(iStart, iEnd);
								// 애니메이션의 이름 기입
								strcpy_s(item.Name, szFileName.c_str());
								// 애니메이션의 Duration 정보
								item.Duration = AnimationList[n]->Get_Duration();
								// 애니메이션의 TimePerSecond 정보
								item.Tps = AnimationList[n]->Get_TickPerSecond();
							}
						}

						// Options
						static ImGuiTableFlags flags =
							ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
							| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
							| ImGuiTableFlags_ScrollY;

						if (ImGui::BeginTable("Animation_Table_Sorting", COLUMNS_COUNT, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), 0.0f))
						{
							// - ImGuiTableColumnFlags_DefaultSort
							// - ImGuiTableColumnFlags_NoSort / ImGuiTableColumnFlags_NoSortAscending / ImGuiTableColumnFlags_NoSortDescending
							// - ImGuiTableColumnFlags_PreferSortAscending / ImGuiTableColumnFlags_PreferSortDescending
							ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Index);
							ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Name);
							ImGui::TableSetupColumn("Duration", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Duration);
							ImGui::TableSetupColumn("Tps", ImGuiTableColumnFlags_PreferSortDescending | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_TPS);
							ImGui::TableSetupColumn("Selected", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Action);
							ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
							ImGui::TableHeadersRow();

							// Sort our data if sort specs have been changed!
							if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
								if (sorts_specs->SpecsDirty)
								{
									MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
									// 내가 세운 기준에 의해 정렬한다.
									if (items.Size > 1)
										qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
									MyItem::s_current_sort_specs = NULL;
									sorts_specs->SpecsDirty = false;
								}

							// Demonstrate using clipper for large vertical lists
							ImGuiListClipper clipper;
							clipper.Begin(items.Size);
							while (clipper.Step())
								for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
								{
									MyItem* item = &items[row_n];
									ImGui::PushID(item->Index);
									ImGui::TableNextRow();
									ImGui::TableNextColumn();
									// Index
									ImGui::Text(CMethod::Convert_Number_S(item->Index).c_str());
									ImGui::TableNextColumn();
									// 애니메이션 이름
									const bool is_selected = (m_iAnimationIndex == item->Index);

									if (ImGui::Selectable(item->Name, is_selected))
									{
										m_iAnimationIndex = item->Index;
										Set_Animation();
									}
									ImGui::TableNextColumn();
									// Duration
									ImGui::Text(CMethod::Convert_Number_S(items[item->Index].Duration, 2).c_str());
									ImGui::TableNextColumn();
									// TPS
									ImGui::Text(CMethod::Convert_Number_S(items[item->Index].Tps, 2).c_str());
									ImGui::TableNextColumn();
									// Selected
									if (is_selected)
										ImGui::SmallButton("Selected");
									ImGui::PopID();
								}
							ImGui::EndTable();
						}
						ImGui::TreePop();
					}
#pragma endregion

#pragma region 채널 리스트
					// 모델이 가진 채널 노드 목록
					if (ImGui::TreeNode("Channel List"))
					{
						// 현재 애니메이션이 갖고있는 채널 리스트
						vector<CChannel*> m_ChannelList = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_ChannelList();

						const int COLUMNS_COUNT = 4;
						// 채널 개수를 적는다.
						string strChannelSize = "All Channel Count : ";
						strChannelSize += CMethod::Convert_Number_S((_uint)m_ChannelList.size());
						ImGui::Text(strChannelSize.c_str());

						ImGui::SameLine();

						static ImVector<MyItem> items;
						// 아이템이 비어있다면 아이템을 만들어준다.
						if (items.Size == 0)
						{
							// 사이즈를 재설정한다.
							items.resize((_int)m_ChannelList.size(), MyItem());
							for (int n = 0; n < items.Size; n++)
							{
								MyItem& item = items[n];
								// 인덱스 정보 기입
								item.Index = n;
								// ( ) 안에 있는 글자만 받아오기
								iStart = (_int)m_ChannelList[n]->Get_Name().find("(", 0) + 1;
								iEnd = (_int)m_ChannelList[n]->Get_Name().find(")", 0) - iStart;
								_string szFileName = m_ChannelList[n]->Get_Name().substr(iStart, iEnd);
								// 채널의 이름 기입
								strcpy_s(item.Name, szFileName.c_str());
								// 채널의 키프레임 정보
								item.FrameCount = m_ChannelList[n]->Get_NumMaxKeyFrames();
							}
						}

						// 활성화 된 채널 개수를 적는다.
						string strActiveChannelSize = " / Activate Channel Count : ";
						_uint iActiveCount = 0;
						for (auto& pChannel : m_ChannelList)
						{
							if (2 < pChannel->Get_NumMaxKeyFrames())
								iActiveCount++;
						}
						strActiveChannelSize += CMethod::Convert_Number_S(iActiveCount);
						ImGui::Text(strActiveChannelSize.c_str());

						if (ImGui::Checkbox("Filter DeActive", &m_bIsFiltered))
						{
							// 아이템 목록 지우기
							items.clear();

							// 채널리스트
							vector<CChannel*> ChannelList;

							// 필터링이 켜진다면 FrameCount가 2보다 큰애들만 출력한다.
							if (true == m_bIsFiltered)
							{
								for (auto& pChannel : m_ChannelList)
								{
									if (2 < pChannel->Get_NumMaxKeyFrames())
										ChannelList.push_back(pChannel);
								}
							}
							else
								ChannelList = m_ChannelList;

							// 사이즈를 재설정한다.
							items.resize((_int)ChannelList.size(), MyItem());
							for (int n = 0; n < items.Size; n++)
							{
								MyItem& item = items[n];
								// 인덱스 정보 기입
								item.Index = n;
								// ( ) 안에 있는 글자만 받아오기
								iStart = (_int)ChannelList[n]->Get_Name().find("(", 0) + 1;
								iEnd = (_int)ChannelList[n]->Get_Name().find(")", 0) - iStart;
								_string szFileName = ChannelList[n]->Get_Name().substr(iStart, iEnd);
								// 채널의 이름 기입
								strcpy_s(item.Name, szFileName.c_str());
								// 채널의 키프레임 정보
								item.FrameCount = ChannelList[n]->Get_NumMaxKeyFrames();
							}
						}

						// Options
						static ImGuiTableFlags flags =
							ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
							| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
							| ImGuiTableFlags_ScrollY;

						if (ImGui::BeginTable("Channel_Table_Sorting", COLUMNS_COUNT, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), 0.0f))
						{
							// - ImGuiTableColumnFlags_DefaultSort
							// - ImGuiTableColumnFlags_NoSort / ImGuiTableColumnFlags_NoSortAscending / ImGuiTableColumnFlags_NoSortDescending
							// - ImGuiTableColumnFlags_PreferSortAscending / ImGuiTableColumnFlags_PreferSortDescending
							ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Index);
							ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Name);
							ImGui::TableSetupColumn("KeyFrames", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_FrameCount);
							ImGui::TableSetupColumn("Selected", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Action);
							ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
							ImGui::TableHeadersRow();

							// Sort our data if sort specs have been changed!
							if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
								if (sorts_specs->SpecsDirty)
								{
									MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
									// 내가 세운 기준에 의해 정렬한다.
									if (items.Size > 1)
										qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
									MyItem::s_current_sort_specs = NULL;
									sorts_specs->SpecsDirty = false;
								}

							// Demonstrate using clipper for large vertical lists
							ImGuiListClipper clipper;
							clipper.Begin(items.Size);
							while (clipper.Step())
								for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
								{
									MyItem* item = &items[row_n];
									ImGui::PushID(item->Index);
									ImGui::TableNextRow();
									ImGui::TableNextColumn();
									// Index
									ImGui::Text(CMethod::Convert_Number_S(item->Index).c_str());
									ImGui::TableNextColumn();
									// 이름
									const bool is_selected = (m_iChannelIndex == item->Index);

									if (ImGui::Selectable(item->Name, is_selected))
									{
										m_iChannelIndex = item->Index;
									}
									ImGui::TableNextColumn();
									// KeyFrames
									ImGui::Text(CMethod::Convert_Number_S(items[item->Index].FrameCount, 2).c_str());
									ImGui::TableNextColumn();
									// Selected
									if (is_selected)
										ImGui::SmallButton("Selected");
									ImGui::PopID();
								}
							ImGui::EndTable();
						}
						ImGui::TreePop();
					}
#pragma endregion

#pragma region 하이어라키 노드 리스트
					if (ImGui::TreeNode("Hierarchy Node List"))
					{
						vector<CHierarchyNode*> m_HierarchyNodeList = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_HierarchyNodeList();

						const int COLUMNS_COUNT = 5;
						// 애니메이션 개수를 글자로 쓰기위한 string 변수
						string strChannelSize = "All Bone Count : ";
						strChannelSize += CMethod::Convert_Number_S((_uint)m_HierarchyNodeList.size());
						ImGui::Text(strChannelSize.c_str());

						ImGui::SameLine();

						// 현재 선택된 본을 알려준다.
						string strCurrentBone = "/ Current Bone : ";
						if (-1 == m_iBoneIndex)
							strCurrentBone += "None Selected";
						else
							strCurrentBone += m_HierarchyNodeList[m_iBoneIndex]->Get_Name();

						ImGui::Text(strCurrentBone.c_str());

						// 선택된 본이 있다면 본을 해제할 수 있는 버튼을 만들어준다.
						if (-1 != m_iBoneIndex)
						{
							ImGui::SameLine();

							if (ImGui::SmallButton("Unselect Bone"))
							{
								m_pAnimatorCom->Set_RootBoneName("Root");
								m_iBoneIndex = -1;
							}

							_float4 vBonePos;

							if (-1 != m_iBoneIndex)
								vBonePos = CMethod::XMMatrixGetFloat4(m_HierarchyNodeList[m_iBoneIndex]->Get_CombinedTransformationMatrix(), 3);

							_float4 vPlayerPos = CMethod::XMMatrixGetFloat4(m_pAnimObject->Get_Transform()->Get_WorldMatrix(), 3);

							ImGui::Text("Current BonePos [ x: ");
							ImGui::SameLine();
							ImGui::Text(CMethod::Convert_Number_S(vBonePos.x, 2).c_str());
							ImGui::SameLine();
							ImGui::Text(" / y: ");
							ImGui::SameLine();
							ImGui::SameLine();
							ImGui::Text(CMethod::Convert_Number_S(vBonePos.y, 2).c_str());
							ImGui::SameLine();
							ImGui::Text(" / z: ");
							ImGui::SameLine();
							ImGui::Text(CMethod::Convert_Number_S(vBonePos.z, 2).c_str());
							ImGui::SameLine();
							ImGui::Text(" ]");

							ImGui::Text("Test Model Pos [ x: ");
							ImGui::SameLine();
							ImGui::Text(CMethod::Convert_Number_S(vPlayerPos.x, 2).c_str());
							ImGui::SameLine();
							ImGui::Text(" / y: ");
							ImGui::SameLine();
							ImGui::SameLine();
							ImGui::Text(CMethod::Convert_Number_S(vPlayerPos.y, 2).c_str());
							ImGui::SameLine();
							ImGui::Text(" / z: ");
							ImGui::SameLine();
							ImGui::Text(CMethod::Convert_Number_S(vPlayerPos.z, 2).c_str());
							ImGui::SameLine();
							ImGui::Text(" ]");
						}

						// 위치값 잡을때 사용할 수도 있는 RootBone찾기
						if (ImGui::Button("Find RootBone"))
						{
							int iIndexCount = 0;

							for (auto& pBone : m_HierarchyNodeList)
							{
								// Root라는 이름의 Bone을 찾았다면 BoneIndex를 변경하고 반복문을 탈출한다.
								if (!strcmp(pBone->Get_Name().c_str(), "Root"))
								{
									m_iBoneIndex = iIndexCount;
									m_pAnimatorCom->Set_RootBoneName("Root");
									break;
								}
								iIndexCount++;
							}
						}

						ImGui::SameLine();

						// 무기뼈 위치값 잡을때 사용할 RootBone 담기
						if (ImGui::Button("Add Bone To Animator"))
							m_pAnimatorCom->Add_WeaponBone(m_HierarchyNodeList[m_iBoneIndex]);

						static ImVector<MyItem> items;
						// 아이템이 비어있다면 아이템을 만들어준다.
						if (items.Size == 0)
						{
							// 사이즈를 재설정한다.
							items.resize((_int)m_HierarchyNodeList.size(), MyItem());
							for (int n = 0; n < items.Size; n++)
							{
								MyItem& item = items[n];
								// 인덱스 정보 기입
								item.Index = n;
								// ( ) 안에 있는 글자만 받아오기
								iStart = (_int)m_HierarchyNodeList[n]->Get_Name().find("(", 0) + 1;
								iEnd = (_int)m_HierarchyNodeList[n]->Get_Name().find(")", 0) - iStart;
								_string szFileName = m_HierarchyNodeList[n]->Get_Name().substr(iStart, iEnd);
								// 뼈의 이름 기입
								strcpy_s(item.Name, szFileName.c_str());
								// 뼈의 Depth 정보
								item.Depth = (_uint)m_HierarchyNodeList[n]->Get_Depth();
								// 뼈가 가진 부모뼈의 이름
								iStart = (_int)m_HierarchyNodeList[n]->Get_ParentName().find("(", 0) + 1;
								iEnd = (_int)m_HierarchyNodeList[n]->Get_ParentName().find(")", 0) - iStart;
								_string szParentName = m_HierarchyNodeList[n]->Get_ParentName().substr(iStart, iEnd);
								strcpy_s(item.ParentName, szParentName.c_str());
							}
						}

						// 만약 item목록중 부모뼈와 이름이 같은 객체가 있다면 인덱스번호를 부모뼈 이름의 앞에 붙여준다.
						for (auto& pParent : items)
						{
							for (auto& pChild : items)
							{
								if (!strcmp(pParent.Name, pChild.ParentName))
								{
									_string szParentName = "[";
									szParentName += CMethod::Convert_Number_S(pParent.Index);
									szParentName += "]";
									strcpy_s(pChild.ParentName, (szParentName + pChild.ParentName).c_str());
									pChild.ParentIndex = pParent.Index;
									continue;
								}
							}
						}

						// Options
						static ImGuiTableFlags flags =
							ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
							| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
							| ImGuiTableFlags_ScrollY;

						if (ImGui::BeginTable("Animation_Table_Sorting", COLUMNS_COUNT, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), 0.0f))
						{
							// - ImGuiTableColumnFlags_DefaultSort
							// - ImGuiTableColumnFlags_NoSort / ImGuiTableColumnFlags_NoSortAscending / ImGuiTableColumnFlags_NoSortDescending
							// - ImGuiTableColumnFlags_PreferSortAscending / ImGuiTableColumnFlags_PreferSortDescending
							ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Index);
							ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Name);
							ImGui::TableSetupColumn("Depth", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Depth);
							ImGui::TableSetupColumn("ParentNode", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Parent);
							ImGui::TableSetupColumn("Selected", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Action);
							ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
							ImGui::TableHeadersRow();

							// Sort our data if sort specs have been changed!
							if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
								if (sorts_specs->SpecsDirty)
								{
									MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
									// 내가 세운 기준에 의해 정렬한다.
									if (items.Size > 1)
										qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
									MyItem::s_current_sort_specs = NULL;
									sorts_specs->SpecsDirty = false;
								}

							// Demonstrate using clipper for large vertical lists
							ImGuiListClipper clipper;
							clipper.Begin(items.Size);
							while (clipper.Step())
								for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
								{
									MyItem* item = &items[row_n];
									ImGui::PushID(item->Index);
									ImGui::TableNextRow();
									ImGui::TableNextColumn();
									// Index
									ImGui::Text(CMethod::Convert_Number_S(item->Index).c_str());
									ImGui::TableNextColumn();
									// 뼈의 이름
									const bool is_selected = (m_iBoneIndex == item->Index);

									if (ImGui::Selectable(item->Name, is_selected))
									{
										m_iBoneIndex = item->Index;
										m_pAnimatorCom->Set_RootBoneName(item->Name);
									}
									ImGui::TableNextColumn();
									// Depth
									ImGui::Text(CMethod::Convert_Number_S(items[item->Index].Depth, 2).c_str());
									ImGui::TableNextColumn();
									// 부모 뼈의 이름
									//static int iParentIndex = 0;
									//const bool isParent_selected = (iParentIndex == row_n);

									if (ImGui::Selectable(item->ParentName, false))
									{
										m_iBoneIndex = item->ParentIndex;
									}
									ImGui::TableNextColumn();
									// Selected
									if (is_selected)
										ImGui::SmallButton("Selected");
									ImGui::PopID();
								}
							ImGui::EndTable();
						}
						ImGui::TreePop();
					}
#pragma endregion

#pragma region 메쉬 컨테이너 리스트
					if (ImGui::TreeNode("MeshContainer List"))
					{
						vector<CMeshContainer*> m_MeshList = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_MeshContainerList();

						const int COLUMNS_COUNT = 3;
						// 애니메이션 개수를 글자로 쓰기위한 string 변수
						string strMeshSize = "All Meshes Count : ";
						strMeshSize += CMethod::Convert_Number_S((_uint)m_MeshList.size());
						ImGui::Text(strMeshSize.c_str());

						static ImVector<MyItem> items;
						// 아이템이 비어있다면 아이템을 만들어준다.
						if (items.Size == 0)
						{
							// 사이즈를 재설정한다.
							items.resize((_int)m_MeshList.size(), MyItem());
							for (int n = 0; n < items.Size; n++)
							{
								MyItem& item = items[n];
								// 인덱스 정보 기입
								item.Index = n;
								// ( ) 안에 있는 글자만 받아오기
								iStart = (_int)m_MeshList[n]->Get_Name().find("(", 0) + 1;
								iEnd = (_int)m_MeshList[n]->Get_Name().find(")", 0) - iStart;
								_string szFileName = m_MeshList[n]->Get_Name().substr(iStart, iEnd);
								// 메쉬의 이름 기입
								strcpy_s(item.Name, szFileName.c_str());
								// 메쉬의 활성화 정보 ( 처음 생성시는 무조건 true )
								item.Action = true;
							}
						}

						// 아이템 목록을 순회하며 Action이 True인 객체를 찾아 객체의 Index를 넣어준다.
						m_MeshContainerList.clear();
						for (auto& pMesh : items)
						{
							if (true == pMesh.Action)
								m_MeshContainerList.push_back(pMesh.Index);
						}

						ImGui::SameLine();

						// 현재 활성화된 메쉬의 개수를 알려준다.
						string strMeshCount = "/ Active Meshes Count : ";
						strMeshCount += CMethod::Convert_Number_S((_uint)m_MeshContainerList.size());

						ImGui::Text(strMeshCount.c_str());

						// Options
						static ImGuiTableFlags flags =
							ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
							| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
							| ImGuiTableFlags_ScrollY;

						if (ImGui::BeginTable("Animation_Table_Sorting", COLUMNS_COUNT, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), 0.0f))
						{
							// - ImGuiTableColumnFlags_DefaultSort
							// - ImGuiTableColumnFlags_NoSort / ImGuiTableColumnFlags_NoSortAscending / ImGuiTableColumnFlags_NoSortDescending
							// - ImGuiTableColumnFlags_PreferSortAscending / ImGuiTableColumnFlags_PreferSortDescending
							ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Index);
							ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Name);
							ImGui::TableSetupColumn("Active", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Action);
							ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
							ImGui::TableHeadersRow();

							// Sort our data if sort specs have been changed!
							if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
								if (sorts_specs->SpecsDirty)
								{
									MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
									// 내가 세운 기준에 의해 정렬한다.
									if (items.Size > 1)
										qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
									MyItem::s_current_sort_specs = NULL;
									sorts_specs->SpecsDirty = false;
								}

							// Demonstrate using clipper for large vertical lists
							ImGuiListClipper clipper;
							clipper.Begin(items.Size);
							while (clipper.Step())
								for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
								{
									MyItem* item = &items[row_n];
									ImGui::PushID(item->Index);
									ImGui::TableNextRow();
									ImGui::TableNextColumn();
									// Index
									ImGui::Text(CMethod::Convert_Number_S(item->Index).c_str());
									ImGui::TableNextColumn();
									// Name
									ImGui::Text(item->Name);
									ImGui::TableNextColumn();
									// Active
									ImGui::Checkbox("", &item->Action);
									ImGui::PopID();
								}

							static_cast<CAnimViewerModel*>(m_pAnimObject)->Set_MeshIndexList(m_MeshContainerList);

							ImGui::EndTable();
						}
						ImGui::TreePop();
					}
#pragma endregion

#pragma region 이벤트 리스트
					// 애니메이션 클립이 갖고있는 이벤트 리스트 
					if (ImGui::TreeNode("Events List"))
					{
						static _int iSelectEvent = -1;

						// 애니메이터 컴포넌트가 갖고있는 Test Clip을 받아온다.
						CAnimationClip* pClip = m_pAnimatorCom->Get_AnimationClip("Test");
						if (nullptr == pClip)
							return;

						CModel* pModel = m_pAnimatorCom->Get_Model();
						if (nullptr == pModel)
							return;

						// 이벤트 목록을 받아온다.
						vector<CAnimationClip::ANIMEVENTDESC> eventList = pClip->Get_EventList();

						const int COLUMNS_COUNT = 5;
						// 현재 클립이 가진 이벤트의 개수
						string strEventSize = "All Event Count : ";
						strEventSize += CMethod::Convert_Number_S((_uint)eventList.size());
						ImGui::Text(strEventSize.c_str());

						ImGui::SameLine();

						if (ImGui::Button("Add Event"))
						{
							CAnimationClip::ANIMEVENTDESC animDesc;
							// 현재 Ratio를 담는다.
							animDesc.dStartRatio = pClip->Get_Ratio();
							// 이벤트 리스트를 push_back 한다.
							eventList.push_back(animDesc);
						}

						ImGui::SameLine();

						if (ImGui::Button("Remove Event"))
						{
							// 리스트에서 지운다.
							eventList.erase(eventList.begin() + iSelectEvent);

							// -1로 만들어준다.
							iSelectEvent = -1;
						}

						static ImVector<MyItem> items;
						// 아이템이 비어있다면 아이템을 만들어준다.
						if (items.Size == 0 || eventList.size() != items.Size || m_bIsClipUpdate)
						{
							// 이벤트 리스트의 사이즈로 사이즈를 재설정한다.
							items.resize((_int)eventList.size(), MyItem());
							for (int n = 0; n < items.Size; n++)
							{
								MyItem& item = items[n];
								// 애니메이션 클립의 인덱스 정보 기입
								item.Index = n;
								// ( ) 안에 있는 글자만 받아오기
								iStart = (_int)eventList[n].strName.find("(", 0) + 1;
								iEnd = (_int)eventList[n].strName.find(")", 0) - iStart;
								_string szEventName = eventList[n].strName.substr(iStart, iEnd);
								// 애니메이션 클립의 이름 기입
								strcpy_s(item.Name, szEventName.c_str());
								// 이벤트 타입
								item.iEventType = (_uint)eventList[n].tEventType;
								// 이벤트 시작 비율
								item.dStartRatio = eventList[n].dStartRatio;
								// 이벤트 종료 비율
								item.dEndRatio = eventList[n].dEndRatio;
								// 이벤트 종료 시간
								item.dEndTime = eventList[n].dEndTime;
								// 스폰 위치 
								item.vEventPos = eventList[n].vEventPos;
								// 옵셋 본의 이름
								strcpy_s(item.EventText, eventList[n].strEventText.c_str());
							}
						}

						static const char* eventTypes[] =
						{
							"Effect","UI", "Sound", "Collider", "WeaponBone", "Camera", "Hide", "SpeedSection","BoneTransform","Not_Selected"
						};

						// Options
						static ImGuiTableFlags flags =
							ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
							| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
							| ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX;

						if (ImGui::BeginTable("Event_Table_AniClipTest", COLUMNS_COUNT, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), 0.0f))
						{
							// - ImGuiTableColumnFlags_DefaultSort
							// - ImGuiTableColumnFlags_NoSort / ImGuiTableColumnFlags_NoSortAscending / ImGuiTableColumnFlags_NoSortDescending
							// - ImGuiTableColumnFlags_PreferSortAscending / ImGuiTableColumnFlags_PreferSortDescending
							ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Index);
							ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoSort, 0.0f, MyItemColumnID_Name);
							ImGui::TableSetupColumn("BoneName", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);
							ImGui::TableSetupColumn("EventType", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_EventType);
							ImGui::TableSetupColumn("Values", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);
							ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
							ImGui::TableHeadersRow();

							// Sort our data if sort specs have been changed!
							if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
								if (sorts_specs->SpecsDirty)
								{
									MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
									// 내가 세운 기준에 의해 정렬한다.
									if (items.Size > 1)
										qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
									MyItem::s_current_sort_specs = NULL;
									sorts_specs->SpecsDirty = false;
								}

							// Demonstrate using clipper for large vertical lists
							ImGuiListClipper clipper;
							clipper.Begin(items.Size);
							while (clipper.Step())
								for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
								{
									MyItem* item = &items[row_n];
									ImGui::PushID(item->Index);
									ImGui::TableNextRow();
									ImGui::TableNextColumn();

									// 선택됐다면 파란색으로 그린다.
									if (iSelectEvent == item->Index)
									{
										ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.3f, 0.5f));
										ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, cell_bg_color);
									}

									// Index
									if (ImGui::Selectable(CMethod::Convert_Number_S(item->Index).c_str()))
									{
										iSelectEvent = item->Index;
									}
									ImGui::TableNextColumn();
									// Name
									char strText[MAX_PATH] = "";
									strcpy_s(strText, item->Name);
									ImGui::SetNextItemWidth(-FLT_MIN);
									if (ImGui::InputText("##10", strText, sizeof(char) * MAX_PATH))
									{
										strcpy_s(item->Name, strText);
										eventList[item->Index].strName = strText;
									}
									ImGui::TableNextColumn();

									static _string strInput;
									static vector<CHierarchyNode*> BoneList;
									// BoneName
									if (ImGui::Selectable(eventList[item->Index].strEventText.c_str()))
										ImGui::OpenPopup("BoneName");

									if (ImGui::BeginPopup("BoneName"))
									{
										ImGui::Text(u8"본 이름을 입력하시오");
										_char INPUT[MAX_PATH] = "";
										strcpy_s(INPUT, strInput.c_str());
										_bool bInput = false;
										bInput = ImGui::InputText("BoneName", INPUT, sizeof(_char) * MAX_PATH);
										strInput = INPUT;

										if (!strcmp(strInput.c_str(), ""))
										{
											for (auto& iter : pModel->Get_HierachyNodeList())
											{
												if (ImGui::Selectable(iter->Get_Name().c_str()))
												{
													eventList[item->Index].strEventText = iter->Get_Name();
												}
											}
										}
										else if (true == bInput)
										{
											BoneList.clear();
											for (auto& iter : pModel->Get_HierachyNodeList())
											{
												if (CMethod::Is_Same_Text(iter->Get_Name().c_str(), strInput.c_str()))
													BoneList.push_back(iter);
											}
										}
										else
										{
											for (auto& iter : BoneList)
											{
												if (ImGui::Selectable(iter->Get_Name().c_str()))
												{
													eventList[item->Index].strEventText = iter->Get_Name();
												}
											}
										}

										ImGui::EndPopup();
									}
									ImGui::TableNextColumn();

									// Event
									if (ImGui::Selectable(eventTypes[item->iEventType]))
										ImGui::OpenPopup("EventType");

									if (ImGui::BeginPopup("EventType"))
									{
										for (int i = 0; i < IM_ARRAYSIZE(eventTypes); ++i)
										{
											if (ImGui::Selectable(eventTypes[i]))
											{
												eventList[item->Index].tEventType = (CAnimationClip::ANIMEVENTTYPE)i;
												item->iEventType = i;
											}
										}
										ImGui::EndPopup();
									}
									ImGui::TableNextColumn();

									// Value 모음
									if (ImGui::Selectable("##Values"))
										ImGui::OpenPopup("##EventValues");

									if (ImGui::BeginPopup("##EventValues"))
									{
										ImGui::Text(u8"안쓰는 값은 냅두면 됨");

										ImGui::SameLine();

										if (ImGui::SmallButton("Reset"))
										{
											eventList[item->Index].dStartRatio = pClip->Get_Ratio();
											eventList[item->Index].dEndRatio = 0.0;
											eventList[item->Index].dEndTime = 0.0;
											eventList[item->Index].vEventPos = _float4(0.f, 0.f, 0.f, 1.f);
											eventList[item->Index].fValue = 0.f;
											eventList[item->Index].fAddValue = 0.f;
										}

										ImGui::Separator();

										ImGui::Text(u8"시작지점");

										_float fRatio = (_float)eventList[item->Index].dStartRatio;
										if (ImGui::DragFloat("##1", &fRatio, 0.01f))
											eventList[item->Index].dStartRatio = fRatio;

										ImGui::SameLine();

										if (ImGui::Button("CurTime"))
											eventList[item->Index].dStartRatio = pClip->Get_Ratio();

										ImGui::Text(u8"종료지점");

										_float fEndRatio = (_float)eventList[item->Index].dEndRatio;
										if (ImGui::DragFloat("##2", &fEndRatio, 0.01f))
											eventList[item->Index].dEndRatio = fEndRatio;

										ImGui::Text(u8"종료시간");

										_float fEndTime = (_float)eventList[item->Index].dEndTime;
										if (ImGui::DragFloat("##3", &fEndTime, 0.01f))
											eventList[item->Index].dEndTime = (_double)fEndTime;

										switch (item->iEventType)
										{
										case 0:
											break;

										case 1:
											break;

										case 2:
											break;

										case 3:
											ImGui::Text(u8"애니메이션 재생 속도");
											break;

										case 4:
											ImGui::Text(u8"무기 뼈 인덱스");
											break;

										case 5:
											break;

										case 6:
											break;

										case 7:
											ImGui::Text(u8"애니메이션 재생 속도");
											break;

										case 8:
											ImGui::Text(u8"x = Right, y = Up, z = Look 1 또는 -1 넣으시오");
											break;

										default:
											ImGui::Text(u8"생성될 위치");
											break;
										}

										_float4 vPos = eventList[item->Index].vEventPos;
										_float vEventPos[4] = { vPos.x, vPos.y, vPos.z, vPos.w };
										if (ImGui::DragFloat4("##4", vEventPos, 0.01f))
											memcpy(&eventList[item->Index].vEventPos, &vEventPos, sizeof(_float4));


										switch (item->iEventType)
										{
										case 3:
											ImGui::Text(u8"콜라이더의 방향값입니다.");
											break;

										default:
											ImGui::Text(u8"생성될 위치");
											break;
										}

										_float4 vDir = eventList[item->Index].vEventDir;
										_float vEventDir[4] = { vDir.x, vDir.y, vDir.z, vDir.w };
										if (ImGui::DragFloat4("##EventDirValues", vEventDir, 0.01f))
											memcpy(&eventList[item->Index].vEventDir, &vEventDir, sizeof(_float4));


										switch (item->iEventType)
										{
										case 3:
											ImGui::Text(u8"콜라이더의 크기입니다.");
											break;

										default:
											ImGui::Text(u8"Value값을 넣어주세요");
											break;
										}

										_float4 vValues = eventList[item->Index].vEventValues;
										_float vEventValues[4] = { vValues.x, vValues.y, vValues.z, vValues.w };
										if (ImGui::DragFloat4("##EventValueList", vEventValues, 0.01f))
											memcpy(&eventList[item->Index].vEventValues, &vEventValues, sizeof(_float4));

										switch (item->iEventType)
										{
										case 0:
											break;

										case 1:
											break;

										case 2:
											break;

										case 3:
											ImGui::Text(u8"미는 힘을 입력해주세요");
											break;

										case 4:
											ImGui::Text(u8"무기 뼈 인덱스를 넣어주세요");
											break;

										case 5:
											break;

										case 6:
											break;

										case 7:
											ImGui::Text(u8"애니메이션 재생 속도를 입력하세요");
											break;

										case 8:
											ImGui::Text(u8"이동값을 넣어주세요");
											break;

										default:
											ImGui::Text(u8"Value값을 넣어주세요");
											break;
										}
										_float fValue = eventList[item->Index].fValue;
										if (ImGui::DragFloat("##EventFValue", &fValue, 0.01f))
											eventList[item->Index].fValue = fValue;

										switch (item->iEventType)
										{
										case 0:
											break;

										case 1:
											break;

										case 2:
											break;

										case 3:
											ImGui::Text(u8"포물선의 높이를 입력해주세요");
											break;

										case 4:
											ImGui::Text(u8"무기 뼈 인덱스를 넣어주세요");
											break;

										case 5:
											break;

										case 6:
											break;

										case 7:
											ImGui::Text(u8"애니메이션 재생 속도를 입력하세요");
											break;

										case 8:
											ImGui::Text(u8"이동값을 넣어주세요");
											break;

										default:
											ImGui::Text(u8"추가 Value값을 넣어주세요");
											break;
										}
										_float fAddValue = eventList[item->Index].fAddValue;
										if (ImGui::DragFloat("##EventFAddValue", &fAddValue, 0.01f))
											eventList[item->Index].fAddValue = fAddValue;
										ImGui::EndPopup();
									}
									ImGui::PopID();
								}
							// 이벤트를 세팅한다.
							pClip->Set_Event(eventList);
							ImGui::EndTable();
						}
						ImGui::TreePop();
					}
#pragma endregion
					// 애니메이터에 클립을 추가합니다.
					if (ImGui::Button("Add Clip"))
					{
						Add_AnimationClip();
						// 애니메이터 쪽에서 클립의 내용을 업데이트해준다.
						m_bIsClipUpdate = true;
					}

					ImGui::SameLine();

					if (ImGui::Button("Add AllClip"))
					{
						Add_AllAnimationClip();
						m_bIsClipUpdate = true;
					}
				}
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Animator"))
		{
			if (-1 == m_iModelIndex)
				ImGui::Text("Please Select Model");
			else
			{
				if (ImGui::Button("Save Data"))
					Save_Animator();


				if (ImGui::TreeNode("Animator List"))
				{
					ImGui::Text("Animator Component List");
					// 바이너리화 된 애니메이션 캐릭터 모델들을 보여주는 리스트를 띄운다. ( 이 단계에서 나오는 이름은 단순히 파일명일뿐임 )
					if (ImGui::BeginListBox("Animator Component List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
					{
						for (int i = 0; i < m_SavedAnimatorList.size(); i++)
						{
							const bool is_selected = (m_iAnimatorIndex == i);
							if (ImGui::Selectable(CMethod::ConvertWToS(m_SavedAnimatorList[i].strFileName).c_str(), is_selected))
							{
								m_iAnimatorIndex = i;

								// 애니메이션 테스트 모델의 애니메이터를 넣어준다..
								Set_Animator(m_SavedAnimatorList[i].strFileName);
								m_bIsClipUpdate = true;
							}
						}
						ImGui::EndListBox();
					}
					ImGui::TreePop();
				}

				ImGui::Text("Merge Into Current Animator");
				ImGui::SameLine();
				if (ImGui::Button("Select"))
				{
					ImGui::OpenPopup("Animator List");
				}

				if (ImGui::BeginPopup("Animator List"))
				{
					ImGui::Text("Select Animator");

					ImGui::SameLine();

					static _string strText = "";
					// 애니메이션 클립을 담을 벡터 리스트
					static vector<FILEDATA> SaveToFindAnimatorList;

					_char INPUT[MAX_PATH] = "";
					strcpy_s(INPUT, strText.c_str());
					_bool bInput = false;
					bInput = ImGui::InputText("Animator", INPUT, sizeof(_char) * MAX_PATH);
					strText = INPUT;

					ImGui::Separator();

					if (!strcmp(strText.c_str(), ""))
					{
						for (int i = 0; i < m_SavedAnimatorList.size(); ++i)
						{
							if (ImGui::Selectable(CMethod::ConvertWToS(m_SavedAnimatorList[i].strFileName).c_str()))
							{
								Merge_Animator(m_SavedAnimatorList[i].strFileName);
							}
						}
					}
					else if (true == bInput)
					{
						SaveToFindAnimatorList.clear();
						for (auto& iter : m_SavedAnimatorList)
						{
							if (CMethod::Is_Same_Text(CMethod::ConvertWToS(iter.strFileName).c_str(), strText.c_str()))
								SaveToFindAnimatorList.push_back(iter);
						}
					}
					else
					{
						for (int i = 0; i < SaveToFindAnimatorList.size(); ++i)
						{
							if (ImGui::Selectable(CMethod::ConvertWToS(SaveToFindAnimatorList[i].strFileName).c_str()))
							{
								Merge_Animator(SaveToFindAnimatorList[i].strFileName);
							}
						}
					}
					ImGui::EndPopup();
				}

				// 아이템이 비어있다면 아이템을 만들어준다.
				m_ClipList = m_pAnimatorCom->Get_AnimationClipList();

				// 현재 컴포넌트가 갖고있는 애니메이션 클립 리스트
				if (ImGui::TreeNode("AnimationClip List"))
				{
					vector<_string> EnterNameList;
					for (auto& clip : m_ClipList)
						EnterNameList.push_back(clip->Get_Name());

					//// 클리이 바뀌었는지 체크한다.
					//static _bool m_bIsClipUpdate = false;

					const int COLUMNS_COUNT = 9;
					// 트랜지션 개수를 글자로 쓰기위한 string 변수
					string strMeshSize = "All Clips Count : ";
					strMeshSize += CMethod::Convert_Number_S((_uint)m_ClipList.size());
					ImGui::Text(strMeshSize.c_str());

					ImGui::Text("Current Playing Clip : ");

					ImGui::SameLine();

					if (nullptr == m_pAnimatorCom->Get_CurrentAnimationClip())
						ImGui::Text("None");
					else
						// 현재 재생중인 애니메이션 클립의 이름을 넣어준다.
						ImGui::Text(m_pAnimatorCom->Get_CurrentAnimationClip()->Get_Name().c_str());

					// 애니메이션의 진행도를 받아온다.
					_float fRatio = (_float)m_pAnimatorCom->Get_Ratio();
					if (ImGui::SliderFloat("##Animation Progress Ratio", &fRatio, 0.0f, 1.0f))
					{
						// 애니메이션의 진행도를 변경한다.
						static_cast<CAnimViewerModel*>(m_pAnimObject)->Set_ProgressRatio(fRatio);
					}

					ImGui::SameLine();

					_string strAnimSpeed = "Speed : ";
					strAnimSpeed += CMethod::Convert_Number_S(m_pAnimatorCom->Get_AnimationSpeed(), 2);

					ImGui::Text(strAnimSpeed.c_str());

					ImGui::SameLine();

					// 애니메이션 속도값
					if (ImGui::SmallButton("Set"))
					{
						ImGui::OpenPopup("Set");
					}

					if (ImGui::BeginPopup("Set"))
					{
						ImGui::Text("Set Speed");

						ImGui::SameLine();

						if (ImGui::SmallButton("Reset"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(1.f);

						ImGui::Separator();

						if (ImGui::Selectable("0.5x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(0.5f);

						if (ImGui::Selectable("2x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(2.f);

						if (ImGui::Selectable("3x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(3.f);

						if (ImGui::Selectable("4x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(4.f);

						if (ImGui::Selectable("5x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(5.f);

						if (ImGui::Selectable("6x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(6.f);

						_float fSpeed = m_pAnimatorCom->Get_CurrentAnimationClip()->Get_MainAnimation()->Get_FastValue();

						if (ImGui::DragFloat("Current Speed", &fSpeed, 0.1f, 0.f, 10.f))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(fSpeed);

						m_bIsClipUpdate = true;

						ImGui::EndPopup();
					}

					ImGui::Separator();

					// 애니메이션 타입 
					static const char* AnimTypes[]{ "IDLE","MOVE","PRE_JUMP","JUMP","ATTACK","AIR_ATTACK","CHARGE_ATTACK","GUARD",
					"THROW","PRE_DASH","DASH","AIR_DASH","PARING","HIT","AIR_HIT","DOWN","AIR_DOWN","STEP","AIR_STEP","PUSH",
					"SKILL_1","SKILL_2","SKILL_3","STRIKE","CHARGE","BOUND","DIE","AWAKE","ULTIMATE","SPIN","PASSIVE","ROLL","BLOW", "TURN", "BOSS_HEAL"
					, "INIT" };

					static ImVector<MyItem> items;
					_int iStart = 0;
					_int iEnd = 0;

					if (items.Size == 0 || true == m_bIsClipUpdate || items.Size != m_ClipList.size())
					{
						// 사이즈를 재설정한다.
						items.resize((_int)m_ClipList.size(), MyItem());
						for (int n = 0; n < items.Size; n++)
						{
							MyItem& item = items[n];
							// 인덱스 정보 기입
							item.Index = n;

							// 이름 받아오기
							strcpy_s(item.Name, m_ClipList[n]->Get_Name().c_str());

							_string szMainAnimName;
							// ( ) 안에 있는 글자만 받아오기
							if (nullptr == m_ClipList[n]->Get_MainAnimation())
								szMainAnimName = "None";
							else
							{
								iStart = (_int)m_ClipList[n]->Get_MainAnimationName().find("(", 0) + 1;
								iEnd = (_int)m_ClipList[n]->Get_MainAnimationName().find(")", 0) - iStart;
								szMainAnimName = m_ClipList[n]->Get_MainAnimationName().substr(iStart, iEnd);
							}
							// 메인 애니메이션의 이름 기입
							strcpy_s(item.MainAnim, szMainAnimName.c_str());

							_string szSubAnimName;
							// ( ) 안에 있는 글자만 받아오기
							if (nullptr == m_ClipList[n]->Get_SubAnimation())
								szSubAnimName = "None";
							else
							{
								iStart = (_int)m_ClipList[n]->Get_SubAnimationName().find("(", 0) + 1;
								iEnd = (_int)m_ClipList[n]->Get_SubAnimationName().find(")", 0) - iStart;
								szSubAnimName = m_ClipList[n]->Get_SubAnimationName().substr(iStart, iEnd);
							}
							// 다음 클립의 이름 기입
							strcpy_s(item.SubAnim, szSubAnimName.c_str());

							// 아이템의 루프여부 받아오기
							item.Loop = m_ClipList[n]->Is_Loop();

							// 루트본 여부 받아오기
							item.RootBone = m_ClipList[n]->Is_RootBone();

							// 애니메이션 클립의 속도
							item.fAnimSpeed = m_ClipList[n]->Get_AnimationSpeed();

							// 애니메이션의 타입
							strcpy_s(item.AnimationType, m_ClipList[n]->Get_AnimationType().c_str());
							//for (int i = 0; i < IM_ARRAYSIZE(AnimTypes); ++i)
							//{
								//if ("" == m_ClipList[n]->Get_AnimationType())
								//{
								//	strcpy_s(item.AnimationType, "None");
								//}
								//else
								//{
										//strcpy_s(item.AnimationType, AnimTypes[i]);
									//if (!strcmp(AnimTypes[i], m_ClipList[n]->Get_AnimationType().c_str()))
								//}
							//}

							// Event개수
							item.EventCount = (_int)m_ClipList[n]->Get_EventList().size();
						}
						m_bIsClipUpdate = false;
					}

					static _int iClipIndex = -1;

					// Options
					static ImGuiTableFlags flags =
						ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
						| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
						| ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX;

					if (ImGui::BeginTable("Clip_Table_Sorting", COLUMNS_COUNT, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), 0.0f))
					{
						// - ImGuiTableColumnFlags_DefaultSort
						// - ImGuiTableColumnFlags_NoSort / ImGuiTableColumnFlags_NoSortAscending / ImGuiTableColumnFlags_NoSortDescending
						// - ImGuiTableColumnFlags_PreferSortAscending / ImGuiTableColumnFlags_PreferSortDescending
						ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Index);
						ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_EnterState);
						ImGui::TableSetupColumn("MainAnim", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Name);
						ImGui::TableSetupColumn("SubAnim", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Name);
						ImGui::TableSetupColumn("Loop", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Action);
						ImGui::TableSetupColumn("RootBone", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Action);
						ImGui::TableSetupColumn("Speed", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Action);
						ImGui::TableSetupColumn("AnimType", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_AnimationType);
						ImGui::TableSetupColumn("EventCount", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_EventCount);
						ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
						ImGui::TableHeadersRow();

						// Sort our data if sort specs have been changed!
						if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
							if (sorts_specs->SpecsDirty)
							{
								MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
								// 내가 세운 기준에 의해 정렬한다.
								if (items.Size > 1)
									qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
								MyItem::s_current_sort_specs = NULL;
								sorts_specs->SpecsDirty = false;
							}

						// Demonstrate using clipper for large vertical lists
						ImGuiListClipper clipper;
						clipper.Begin(items.Size);
						while (clipper.Step())
							for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
							{
								MyItem* item = &items[row_n];
								ImGui::PushID(item->Index);
								ImGui::TableNextRow();
								ImGui::TableNextColumn();

								// 선택된 클립은 파란색으로 처리함.
								if (item->Index == iClipIndex)
								{
									ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.f, 0.4f, 0.7f, 0.5f));
									ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, cell_bg_color);
								}

								// Index
								ImGui::Text(CMethod::Convert_Number_S(item->Index).c_str());
								ImGui::TableNextColumn();

								// Clip의 이름 선택하기
								if (ImGui::Selectable(item->Name))
								{
									iClipIndex = item->Index;
									m_bIsClipUpdate = true;
									m_bIsEventUpdate = true;
								}
								ImGui::TableNextColumn();

								// 메인 애니메이션 이름
								ImGui::Text(item->MainAnim);
								ImGui::TableNextColumn();

								// 서브 애니메이션 이름
								ImGui::Text(item->SubAnim);
								ImGui::TableNextColumn();

								// Loop 여부
								_bool isLoop = item->Loop;
								ImGui::SetNextItemWidth(-FLT_MIN);
								ImGui::PushID(0);
								if (ImGui::Checkbox("", &isLoop))
								{
									item->Loop = isLoop;
									m_ClipList[item->Index]->Set_Loop(isLoop);
								}
								ImGui::PopID();

								ImGui::TableNextColumn();
								// RootBone 여부
								ImGui::PushID(1);
								_bool isRootBone = item->RootBone;
								ImGui::SetNextItemWidth(-FLT_MIN);
								if (ImGui::Checkbox("", &isRootBone))
								{
									item->RootBone = isRootBone;
									m_ClipList[item->Index]->Set_RootBone(isRootBone);
								}
								ImGui::TableNextColumn();
								ImGui::PopID();

								// 애니메이션 속도
								if (ImGui::Selectable(CMethod::Convert_Number_S(item->fAnimSpeed, 1).c_str()))
									ImGui::OpenPopup("AnimSpeed");

								if (ImGui::BeginPopup("AnimSpeed"))
								{
									ImGui::Text("Adjust AnimSpeed");
									ImGui::SameLine();
									if (ImGui::SmallButton("Reset"))
									{
										m_ClipList[item->Index]->Set_AnimationSpeed(1.f);
										m_bIsClipUpdate = true;
									}
									ImGui::Separator();

									_float fSpeed = item->fAnimSpeed;
									if (ImGui::DragFloat("##15", &fSpeed, 0.1f))
									{
										m_ClipList[item->Index]->Set_AnimationSpeed(fSpeed);
										m_bIsClipUpdate = true;
									}
									ImGui::EndPopup();
								}
								ImGui::TableNextColumn();

								char AnimationType[MAX_PATH] = "##";

								strcat_s(AnimationType, item->AnimationType);
								// 애니메이션의 타입
								if (ImGui::Selectable(item->AnimationType))
									ImGui::OpenPopup("AnimType");

								if (ImGui::BeginPopup("AnimType"))
								{
									ImGui::Text("Choose AnimType");
									ImGui::SameLine();
									if (ImGui::SmallButton(u8"지우기"))
									{
										m_ClipList[item->Index]->Set_AnimationType(_string(""));
										m_bIsClipUpdate = true;
									}

									char strAnimType[MAX_PATH] = "";
									
									strcpy_s(strAnimType, m_strAnimType.c_str());

									ImGui::Text(u8"타입 입력창은 되도록 너무 많이 사용하지 마세요");
									
									// 콜라이더 타입 인풋창
									if(ImGui::InputText("##AnimTypeInput", strAnimType, sizeof(_char) * MAX_PATH))
										m_strAnimType = strAnimType;	

									ImGui::SameLine();

									// 콜라이더 타입을 넣는다.
									if (true == ImGui::SmallButton("Input"))
									{
										m_ClipList[item->Index]->Set_AnimationType(strAnimType);																		
										m_bIsClipUpdate = true;
									}

									ImGui::Separator();

									for (_uint i = 0; i < IM_ARRAYSIZE(AnimTypes); ++i)
									{
										if (ImGui::Selectable(AnimTypes[i]))
										{
											m_ClipList[item->Index]->Set_AnimationType(AnimTypes[i]);
											m_bIsClipUpdate = true;
										}
									}
									ImGui::EndPopup();
								}
								ImGui::TableNextColumn();

								// 이벤트 개수
								ImGui::Text(CMethod::Convert_Number_S(item->EventCount).c_str());

								// 애니메이션 클립 리스트를 업데이트한다.
								m_pAnimatorCom->Set_AnimationClipList(m_ClipList);

								ImGui::PopID();
							}
						ImGui::EndTable();
					}
					ImGui::Text("Selected Clip : ");

					ImGui::SameLine();

					if (-1 != iClipIndex)
						ImGui::Text(m_ClipList[iClipIndex]->Get_Name().c_str());
					else
						ImGui::Text("Please Select Clip");

					ImGui::SameLine();

					if (ImGui::Button("Delete"))
					{
						if (-1 != iClipIndex)
						{
							Delete_AnimationClip(m_ClipList[iClipIndex]->Get_Name());
							iClipIndex = -1;
							m_ClipList = m_pAnimatorCom->Get_AnimationClipList();
							m_bIsClipUpdate = true;
						}
					}

					// 골라놓은 클립의 이름을 현재 클립으로 설정한다.
					if (ImGui::Button("Set_Clip"))
						if (-1 != iClipIndex)
							m_pAnimatorCom->Set_CurrentAnimationClip(m_pAnimatorCom->Get_AnimationClip(items[iClipIndex].Name));

					ImGui::SameLine();

					if (ImGui::Button("Play"))
						m_pAnimatorCom->Set_Play(true);

					if (nullptr != m_pAnimObject)
					{
						_vector vPos = m_pAnimObject->Get_Transform()->Get_Pos();

						ImGui::Text("Test Model Pos [ x: ");
						ImGui::SameLine();
						ImGui::Text(CMethod::Convert_Number_S(XMVectorGetX(vPos), 2).c_str());
						ImGui::SameLine();
						ImGui::Text(" / y: ");
						ImGui::SameLine();
						ImGui::SameLine();
						ImGui::Text(CMethod::Convert_Number_S(XMVectorGetY(vPos), 2).c_str());
						ImGui::SameLine();
						ImGui::Text(" / z: ");
						ImGui::SameLine();
						ImGui::Text(CMethod::Convert_Number_S(XMVectorGetZ(vPos), 2).c_str());
						ImGui::SameLine();
						ImGui::Text(" ]");

						_float4 vRootRight = CMethod::XMMatrixGetFloat4(m_pAnimatorCom->Get_Model()->
							Get_HierarchyNodes("Root")->Get_CombinedTransformationMatrix(), MATROW_RIGHT);

						_float4 vRootUp = CMethod::XMMatrixGetFloat4(m_pAnimatorCom->Get_Model()->
							Get_HierarchyNodes("Root")->Get_CombinedTransformationMatrix(), MATROW_UP);

						_float4 vRootLook = CMethod::XMMatrixGetFloat4(m_pAnimatorCom->Get_Model()->
							Get_HierarchyNodes("Root")->Get_CombinedTransformationMatrix(), MATROW_LOOK);

						ImGui::Text("Root Bone Look [ x: ");
						ImGui::SameLine();
						ImGui::Text(CMethod::Convert_Number_S(vRootRight.x, 2).c_str());
						ImGui::SameLine();
						ImGui::Text(" / y: ");
						ImGui::SameLine();
						ImGui::SameLine();
						ImGui::Text(CMethod::Convert_Number_S(vRootUp.y, 2).c_str());
						ImGui::SameLine();
						ImGui::Text(" / z: ");
						ImGui::SameLine();
						ImGui::Text(CMethod::Convert_Number_S(vRootLook.z, 2).c_str());
						ImGui::SameLine();
						ImGui::Text(" ]");

						
						_float4 vRootMatrixRight = CMethod::XMMatrixGetFloat4(m_pAnimatorCom->Get_RootBoneMatrix(), MATROW_RIGHT);
						_float4 vRootMatrixUp = CMethod::XMMatrixGetFloat4(m_pAnimatorCom->Get_RootBoneMatrix(), MATROW_UP);
						_float4 vRootMatrixLook = CMethod::XMMatrixGetFloat4(m_pAnimatorCom->Get_RootBoneMatrix(), MATROW_LOOK);

						ImGui::Text("Root Matrix [ x: ");
						ImGui::SameLine();
						ImGui::Text(CMethod::Convert_Number_S(vRootMatrixRight.x, 2).c_str());
						ImGui::SameLine();
						ImGui::Text(" / y: ");
						ImGui::SameLine();
						ImGui::SameLine();
						ImGui::Text(CMethod::Convert_Number_S(vRootMatrixUp.y, 2).c_str());
						ImGui::SameLine();
						ImGui::Text(" / z: ");
						ImGui::SameLine();
						ImGui::Text(CMethod::Convert_Number_S(vRootMatrixLook.z, 2).c_str());
						ImGui::SameLine();
						ImGui::Text(" ]");
					}

#pragma region 애니메이터가 들고있는 뼈 정보들
					if (ImGui::TreeNode("Animator WeaponList"))
					{
						static _int iWeaponBoneIndex = -1;

						vector<CHierarchyNode*> weaponBoneList = m_pAnimatorCom->Get_WeaponBoneList();

						if (-1 != iWeaponBoneIndex)
						{
							_string strWeaponBone = "Current Bone : ";
							strWeaponBone += weaponBoneList[iWeaponBoneIndex]->Get_Name();
							ImGui::Text(strWeaponBone.c_str());
						}

						for (int i = 0; i < m_pAnimatorCom->Get_WeaponBoneList().size(); ++i)
						{
							if (ImGui::Selectable(weaponBoneList[i]->Get_Name().c_str()))
								iWeaponBoneIndex = i;
						}

						if (ImGui::Button("Remove WeaponBone"))
						{
							if (-1 != iWeaponBoneIndex)
							{
								m_pAnimatorCom->Remove_WeaponBone(iWeaponBoneIndex);
								iWeaponBoneIndex = -1;
							}
						}
						ImGui::TreePop();
					}
#pragma endregion

#pragma region 현재 선택된 클립의 이벤트 목록
					if (-1 != iClipIndex)
					{
						if (ImGui::TreeNode("Current Clip Events"))
						{
							static _int iSelectEvent = -1;

							// 애니메이터 컴포넌트가 갖고있는 Test Clip을 받아온다.
							CAnimationClip* pClip = m_pAnimatorCom->Get_AnimationClip(items[iClipIndex].Name);
							if (nullptr == pClip)
								return;

							CModel* pModel = m_pAnimatorCom->Get_Model();
							if (nullptr == pModel)
								return;

							// 이벤트 목록을 받아온다.
							vector<CAnimationClip::ANIMEVENTDESC> eventList = pClip->Get_EventList();

							const int eventColumn_COUNT = 5;
							// 현재 클립이 가진 이벤트의 개수
							string strEventSize = "All Event Count : ";
							strEventSize += CMethod::Convert_Number_S((_uint)eventList.size());
							ImGui::Text(strEventSize.c_str());

							ImGui::SameLine();

							if (ImGui::Button("Add Event"))
							{
								CAnimationClip::ANIMEVENTDESC animDesc;
								// 현재 Ratio를 담는다.
								animDesc.dStartRatio = pClip->Get_Ratio();
								// 이벤트 리스트를 push_back 한다.
								eventList.push_back(animDesc);
								m_bIsClipUpdate = true;
								m_bIsEventUpdate = true;
							}

							ImGui::SameLine();

							if (ImGui::Button("Remove Event"))
							{
								if (iSelectEvent <= eventList.size())
								{
									// 리스트에서 지운다.
									eventList.erase(eventList.begin() + iSelectEvent);

									// -1로 만들어준다.
									iSelectEvent = -1;
									m_bIsClipUpdate = true;
									m_bIsEventUpdate = true;
								}
							}

							static ImVector<MyItem> eventItems;
							// 아이템이 비어있다면 아이템을 만들어준다.
							if (eventItems.Size == 0 || true == m_bIsEventUpdate || pClip->Get_EventList().size() != eventItems.Size)
							{
								// 이벤트 리스트의 사이즈로 사이즈를 재설정한다.
								eventItems.resize((_int)eventList.size(), MyItem());
								for (int n = 0; n < eventItems.Size; n++)
								{
									MyItem& item = eventItems[n];
									// 애니메이션 클립의 인덱스 정보 기입
									item.Index = n;
									// ( ) 안에 있는 글자만 받아오기
									iStart = (_int)eventList[n].strName.find("(", 0) + 1;
									iEnd = (_int)eventList[n].strName.find(")", 0) - iStart;
									_string szEventName = eventList[n].strName.substr(iStart, iEnd);
									// 애니메이션 클립의 이름 기입
									strcpy_s(item.Name, szEventName.c_str());
									// 이벤트 타입
									item.iEventType = (_uint)eventList[n].tEventType;
									// 이벤트 시작 비율
									item.dStartRatio = eventList[n].dStartRatio;
									// 이벤트 종료 비율
									item.dEndRatio = eventList[n].dEndRatio;
									// 이벤트 종료 시간
									item.dEndTime = eventList[n].dEndTime;
									// 스폰 위치 
									item.vEventPos = eventList[n].vEventPos;
									// 뼈의 이름등을 담는 이벤트 텍스트
									strcpy_s(item.EventText, eventList[n].strEventText.c_str());
								}

								m_bIsEventUpdate = false;
							}

							static const char* eventTypes[] =
							{
								"Effect","UI", "Sound", "Collider", "WeaponBone", "Camera", "Hide", "SpeedSection","BoneTransform","Gravity","TimeCheck","Trail","Not_Selected"
							};

							// Options
							static ImGuiTableFlags flags =
								ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
								| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
								| ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX;

							if (ImGui::BeginTable("Event_Table_Animator", eventColumn_COUNT, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), 0.0f))
							{
								// - ImGuiTableColumnFlags_DefaultSort
								// - ImGuiTableColumnFlags_NoSort / ImGuiTableColumnFlags_NoSortAscending / ImGuiTableColumnFlags_NoSortDescending
								// - ImGuiTableColumnFlags_PreferSortAscending / ImGuiTableColumnFlags_PreferSortDescending
								ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Index);
								ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoSort, 0.0f, MyItemColumnID_Name);
								ImGui::TableSetupColumn("EventText", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);
								ImGui::TableSetupColumn("EventType", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_EventType);
								ImGui::TableSetupColumn("Values", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);
								ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
								ImGui::TableHeadersRow();

								// Sort our data if sort specs have been changed!
								if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
									if (sorts_specs->SpecsDirty)
									{
										MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
										// 내가 세운 기준에 의해 정렬한다.
										if (eventItems.Size > 1)
											qsort(&eventItems[0], (size_t)eventItems.Size, sizeof(eventItems[0]), MyItem::CompareWithSortSpecs);
										MyItem::s_current_sort_specs = NULL;
										sorts_specs->SpecsDirty = false;
									}

								// Demonstrate using clipper for large vertical lists
								ImGuiListClipper clipper;
								clipper.Begin(eventItems.Size);
								while (clipper.Step())
									for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
									{
										MyItem* item = &eventItems[row_n];
										ImGui::PushID(item->Index);
										ImGui::TableNextRow();
										ImGui::TableNextColumn();

										// 선택됐다면 파란색으로 그린다.
										if (iSelectEvent == item->Index)
										{
											ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.3f, 0.5f));
											ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, cell_bg_color);
										}

										// Index
										if (ImGui::Selectable(CMethod::Convert_Number_S(item->Index).c_str()))
										{
											iSelectEvent = item->Index;
										}
										ImGui::TableNextColumn();
										// Name
										char strText[MAX_PATH] = "";
										strcpy_s(strText, item->Name);
										ImGui::SetNextItemWidth(-FLT_MIN);
										if (ImGui::InputText("##EventName", strText, sizeof(char) * MAX_PATH))
										{
											strcpy_s(item->Name, strText);
											eventList[item->Index].strName = strText;
										}
										ImGui::TableNextColumn();

										static _string strInput;
										static vector<CHierarchyNode*> BoneList;
										static vector<_string> ColliderList;

										const char* colliderTypes[] =
										{
											"HIT_FRONT","HIT_LEFT","HIT_RIGHT","HIT_UP","STRIKE","BLOW",
											"SPIN","HIT_UPPER","HIT_DOWN","SKILL_1","SKILL_2","SKILL_3",
											"PUSH","THROW","ULTIMATE","HIT_AWAKE"
										};

										if (ImGui::Selectable(item->EventText))
											ImGui::OpenPopup("##EventText");

										if (ImGui::BeginPopup("##EventText"))
										{
											if (3 == item->iEventType)
												ImGui::Text(u8"콜라이더 타입을 골라주세요");
											else
												ImGui::Text(u8"본 이름을 입력하세요");

											if (ImGui::SmallButton("Reset"))
											{
												eventList[item->Index].strEventText = "";
												m_bIsEventUpdate = true;
											}


											_char INPUT[MAX_PATH] = "";
											strcpy_s(INPUT, strInput.c_str());
											_bool bInput = false;
											bInput = ImGui::InputText("FindText", INPUT, sizeof(_char) * MAX_PATH);
											strInput = INPUT;

											// 콜라이더인 경우
											if (3 == item->iEventType)
											{
												if (!strcmp(strInput.c_str(), ""))
												{
													for (auto& iter : colliderTypes)
													{
														if (ImGui::Selectable(iter))
														{
															eventList[item->Index].strEventText = iter;
															m_bIsClipUpdate = true;
															m_bIsEventUpdate = true;
														}
													}
												}
												else if (true == bInput)
												{
													ColliderList.clear();
													for (auto& iter : colliderTypes)
													{
														if (CMethod::Is_Same_Text(iter, strInput.c_str()))
															ColliderList.push_back(iter);
													}
												}
												else
												{
													for (auto& iter : ColliderList)
													{
														if (ImGui::Selectable(iter.c_str()))
														{
															eventList[item->Index].strEventText = iter;
															m_bIsClipUpdate = true;
															m_bIsEventUpdate = true;
														}
													}
												}
											}
											// 사운드인 경우
											else if (2 == item->iEventType)
											{
												if (!strcmp(strInput.c_str(), ""))
												{
													for (auto& iter : m_SoundNameList)
													{
														if (ImGui::Selectable(iter.c_str()))
														{
															eventList[item->Index].strEventText = iter;
															m_bIsClipUpdate = true;
															m_bIsEventUpdate = true;
														}
													}
												}
												else if (true == bInput)
												{
													m_ResearchSoundList.clear();
													for (auto& iter : m_SoundNameList)
													{
														if (CMethod::Is_Same_Text(iter, strInput))
															m_ResearchSoundList.push_back(iter);
													}
												}
												else
												{
													for (auto& iter : m_ResearchSoundList)
													{
														if (ImGui::Selectable(iter.c_str()))
														{
															eventList[item->Index].strEventText = iter;
															m_bIsClipUpdate = true;
															m_bIsEventUpdate = true;
														}
													}
												}												
											}
											else
											{
												if (!strcmp(strInput.c_str(), ""))
												{
													for (auto& iter : pModel->Get_HierachyNodeList())
													{
														if (ImGui::Selectable(iter->Get_Name().c_str()))
														{
															eventList[item->Index].strEventText = iter->Get_Name();
															m_bIsClipUpdate = true;
															m_bIsEventUpdate = true;
														}
													}
												}
												else if (true == bInput)
												{
													BoneList.clear();
													for (auto& iter : pModel->Get_HierachyNodeList())
													{
														if (CMethod::Is_Same_Text(iter->Get_Name().c_str(), strInput.c_str()))
															BoneList.push_back(iter);
													}
												}
												else
												{
													for (auto& iter : BoneList)
													{
														if (ImGui::Selectable(iter->Get_Name().c_str()))
														{
															eventList[item->Index].strEventText = iter->Get_Name();
															m_bIsClipUpdate = true;
															m_bIsEventUpdate = true;
														}
													}
												}
											}

											ImGui::EndPopup();
										}
										ImGui::TableNextColumn();

										// 4번째줄인 경우 ( 버그 있음 ) NoneSelect로 만든다.
										if (4 == item->Index)
										{
											if (IM_ARRAYSIZE(eventTypes) - 1 != item->iEventType)
											{
												eventList[item->Index].tEventType = CAnimationClip::ANIMEVENTTYPE(IM_ARRAYSIZE(eventTypes) - 1);
												m_bIsEventUpdate = true;
											}
										}

										// Event
										if (ImGui::Selectable(eventTypes[item->iEventType]))
											ImGui::OpenPopup("EventType");

										if (ImGui::BeginPopup("EventType"))
										{
											for (int i = 0; i < IM_ARRAYSIZE(eventTypes); ++i)
											{
												if (ImGui::Selectable(eventTypes[i]))
												{
													eventList[item->Index].tEventType = (CAnimationClip::ANIMEVENTTYPE)i;
													item->iEventType = i;
													m_bIsClipUpdate = true;
													m_bIsEventUpdate = true;
												}
											}
											ImGui::EndPopup();
										}
										ImGui::TableNextColumn();

										// Value 모음
										if (ImGui::Selectable("Values"))
											ImGui::OpenPopup("Values");

										if (ImGui::BeginPopup("Values"))
										{
											ImGui::Text(u8"안쓰는 값은 냅두면 됨");

											ImGui::SameLine();

											if (ImGui::SmallButton("Reset"))
											{
												eventList[item->Index].dStartRatio = pClip->Get_Ratio();
												eventList[item->Index].dEndRatio = 0.0;
												eventList[item->Index].dEndTime = 0.0;
												eventList[item->Index].vEventPos = _float4(0.f, 0.f, 0.f, 1.f);
												eventList[item->Index].fValue = 0.f;
												eventList[item->Index].fAddValue = 0.f;
												m_bIsEventUpdate = true;
											}

											ImGui::Separator();

											ImGui::Text(u8"시작지점");

											_float fRatio = (_float)eventList[item->Index].dStartRatio;
											if (ImGui::DragFloat("##StartRatio", &fRatio, 0.01f))
											{
												eventList[item->Index].dStartRatio = fRatio;
												m_bIsEventUpdate = true;
											}

											ImGui::SameLine();

											if (ImGui::Button("CurTime"))
											{
												eventList[item->Index].dStartRatio = pClip->Get_Ratio();
												m_bIsEventUpdate = true;
											}

											ImGui::Text(u8"종료지점");

											_float fEndRatio = (_float)eventList[item->Index].dEndRatio;
											if (ImGui::DragFloat("##EndRatio", &fEndRatio, 0.01f))
											{
												eventList[item->Index].dEndRatio = fEndRatio;
												m_bIsEventUpdate = true;
											}

											ImGui::Text(u8"종료시간");

											_float fEndTime = (_float)eventList[item->Index].dEndTime;
											if (ImGui::DragFloat("##EndTime", &fEndTime, 0.01f))
											{
												eventList[item->Index].dEndTime = (_double)fEndTime;
												m_bIsEventUpdate = true;
											}

											switch (item->iEventType)
											{
											case CLIPEVENTTYPE::EVENTTYPE_COLLIDER:
												ImGui::Text(u8"콜라이더의 위치값입니다.");
												break;
											case CLIPEVENTTYPE::EVENTTYPE_WEAPONBONE:
												ImGui::Text(u8"무기 뼈 인덱스");
												break;
											case CLIPEVENTTYPE::EVENTTYPE_SPEEDSECTION:
												ImGui::Text(u8"애니메이션 재생 속도");
												break;
											case CLIPEVENTTYPE::EVENTTYPE_BONETRANSFORM:
												ImGui::Text(u8"x = Right, y = Up, z = Look 1 또는 -1 넣으시오");
												break;
											default:
												ImGui::Text(u8"이벤트의 위치 값");
												break;
											}

											_float4 vPos = eventList[item->Index].vEventPos;
											_float vEventPos[4] = { vPos.x, vPos.y, vPos.z, vPos.w };
											if (ImGui::DragFloat4("##SpawnPos", vEventPos, 0.01f))
											{
												memcpy(&eventList[item->Index].vEventPos, &vEventPos, sizeof(_float4));
												m_bIsEventUpdate = true;
											}

											switch (item->iEventType)
											{
											case CLIPEVENTTYPE::EVENTTYPE_COLLIDER:
												ImGui::Text(u8"x = 데미지 , y = 멈출 시간 , z = 쉐이크 시간 w = 쉐이크 세기");
												break;
											default:
												ImGui::Text(u8"이벤트의 Dir값");
												break;
											}

											_float4 vDir = eventList[item->Index].vEventDir;
											_float vEventDir[4] = { vDir.x, vDir.y, vDir.z, vDir.w };
											if (ImGui::DragFloat4("##EventDir", vEventDir, 0.01f))
											{
												memcpy(&eventList[item->Index].vEventDir, &vEventDir, sizeof(_float4));
												m_bIsEventUpdate = true;
											}

											switch (item->iEventType)
											{
											case CLIPEVENTTYPE::EVENTTYPE_COLLIDER:
												ImGui::Text(u8"콜라이더의 크기입니다.");
												break;
											default:
												ImGui::Text(u8"AddValue값은 필요에 따라 넣어주세요");
												break;
											}
											
											_float4 vValues = eventList[item->Index].vEventValues;
											_float vEventValues[4] = { vValues.x, vValues.y, vValues.z, vValues.w };
											if (ImGui::DragFloat4("##EventValues", vEventValues, 0.01f))
											{
												memcpy(&eventList[item->Index].vEventValues, &vEventValues, sizeof(_float4));
												m_bIsEventUpdate = true;
											}

											switch (item->iEventType)
											{
											case CLIPEVENTTYPE::EVENTTYPE_SOUND:
												ImGui::Text(u8"볼륨의 크기를 0 ~ 1 사이의 값으로 넣어주세요");
												break;
											case CLIPEVENTTYPE::EVENTTYPE_COLLIDER:
												ImGui::Text(u8"넉백 수치를 입력해주세요");
												break;

											case CLIPEVENTTYPE::EVENTTYPE_WEAPONBONE:
												ImGui::Text(u8"무기 뼈 인덱스를 넣어주세요");
												break;

											case CLIPEVENTTYPE::EVENTTYPE_SPEEDSECTION:
												ImGui::Text(u8"애니메이션 재생 속도를 입력하세요");
												break;

											case CLIPEVENTTYPE::EVENTTYPE_BONETRANSFORM:
												ImGui::Text(u8"이동값을 넣어주세요");
												break;

											case CLIPEVENTTYPE::EVENTTYPE_TRAIL:
												ImGui::Text(u8"트레일 시작값을 넣어주세요");
												break;

											default:
												ImGui::Text(u8"Value값을 넣어주세요");
												break;
											}
											_float fValue = eventList[item->Index].fValue;
											if (ImGui::DragFloat("##EventValue", &fValue, 0.01f))
											{
												eventList[item->Index].fValue = fValue;
												m_bIsEventUpdate = true;
											}

											switch (item->iEventType)
											{
											case CLIPEVENTTYPE::EVENTTYPE_COLLIDER:
												ImGui::Text(u8"포물선의 높이를 입력해주세요");
												break;

											case CLIPEVENTTYPE::EVENTTYPE_TRAIL:
												ImGui::Text(u8"트레일 끝값을 넣어주세요");
												break;

											default:
												ImGui::Text(u8"AddValue값은 필요에 따라 넣어주세요");
												break;
											}
											_float fAddValue = eventList[item->Index].fAddValue;
											if (ImGui::DragFloat("##EventAddValue", &fAddValue, 0.01f))
											{
												eventList[item->Index].fAddValue = fAddValue;
												m_bIsEventUpdate = true;
											}
											ImGui::EndPopup();
										}
										ImGui::PopID();
									}
								// 이벤트를 세팅한다.
								pClip->Set_Event(eventList);
								ImGui::EndTable();
							}
							ImGui::TreePop();
						}
					}
#pragma endregion
					ImGui::TreePop();
				}

				// 트랜지션 리스트.
				if (ImGui::TreeNode("Transition List"))
				{
					vector<CAnimator::TRANSITION>transitionList = m_pAnimatorCom->Get_TransitionList();

					vector<_string> EnterNameList;
					for (auto& clip : m_ClipList)
						EnterNameList.push_back(clip->Get_Name());

					const int COLUMNS_COUNT = 5;
					// 트랜지션 개수를 글자로 쓰기위한 string 변수
					string strMeshSize = "All Transitions Count : ";
					strMeshSize += CMethod::Convert_Number_S((_uint)transitionList.size());
					ImGui::Text(strMeshSize.c_str());

					ImGui::SameLine();

					static _bool isTransitionChange = false;

					if (ImGui::Button("Add Transition"))
					{
						m_pAnimatorCom->Make_Transition();
						transitionList = m_pAnimatorCom->Get_TransitionList();
						isTransitionChange = true;
					}

					static ImVector<MyItem> items;
					_int iStart = 0;
					_int iEnd = 0;

					if (items.Size == 0 || true == isTransitionChange || items.Size != transitionList.size())
					{
						// 사이즈를 재설정한다.
						items.resize((_int)transitionList.size(), MyItem());
						for (int n = 0; n < items.Size; n++)
						{
							MyItem& item = items[n];
							// 인덱스 정보 기입
							item.Index = n;

							// EnterState
							item.EnterState = (_int)transitionList[n].tEnterState;

							_string szCurAnimName;
							// ( ) 안에 있는 글자만 받아오기
							if (nullptr == transitionList[n].pEnterClip)
								szCurAnimName = "None";
							else
							{
								iStart = (_int)transitionList[n].pEnterClip->Get_Name().find("(", 0) + 1;
								iEnd = (_int)transitionList[n].pEnterClip->Get_Name().find(")", 0) - iStart;
								szCurAnimName = transitionList[n].pEnterClip->Get_Name().substr(iStart, iEnd);
							}
							// 다음 클립의 이름 기입
							strcpy_s(item.EnterAnim, szCurAnimName.c_str());

							_string szNextAnimName;
							// ( ) 안에 있는 글자만 받아오기
							if (nullptr == transitionList[n].pExitClip)
								szNextAnimName = "None";
							else
							{
								iStart = (_int)transitionList[n].pExitClip->Get_Name().find("(", 0) + 1;
								iEnd = (_int)transitionList[n].pExitClip->Get_Name().find(")", 0) - iStart;
								szNextAnimName = transitionList[n].pExitClip->Get_Name().substr(iStart, iEnd);
							}
							// 다음 클립의 이름 기입
							strcpy_s(item.ExitAnim, szNextAnimName.c_str());

							// Enter Clip의 보간 시작 비율
							item.EnterRatio = transitionList[n].dEnterRatio;

							// Exit Clip의 보간 시작 비율
							item.ExitRatio = transitionList[n].dExitRatio;

							// 트리거를 인식하는 비율
							item.TriggerRatio = transitionList[n].InputTrigger.dAnimRatio;

							// 트리거의 이름
							strcpy_s(item.Trigger, transitionList[n].InputTrigger.strInput.c_str());

							// 보간 속도
							item.LerpSpeed = (_float)transitionList[n].dLerpSpeed;
						}
						isTransitionChange = false;
					}

					// Options
					static ImGuiTableFlags flags =
						ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
						| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
						| ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX;

					if (ImGui::BeginTable("Transition_Table_Sorting", COLUMNS_COUNT, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 15), 0.0f))
					{
						// - ImGuiTableColumnFlags_DefaultSort
						// - ImGuiTableColumnFlags_NoSort / ImGuiTableColumnFlags_NoSortAscending / ImGuiTableColumnFlags_NoSortDescending
						// - ImGuiTableColumnFlags_PreferSortAscending / ImGuiTableColumnFlags_PreferSortDescending
						ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.f, MyItemColumnID_Index);
						ImGui::TableSetupColumn("EnterState", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_EnterState);
						ImGui::TableSetupColumn("EnterAnim", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_EnterClip);
						ImGui::TableSetupColumn("ExitAnim", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_ExitClip);
						ImGui::TableSetupColumn("Trigger", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Trigger);
						ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
						ImGui::TableHeadersRow();

						// Sort our data if sort specs have been changed!
						if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
							if (sorts_specs->SpecsDirty)
							{
								MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
								// 내가 세운 기준에 의해 정렬한다.
								if (items.Size > 1)
									qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
								MyItem::s_current_sort_specs = NULL;
								sorts_specs->SpecsDirty = false;
							}

						static _bool isDelete = false;
						static _int	 iDeleteIndex = -1;

						// Demonstrate using clipper for large vertical lists
						ImGuiListClipper clipper;
						clipper.Begin(items.Size);
						while (clipper.Step())
							for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
							{
								MyItem* item = &items[row_n];
								ImGui::PushID(item->Index);
								ImGui::TableNextRow();
								ImGui::TableNextColumn();

								if (-1 != m_iTransitionIndex)
								{
									if (item->Index == m_iTransitionIndex)
									{
										ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.f, 0.4f, 0.7f, 0.5f));
										ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, cell_bg_color);
									}
								}

								// Index								
								if (ImGui::Selectable(CMethod::Convert_Number_S(item->Index).c_str()))
									m_iTransitionIndex = item->Index;

								ImGui::TableNextColumn();

								// EnterState
								const char* enterStates[] = { "ENTRY", "ANYSTATE", "DEFAULT", "NONE_SELECT" };

								int iSelectedState = item->EnterState;

								if (ImGui::Selectable(enterStates[iSelectedState]))
									ImGui::OpenPopup("EnterState");

								if (ImGui::BeginPopup("EnterState"))
								{
									ImGui::Text("Select EnterState");
									ImGui::Separator();
									for (int i = 0; i < IM_ARRAYSIZE(enterStates); i++)
										if (ImGui::Selectable(enterStates[i]))
										{
											item->EnterState = i;
											transitionList[item->Index].tEnterState = CAnimator::ENTERSTATE(i);
										}
									ImGui::EndPopup();
								}
								ImGui::TableNextColumn();


								ImGui::PushID("##1");
								// EnterAnimation 선택하기
								if (ImGui::Selectable(item->EnterAnim))
									ImGui::OpenPopup("EnterClip");

								if (ImGui::BeginPopup("EnterClip"))
								{
									ImGui::Text("Select EnterClip");

									ImGui::SameLine();

									if (ImGui::SmallButton("Unselect"))
									{
										transitionList[item->Index].pEnterClip = nullptr;
										strcpy_s(item->EnterAnim, "None");
									}

									static _string strText = "";
									// 애니메이션 클립을 담을 벡터 리스트
									static vector<CAnimationClip*> SaveToFindClipList;

									_char INPUT[MAX_PATH] = "";
									strcpy_s(INPUT, strText.c_str());
									_bool bInput = false;
									bInput = ImGui::InputText("Anim", INPUT, sizeof(_char) * MAX_PATH);
									strText = INPUT;

									ImGui::Separator();

									if (!strcmp(strText.c_str(), ""))
									{
										for (int i = 0; i < m_ClipList.size(); ++i)
										{
											if (ImGui::Selectable(m_ClipList[i]->Get_Name().c_str()))
											{
												_int iStart = (_int)m_ClipList[i]->Get_Name().find("(", 0) + 1;
												_int iEnd = (_int)m_ClipList[i]->Get_Name().find(")", 0) - iStart;
												string strClipName = m_ClipList[i]->Get_Name().substr(iStart, iEnd);

												transitionList[item->Index].pEnterClip = m_ClipList[i];
												strcpy_s(item->EnterAnim, strClipName.c_str());
											}
										}
									}
									else if (true == bInput)
									{
										SaveToFindClipList.clear();
										for (auto& iter : m_ClipList)
										{
											if (CMethod::Is_Same_Text(iter->Get_Name().c_str(), strText.c_str()))
												SaveToFindClipList.push_back(iter);
										}
									}
									else
									{
										for (int i = 0; i < SaveToFindClipList.size(); ++i)
										{
											if (ImGui::Selectable(SaveToFindClipList[i]->Get_Name().c_str()))
											{
												_int iStart = (_int)SaveToFindClipList[i]->Get_Name().find("(", 0) + 1;
												_int iEnd = (_int)SaveToFindClipList[i]->Get_Name().find(")", 0) - iStart;
												string strClipName = SaveToFindClipList[i]->Get_Name().substr(iStart, iEnd);

												transitionList[item->Index].pEnterClip = SaveToFindClipList[i];
												strcpy_s(item->EnterAnim, strClipName.c_str());
											}
										}
									}
									ImGui::EndPopup();
								}
								ImGui::PopID();
								ImGui::TableNextColumn();

								// ExitClip 인덱스값 구하기

								ImGui::PushID("##2");
								// ExitAnimation 선택하기
								if (ImGui::Selectable(item->ExitAnim))
								{
									ImGui::OpenPopup("ExitClip");
								}

								if (ImGui::BeginPopup("ExitClip"))
								{
									ImGui::Text("Select ExitClip");

									ImGui::SameLine();

									if (ImGui::SmallButton("Unselect"))
									{
										transitionList[item->Index].pExitClip = nullptr;
										strcpy_s(item->ExitAnim, "None");
									}

									static _string strText = "";
									// 애니메이션 클립을 담을 벡터 리스트
									static vector<CAnimationClip*> SaveToFindClipList;

									_char INPUT[MAX_PATH] = "";
									strcpy_s(INPUT, strText.c_str());
									_bool bInput = false;
									bInput = ImGui::InputText("Anim", INPUT, sizeof(_char) * MAX_PATH);
									strText = INPUT;

									ImGui::Separator();

									if (!strcmp(strText.c_str(), ""))
									{
										for (int i = 0; i < m_ClipList.size(); ++i)
										{
											if (ImGui::Selectable(m_ClipList[i]->Get_Name().c_str()))
											{
												_int iStart = (_int)m_ClipList[i]->Get_Name().find("(", 0) + 1;
												_int iEnd = (_int)m_ClipList[i]->Get_Name().find(")", 0) - iStart;
												string strClipName = m_ClipList[i]->Get_Name().substr(iStart, iEnd);

												transitionList[item->Index].pExitClip = m_ClipList[i];
												strcpy_s(item->ExitAnim, strClipName.c_str());
											}
										}
									}
									else if (true == bInput)
									{
										SaveToFindClipList.clear();
										for (auto& iter : m_ClipList)
										{
											if (CMethod::Is_Same_Text(iter->Get_Name().c_str(), strText.c_str()))
												SaveToFindClipList.push_back(iter);
										}
									}
									else
									{
										for (int i = 0; i < SaveToFindClipList.size(); ++i)
										{
											if (ImGui::Selectable(SaveToFindClipList[i]->Get_Name().c_str()))
											{
												_int iStart = (_int)SaveToFindClipList[i]->Get_Name().find("(", 0) + 1;
												_int iEnd = (_int)SaveToFindClipList[i]->Get_Name().find(")", 0) - iStart;
												string strClipName = SaveToFindClipList[i]->Get_Name().substr(iStart, iEnd);

												transitionList[item->Index].pExitClip = SaveToFindClipList[i];
												strcpy_s(item->ExitAnim, strClipName.c_str());
											}
										}
									}
									ImGui::EndPopup();
								}
								ImGui::PopID();
								ImGui::TableNextColumn();

								// InputTrigger 값을 채워준다.
								ImGui::SetNextItemWidth(-FLT_MIN);
								if (ImGui::InputText("", item->Trigger, sizeof(item->Trigger)))
								{
									transitionList[item->Index].InputTrigger.strInput = item->Trigger;
								}

								ImGui::PopID();
							}
						ImGui::EndTable();

						if (-1 != m_iTransitionIndex)
						{
							ImGui::Text("Current Clip : ");

							ImGui::SameLine();

							if (nullptr == m_pAnimatorCom->Get_CurrentAnimationClip())
								ImGui::Text("None Clip");
							else
								ImGui::Text(m_pAnimatorCom->Get_CurrentAnimationClip()->Get_Name().c_str());

							ImGui::SameLine();

							// 해당 트랜지션으로 이동한다.
							if (ImGui::Button("Go"))
							{
								if (nullptr != transitionList[m_iTransitionIndex].pEnterClip)
									m_pAnimatorCom->Set_CurrentAnimationClip(transitionList[m_iTransitionIndex].pEnterClip);
							}

							ImGui::SameLine();

							if (ImGui::ArrowButton("##Up", ImGuiDir_Up))
							{
								if (-1 < m_iTransitionIndex - 1)
								{
									_uint iIndex = m_iTransitionIndex;

									CAnimator::TRANSITION transition = transitionList[iIndex];

									transitionList.erase(transitionList.begin() + iIndex);
									transitionList.insert(transitionList.begin() + iIndex - 1, transition);
									if (-1 < m_iTransitionIndex - 1)
										m_iTransitionIndex -= 1;
									isTransitionChange = true;
								}
							}

							ImGui::SameLine();

							if (ImGui::ArrowButton("##Down", ImGuiDir_Down))
							{
								if (transitionList.size() > m_iTransitionIndex + 1)
								{
									_uint iIndex = m_iTransitionIndex;

									CAnimator::TRANSITION transition = transitionList[iIndex];

									transitionList.erase(transitionList.begin() + iIndex);
									transitionList.insert(transitionList.begin() + iIndex + 1, transition);
									if (transitionList.size() > m_iTransitionIndex + 1)
										m_iTransitionIndex += 1;
									isTransitionChange = true;
								}
							}

							// 애니메이션의 진행도를 받아온다.
							_float fRatio = (_float)m_pAnimatorCom->Get_Ratio();
							if (ImGui::SliderFloat("##Animation Progress Ratio", &fRatio, 0.0f, 1.0f))
							{
								// 애니메이션의 진행도를 변경한다.
								static_cast<CAnimViewerModel*>(m_pAnimObject)->Set_ProgressRatio(fRatio);
							}

							ImGui::SameLine();

							_string strAnimSpeed = "Speed : ";
							strAnimSpeed += CMethod::Convert_Number_S(m_pAnimatorCom->Get_AnimationSpeed(), 2);

							ImGui::Text(strAnimSpeed.c_str());

							ImGui::SameLine();

							// 애니메이션 속도값
							if (ImGui::SmallButton("Set"))
							{
								ImGui::OpenPopup("Set");
							}

							if (ImGui::BeginPopup("Set"))
							{
								ImGui::Text("Set Speed");

								ImGui::SameLine();

								if (ImGui::SmallButton("Reset"))
									m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(1.f);

								ImGui::Separator();

								if (ImGui::Selectable("0.5x Speed"))
									m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(0.5f);

								if (ImGui::Selectable("2x Speed"))
									m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(2.f);

								if (ImGui::Selectable("3x Speed"))
									m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(3.f);

								if (ImGui::Selectable("4x Speed"))
									m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(4.f);

								if (ImGui::Selectable("5x Speed"))
									m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(5.f);

								if (ImGui::Selectable("6x Speed"))
									m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(6.f);

								_float fSpeed = m_pAnimatorCom->Get_CurrentAnimationClip()->Get_MainAnimation()->Get_FastValue();

								if (ImGui::DragFloat("Current Speed", &fSpeed, 0.1f, 0.f, 10.f))
									m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(fSpeed);

								m_bIsClipUpdate = true;

								ImGui::EndPopup();
							}

							ImGui::Separator();

							_float fEnterRatio = (_float)transitionList[m_iTransitionIndex].dEnterRatio;
							if (ImGui::DragFloat("EnterRatio", &fEnterRatio, 0.001f, 0.f, 1.f))
								transitionList[m_iTransitionIndex].dEnterRatio = (_double)fEnterRatio;

							_float fExitRatio = (_float)transitionList[m_iTransitionIndex].dExitRatio;
							if (ImGui::DragFloat("ExitRatio", &fExitRatio, 0.001f, 0.f, 1.f))
								transitionList[m_iTransitionIndex].dExitRatio = (_double)fExitRatio;

							_float fChangeRatio = (_float)transitionList[m_iTransitionIndex].dChangeRatio;
							if (ImGui::DragFloat("ChangeRatio", &fChangeRatio, 0.001f, 0.f, 1.f))
								transitionList[m_iTransitionIndex].dChangeRatio = (_double)fChangeRatio;

							ImGui::SameLine();

							_bool isOver = transitionList[m_iTransitionIndex].bIsOverChangeRatio;
							if (ImGui::Checkbox("OverChangeRatio", &isOver))
								transitionList[m_iTransitionIndex].bIsOverChangeRatio = isOver;

							_float fLerpSpeed = (_float)transitionList[m_iTransitionIndex].dLerpSpeed;
							if (ImGui::DragFloat("LerpSpeed", &fLerpSpeed, 0.001f, 0.f, 1.f))
								transitionList[m_iTransitionIndex].dLerpSpeed = (_double)fLerpSpeed;

							_float fInputRatio = (_float)transitionList[m_iTransitionIndex].InputTrigger.dAnimRatio;
							if (ImGui::DragFloat("InputRatio", &fInputRatio, 0.001f, 0.f, 1.f))
								transitionList[m_iTransitionIndex].InputTrigger.dAnimRatio = (_double)fInputRatio;

							ImGui::SameLine();

							_bool isOverRatio = transitionList[m_iTransitionIndex].InputTrigger.bIsOverRatio;
							if (ImGui::Checkbox(u8"이상, 이하", &isOverRatio))
								transitionList[m_iTransitionIndex].InputTrigger.bIsOverRatio = isOverRatio;

							if (ImGui::Button("UnSelect"))
								m_iTransitionIndex = -1;

							ImGui::SameLine();

							if (ImGui::Button("Remove"))
							{
								isDelete = true;
								iDeleteIndex = m_iTransitionIndex;
								m_iTransitionIndex = -1;
							}
						}

						// 지운다는 bool값이 true라면 리스트에서 지운다.
						if (true == isDelete)
						{
							transitionList.erase(transitionList.begin() + iDeleteIndex);
							isDelete = false;
							iDeleteIndex = -1;
							isTransitionChange = true;
						}

						m_pAnimatorCom->Set_TransitionList(transitionList);

						static char szTrigger[MAX_PATH];
						ImGui::SetNextItemWidth(200.f);
						ImGui::InputText("Input Trigger", szTrigger, sizeof(szTrigger));

						ImGui::SameLine();

						// 애니메이터로 Trigger를 전송합니다.
						if (ImGui::Button("Set Trigger"))
							m_pAnimatorCom->Input_Trigger(_string(szTrigger));

					}
					ImGui::TreePop();
				}
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	if (ImGui::CollapsingHeader("SwordViewer_TabBar"))
	{
		Attach_SwordModel();
	}
	if (ImGui::CollapsingHeader("ShelteViewer_TabBar"))
	{
		Attach_ShelteModel();
	}
	// 모델 객체의 트랜스폼을 초기화합니다.
	if (ImGui::Button("Reset Transform"))
	{
		if (nullptr != m_pAnimObject)
		{
			m_pAnimObject->Get_Transform()->Set_Pos(XMVectorZero());
			if (nullptr != m_pAnimatorCom)
				m_pAnimatorCom->Initialize_BeginTransform();
		}
	}
}

HRESULT CAnimViewer_Tool::Create_AnimObject()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pAnimObject = pGameInstance->Clone_GameObject_Add(g_iCurrentLevel, LAYER_ANIMVIEWER, PROTO_GOB_ANIMVIEWERMODEL);

	if (nullptr == m_pAnimObject)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	// 애님 오브젝트의 레퍼런스 카운트를 늘린다.
	Safe_AddRef(m_pAnimObject);

	// 툴용 애니메이터 객체를 하나 만든다
	m_pAnimatorCom = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR);

	// 빈 애니메이션 클립 하나를 생성한다.
	CAnimationClip* pClip = CAnimationClip::Create();

	// 이름을 Test로 지정한다.
	pClip->Set_Name("Test");

	// 애니메이터 컴포넌트에게 애니메이션 클립을 넣어준다.
	if (nullptr != pClip)
		m_pAnimatorCom->Add_AnimationClip(pClip);

	if (nullptr != m_pAnimObject)
		m_pAnimatorCom->Set_Transform(m_pAnimObject->Get_Transform());

	// AnimObject에게 Animator를 넣어준다.
	static_cast<CAnimViewerModel*>(m_pAnimObject)->Set_Animator(m_pAnimatorCom);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CAnimViewer_Tool::Set_Model()
{
	if (nullptr == m_pAnimObject)
		return E_FAIL;

	// 모델 객체를 애니메이션 테스트 모델에 Set 해준다. ( RefCount는 내부에서 증가된다. )
	CModel* pModel = static_cast<CModel*>(m_ModelList[m_iModelIndex]->Clone(m_ModelList[m_iModelIndex]));
	if (FAILED(static_cast<CAnimViewerModel*>(m_pAnimObject)->Set_Model(pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimViewer_Tool::Set_Animation()
{
	if (nullptr == m_pAnimObject)
		return E_FAIL;

	// 선택한 애니메이션 인덱스가 모델리스트의 애니메이션 개수보다 크거나 같다면 리턴한다.
	if (m_iAnimationIndex >= m_pAnimatorCom->Get_Model()->Get_Animations().size())
		return E_FAIL;

	// 애니메이션 테스트 모델이 현재 갖고있는 모델객체의 애니메이션을 받아온다.
	vector<CAnimation*> AnimationList = m_pAnimatorCom->Get_Model()->Get_Animations();

	// 애니메이션 클립의 메인 애니메이션을 바꾼다.
	m_pAnimatorCom->Get_CurrentAnimationClip()->Set_MainAnimation(AnimationList[m_iAnimationIndex]);

	// 뼈 정보를 업데이트한다.
	m_pAnimatorCom->Update_InitBoneState();

	return S_OK;
}

HRESULT CAnimViewer_Tool::Add_AnimationClip()
{
	if (nullptr == m_pAnimatorCom)
		return E_FAIL;

	// () 안에 들어있는 이름만 애니메이션의 이름으로 받아온다.
	_int iStart = (_int)m_pAnimatorCom->Get_CurrentAnimationClip()->Get_MainAnimationName().find("(", 0) + 1;
	_int iEnd = (_int)m_pAnimatorCom->Get_CurrentAnimationClip()->Get_MainAnimationName().find(")", 0) - iStart;
	_string szClipName = m_pAnimatorCom->Get_CurrentAnimationClip()->Get_MainAnimationName().substr(iStart, iEnd);

	// 툴에서 사용하기위한 Test 클립을 받아온다.
	CAnimationClip* pTestClip = m_pAnimatorCom->Get_AnimationClip("Test");

	CAnimationClip* pClip = pTestClip->Clone();
	pClip->Set_Name(szClipName);
	m_pAnimatorCom->Add_AnimationClip(pClip);

	// 이벤트 리스트를 비워준다.
	vector<CAnimationClip::ANIMEVENTDESC> eventList;

	pTestClip->Set_Event(eventList);

	return S_OK;
}

HRESULT CAnimViewer_Tool::Add_AllAnimationClip()
{
	if (nullptr == m_pAnimatorCom)
		return E_FAIL;

	for (auto& pAnimation : m_pAnimatorCom->Get_Model()->Get_Animations())
	{
		// () 안에 들어있는 이름만 애니메이션의 이름으로 받아온다.
		_int iStart = (_int)pAnimation->Get_Name().find("(", 0) + 1;
		_int iEnd = (_int)pAnimation->Get_Name().find(")", 0) - iStart;
		_string szClipName = pAnimation->Get_Name().substr(iStart, iEnd);

		CAnimationClip* pClip = CAnimationClip::Create();
		pClip->Set_Name(szClipName);
		pClip->Set_MainAnimation(pAnimation);
		m_pAnimatorCom->Add_AnimationClip(pClip);
	}

	return S_OK;
}

HRESULT CAnimViewer_Tool::Set_Animator(_wstring _strPath)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CAnimator::ANIMATORDESC animatorDesc;
	animatorDesc.pModel = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_Model();
	animatorDesc.strFilePath = _strPath;
	animatorDesc.pTransform = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_Transform();

	CAnimator* pAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, &animatorDesc);

	// 원래있던 애니메이터를 없애기 위해 Safe_Release를 해준다.
	static_cast<CAnimViewerModel*>(m_pAnimObject)->Set_Animator(pAnimator);

	Safe_Release(m_pAnimatorCom);

	// AnimObject가 가진 애니메이터가 바뀌었다면 m_pAnimatorCom에 다시 넣어준다.
	m_pAnimatorCom = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_Animator();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CAnimViewer_Tool::Delete_AnimationClip(_string strName)
{
	if (nullptr == m_pAnimatorCom)
		return E_FAIL;

	m_pAnimatorCom->Delete_AnimationClip(strName);

	return S_OK;
}

HRESULT CAnimViewer_Tool::Save_Animator()
{
	if (nullptr == m_pAnimatorCom)
		return E_FAIL;

	// 파일 경로
	_wstring strFilePath = L"..\\..\\Resources\\Data\\Animator\\";
	// 폴더생성
	_wmkdir(strFilePath.c_str());
	// 선택된 모델 받아오기
	strFilePath += m_ModelList[m_iModelIndex]->Get_FileData()->strFileName;
	strFilePath += L"\\";
	// 모델이름으로 폴더 생성L"..\\..\\Resources\\Animator\\모델명"
	_wmkdir(strFilePath.c_str());
	// 모델이름으로 애니메이터 파일 생성 L"..\\..\\Resources\\Animator\\모델명\\모델명.bin"
	strFilePath += m_ModelList[m_iModelIndex]->Get_FileData()->strFileName;
	strFilePath += L".bin";

	_ulong dwByte = 0;

	HANDLE hFile = CMethod::Begin_Save(strFilePath.c_str());
	if (0 == hFile)
		return E_FAIL;

	// 애니메이션 클립 개수를 담아준다.
	_uint iClipSize = (_uint)m_pAnimatorCom->Get_AnimationClipList().size();
	WriteFile(hFile, &iClipSize, sizeof(_uint), &dwByte, nullptr);

	// 불러올 시 애니메이션 클립 리스트에 담아둘 애니메이션 클립들
	for (auto& pClip : m_pAnimatorCom->Get_AnimationClipList())
	{
		// 애니메이션 클립의 이름을 저장한다.
		CMethod::Save_File_String(hFile, dwByte, pClip->Get_Name());
		// 메인 애니메이션의 이름을 저장합니다.
		CMethod::Save_File_String(hFile, dwByte, pClip->Get_MainAnimationName());
		// 서브 애니메이션의 이름을 저장합니다.
		if (nullptr == pClip->Get_SubAnimation())
			CMethod::Save_File_String(hFile, dwByte, "");
		else
			CMethod::Save_File_String(hFile, dwByte, pClip->Get_SubAnimationName());

		// 루프 여부를 저장한다.
		WriteFile(hFile, &pClip->Is_Loop(), sizeof(_bool), &dwByte, nullptr);
		// 루트본 여부를 저장한다
		_bool IsRootBone = pClip->Is_RootBone();
		WriteFile(hFile, &IsRootBone, sizeof(_bool), &dwByte, nullptr);
		// 루트본 이름을 저장한다.
		CMethod::Save_File_String(hFile, dwByte, pClip->Get_RootBoneName());
		// 애니메이션의 속도를 저장한다.
		WriteFile(hFile, &pClip->Get_AnimationSpeed(), sizeof(_float), &dwByte, nullptr);
		// 애니메이션 타입을 저장한다.
		CMethod::Save_File_String(hFile, dwByte, pClip->Get_AnimationType());

		// 이벤트 개수를 저장한다.
		_uint iEventSize = (_uint)pClip->Get_EventList().size();
		WriteFile(hFile, &iEventSize, sizeof(_uint), &dwByte, nullptr);
		// 이벤트를 저장한다.
		for (auto& event : pClip->Get_EventList())
		{
			// 이벤트 타입을 저장한다.
			WriteFile(hFile, &event.tEventType, sizeof(_uint), &dwByte, nullptr);
			// 이벤트 이름을 저장한다
			CMethod::Save_File_String(hFile, dwByte, event.strName);
			// 이벤트가 시작되는 시간비율
			WriteFile(hFile, &event.dStartRatio, sizeof(_double), &dwByte, nullptr);
			// 이벤트가 끝나는 시간비율
			WriteFile(hFile, &event.dEndRatio, sizeof(_double), &dwByte, nullptr);
			// 이벤트를 끝낼 시간 ( 시작 후에 시간초로 끝내고 싶을때 )
			WriteFile(hFile, &event.dEndTime, sizeof(_double), &dwByte, nullptr);
			// Value값 저장
			WriteFile(hFile, &event.fValue, sizeof(_float), &dwByte, nullptr);
			// AddValue값 저장
			WriteFile(hFile, &event.fAddValue, sizeof(_float), &dwByte, nullptr);
			// 생성 위치
			WriteFile(hFile, &event.vEventPos, sizeof(_float4), &dwByte, nullptr);
			// 이벤트가 가진 Dir값
			WriteFile(hFile, &event.vEventDir, sizeof(_float4), &dwByte, nullptr);
			// 이벤트가 가진 Value들 ( 다양하게 사용가능 )
			WriteFile(hFile, &event.vEventValues, sizeof(_float4), &dwByte, nullptr);
			// 사용할 뼈의 이름
			CMethod::Save_File_String(hFile, dwByte, event.strEventText);
		}
	}

	// 무기뼈 사이즈를 저장한다.
	_uint iWeaponBoneSize = (_uint)m_pAnimatorCom->Get_WeaponBoneList().size();
	WriteFile(hFile, &iWeaponBoneSize, sizeof(_uint), &dwByte, nullptr);

	for (auto& pWeaponBone : m_pAnimatorCom->Get_WeaponBoneList())
		CMethod::Save_File_String(hFile, dwByte, pWeaponBone->Get_Name());

	// 로드시 생성된 애니메이션 클립들로 구성할 TransitionList
	for (auto& transition : m_pAnimatorCom->Get_TransitionList())
	{
		// 트랜지션의 EnterState
		WriteFile(hFile, &transition.tEnterState, sizeof(_uint), &dwByte, nullptr);
		// 트랜지션의 EnterClip이름을 저장한다
		if (nullptr == transition.pEnterClip)
			CMethod::Save_File_String(hFile, dwByte, "");
		else
			CMethod::Save_File_String(hFile, dwByte, transition.pEnterClip->Get_Name());
		// 트랜지션의 ExitClip이름을 저장한다.
		if (nullptr == transition.pExitClip)
			CMethod::Save_File_String(hFile, dwByte, "");
		else
			CMethod::Save_File_String(hFile, dwByte, transition.pExitClip->Get_Name());
		// 트랜지션의 EnterRatio
		WriteFile(hFile, &transition.dEnterRatio, sizeof(_double), &dwByte, nullptr);
		// 트랜지션의 ExitRatio
		WriteFile(hFile, &transition.dExitRatio, sizeof(_double), &dwByte, nullptr);
		// 트랜지션의 ChangeRatio
		WriteFile(hFile, &transition.dChangeRatio, sizeof(_double), &dwByte, nullptr);
		// 트랜지션의 OverChange 불값
		WriteFile(hFile, &transition.bIsOverChangeRatio, sizeof(_bool), &dwByte, nullptr);
		// 트랜지션의 LerpSpeed
		WriteFile(hFile, &transition.dLerpSpeed, sizeof(_double), &dwByte, nullptr);
		// 트랜지션의 Trigger		
		CMethod::Save_File_String(hFile, dwByte, transition.InputTrigger.strInput);
		// 트랜지션의 Trigger Input Ratio
		WriteFile(hFile, &transition.InputTrigger.dAnimRatio, sizeof(_double), &dwByte, nullptr);
		// 트랜지션의 InputRatio의 이상, 이하 여부
		WriteFile(hFile, &transition.InputTrigger.bIsOverRatio, sizeof(_bool), &dwByte, nullptr);
	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CAnimViewer_Tool::Save_AnimationClip()
{
	if (nullptr == m_pAnimatorCom)
		return E_FAIL;

	// 애니메이션 클립 리스트
	vector<CAnimationClip*>animm_ClipList = m_pAnimatorCom->Get_AnimationClipList();

	return S_OK;
}

HRESULT CAnimViewer_Tool::Load_Animator()
{
	if (nullptr == m_pAnimatorCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimViewer_Tool::Load_AnimationClip()
{
	if (nullptr == m_pAnimatorCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimViewer_Tool::Initialize_ToolData()
{
	return S_OK;
}

HRESULT CAnimViewer_Tool::Merge_Animator(_wstring _strPath)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CAnimator::ANIMATORDESC animatorDesc;
	animatorDesc.pModel = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_Model();
	animatorDesc.strFilePath = _strPath;
	animatorDesc.pTransform = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_Transform();

	CAnimator* pAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, &animatorDesc);

	vector<CAnimationClip*> FilterList = pAnimator->Get_AnimationClipList();

	// 처음에 이상한것들을 다 걸러준다.
	for (_uint i = 0; i < FilterList.size();)
	{
		if (nullptr == FilterList[i]->Get_MainAnimation() && nullptr == FilterList[i]->Get_SubAnimation())
		{
			Safe_Release(FilterList[i]);
			FilterList.erase(FilterList.begin() + i);
		}
		else
			i++;
	}

	pAnimator->Set_AnimationClipList(FilterList);

	// 클립리스트를 합친다.
	vector<CAnimationClip*> ClipList;

	for (auto& iter : pAnimator->Get_AnimationClipList())
	{
		CAnimationClip* pClip = CAnimationClip::Create();
		if (nullptr == pClip)
			return E_FAIL;

		if ("Test" == iter->Get_Name())
		{
			Safe_Release(pClip);
			continue;
		}

		// 애니메이션 클립의 이름을 받아온다.
		pClip->Set_Name(iter->Get_Name());

		// 메인과 서브가 모두 비어있다면 for문을 넘긴다.
		if (nullptr == iter->Get_MainAnimation() && nullptr == iter->Get_SubAnimation())
		{
			Safe_Release(pClip);		
			continue;		
		}

		// 메인 애니메이션을 받아서 넣습니다.
		_string strMainAnim = iter->Get_MainAnimationName();

		pClip->Set_MainAnimation(m_pAnimatorCom->Get_Model()->Get_Animation(strMainAnim));
		// 서브 애니메이션을 받아서 넣습니다.
		//_string strSubAnim = iter->Get_SubAnimationName();
		//pClip->Set_SubAnimation(m_pAnimatorCom->Get_Model()->Get_Animation(strSubAnim));
		// 루프를 설정한다.
		pClip->Set_Loop(iter->Is_Loop());
		// 루트본 여부를 설정한다
		pClip->Set_RootBone(iter->Is_RootBone());
		// 루트본 이름을 설정한다.
		pClip->Set_RootBoneName(iter->Get_RootBoneName());
		// 스피드값을 불러온다.
		pClip->Set_AnimationSpeed(iter->Get_AnimationSpeed());
		// 애니메이션 타입값을 가져온다.
		pClip->Set_AnimationType(iter->Get_AnimationType());

		vector<CAnimationClip::ANIMEVENTDESC> eventList;

		vector<CAnimationClip::ANIMEVENTDESC> ClipEventList = iter->Get_EventList();

		// 이벤트를 저장한다.
		for (_uint k = 0; k < ClipEventList.size(); ++k)
		{
			CAnimationClip::ANIMEVENTDESC event;

			// 이벤트 타입을 복사한다
			event.tEventType = ClipEventList[k].tEventType;
			// 이벤트 이름을 불러온다
			event.strName = ClipEventList[k].strEventText;
			// 이벤트가 시작되는 시간비율
			event.dStartRatio = ClipEventList[k].dStartRatio;
			// 이벤트가 끝나는 시간비율
			event.dEndRatio = ClipEventList[k].dEndRatio;
			// 이벤트를 끝낼 시간 ( 시작 후에 시간초로 끝내고 싶을때 )
			event.dEndTime = ClipEventList[k].dEndTime;
			// Value값 저장
			event.fValue = ClipEventList[k].fValue;
			// AddValue값 저장
			event.fAddValue = ClipEventList[k].fAddValue;
			// 생성 위치
			event.vEventPos = ClipEventList[k].vEventPos;
			// 사용할 뼈의 이름
			event.strEventText = ClipEventList[k].strEventText;
			// 이벤트가 들고있는 Dir값
			event.vEventDir = ClipEventList[k].vEventDir;
			// 이벤트가 들고있는 Value들
			event.vEventValues = ClipEventList[k].vEventValues;

			eventList.push_back(event);
		}
		// 생성된 클립에게 이벤트리스트를 넣어준다.
		pClip->Set_Event(eventList);
		// 생성된 클립에게 현재 모델 객체의 노드 리스트를 넣어준다.
		pClip->Set_HierarchyNodeList(m_pAnimatorCom->Get_Model()->Get_HierachyNodeList());
		// 클립을 클립 리스트에 넣는다.
		ClipList.push_back(pClip);
	}
	// 원래갖고있던 애니메이션 클립을 리스트에 넣는다.
	for (auto& pAnimatorClip : m_pAnimatorCom->Get_AnimationClipList())
	{
		_uint iCount = 0;

		for (auto& pMergeClip : pAnimator->Get_AnimationClipList())
		{
			if (pMergeClip->Get_Name() == pAnimatorClip->Get_Name())
				iCount++;
		}
		if (0 == iCount)
		{
			// 레퍼런스 증가 시킨다.
			Safe_AddRef(pAnimatorClip);
			ClipList.push_back(pAnimatorClip);
		}
		// 중복되는것이 있다면 해당 애니메이션 클립을 갖고있는 트랜지션을 삭제한다.
		else
		{
			vector<CAnimator::TRANSITION> newTransitionList;
			vector<CAnimator::TRANSITION> transitionList = m_pAnimatorCom->Get_TransitionList();
			for (auto& transition : m_pAnimatorCom->Get_TransitionList())
			{
				if (pAnimatorClip == transition.pEnterClip || pAnimatorClip == transition.pExitClip)
					continue;

				newTransitionList.push_back(transition);
			}
			m_pAnimatorCom->Set_TransitionList(newTransitionList);
		}
	}

	for (auto& pPreviousClip : m_pAnimatorCom->Get_AnimationClipList())
		Safe_Release(pPreviousClip);

	// 애니메이션 클립 리스트를 교체한다.
	m_pAnimatorCom->Set_AnimationClipList(ClipList);

	vector<CAnimator::TRANSITION> transitionList;

	// 트랜지션 목록을 받아온다.
	for (auto& mergeTransition : pAnimator->Get_TransitionList())
	{
		CAnimator::TRANSITION transition;

		// 트랜지션의 EnterState
		transition.tEnterState = mergeTransition.tEnterState;
		// EnterClip을 트랜지션에 넣어준다.
		if (nullptr != mergeTransition.pEnterClip)
			transition.pEnterClip = m_pAnimatorCom->Get_AnimationClip(mergeTransition.pEnterClip->Get_Name());
		// ExitClip을 트랜지션에 넣어준다.
		if (nullptr != mergeTransition.pExitClip)
			transition.pExitClip = m_pAnimatorCom->Get_AnimationClip(mergeTransition.pExitClip->Get_Name());

		// 트랜지션의 EnterRatio
		transition.dEnterRatio = mergeTransition.dEnterRatio;
		// 트랜지션의 ExitRatio
		transition.dExitRatio = mergeTransition.dExitRatio;
		// 트랜지션의 ChangeRatio
		transition.dChangeRatio = mergeTransition.dChangeRatio;
		// 트랜지션의 OverChange 불값
		transition.bIsOverChangeRatio = mergeTransition.bIsOverChangeRatio;
		// 트랜지션의 LerpSpeed
		transition.dLerpSpeed = mergeTransition.dLerpSpeed;
		// 트랜지션의 Trigger		
		transition.InputTrigger.strInput = mergeTransition.InputTrigger.strInput;
		// 트랜지션의 Trigger Input Ratio
		transition.InputTrigger.dAnimRatio = mergeTransition.InputTrigger.dAnimRatio;
		// 트랜지션의 InputRatio 이상, 이하 여부 판다
		transition.InputTrigger.bIsOverRatio = mergeTransition.InputTrigger.bIsOverRatio;

		// 리스트에 담아준다.
		transitionList.push_back(transition);
	}

	for (auto& animatorTransition : m_pAnimatorCom->Get_TransitionList())
	{
		transitionList.push_back(animatorTransition);
	}

	m_pAnimatorCom->Set_TransitionList(transitionList);

	Safe_Release(pAnimator);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CAnimViewer_Tool::Merge_AnimatorEvent(_wstring _strPath)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CAnimator::ANIMATORDESC animatorDesc;
	animatorDesc.pModel = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_Model();
	animatorDesc.strFilePath = _strPath;
	animatorDesc.pTransform = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_Transform();

	CAnimator* pAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, &animatorDesc);

	vector<CAnimationClip*> FilterList = pAnimator->Get_AnimationClipList();

	// 처음에 이상한것들을 다 걸러준다.
	for (_uint i = 0; i < FilterList.size();)
	{
		if (nullptr == FilterList[i]->Get_MainAnimation() && nullptr == FilterList[i]->Get_SubAnimation())
		{
			Safe_Release(FilterList[i]);
			FilterList.erase(FilterList.begin() + i);
		}
		else
			i++;
	}

	pAnimator->Set_AnimationClipList(FilterList);

	for (auto& pClip : pAnimator->Get_AnimationClipList())
	{
		for (auto& pMyClip : m_pAnimatorCom->Get_AnimationClipList())
		{
			if (pClip->Get_Name() == pMyClip->Get_Name())
			{
				pMyClip->Set_Event(pClip->Get_EventList());
			}
		}
	}

	Safe_Release(pAnimator);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

// ================== 태현 추가 ==================
HRESULT CAnimViewer_Tool::Attach_SwordModel()
{
	ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), u8"모델 선택");
	for (auto& iter : m_ItemModelList)
	{
		if (nullptr != iter)
		{
			FILEGROUP* pFileGroup = iter->Get_FileGroup();
			FILEDATA* pFileData = iter->Get_FileData();

			if (nullptr != pFileGroup && nullptr != pFileData)
			{
				_wstring str = pFileGroup->strUppderName;
				str.append(L" ");
				str.append(pFileData->strFileName);
				if (ImGui::Selectable(CMethod::ConvertWToS(str).c_str()))
				{
					if (nullptr != m_pSwordTool)
						m_pSwordTool->Bine_Model(iter);
				}
			}
		}
	}
	ImGui::NewLine();
	if (nullptr != m_pSwordTool)
	{
		if (-1 == m_iModelIndex)
			return E_FAIL;
		// Anim Model
		m_pSwordTool->Find_Bones(m_pAnimObject);
	}
	return S_OK;
}

HRESULT CAnimViewer_Tool::Attach_ShelteModel()
{
	ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), u8"모델 선택");
	for (auto& iter : m_ItemModelList)
	{
		if (nullptr != iter)
		{
			FILEGROUP* pFileGroup = iter->Get_FileGroup();
			FILEDATA* pFileData = iter->Get_FileData();

			if (nullptr != pFileGroup && nullptr != pFileData)
			{
				_wstring str = pFileGroup->strUppderName;
				str.append(L" ");
				str.append(pFileData->strFileName);
				if (ImGui::Selectable(CMethod::ConvertWToS(str).c_str()))
				{
					if (nullptr != m_pShelteTool)
						m_pShelteTool->Bine_Model(iter);
				}
			}
		}
	}
	ImGui::NewLine();
	if (nullptr != m_pShelteTool)
	{
		if (-1 == m_iModelIndex)
			return E_FAIL;
		// Anim Model
		m_pShelteTool->Find_Bones(m_pAnimObject);
	}
	return S_OK;
}

CAnimViewer_Tool* CAnimViewer_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CAnimViewer_Tool* pInstance = new CAnimViewer_Tool(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CAnimViewer_Tool::Create To Failed");
	}
	return pInstance;
}

void CAnimViewer_Tool::Free(void)
{
	// 모델을 적용하던 객체의 레퍼런스 카운트를 감소한다.
	Safe_Release(m_pAnimObject);
	// 테스트용 Animator 컴포넌트 객체의 레퍼런스 카운트를 감소시킨다.
	Safe_Release(m_pAnimatorCom);
	// 테스트용 Animation 클립을 없앤다.
	Safe_Release(m_pAnimationClip);

	for (auto& pModel : m_ModelList)
		Safe_Release(pModel);

	for (auto& pModel : m_ItemModelList)
		Safe_Release(pModel);

	m_ModelList.clear();

	Safe_Release(m_pSwordTool);
	Safe_Release(m_pShelteTool);

	m_ItemModelList.clear();
	__super::Free();
}
