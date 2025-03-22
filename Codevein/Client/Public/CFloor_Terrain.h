#pragma once
#include "CGameObject_Client.h"

BEGIN(Client)

class CFloor_Terrain final : public CGameObject_Client
{
public:
	typedef struct tagTerrainDesc
	{
		_uint iNumVerticesX = 0;
		_uint iNumVerticesZ = 0;
	}TERRAINDESC;

	typedef struct tagFloorTerrainSaveDesc : public GOBCLIENTDESC
	{
		_uint			iNumVerticesX = 0;
		_uint			iNumVerticesZ = 0;
	}FLOORTERSAVEDESC;
private:
	explicit CFloor_Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CFloor_Terrain(const CFloor_Terrain& _rhs);
	virtual ~CFloor_Terrain() = default;
public:
	virtual const char* Get_Name() override { return "Floor_Terrain"; }
	virtual const _tchar* Get_Proto() override { return PROTO_GOB_FLOORTERRAIN; }
	virtual const _tchar* Get_Layer() override { return LAYER_TERRAIN; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
public:
#pragma region USE_TOOL_METHOD
	// Extern EditValue
	virtual _bool Extern_EditValue() override;
	// Extern Hierakey Value
	virtual _bool Extern_HierakeyView(const string& _str) override;
	// Create Self 
	virtual HRESULT Create_Self(CGameObject** _ppOut) override;
	// Delete Self
	virtual HRESULT Delete_Self() override;
	// Save Data 
	virtual HRESULT Save_Data() override;
	// Load Data 
	virtual HRESULT Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData = nullptr) override;
	// Get Picking
	virtual _bool Is_Picking(_float3* _pOut = nullptr) override;
	// Reset Data 
	virtual void Reset_Data() override;
#pragma endregion USE_TOOL_METHOD

public:
	static CFloor_Terrain* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CFloor_Terrain* Clone(void* _pArg) override;
	virtual void Free() override;
};

END