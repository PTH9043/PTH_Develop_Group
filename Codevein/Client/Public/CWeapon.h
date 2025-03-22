#pragma once
#include "CEquipment.h"
#include "CCharacter.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)
class CPawn;
class CCharacter;

class CWeapon : public CEquipment
{
public:
	enum WEAPONTYPE
	{
		WEAPON_ONEHAND, WEAPON_TWOHAND, WEAPON_GUN, WEAPON_HAMER, WEAPON_HARBUD, WEAPON_END
	};
	typedef struct tagWeaponDesc
	{
		_float3						v3PivotColliderPos = _float3(0.f, 0.f, 0.f);
		_float3						v3PivotColliderSize = _float3(0.f, 0.f, 0.f);
	}WEAPONDESC;
protected:
	explicit CWeapon(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CWeapon(const CWeapon& _rhs);
	virtual ~CWeapon() = default;
public:
	const WEAPONTYPE& Get_WeaponType() const { return m_eWeaponType; }
	// Get Weapon Name
	_wstring Get_WeaponName();

	CCharacter::CHARINFO Get_CharInfo() { return m_tCharInfo; }
	CTransform* Get_TargetTransform() { return m_pTargetTransform; }
public:
	// Check Collider
	HRESULT Add_CheckColliderList(CPawn* _pPawnObject);
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	// Find HierachyNode
	virtual HRESULT Find_HierackyNode(CGameObject* _pGameObject, const string& _str = "RightHandAttach") override;
public:
	virtual CEquipment* ReturnSettingToValue(CCharacter* _pCharacter) override;
	void Set_AtkStrong(const CCharacter::ATK_STR& _eAtkStr) { this->m_eOwnerAtkStrong = _eAtkStr; }
	const CCharacter::ATK_STR& Get_OwnerAtkStrong() const { return m_eOwnerAtkStrong; }
protected:
	// Bind Weapon Status
	void Bind_Weapon_Status(CCharacter* _pCharacter);
protected:
	// Ready Component
	virtual HRESULT Ready_Component() override;
	// SetUp ConstantValue
	virtual HRESULT SetUp_ConstantTable() override;
	// Update Transform
	HRESULT Update_Transform();
	// Upate Transform
	virtual HRESULT  Bind_Transform();
	// Setting Weapon Pivot Pos
	void Setting_WeaponPivotPos(const _fvector& _vPivotPos, const _fvector& _vPivotScale);
	// Check Collision
	void Check_Collision();
protected:
	// Coll Tick
	virtual void Coll_Tick(_matrix matWorld);
#ifdef _DEBUG
	virtual _bool Add_Collider_Render() override;
#endif
protected:
	virtual void Initialize_InsertData();
protected:
	// 장착당할 노드
	CHierarchyNode*					m_pEquipMatrixNode = nullptr;
	// Node의 Hieracky Node
	CHierarchyNode*					m_pBindMatrixNode = nullptr;
	// Target Transform 
	CTransform*							m_pTargetTransform = nullptr;
	// First Check Collider
	CCollider*									m_pFirstCheckCollider = nullptr;
	// Target Model
	CModel*									m_pTargetModel = nullptr;
	// Weapon Type
	WEAPONTYPE							m_eWeaponType = WEAPON_END;
	// Collision Check List
	list<CPawn*>							m_CollisionCheckList;
	// Char Info
	CCharacter::CHARINFO			m_tCharInfo;
	// 위 무기에 주인에 대한 어택 스트롱
	CCharacter::ATK_STR			m_eOwnerAtkStrong = CCharacter::AS_END;
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
	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override;
};

END