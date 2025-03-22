#pragma once
#include "ImGuiClass_Tool.h"

BEGIN(Engine)
class CCamera;
class CGameInstance;
class CVIBuffer_Terrain;
class CShader;
class CTexture;
class CTransform;
END

BEGIN(Tool)
class CCamera_Cinematic_Tool;

class CCamViewer_Tool : public CImGuiClass_Tool
{
private:
	explicit  CCamViewer_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCamViewer_Tool() = default;
public:

public:
	virtual HRESULT NativeConstruct() override;
	// Native Inner Value (Reset ImGuiClass Variable)
	virtual HRESULT NativeConstruct_InnerValue() override;
	virtual HRESULT Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Late_Tick(const _double& _dTimeDelta) override;
	// Render
	virtual HRESULT Render() override;
	// Release Inner Value (Reset ImGuiClass Variable)
	virtual HRESULT Release_InnerValue() override;
	// Run
	static void Run_Thread();
protected:
	// Show Window
	virtual void  Show_Window() override;
	// Select View 
	void Select_View();
	// Cam View
	void Cam_View();
protected:
	// Move Terrain Position
	void Move_TerrainPosition();
	// Add Terrain Data 
	void Show_TerrainData();
protected:
	// Show Bounding Box
	void Show_BoundingBox(const _float3& _v3Size, const _float3& _v3Pos, CGameInstance* _pGameInstance);
	// Select Situation
	void Select_Menu();
	// Mode Render
	void ModeTo_Render();
protected:
	// Cinematic Cam
	CCamera_Cinematic_Tool*		m_pCinematicCamera = nullptr;
	// Assign Cam List
	vector<CCamera*>					m_AssignCamList;
	// Select Camera
	CCamera*										m_pSelectCamera = nullptr;
	// Dock Space Init Setting
	_bool												m_bInitSetting = false;
	_bool												m_bSelectCamera = false;
	_string											m_strSelectCamName = "";
	// Dock Desc
	DOCKDESC									m_tMainView;
	DOCKDESC									m_tSelectView;
	DOCKDESC									m_tCamView;
	// Mode 
	_uint												m_iMode = 0;
	// Terrain 
	CVIBuffer_Terrain*						m_pTerrainData = nullptr;
	CShader*										m_pShaderCom = nullptr;
	// Terrain Position Control
	_float3											m_vTerrainPositionControl = _float3(-0.f, 0.f, -0.f);
	CTransform*								m_pTransformComp = nullptr;
	// Save Load name
	_string											m_strSaveName = "";
	_string											m_strLoadName = "";
public:
	static CCamViewer_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END