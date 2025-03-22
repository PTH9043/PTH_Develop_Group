#pragma once
#include "CIMGUI.h"

BEGIN(Engine)
class CTexture;
END


BEGIN(Client)
class CGameObject_Client;
class CCubeObj;

class CMapTool final : public CIMGUI
{
public:
	enum SELECT_OBJ
	{
		SEL_1, SEL_2, SEL_3, SEL_END
	};

	typedef struct tagSelectCell
	{
		tagSelectCell() = default;
		tagSelectCell(const _float& _fLength, const _uint& _iIndex, const _float3& _v3Pos)
			: fLength(_fLength), iIndex(_iIndex), v3Pos(_v3Pos)
		{}
		_float			fLength = 0.f;
		_uint			iIndex = 0;
		_float3		v3Pos = _float3(0.f, 0.f, 0.f);
	}SELCELL;
protected:
	explicit CMapTool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CMapTool() = default;
public:
	CGameObject_Client* Get_SelectObject() { return m_pSelectObject; }
public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct_Tool() override;
	virtual HRESULT Tick(const _double& _dDeltaSeconds) override;
	virtual HRESULT Render() override;
	virtual HRESULT Release_Tool() override;
	void Reset();
public:

	void Reset_Index();
protected:
	// Pick Terrain
	void Pick_Terrain();
protected:
	// Show Window
	virtual void Show_Window() override;
protected:
	// Modify_Select_Terrain 
	void Modify_Selected_Terrain(class CTerrain* _pTerrain);
	// Reset
	void Reset_Selected_Terrain(class CTerrain* _pTerrain);
	// Select Debug Img
	void Select_Debug_Img();
protected:
	// Modify Navigation
	void Modfiy_Navigation(class CTerrain* _pTerrain);
	// Show Cell Navigation
	void Show_Cell_Navigation(CTerrain* _pTerrain);
	// Reset Data
	void Reset_Nav_Data();
private:
	// Select Obj
	CGameObject_Client*					m_pSelectObject = nullptr;
	_float3												m_v3PickPos;
	_float2												m_v2Range = _float2(0.f, 0.f);
	// Debug Texture
	CTexture* m_pDebugTexture = nullptr;
	// Select Mode
	_bool													m_bClicked = false;
	_bool													m_bModifyCubeMode = false;
	_uint													m_iSelectTextureIndex = 0;
private:
	_bool													m_bModifyNavigation = false;
private:
	_float3												m_v3ModifPosition = _float3(0.f, 0.f, 0.f);
	vector<_float3	>								m_vecPosList;
	_uint													m_iCellIndex = 0;
private:
	CCubeObj*										m_pCubePosArr[SEL_END] = { nullptr };
	_uint													m_iSelIndex = SEL_1;
	_bool													m_bSelEnd = false;
public:
	static CMapTool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END

/*
* 	_bool													m_bHeightModifyMode = false;
	_bool													m_bFilterModfityMode = false;
	_string												m_strHeightTextureName = "";
private:
	_string												m_strFilterName = "";
*/