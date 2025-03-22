#include "../Public/ImGui_UI_Tool.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"
#include "UI_Base.h"
#include <codecvt>

CImGui_UI_Tool::CImGui_UI_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CImGuiClass_Tool(_pDevice, _pContext)
{
}
HRESULT CImGui_UI_Tool::NativeConstruct()
{
	m_pName = "UI_Tool";

	m_Windesc = WINDESC("UI", ImGuiWindowFlags_MenuBar, ImVec2(1350.f, 1.f), ImVec2(550.f, 1000.f), true);
	

	return S_OK;

}

HRESULT CImGui_UI_Tool::NativeConstruct_InnerValue()
{
	selectedObject = nullptr;
	gameObject.clear();
	return S_OK;
}

HRESULT CImGui_UI_Tool::Tick(const _double& _dTimeDelta)
{
	//Load_Texture();




	
	return S_OK;
}

HRESULT CImGui_UI_Tool::Late_Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Late_Tick(_dTimeDelta)))
		return E_FAIL;
	ImGui::SetNextWindowPos(m_Windesc.v2Pos, ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(m_Windesc.v2Size, ImGuiCond_Appearing);
	ImGui::Begin(m_Windesc.strName.c_str(), &m_Windesc.bEnable, m_Windesc.imGuiWindowFlags);
	
	ShowPrototypeObject();
	
	Convert_Transform();

	Alpha_Control(_dTimeDelta);
	
	Picking();

	Picking_Find_Object();
	
	Create_UI_Object();

	Show_Texture();
	
	Save();
	
	Load();

	Effect_UI(_dTimeDelta);

	Dialogue_UI();
	
	ImGui::End();

	return S_OK;
}

HRESULT CImGui_UI_Tool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	wsprintf(m_szFONTS, TEXT("%hs"), text.Text_Input);
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	//기본 : 18 
	pGameInstance->Align_RenderFont(TEXT("Font_Maru18"), m_szFONTS, _float2(text.TextX, text.TextY), XMVectorSet(text.TextR, text.TextG, text.TextB, 1.f), true);
	
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


void CImGui_UI_Tool::Picking()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	v2MousePos = pGameInstance->Get_MousePosition();
	POINT pt = { (_long)v2MousePos.x, (_long)v2MousePos.y };
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);
	

	ImGui::Checkbox("Picking", &bPicking);
	

	if (bPicking && pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
	{
		if (selectedObject != nullptr)
		{
			selectedObject->Set_Picking(true);
		    selectedObject->Set_Pos(_float2(_float(pt.x - (_float)g_iWinSizeX/2), _float(-pt.y + (_float)g_iWinSizeY / 2)));
			bPicking = true;
			selectedObject = nullptr;
		}
	}
	
	if (bPicking && pGameInstance->Get_DIMBtnUp(CInput_Device::DIMB_L))
	{
		if (selectedObject != nullptr)
			selectedObject->Set_Picking(false);
		bPicking = false;
	}
	
	RELEASE_INSTANCE(CGameInstance);
}


void CImGui_UI_Tool::ShowPrototypeObject()
{
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "UI Object");
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	list<class CGameObject*> Prototype_GameObjects = pGameInstance->Find_GameObjectList(g_iCurrentLevel, LAYER_UI);
	if (Prototype_GameObjects.size() == 0)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	vector<_tchar*> vecUI;
	for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
		vecUI.push_back(dynamic_cast<CUI_Base*>(*it)->Get_Name());


	static int item_current_idx = 0;


	if (selectedObject != nullptr)
		sprintf_s(str, "%ws", selectedObject->Get_Name());
	
	if (ImGui::BeginCombo("##", str))
	{
		for (int n = 0; n < vecUI.size(); n++)
		{
			bool is_selected = (item_current_idx == n);
			
			_char szName[256] = {};
			sprintf_s(szName, "%ws", vecUI[n]);
			if (ImGui::Selectable(szName, is_selected))
			{   
				int i = 0;
 
				for (auto it=Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
				{
					if (i == n)
					{
						selectedObject = dynamic_cast<CUI_Base*>(*it);
					}
					i++;
				}
				item_current_idx = n;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	RELEASE_INSTANCE(CGameInstance);

}


void CImGui_UI_Tool::Convert_Transform()
{
	
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Transform");
	if (selectedObject != nullptr)
	{
		_float2 pos = selectedObject->Get_Pos();
		_float2 size = selectedObject->Get_Size();
		_int  depth = selectedObject->Get_Depth();
		
		ImGui::Text("Position"); ImGui::SameLine();
		ImGui::DragFloat2("##Position", &pos.x, 0.3f, -5000.f, 5500.f);
		ImGui::Text("Size"); ImGui::SameLine();
		ImGui::DragFloat2("##Size", &size.x, 0.3f, -2000.f, 2000.f);
		ImGui::Text("Depth"); ImGui::SameLine();
		ImGui::DragInt("##Depth", &depth, 0.1f, -1000, 1000);

		selectedObject->Set_Pos(pos);
		selectedObject->Set_Size(size);
		selectedObject->Set_Depth(depth);
		
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (ImGui::Button("Delete"))
		{
			pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_UI, selectedObject);
			selectedObject = nullptr;
		}
		RELEASE_INSTANCE(CGameInstance);

		
		Change_UI_NameTag();
		
		Change_Texture();
		
	}
}


void CImGui_UI_Tool::Picking_Find_Object()

{
	ImGui::SameLine();
	ImGui::Checkbox("Find", &bFind);
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	list<class CGameObject*> Find_List = pGameInstance->Find_GameObjectList(g_iCurrentLevel, LAYER_UI);

	if (bFind && pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
	{
		for (auto it = Find_List.begin(); it != Find_List.end(); ++it)
		{
			CUI_Base* pUI = dynamic_cast<CUI_Base*>(*it);
			if (true == pUI->InterSect((_int)v2MousePos.x - (_int)g_iWinSizeX / 2, (_int)-v2MousePos.y + (_int)g_iWinSizeY / 2))
			{
				selectedObject = pUI;
				bFind = true;
			}
			else 
				bFind = false; 
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CImGui_UI_Tool::Create_UI_Object()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	char NameTag[50] = "Creat UI";
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), NameTag);
	
	ImGui::PushItemWidth(200.f);
	ImGui::InputText("Name Tag", name_Input, 50);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	
	if (ImGui::Button("Clone"))
	{
		_tchar buf[50];
		wsprintf(buf, TEXT("%hs"), name_Input);
		CUI_Base* test = dynamic_cast<CUI_Base * >(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel, LAYER_UI, PROTO_GOB_UI_TOOL));
		test->Set_Name(buf);
		ZeroMemory(name_Input, sizeof(char) * 50);
    }

	
	
	
	RELEASE_INSTANCE(CGameInstance);
}

void CImGui_UI_Tool::Change_UI_NameTag()
{

	char ChangeTag[50] = "Change NameTag";
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), ChangeTag);

	ImGui::PushItemWidth(200.f);
	ImGui::InputText("Change NameTag", tag_Change_NameInput, 50);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	if (ImGui::Button("Change_NameTag"))
	{
		if (selectedObject != nullptr)
		{
			_tchar tag[50];
			wsprintf(tag, TEXT("%hs"), tag_Change_NameInput);
			selectedObject->Set_Name(tag);
			ZeroMemory(tag_Change_NameInput, sizeof(char) * 50);
		}
	}

	char PrototypeTag[50] = "Prototype Tag";
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), PrototypeTag);
	
	/*if (selectedObject != nullptr)
		sprintf_s(prototype_Input, "%ws", selectedObject->Get_PrototypeTag());*/
	
	ImGui::PushItemWidth(200.f);
	ImGui::InputText("Prototype Tag", prototype_Input, 50);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	if (ImGui::Button("PrototypeTag"))
	{
		if (selectedObject != nullptr)
		{
			_tchar prototag[50];
			wsprintf(prototag, TEXT("%hs"), prototype_Input);
			selectedObject->Set_PrototypeTag(prototag);
			ZeroMemory(prototype_Input, sizeof(char) * 50);
		}
	}
}

void CImGui_UI_Tool::Show_Texture()
{
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Texture");
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	PATHS paths = { L"Texture"};
	//PATHS paths = { L"Dialogue" };
	FILEGROUP* pTextureFolder = pGameInstance->Find_Folder(&paths);
	if (nullptr == pTextureFolder)
		return;

	vector<FILEDATA> TextureData = pTextureFolder->vecFileDataList;
	
	vector<_string> vecUI;

	for (auto it = TextureData.begin(); it != TextureData.end(); ++it)
		vecUI.push_back(CMethod::ConvertWToS(it->strFileName).c_str());
	

	static int item_current_idx = 0;
	
	if (ImGui::BeginListBox("##listbox", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (int n = 0; n < vecUI.size(); n++)
		{
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(vecUI[n].c_str(), is_selected))
			{
				item_current_idx = n;
				
				selectedtexname = vecUI[item_current_idx];
				
				selectedTexture = pTextureFolder->FindData(CMethod::ConvertSToW(selectedtexname));

			}
			if (is_selected)
			{

				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CImGui_UI_Tool::Change_Texture()
{
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Change Texture");
	
	if (ImGui::Button("Change_Texture"))
	{
		if (selectedObject != nullptr)
		{
			_tchar textag[50];
			char name[100];
			strcpy_s(name, selectedtexname.c_str());
			wsprintf(textag, TEXT("%hs"), name);
			selectedObject->Set_Texture(textag);
			ZeroMemory(textag, sizeof(char) * 50);
			
		}
	}

}

void CImGui_UI_Tool::Save()
{
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Save");
	
	if (ImGui::Button("Save"))
	{
		HANDLE    hFile = CreateFile(TEXT("../../Resources/Data/Rattan_Msg.dat"), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

		if (INVALID_HANDLE_VALUE == hFile)
			return;
		
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		list<class CGameObject*> Find_List = pGameInstance->Find_GameObjectList(g_iCurrentLevel, LAYER_UI);

		RELEASE_INSTANCE(CGameInstance);
		
		for (auto it = Find_List.begin(); it != Find_List.end(); ++it)
		{
			CUI_Base* pUI = dynamic_cast<CUI_Base*>(*it);
			pUI->Serialize(hFile, g_iCurrentLevel, pUI);
		}
		CloseHandle(hFile);
	} 
}

void CImGui_UI_Tool::Load()
{
	ImGui::SameLine(); 
	if (ImGui::Button("Load"))
	{
		HANDLE    hFile = CreateFile(TEXT("../../Resources/Data/Rattan_Msg.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if (INVALID_HANDLE_VALUE == hFile)
			return;
		
		FileInfo info;

		DWORD    dwByte = 0;
		DWORD    dwStrByte = 0;


		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	
		while (true)
		{
			
			FileInfo    info;


			ReadFile(hFile, &info, sizeof(FileInfo), &dwByte, nullptr);

			info.CurrentLevel = g_iCurrentLevel;
			
			if (0 == dwByte)
			{
				break;
			}
			
			pGameInstance->Clone_GameObject(g_iCurrentLevel, LAYER_UI, PROTO_GOB_UI_TOOL,&info);
			
		}
		
		CloseHandle(hFile);

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CImGui_UI_Tool::Effect_UI(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "UI Effect");

	ImGui::PushItemWidth(100.f);
	ImGui::InputFloat("CreatTime", &creattime, 0.f, 1.0f);
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::PushItemWidth(100.f);
	ImGui::InputFloat("DeadTime", &deadtime, 0.f, 1.0f);
	ImGui::PopItemWidth();

	ImGui::PushItemWidth(100.f);
	ImGui::InputFloat("Speed", &speed, 0.f, 1.0f);
	ImGui::PopItemWidth();
	
	ImGui::SameLine();
	ImGui::PushItemWidth(100.f);
	ImGui::InputFloat("MoveX", &movex, 0.f, 1.0f);
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::PushItemWidth(100.f);
	ImGui::InputFloat("MoveY", &movey, 0.f, 1.0f);
	ImGui::PopItemWidth();

	ImGui::PushItemWidth(100.f);
	ImGui::InputFloat("MaxSize", &maxsize, 0.f, 1.0f);
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::PushItemWidth(100.f);
	ImGui::InputFloat("MinSize", &minsize, 0.f, 1.0f);
	ImGui::PopItemWidth();

	    ImGui::Checkbox("Grow", &bGrow);
		ImGui::SameLine();
		ImGui::Checkbox("GrowDecrease", &bGrowDecrease);
		ImGui::Checkbox("MoveRignt", &bRight);
		ImGui::SameLine();
		ImGui::Checkbox("MoveLeft", &bLeft);
		ImGui::SameLine();
		ImGui::Checkbox("MoveDown", &bDown);

    if (bGrow && pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
			bGrow = true;

	if (bGrowDecrease && pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
		bGrowDecrease = true;

	if (bRight && pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
		bRight = true;

	if (bLeft && pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
		bLeft = true;

	if (bDown && pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
		bDown = true;

	
		if (bGrow == true && selectedObject != nullptr) //커져라
		{
			if (selectedObject->Get_Effect() != false)
			selectedObject->Grow_UI(speed,maxsize,creattime,deadtime,_dTimeDelta);
			
			if (selectedObject->Get_Effect() == false)
			{
				//selectedObject = nullptr;
				bGrow = false;
			}
		}

		if (bGrowDecrease == true && selectedObject != nullptr) //커졌다 작아져라
		{
			selectedObject->Grow_Decrease(true, speed, maxsize, minsize, creattime, deadtime, _dTimeDelta);

			 if (selectedObject->Get_Effect() == false)
			{
				//selectedObject = nullptr;
				bGrowDecrease = false;
			}
		}

		if (bRight == true && selectedObject != nullptr) //오른쪽으로 가라
		{
			selectedObject->Set_Effect(true);
			selectedObject->Go_Right(movex, speed, creattime, deadtime, _dTimeDelta);

			if (selectedObject->Get_Effect() == false)
			{
				//selectedObject = nullptr;
				bRight = false;
			}
		}
		
		if (bLeft == true && selectedObject != nullptr) //왼쪽으로 가라
		{
			selectedObject->Set_Effect(true);
			selectedObject->Go_Left(movex, speed, creattime, deadtime, _dTimeDelta);

			if (selectedObject->Get_Effect() == false)
			{
				//selectedObject = nullptr;
				bLeft = false;
			}
		}

		if (bDown == true && selectedObject != nullptr) //밑으로 가라
		{
			selectedObject->Go_Down(movey, speed, creattime, deadtime, _dTimeDelta);

			if (selectedObject->Get_Effect() == false)
			{
				//selectedObject = nullptr;
				bDown = false;
			}
		}


	RELEASE_INSTANCE(CGameInstance);


		
}

void CImGui_UI_Tool::Alpha_Control(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Alpha Control");
	if (selectedObject != nullptr)
	{
		_float alpha = selectedObject->Get_Alpha();

		ImGui::Text("Alpha"); ImGui::SameLine();
		ImGui::DragFloat("##alpha", &alpha, 0.1f, -0, 1000);

		selectedObject->Set_Alpha(alpha);
	}

	if (selectedObject != nullptr)
	{
		//selectedObject-> = ALPHA_NONE;

		ImGui::PushItemWidth(100.f);
		ImGui::InputFloat("Alpha Start", &alphaCreat, 0.f, 100.0f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushItemWidth(100.f);
		ImGui::InputFloat("Alpha End", &alphaDead, 0.f, 100.0f);
		ImGui::PopItemWidth();

		
		ImGui::Checkbox("Plus Alpha", &bPlusAlpha);
		ImGui::SameLine();
		ImGui::Checkbox("Minus Alpha", &bMinusAlpha);
		ImGui::SameLine();
		ImGui::Checkbox("Repeat Alpha", &bRepeatAlpha);
		ImGui::SameLine();
		ImGui::Checkbox("NoneAlpha", &bNoneAlpha);
		
		if (bPlusAlpha && pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
			bPlusAlpha = true;

		if (bMinusAlpha && pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
			bMinusAlpha = true;
		
		if (bRepeatAlpha && pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
			bRepeatAlpha = true;

		if (bNoneAlpha && pGameInstance->Get_DIMBtnDown(CInput_Device::DIMB_L))
			bNoneAlpha = true;

		if (bPlusAlpha == true && selectedObject != nullptr)
		{
			selectedObject->Set_Alpha(0.f);
			selectedObject->Plus_Alpha(0.f,alphaCreat, alphaDead, _dTimeDelta);

			if (selectedObject->Get_Effect() == false)
				bPlusAlpha = false;
		}

		if (bMinusAlpha == true && selectedObject != nullptr)
		{
			//selectedObject->Set_Alpha(1.f);
			selectedObject->Minus_Alpha(0.f, alphaCreat, alphaDead, _dTimeDelta);

			if (selectedObject->Get_Effect() == false)
				bMinusAlpha = false;
		}

		if (bRepeatAlpha == true && selectedObject != nullptr)
		{
			selectedObject->Set_Alpha(0.f);
			selectedObject->Repeat_Alpha(0.f, alphaCreat, alphaDead, 0.f, _dTimeDelta);
		}

		if (bNoneAlpha == true && selectedObject != nullptr)
		{
			selectedObject->Set_Alpha(0.f);
			selectedObject->Set_Alpha_Effect(UIEFFET_NONE);
		}
	}
	RELEASE_INSTANCE(CGameInstance);

	
}

void CImGui_UI_Tool::Dialogue_UI()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Dialogue");

	setlocale(LC_ALL, "Korean");
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
	_tchar str16[1000];
	wsprintf(str16, L"%hs", text.Text_Input);
	strcpy_s(text.Text_Input, convert.to_bytes(str16).c_str());
	ImGui::PushItemWidth(500.f);
	ImGui::InputTextMultiline("Text", text.Text_Input, 1000);
	ImGui::PopItemWidth();
	lstrcpy(str16, convert.from_bytes(text.Text_Input).c_str());
	sprintf_s(text.Text_Input, "%ws", str16);
	

	ImGui::PushItemWidth(100.f);
	ImGui::DragFloat("PostionX", &text.TextX, 0.5f,0.f, 1280.0f);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushItemWidth(100.f);
	ImGui::DragFloat("PostionY", &text.TextY, 0.5f,0.f, 720.0f);
	ImGui::PopItemWidth();
	
	ImGui::PushItemWidth(50.f);
	ImGui::InputFloat("R", &text.TextR, 0.f, 255.f);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushItemWidth(50.f);
	ImGui::InputFloat("G", &text.TextG, 0.f, 255.f);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushItemWidth(50.f);
	ImGui::InputFloat("B", &text.TextB, 0.f, 255.f);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	
	ImGui::Dummy(ImVec2(30.0f, 0.0f));
	ImGui::SameLine();
	
	ImGui::PushItemWidth(100.f);
	ImGui::InputText("Tag", text.Text_Tag, 1000);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	if (ImGui::Button("SaveText"))
	{
		string test = "../../Resources/Dialogue/";
		test += text.Text_Tag;

		_wstring PATH = CMethod::ConvertSToW(test);

		PATH.append(L".dds");

	
		pView = pGameInstance->Get_BackBufferSRV();
		pView->GetResource(&pResoure);
		SaveDDSTextureToFile(m_pContext,  pResoure, PATH.c_str());
		
		Safe_Release_DV(pResoure);
	}
	
	RELEASE_INSTANCE(CGameInstance);

}


CImGui_UI_Tool* CImGui_UI_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CImGui_UI_Tool* pInstance = new CImGui_UI_Tool(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Created : CImGui_UI_Tool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImGui_UI_Tool::Free()
{
	__super::Free();
}
 