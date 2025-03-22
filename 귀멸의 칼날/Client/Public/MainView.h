#pragma once
#include "ImGuiClass.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CRefDebugActor;
END


BEGIN(Client)

class CMainView : public CImGuiClass
{
public:
	typedef struct tagSelectDesc
	{
		CGameObject*		pGameObject = nullptr;
		string						strName = "";
	}SELECTDESC;
protected:
	explicit CMainView(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CMainView() = default;
public:

public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct_InnerValue() override;
	virtual HRESULT Tick(const _double& _dDeltaSeconds)  override;
	virtual HRESULT Late_Tick(const _double& _dDeltaSeconds);
	virtual HRESULT Release_InnerValue() override;
	// Add Explain 
	void Add_Explain(const _string& _strExplain);
private:
	// Ready ImGuiClass
	HRESULT Ready_ImGuiClass();
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
	// Hierachy Control
	void Hierachy_Control();
	// Perspective 
	void Perspective_Control();
	void Show_Clones_Update(CGameInstance* _pGameInstance);
	// Open Level
	void OpenLevel();
#ifdef _DEBUG
	// Receive 
	void Receive_CameraObject();
#endif
private:
	vector<const _tchar*>		m_ClonesLayerStr;
	vector<CImGuiClass*>		m_ImGuiList;
private:
	DOCKDESC							m_tMainView;
	DOCKDESC							m_tPerspective;
	DOCKDESC							m_tHierarchy;
private:
	_bool										m_bInitSetting = false;
private:
	_int											m_iSelectComboBox = 0; 
private:
	CRefDebugActor*				m_pRefDebugActor = nullptr;
private:
	map<_string, map<_string, list<CGameObject*>>> m_ShowCloneMap;
	_uint m_iShowSize = 0;
private:
	// OpenLevel
	map<_uint, _string>			m_OpenLevelMap;
	// 
	_uint										 m_iSelectLevel = 0;
	// Select
	_bool										m_bSelectClickLevel = false;
	// Open Level
	_bool										m_bOpenLevel = false;
	// Add Explain
	list<_string>							m_AddExplain; 
#ifdef _DEBUG
	// OneMoreTime Frame
	_bool										m_bRestrictFrame = false;
	// Cam Object List
	list<CGameObject*>			m_CameraObjectList;
#endif
public:
	static CMainView* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END