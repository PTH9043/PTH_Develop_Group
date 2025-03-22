#pragma once
#include "ImGuiClass_Tool.h"

BEGIN(Engine)
class CGameInstance;
END


BEGIN(Tool)
class CGameObject_Tool;

class CMainView_Tool : public CImGuiClass_Tool
{
public:
	typedef struct tagSelectDesc
	{
		CGameObject_Tool*		pGameObject = nullptr;
		string									strName = "";
	}SELECTDESC;
protected:
	explicit CMainView_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CMainView_Tool() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct_InnerValue() override;
	virtual HRESULT Tick(const _double& _dDeltaSeconds)  override;
	virtual HRESULT Late_Tick(const _double& _dDeltaSeconds);
	virtual HRESULT Render() override;
	virtual HRESULT Release_InnerValue() override;
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
	void Game_Control();
private:
	// Hierachy Control
	void Hierachy_Control();
	// Perspective 
	void Perspective_Control();
	void Show_Clones_Update(CGameInstance* _pGameInstance);
private:
	/*  Tool Function */
	// Reset Tool Function
	void Reset_ToolFunction();
private:
	HRESULT Ready_ReAssignValue();
private:
	list<const _tchar*>						m_ClonesLayerStr;
	vector<CImGuiClass_Tool*>		m_ImGuiList;
private:
	DOCKDESC							m_tMainView;
	DOCKDESC							m_tPerspective;
	DOCKDESC							m_tHierarchy;
private:
	_bool										m_bInitSetting = false;
	_bool										m_bPlay = false;
	_bool										m_bPause = false;
	_bool										m_bStop = false;
	_bool										m_bReset = false;
private:
	char										m_szSaveName[MAX_PATH] = "";
	char										m_szLoadName[MAX_PATH] = "";
private:
	map<_string, map<_string, list<SELECTDESC>>> m_ShowCloneMap;
	_uint m_iShowSize = 0;
	// Select Desc
	SELECTDESC																	 m_tSelectDesc;
public:
	static CMainView_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END