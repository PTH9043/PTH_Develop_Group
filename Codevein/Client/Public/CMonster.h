#pragma once
#include "CCharacter.h"

BEGIN(Engine)
class CGameInstance;
class CCollider;
class CTexture;
END

BEGIN(Client)
class CPlayer;
class CMonsterEPController;

class CMonster : public CCharacter
{
public:
	typedef struct tagSightDesc
	{
		tagSightDesc() = default;
		tagSightDesc(const _float& _fFindAngle, const _float& _fFindDistance, const _float& _fYFindAngle)
			: fFindAngle(_fFindAngle), fFindDistance(_fFindDistance), fYFindAngle(_fYFindAngle)
		{}
		_float fFindAngle = 0.f;
		_float fFindDistance = 0.f;
		_float fYFindAngle = 0.f;
	}SIGHTDESC;

	typedef struct tagMonsterDesc
	{
		explicit tagMonsterDesc() = default;
		explicit tagMonsterDesc(const _wstring& _strModelName, const SIGHTDESC& _SightDesc = SIGHTDESC())
			: strModelName(_strModelName), tSightDesc(_SightDesc)
		{}
		_wstring			strModelName = L"";
		SIGHTDESC		tSightDesc;
	}MONSTERDESC;

	enum TODIR
	{
		TD_LEFT, TD_RIGHT, TD_BACK, TD_FORWARD, TD_END
	};
protected:
	explicit CMonster(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CMonster(const CMonster& _rhs);
	virtual ~CMonster() = default;
public:
	CEquipment* Get_Equip(const CEquipment::EQUIPTYPE& _eType) { return m_pEquipmentArr[_eType]; }
	const _uint& Get_AttackPattern() const { return m_iAttackPattern; }
	const _uint& Get_AttackIndex() const { return m_iAttackIndex; }
	const _bool& Is_FindPlayer() const { return m_bFindPlayer; }
	const ATK_STR& Get_AtkStr() const { return m_eAtkStrong; }
	const _bool& Is_Damaged() const { return m_bDamaged; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
public:
	// Calculate Player To Distance
	_float PlayerToDistance();
	// Player to Dot
	_float PlayerToDot_DirToSelfLook();
	_float PlayerToDot_DirToSelfRight();
	_float PlayerToDot_LookToLook();
	_float PlayerToDot_DirToSelfLookConvert();
	_float SelfToDot_DirToPlayerLook();
	_float SelfToDot_DirToPlayerRight();
	_float SelfToDot_DirToPlayerLookConvert();
protected:
	_float PlayerToTan2ZXValue();
	_float PlayerToTan2XZValue();
	_float PlayerToGetZValue();
	_float PlayerToGetYValue();
	_float PlayerToGetXValue();
	// Rotate Look To Player
	_bool RotateLookToPlayer(const _double& _dTimeDelta, const double& _dAngleValue = 10.);
	_bool RotateLookToPlayer_JudgeNotRepeat(const _double& _dTimeDelta, const _double& _dAngleValue = 10.);
	// Just Rotate
	_bool JustRotate(const _double& _dTimeDelta, const _float& _fPMValue);
protected:
	void Calculate_DirectionToLookPlayer();
	void Calculate_DirectionToPlayerPos();
	void Move_ToDirection(const _double& _dTimeDelta);
	void Dead_Monster(const _double& _dTimeDelta);
protected :
	// Ready Component
	virtual HRESULT Ready_Component() override;
	// SetUp ConstantValue
	virtual HRESULT SetUp_ConstantTable() override;
	// Ready State Machine
	virtual HRESULT Ready_Others();
protected:
	// Find Target
	_bool Is_FindTarget(CGameInstance* _pGameInstance);
protected:
	// Find Player
	void Find_Player(CGameInstance* _pGameInstnace);
	// shoot 
	_bool IsShootRayToLook();
protected:
	// Setting Current Look Pos
	void Setting_CurrentLookPos();
protected:
	virtual void Add_AroundPossibleAttackPlayer();
	// Restrict AnimationToMoveOffset
	virtual void Restrict_AnimationToMoveOffset(class CMonsterState* _pState);
protected:
	// Coll Tick
	virtual void Coll_Tick(_matrix matWorld) override;
#ifdef _DEBUG
	virtual _bool Add_Collider_Render() override;
#endif
protected:
	virtual void Bind_Attack_Postion() override;
	virtual void Bind_Supplement_Position() override;
protected:
	// Target Player
	CPlayer*								m_pTargetPlayer = nullptr;
	// Atatck Collider
	CCollider*								m_pAttackNowRangeCollider = nullptr;
	// Disorve Texture
	CTexture*								m_pDisorveTexture = nullptr;
	// Monster EPController
	CMonsterEPController*	m_pMonsterEPController = nullptr;
	// Monster Desc
	MONSTERDESC		m_tMonsterDesc;
	// ATK STR 
	ATK_STR					m_eAtkStrong = AS_END;
	// Damaged
	_bool							m_bDamaged = false;
	_uint							m_iPlayerAttackCount = 0;
protected:
	// Attack Pattern
	_uint							m_iAttackPattern = 0;
	_uint							m_iAttackIndex = 0;
	_int								m_iPMMuiltpleValue = 0;
protected:
	_float							m_fRotationStandardValue = 0.f;
	_float							m_fRotValue = 0.f;
protected:
	_bool							m_bFindPlayer = false;
	_bool							m_bRotationDot = false;
	_uint							m_iRotMultipleValue = 1;
protected:
	_float							m_fLookToRotateValue = 0.f;
	_bool							m_bInitRotate = false;
	_bool							m_bDead = false;
	_ubyte						m_byPassIndex = 0;
	_float							m_DeadTime_fPlusTimeDelta = 0.f;
protected:
	_float							m_fDrawRadius = 0.f;
private:
	_uint							m_iRotateCount = 0;
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
	// Control Collider Data 
	virtual void Control_ColliderData() override;
protected:
	void Extern_ModifySightDesc();

#pragma endregion USE_TOOL_METHOD
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END