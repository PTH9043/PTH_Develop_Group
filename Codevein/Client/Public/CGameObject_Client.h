#pragma once
#include"Client_Defines.h"
#include "CGameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END


BEGIN(Client)

class CGameObject_Client abstract : public CGameObject
{
public:
	typedef struct tagGobClientDesc
	{
		_float3				v3Pos = _float3(0.f, 0.f, 0.f);
		_float3				v3Scale = _float3(1.f, 1.f, 1.f);
		_float3				v3Rotate = _float3(0.f, 0.f, 0.f);
	}GOBCLIENTDESC;
protected:
	explicit CGameObject_Client(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CGameObject_Client(const CGameObject_Client& _rhs);
	virtual ~CGameObject_Client() = default;
public:
	const GOBCLIENTDESC& Get_GobClientDesc() const { return m_tGobDesc; }
	void Set_GobClientDesc(const GOBCLIENTDESC& _tGobDesc) { this->m_tGobDesc = _tGobDesc;}
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
	// SetUp TransformData
	HRESULT Reset_TransformData();
protected:
	// Receive Model Names
	HRESULT Receive_ModelNames();
protected:
	// 게임 오브젝트 저장에 관하여 필수적인 데이터들을 저장해 놓은 것
	GOBCLIENTDESC m_tGobDesc;
public:
	void Set_ShowName(const _string& _strShowName) { this->m_strShowName = _strShowName; }
	const _string& Get_ShowName() const { return m_strShowName; }
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
	// 외부로 노출할 ImGui
	void Extern_GobData();

protected:
	// Show Name
	_string m_strShowName = "";
public:
	virtual CGameObject* Clone(void* pArg)  = 0;
	virtual void Free() override;
};

END