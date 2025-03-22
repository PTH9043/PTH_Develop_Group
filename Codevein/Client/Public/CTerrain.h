#pragma once
#include "CGameObject_Client.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer_Terrain;
class CNavigation;
class CCell;
END

BEGIN(Client)
class CCubeObj;

class CTerrain final : public CGameObject_Client
{
public:
	typedef struct tagCellDesc
	{
		_float3 v3PosArr[3];
	}POSINFO;
	typedef struct tagTerrainDesc
	{
		_uint iNumVerticesX = 0;
		_uint iNumVerticesZ = 0;
	}TERRAINDESC;

	typedef struct tagTerrianSaveDesc 
	{
		_uint						iNumVerticesX = 0;
		_uint						iNumVerticesZ = 0;
		_uint						iNumVertices = 0;
		// Not Save
		_float3*					pVertices = nullptr;
		_uint						iNavIndex = 0;
		CNavigation*		pNavigation = nullptr;
		_wstring				strPath = L"";
		GOBCLIENTDESC tGobData;
	}TERRIANSAVEDESC;

	typedef struct tagTerrianModifyDesc
	{
		ID3D11ShaderResourceView* pModifyTexture = nullptr;
		_float3		v3Pos;
		_float2		v2Radius;
	}TERRMODIFYDESC;
private:
	explicit CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTerrain(const CTerrain& _rhs);
	virtual ~CTerrain() = default;
public:
	virtual const char* Get_Name() override { return "Terrain"; }
	virtual const _tchar* Get_Proto() override { return PROTO_GOB_TERRAIN; }
	virtual const _tchar* Get_Layer() override { return LAYER_TERRAIN; }

	void Push_TerrainObjects(CGameObject* _pObjects);

	CNavigation* Get_Navigation() { return m_pNavigation; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
protected:
	// Ready Component
	HRESULT Ready_Component();
	// SetUp Constant Table
	HRESULT SetUp_ConstantTable();
private:
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRenderCom = nullptr;
	CVIBuffer_Terrain* m_pVITerrainCom = nullptr;
	CNavigation* m_pNavigation = nullptr;
private:
	// Save Terrian Desc
	TERRAINDESC									m_tTerraInDesc;
	_float													m_fUvValue = 1.f;
	// Not Save
	_float3*												m_pVertices = nullptr;
private:
	//  터레인에 있는 오브젝트들
	list<CGameObject*>						m_TerrainObjects;
public:
#pragma region USE_TOOL_METHOD
	// Extern EditValue
	virtual _bool Extern_EditValue() override;
	// Extern Hierakey Value
	virtual _bool Extern_HierakeyView(const string& _str)  override;
	// Create Self 
	virtual HRESULT Create_Self(CGameObject** _ppOut)  override;
	// Delete Self
	virtual HRESULT Delete_Self()  override;
	// Save Data 
	virtual HRESULT Save_Data()  override;
	// Load Data 
	virtual HRESULT Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData = nullptr)  override;
	// Get Picking
	virtual _bool Is_Picking(_float3* _pOut = nullptr)  override;
	// Reset Data 
	virtual void Reset_Data() override;
#pragma endregion USE_TOOL_METHOD
public:
#pragma region EXTERN_MAPTOOL
	// Setting Value
	HRESULT Setting_Modify_Value(ID3D11ShaderResourceView* _pDubgTexture, const _float3& _v3Pos, const _float2& _v2Radius);
	// Modify Terrain
	HRESULT Modify_Terrain(const _fvector& _v3Pos, const _uint& _iJudge = 0);
	// Reset
	HRESULT Reset_ToModify_Terrain();
	// Add Cell
	HRESULT Add_Cell(class CCell* _pCell);
	// Delete Cell
	HRESULT Delete_Cell(const _uint& _iIndex);
	// Clear 
	HRESULT Clear_Cell();
public:
#ifdef _DEBUG
	typedef struct tagModfiyHeightDesc
	{
		tagModfiyHeightDesc() = default;
		tagModfiyHeightDesc(CCubeObj* _pObj, const _float& _fLength)
			: pObj(_pObj), fLenght(_fLength)
		{}
		CCubeObj* pObj = nullptr;
		_float fLenght = 0.f;
	}MODIFYHEIGHTDESC;

	// Cube Objects
	HRESULT Ready_CubeObjects();
	// Show
	HRESULT Show_CubeObjects();
	// Control
	HRESULT Control_CubeObjsts();
	// Modify Terrain Height
	HRESULT Modfiy_TerrainHeight(const _float& _yPos);
	// Clear Terrain Height
	HRESULT Clear_TerrainHeight();
#endif
#pragma endregion EXTERN_MAPTOOL
private:
	TERRMODIFYDESC							m_tModifyDesc;
	_bool													m_bShow = false;
	_uint													m_iTerrainIndex = 0;
#ifdef _DEBUG
private:
	typedef list< CCubeObj*> CUBEOBJS;
	CUBEOBJS m_CubeObjs;

	CUBEOBJS m_SelectCubeObjs;
#endif
public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END