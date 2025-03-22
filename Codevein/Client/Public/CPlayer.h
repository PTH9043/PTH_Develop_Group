#pragma once
#include "CCharacter.h"
#include "CPlayerStateMachine.h"
#include "CCamera_Free.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CGameInstance;
END

BEGIN(Client)
class CHpBar;
class CMpBar;
class CCheckPoints;
class CPlayerUIController;
class CMonster;
class C2DEffectParticle;
class CPlayerEPControlGroup;
class CMuffler;
class CPotal;

class CPlayer final : public CCharacter
{
public:
	typedef struct tagPlayerDesc
	{
	public:
		tagPlayerDesc() = default;

		GOBCLIENTDESC tGobData;
		CHARDESC			 tCharDesc;
	}PLAYERDESC;
protected:
	typedef struct tagTargetMonsterDesc
	{
		tagTargetMonsterDesc() = default;
		tagTargetMonsterDesc(CMonster* _pMonster, const _float& _fDistance)
			: pMonster(_pMonster), fDistance(_fDistance)
		{}

		CMonster* pMonster = nullptr;
		_float			  fDistance = 0.f;
	}TARGETMONSTERDESC;
protected:
	explicit CPlayer(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CPlayer(const CPlayer& _rhs);
	virtual ~CPlayer() = default;
public:
	void Set_CameraObject(CCamera_Free* _pCamFree);
public:
	virtual const char* Get_Name() { return "Player"; }
	virtual const _tchar* Get_Proto() { return PROTO_GOB_PLAYER; }
	virtual const _tchar* Get_Layer() { return LAYER_PLAYER; }
public:
	CCamera_Free* Get_PlayerCam() { return m_pCamerObject; }
	CEquipment* Get_Equip(const CEquipment::EQUIPTYPE& _eType){ return m_pEquipmentArr[_eType]; }
	CPlayerStateMachine* Get_PlayerStateMachine() { return m_pPlayerStateMachine; }
	const _bool& Is_LevelEnd() const { return m_bLevelEnd; }
public:
	void Find_CheckPoints(CCheckPoints* _pCheckPoints) { m_pCheckPoints = _pCheckPoints; }
	void Find_Potal(CPotal* _pPotal) { this->m_pPotal = _pPotal; }
public:
	// Add Arround Monster
	void Add_ArroundMonster(class CMonster* _pMonster);
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	// Damaged
	virtual HRESULT Damaged(CPawn* _pDamageOwner) override;
public:
	_float DamageToDot_DirToSelfLook();
	_float DamageToDot_DirToSelfRight();
	_float DamageToDot_DirToSelfLookConverter();
private:
	// Ready Component
	virtual HRESULT Ready_Component() override;
	// SetUp ConstantValue
	virtual HRESULT SetUp_ConstantTable() override;
	// Ready State Machine
	HRESULT Ready_Others();
	// Find Camera 
	void Find_Camera();
	// Restrict AnimationToMoveOffset
	void Restrict_AnimationToMoveOffset();
	// Lock On TargetMonster
	void LockOn_TargetMonster(CGameInstance* _pGameInstance);
protected:
	// CamControl
	void Camera_Control_Update(const _double& _dTimeDelta);
	// Late Updat 
	void Camera_Control_Late_Update(const _double& _dTimeDelta);
	// Get CamToDirection
	_vector Get_CamToDirection(CTransform* _pTransform);
	// Control Camera Offset Position
	void Control_CameraOffsetPosition();
protected:
	// Calculate Offset Position
	void Calculate_OffsetPosition();
	void Bind_Roll_Position();
	virtual void Bind_Attack_Postion() override;
	virtual void Bind_Supplement_Position() override;
	void Bind_SpecailAttack_Position();
	void Bind_Hit_Position();
	void End_Render();
protected:
	void Add_AroundPossibleAttackMonster();
	// Reset Arround Monster
	void Reset_AroundMonster();
protected:
	// Move
	void Move(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void Attack(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void Attack_Sp(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void Target(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void Healing(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void Hit(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void Skill(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void OpenSight(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void Ect_Behavior(const _double& _dTimeDelta, CGameInstance* _pGameInstance);
	void Dead(const _double& _dTimeDelta, CGameInstance* _pGameInstance);

protected:
	void Find_CheckPoints_Ect_Behavior(const _double& _dTimeDetla, CGameInstance* _pGameInstnace);
	void Find_Potal_Ect_Behavior(const _double& _dTimeDetla, CGameInstance* _pGameInstnace);
private:
	// State Machine
	CPlayerStateMachine* m_pPlayerStateMachine = nullptr;
	// 
	CPlayerEPControlGroup* m_pPlayerEPControlGroup = nullptr;
	// Get CameraFree
	CCamera_Free*				m_pCamerObject = nullptr;
	// TargetMonster
	CMonster*						m_pTargetMonster = nullptr;
	// Hp Bar
	CHpBar*							m_pHpBar = nullptr;
	CMpBar*							m_pMpBar = nullptr;
	CCheckPoints*				m_pCheckPoints = nullptr;
	CPotal*								m_pPotal = nullptr;
	CPlayerUIController*		m_pPlayerUIController = nullptr;
	CMuffler*							m_pMuffler = nullptr;
private:
	_bool									m_bChangeRollMatrix = false;
	// Arround Monster List
	list<class CMonster*>	m_AroundMonsterList;
private:
	// Charge Attack
	_bool									m_bChargeAttack = false;
	_bool									m_bChangeSpecialAttackMatrix = false;
	_bool									m_bChangeDamagedMatrix = false;
	_bool									m_bEndSpecialAttack = false;
	_bool									m_bEndDamagedMatrix = false;
	_bool									m_bEndRollMatrix = false;
	_bool									m_bNotMove = false;
	_bool									m_bBindActiveCheckPoint = false;
	_bool									m_bLevelEnd = false;
private:
	_float3								m_v3DamagedPos = _float3(0.f, 0.f, 0.f);
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
	static CPlayer* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END