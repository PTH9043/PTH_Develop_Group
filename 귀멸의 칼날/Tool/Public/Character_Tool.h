#pragma once
#include"Tool_Defines.h"
#include "Pawn_Tool.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END


BEGIN(Tool)

class CCharacter_Tool abstract : public CPawn_Tool
{
protected:
	explicit CCharacter_Tool(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CCharacter_Tool(const CCharacter_Tool& _rhs);
	virtual ~CCharacter_Tool() = default;
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
	virtual void Reset_Data()  override;
#pragma endregion USE_TOOL_METHOD
protected:
	// 와부로 노출 할 Collider Setting Data 
	virtual void Control_ColliderData(void* _pData = nullptr) override;
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
