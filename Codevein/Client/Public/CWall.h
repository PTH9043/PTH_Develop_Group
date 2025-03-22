#pragma once
#include "CGameObject_Client.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCubeCollision;
class CVIBuffer_Terrain;
END


BEGIN(Client)

class CWall final : public CGameObject_Client
{
public:
	typedef struct tagWallSaveDesc : public GOBCLIENTDESC
	{
		string		strModelName = "";
	}WALLSAVEDESC;
protected:
	explicit CWall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CWall(const CWall& _rhs);
	virtual ~CWall() = default;
public:
	virtual const char* Get_Name() override { return "Floor"; }
	virtual const _tchar* Get_Proto() override { return PROTO_GOB_WORLDOBJ; }
	virtual const _tchar* Get_Layer() override { return LAYER_WORLDOBJ; }
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
protected:
	WALLSAVEDESC m_tWallSaveDesc;
private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
public:
	static CWall* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END