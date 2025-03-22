#pragma once
#include"Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END


BEGIN(Tool)

class CGameObject_Tool abstract : public CGameObject
{
public:
	typedef struct tagGobClientDesc
	{
		_float3				v3Pos = _float3(0.f, 0.f, 0.f);
		_float3				v3Scale = _float3(1.f, 1.f, 1.f);
		_float3				v3Rotate = _float3(0.f, 0.f, 0.f);
	}GOBDESC;
protected:
	explicit CGameObject_Tool(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CGameObject_Tool(const CGameObject_Tool& _rhs);
	virtual ~CGameObject_Tool() = default;
public:
	const GOBDESC& Get_GobClientDesc() const { return m_tGobDesc; }
	void Set_GobClientDesc(const GOBDESC& _tGobDesc) { this->m_tGobDesc = _tGobDesc; }
public:
	virtual const char* Get_Name() = 0;
	virtual const _tchar* Get_Proto() = 0;
	virtual const _tchar* Get_Layer() = 0;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
protected:
	// Set U Constant Value
	virtual HRESULT SetUp_ConstantValue();
	// Ready  Components 
	virtual HRESULT Ready_Components();
protected:
	// Coll Tick
	virtual void Coll_Tick(_matrix _WorldMatrix);
public:
#pragma region USE_TOOL_METHOD
	// Extern EditValue
	virtual _bool Extern_EditValue();
	// Extern Hierakey Value
	virtual _bool Extern_HierakeyView(const string& _str);
	// Create Self 
	virtual HRESULT Create_Self(CGameObject** _ppOut);
	// Delete Self
	virtual HRESULT Delete_Self();
	// Save Data 
	virtual HRESULT Save_Data();
	// Load Data 
	virtual HRESULT Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData = nullptr);
	// Get Picking
	virtual _bool Is_Picking(_float3* _pOut = nullptr);
	// Reset Data 
	virtual void Reset_Data();
#pragma endregion USE_TOOL_METHOD
protected:
	// 외부로 노출할 ImGui Gob Data
	void Extern_GobData(void* _pData = nullptr);
	// 와부로 노출 할 Collider Setting Data 
	virtual void Control_ColliderData(void* _pData = nullptr);
protected:
	// 게임 오브젝트 저장에 관하여 필수적인 데이터들을 저장해 놓은 것
	GOBDESC		m_tGobDesc;
	// Model
	CModel*		m_pModelCom = nullptr;
	// Renderer 
	CRenderer* m_pRendererCom = nullptr;
	// ShaderCom
	CShader*		m_pShaderCom = nullptr;
	// Collider Com
	CCollider*		m_pColliderCom = nullptr;
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
