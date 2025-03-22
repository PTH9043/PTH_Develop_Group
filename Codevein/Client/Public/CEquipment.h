#pragma once
#include "CItem.h"

BEGIN(Engine)
class CHierarchyNode;
END

BEGIN(Client)
class CPawn;
class CCharacter;

class CEquipment : public CItem
{
public:
	enum EQUIPTYPE
	{
		EQUIP_WEAPON, EQUIP_ARMOR, EQUIP_CLOTH, EQUIP_END
	};

	typedef struct tagEquipInfo
	{

	}EQUIPINFO;

	typedef struct tagEquipDesc
	{
		// Owner Object
		CGameObject* pOwnerObject = nullptr;
		string					strAttachNodeName = " ";
	}EQUIPDESC;
protected:
	explicit CEquipment(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CEquipment(const CEquipment& _rhs);
	virtual ~CEquipment() = default;
public:
	const EQUIPTYPE& Get_EquipType() const { return m_eEquipType; }
	const _float4x4& Get_SockMatrix4x4() const {	return m_SocketMatrix;}
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	// Find HierachyNode
	virtual HRESULT Find_HierackyNode(CGameObject* _pGameObject, const string& _str = "");
public:
	virtual CEquipment* ReturnSettingToValue(CCharacter* _pCharacter);
public:
	void Set_Model_Pivot(_fmatrix matPivot);

protected:
	// Ready Component
	virtual HRESULT Ready_Component() override;
	// SetUp ConstantValue
	virtual HRESULT SetUp_ConstantTable() override;
	// Recieve Model Files
	HRESULT Receive_ModelFiles();
protected:
	EQUIPTYPE	m_eEquipType = EQUIP_END;
	EQUIPDESC	m_tEquipDesc;
protected:
	_float4x4		m_SocketMatrix;
	// FileGroup
	FILEGROUP* m_pFileGroup = nullptr;
	FILEDATA* m_pFileData = nullptr;
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
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END