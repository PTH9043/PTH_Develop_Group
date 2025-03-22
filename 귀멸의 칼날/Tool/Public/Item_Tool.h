#pragma once
#include "Pawn_Tool.h"

BEGIN(Engine)
class CModel;
class CHierarchyNode;
class CVIBuffer_Trail;
END

BEGIN(Tool)

class CItem_Tool : public CPawn_Tool
{
public:
	typedef struct tagItemDesc
	{
		CGameObject* pOwnerObject = nullptr;
		_string					strBindBoneName = "";
		FILEGROUP* pModelFileGroup = nullptr;
		FILEDATA* pFIleData = nullptr;
		_matrix					PivotMatrix;
	}ITEMDESC;
protected:
	explicit CItem_Tool(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CItem_Tool(const CItem_Tool& _rhs);
	virtual ~CItem_Tool() = default;
public:
	virtual const char* Get_Name() { return "Item"; }
	virtual const _tchar* Get_Proto() { return L"Item_Proto"; }
	virtual const _tchar* Get_Layer() { return L"Layer_Item"; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
#pragma region TOOL_METHOD
	void Bine_Model(CModel* _pModel);
	// FInd Bones
	void Find_Bones(CGameObject* _pGameObject);
#pragma endregion TOOL_METHOD
protected:
	// Set U Constant Value
	virtual HRESULT SetUp_ConstantValue();
	// Ready Model Components
	virtual HRESULT Ready_Components() override;
protected:
	// Update BindTransform
	HRESULT Update_BindTransform();
protected:
	CGameObject*				m_pOwnerObject = nullptr;
	CHierarchyNode*		m_pBindBoneNode = nullptr;
	CModel*						m_pOwnerModel = nullptr;
	_float4x4						m_SocketMatrix;
	_float3				m_v3Rotation = _float3(0.f, 0.f, 0.f);
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

protected:
	// Save Value
	void Save_Value();

protected:
	// Text 
	_string			m_strText = "";
	// Save To Find Bone List
	list<CHierarchyNode*> m_SaveToFindBoneList;
	// Start End 
	_float4x4		m_StartMatrix;
	_float4x4		m_EndMatrix;
#pragma endregion USE_TOOL_METHOD
public:
	static CItem_Tool* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END