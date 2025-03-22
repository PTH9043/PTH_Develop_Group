#pragma once
#include "CGameObject_Client.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CVIBuffer_Terrain;
END

BEGIN(Client)

class CWorldObj : public CGameObject_Client
{
public:
	typedef struct tagColliderDesc
	{
		_float3						v3Translate = _float3(0.f, 0.f, 0.f);
		_float3						v3Scale = _float3(1.f, 1.f, 1.f);
	}COLLIDERDESC;
	typedef struct tagFloorSaveDesc 
	{
		_string						strModelName = "";
		_wstring					strPrototype = L"";
		GOBCLIENTDESC		tGobClientDesc;
		COLLIDERDESC		tColliderDesc;
	}WORLDSAVES;
protected:
	explicit CWorldObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWorldObj(const CWorldObj& _rhs);
	virtual ~CWorldObj() = default;
public:
	virtual const char* Get_Name() override;
	virtual const _tchar* Get_Proto() override { return PROTO_GOB_WORLDOBJ; }
	virtual const _tchar* Get_Layer() override { return LAYER_WORLDOBJ; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
protected:
	// Ready Component 
	virtual HRESULT Ready_Component();
	// SetUp ConstantValue
	HRESULT SetUp_ConstantTable();
protected:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*					m_pCollCollider = nullptr;
protected:
	string							m_strModelName = "";
protected:
	COLLIDERDESC		m_tColliderDesc;
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
	// Picking Nav 
	_bool Is_PickingNav();
	// Reset Data 
	virtual void Reset_Data() override;
#pragma endregion USE_TOOL_METHOD
protected:
	// Show Model
	virtual HRESULT Show_Model();
	// ExternCollider
	void			Extern_ColliderData();
protected:
	_bool		m_bPicking = false;
public:
	static CWorldObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END