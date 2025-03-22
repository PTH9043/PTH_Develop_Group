#pragma once
#include "CBase.h"
#include "Engine_Defines.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CModel;
class CGameObject;
END

BEGIN(Client)
class CGameObject_Client;
class CMainView;
class CDebugModel;
class CIMGUI;

class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)
protected:
	explicit CImGui_Manager();
	virtual ~CImGui_Manager() = default;
public:
	const _bool& Is_ToolMode() const { return m_bToolMode; }
	void Set_ToolMode(const _bool& _bToolMode) { this->m_bToolMode = _bToolMode; }
	// Get MainView
	CMainView* Get_MainView() { return m_pMainView; }
	// Get Debug Object
	CGameObject* Get_DebugObject();

	_bool Is_RenderingTerrain();
public:
	// Get Proto Map
	map<const _tchar*, CGameObject_Client*> Get_ProtoObjMap();
	// Get Clone Map
	map<const _tchar*, list< CGameObject_Client*>> Get_CloneObjMap();
	// Get Terrain Texture
	CTexture* Get_SelectTerrainTexture();
	// Add Proto
	HRESULT Add_Proto(const _tchar* _pTag, class CGameObject_Client* _pObject);
	// Set Debug Model
	void Set_DebugObject_Model(CModel* _pModel);
public:
	HRESULT NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	HRESULT NativeConstruct_Tool();
	HRESULT Release_Tool();
	void Tick(const _double& _dDeltaSeconds);
	HRESULT Render();

	void Add_DeleteImGui(CIMGUI* _pImGui);
	void Delete_ImGui();
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	// tool Mode
	_bool m_bToolMode = false;

	list<CIMGUI*> m_DeleteImGuiClass;
private:
	// MainView
	class CMainView* m_pMainView = nullptr;
public:
	virtual void Free() override;
};

END