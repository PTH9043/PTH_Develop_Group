#pragma once
#include "CIMGUI.h"
#include "CCamera.h"

BEGIN(Engine)
class CTexture;
class CGameInstance;
END

BEGIN(Client)
class CGameObject_Client;
class CMainView final : public CIMGUI
{
public:
	typedef struct tagDockDesc : public WINDESC
	{
		tagDockDesc() = default;
		tagDockDesc(const char* _pName, const ImGuiWindowFlags& _imGuiFlags, const ImVec2& _v2StartPos = ImVec2(0.f, 0.f), 
			const ImVec2& _v2Size = ImVec2(0.f, 0.f),const _bool& _bEnable = true)
			: WINDESC(_pName, _imGuiFlags, _v2StartPos, _bEnable), v2Size(_v2Size), iID(0)
		{}
		ImVec2 v2Size;
		ImGuiID iID;
	}DOCKDESC;

	typedef struct tagSelectDesc
	{
		CGameObject_Client*		pGameObject = nullptr;
		string	strName = "";
		_float3									v3Positon;
	}SELECTDESC;

	enum GUI_CLASS 
	{
		GUI_OBJ, GUI_MAP, GUI_CAM, GUI_NAV, GUI_END
	};
protected:
	explicit CMainView(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CMainView() = default;
public:
	const _bool& Is_GamePlay() const { return m_bGamePlay; }
public:
	// Get Proto Map
	typedef map<const _tchar*,  CGameObject_Client*> PROTOOBJMAP;
	PROTOOBJMAP Get_ProtoObjMap() { return m_mProtoObjMap; }
	// get ImGuiClass
	CIMGUI* Get_ImGuiClass(const GUI_CLASS& _eGuiClass) { return m_vecImGui[_eGuiClass]; }
public:
	// Add Proto ObjMap
	HRESULT Add_ProtoObjMap(const _tchar* _pTag,  CGameObject_Client* _pObject);
public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct_Tool() override;
	virtual HRESULT Tick(const _double& _dDeltaSeconds)  override;
	virtual HRESULT Render()  override;
	virtual HRESULT Release_Tool() override;
private:
	// Ready ImGuiClass
	HRESULT Ready_ImGuiClass();
	HRESULT Relase_Tool();
private:
	// Picking Clones
	void Picking_Clones();
private:
	// Show Windows
	void Show_Window();
private:
	// Open ImGui Class 
	void Open_ImguiClass();
private:
	void Game_Control();
private:
	// Scene Control
	void Scene_Control();
	// Save Scene
	void Save_Scene();
	// Load Scene
	void Load_Scene();
private:
	// Play Button
	void Play_Button();
	// Stop Button
	void Stop_Button();
	// Exit Button
	void Exit_Button();
private:
	// Hierachy Control
	void Hierachy_Control();
	// Perspective 
	void Perspective_Control();
	void Show_Clones_Update(CGameInstance* _pGameInstance);

	void Reset_Value();
private:
	string Combine_Int(const _uint& _iIndex, const string& strName);
private:
	// ImGui 
	vector<CIMGUI*>				m_vecImGui;
	// Proto ObjMap
	PROTOOBJMAP					m_mProtoObjMap;

	vector<const _tchar*>				m_ClonesLayerStr;
private:
	DOCKDESC							m_tMainView;
	DOCKDESC							m_tPerspective;
	DOCKDESC							m_tHierarchy;
private:
	_bool										m_bInitSetting = false;
	_bool										m_bGamePlay = false;
	_bool										m_bPlay = false;
	_bool										m_bPause = false;
	_bool										m_bStop = false;
	_bool										m_bSave = false;
	_bool										m_bLoad = false;
	_bool										m_bExit = false;
	_bool										m_bPickingWorldObj = false;
	// Object Move Check
	_bool										m_bObject_MoveCheck = false;
	// Speed
	_float										m_fObject_MoveSpeed = 1.f;
private:
	char										m_szSaveName[MAX_PATH] = "";
	char										m_szLoadName[MAX_PATH] = "";
private:
	SELECTDESC						m_tSelect;
	CCamera::CAMDESC			m_tCameraDesc;

	_uint										m_iRegionIndex = 0;
private:
	map<_string, map<_string, list<CGameObject_Client*>>> m_ShowCloneMap;
	_uint m_iShowSize = 0;
public:
	static CMainView* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END