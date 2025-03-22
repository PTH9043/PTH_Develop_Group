#pragma once
#include "CPawn.h"
#include "CEquipment.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CHierarchyNode;
class CCollider;
class CNavigation;
class CCell;
END

BEGIN(Client)
class CRegion;
class CStage;

class CCharacter : public CPawn
{
public:
	enum DIRECTION
	{
		DIR_FOR, DIR_BACK, DIR_LEFT, DIR_RIGHT, DIR_FORLEFT, DIR_FORRIGHT, DIR_BACKLEFT, DIR_BACKRIGHT, DIR_END
	};
	enum NODES
	{
		NODE_OFFSET, NODE_CHEST, NODE_END
	};

	enum DMG_PT
	{
		DMG_LT, DMG_MD, DMG_BG, DMG_GG, DMG_END
	};

	enum ATK_STR
	{
		AS_WEEK, AS_NORMAL, AS_STRONG, AS_END
	};

	enum HAND_DIR
	{
		HAND_LEFT, HAND_RIGHT, HAND_END
	};

	typedef struct tagCharInfo
	{
	public:
		tagCharInfo() = default;

		_float fCurHp = 0.f;
		_float fMaxHp = 0.f;
		_float fCurEnergy = 0.f;
		_float fMaxEnergy = 0.f;
		_float fAtkPower = 0.f;
		_float fAtkSpeed = 0.f;

		void UpdateInfo(const PAWNSTATUS& _tStatus)
		{
			fMaxHp = _tStatus.iEnergy * 0.2f + _tStatus.iHealth * 0.8f;
			fCurHp = fMaxHp;
			fMaxEnergy = _tStatus.iEnergy * 0.8f + _tStatus.iHealth * 0.2f;
			fCurEnergy = fMaxEnergy;
			fAtkPower = _tStatus.iPower * 0.7f + _tStatus.iEnergy * 0.1f + _tStatus.iHealth * 0.2f;
			fAtkSpeed = _tStatus.iSpeed * 0.8f + _tStatus.iPower * 0.2f;
		}

		_float Get_Damage()	{ return fAtkPower * 0.8f + fAtkSpeed * 0.2f;}
		void Damaged(const _float& fDamage)
		{
			_float fValue = (fDamage);
			if (fValue < 0.f)
				fValue = 0.f;
			fCurHp = fCurHp - fValue;
		}
		_bool Is_CheckDie() { if (fCurHp <= 0) return true; return false; }
		void ResetHp() { fCurHp = fMaxHp; }
	}CHARINFO;

	typedef struct tagCharDesc
	{
		_string strWeaponAttackName;

	}CHARDESC;

	typedef struct tagBonesColliderDesc
	{
		CCollider*						pCollider = nullptr;
		CHierarchyNode*		pHierachyNode = nullptr;
		_bool								bIsCollision = true;

		tagBonesColliderDesc() = default;
		tagBonesColliderDesc(CCollider* _pCollider, CHierarchyNode* _pHierachyNodes)
		{}

		void Add_Colliders(CModel* _pModel, const _char* _pNodeNames,
			const LEVEL& _eLevel, const _tchar* _pTag, void* _pArg = nullptr);
		_matrix MultiplyCollider(_matrix WorldMatrix = XMMatrixIdentity());

		_bool Is_Collision(CCollider* _pCollider);
		_bool Is_Collision(const char* _pName, CCollider* _pCollider);

		HRESULT Save(CModel* _pModel, FILEGROUP* _pFileGroup);
		HRESULT Load(CModel* _pModel, FILEDATA* _pFileData, const LEVEL& _eLevel);
#ifdef _DEBUG
		void Render();
#endif
		void Release();
	}BONESCOLLDESC;
protected:
	explicit CCharacter(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CCharacter(const CCharacter& _rhs);
	virtual ~CCharacter() = default;
public:
	virtual const char* Get_Name() { return "Character"; }
	virtual const _tchar* Get_Proto() { return L"Character"; }
	virtual const _tchar* Get_Layer() { return L"Character"; }
public:
	const _float3& Get_OffsetPosV3() const { return m_v3OffsetPosition; }
	_vector Get_OffsetPos() { return XMLoadFloat3(&m_v3OffsetPosition); }

	const _bool& Is_RestrictAttackPos() const { return m_bRestrictAttackPos; }

	_matrix Get_OffsetWorldMatrix() { return XMLoadFloat4x4(&m_OffsetMatrix); }
	_float4x4 Get_OffsetWorldMatrix4x4() { return m_OffsetMatrix; }

	const DIRECTION& Get_Direction() const { return m_eDirection; }
	const DIRECTION& Get_PrevDirection() const { return m_ePrevDirection; }
	void Set_Direction(const DIRECTION& _eDirection) { this->m_eDirection = _eDirection; }

	const DMG_PT& Get_DmgPercents() const { return m_eDmgPercents; }
	const ATK_STR& Get_AtkStrongEncounter() const { return m_eAtkStrongEncounter; }

	CHARINFO Get_CharInfo() const { return m_tCharInfo; }
	// Cur Speed
	const _float& Get_Speed() const { return m_fCurSpeed; }

	// 장비에 대한 클래스
	CEquipment* Get_Equip(const CEquipment::EQUIPTYPE& _eType) { return m_pEquipmentArr[_eType]; }

	CHierarchyNode* Get_Node(const NODES& _eNode) { return m_pNodeArr[_eNode]; }
	_vector					  Get_NodePosition(const NODES& _eNode);
	_matrix					  Get_NodeMatrix(const NODES& _eNode);
	_vector					  Get_BonePosition(const _char* _pName);
	_matrix					  Get_BoneMatrix(const _char* _pName);
	vector< BONESCOLLDESC> Get_BonesColliderDesc(){ return m_vecBones;}
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
public:
	virtual _bool Is_Collision(CCollider* _pCollider) override;

	HRESULT Damaged(CPawn* _pDamageOwner) override;
protected:
	// Ready Component
	virtual HRESULT Ready_Component() override;
	// SetUp ConstantValue
	virtual HRESULT SetUp_ConstantTable() override;
	// Find Offset Node
	virtual HRESULT Find_OffsetNode();
	// SetUp Animation Table
	HRESULT SetUp_AnimationTable();
protected:
	// Bind Equipment Status
	void		Bind_Equipment_Status();
protected:
	void Update_Bond_OtherCollider();
	// Calculate Series
	void Calculate_OffsetMatrix();
	_vector Calculate_OffsetLook();
	_vector Calculate_OffsetRight();
	_vector Calculate_OffsetMatrixNotPosition();
	// Bind Series
	void Bind_Position();
	virtual void Bind_Attack_Postion();
	virtual void Bind_Supplement_Position();
	virtual void Bind_Damaged_Position();
	virtual void Bind_Die_Position();

	_float3	Get_OffsetArrPositionV3(const NODES& _eOffsetNode);
	_vector Get_OffsetArrPosition(const NODES& _eOffsetNode);
protected:
	void Calculate_DmgPercent(const _float& _fDmgPercents);
protected:
	// Coll Tick
	virtual void Coll_Tick(_matrix matWorld) override;
#ifdef _DEBUG
	virtual _bool Add_Collider_Render() override;
#endif
protected:
	// Return Status
	void ReturnInfo();

	void Load_BonesCollider(CModel* _pModel, vector<BONESCOLLDESC>* _pBones);
	void Save_BonesCollider(CModel* _pModel, vector<BONESCOLLDESC>* _pBones);
protected:
	// TailNodeArr
	vector<BONESCOLLDESC>			m_vecBones;
	// 장비에 대한 클래스
	CEquipment*									m_pEquipmentArr[CEquipment::EQUIP_END] = { nullptr };
	// Colliders
	CCollider*											m_pRangeCollider = nullptr;
	CCollider*											m_pFirstCheckCollider = nullptr;
	// Node Arr
	CHierarchyNode*							m_pNodeArr[NODE_END] = { nullptr };
	// Direction
	DIRECTION										m_eDirection = DIR_END;
	// Direction
	DIRECTION										m_ePrevDirection = DIR_END;
	// Damage Percents
	DMG_PT												m_eDmgPercents = DMG_END;
	// Attack String 
	ATK_STR											m_eAtkStrongEncounter = AS_END;
protected:
	CHARINFO											m_tCharInfo;
	CUSTIMER											m_tHurtTimer;
protected:
	// Stage
	CStage*												m_pStage = nullptr;
	// Cur Region
	CRegion*											m_pCurRegion = nullptr;
	CNavigation*									m_pNavigation = nullptr;
	CCell*													m_pAttackPosCell = nullptr;
	_float3												m_v3RestrictPos = _float3(0.f, 0.f, 0.f);
	_bool													m_bRestrictAttackPos = false;

	// Cur Speed
	_float													m_fCurSpeed = 10.f;
	_float													m_fMaxSpeed = 10.f;
protected:
	_float4x4											m_OffsetMatrix;
	_float4x4											m_DrawAnimMatrix;
	_float3												m_v3OffsetPosition = _float3(0.f, 0.f, 0.f);
	_float3												m_v3SavePos = _float3(0.f, 0.f, 0.f);
protected:
	_bool													m_bChangeAttackMatrix = false;
	_bool													m_bEndAttack = false;
	_bool													m_bChangeSuppleCount = false;
protected:
	_bool													m_bChangeDamagedPosition = false;
	_bool													m_bEndDamagedPosition = false;
	_bool													m_bChangeDiePosition = false;
	_bool													m_bEndDiePosition = false;
protected:
	_float4												m_v4Forward = _float4(0.f, 0.f, -1.f, 1.f);
	_float4												m_v4Right = _float4(-1.f, 0.f, 0.f, 1.f);
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
protected:
	
	void Extern_ShowCurAnimation_Data();
	virtual void Control_ColliderData() override;
	virtual void Extern_ShowCurHierckeyNode(vector<BONESCOLLDESC>* _pBones);
#pragma endregion USE_TOOL_METHOD
protected:
private:
	_float			m_fStartFastSection = 0.f;
	_float			m_fEndFastSection = 0.f;
	_float			m_fFastValue = 0.f;
	_float			m_fChangeValue = 0.f;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END