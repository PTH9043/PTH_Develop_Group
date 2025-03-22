#pragma once
#include "CGameObject_Client.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CPawn : public CGameObject_Client
{
public:
	enum PAWNTYPE
	{
		P_CHARACTER, P_ITEM, P_END
	};

	typedef struct tagCollData
	{
		_float3						v3CollScale = _float3(0.f, 0.f, 0.f);
		_float3						v3Translation = _float3(0.f, 0.f, 0.f);
	}COLLDATA;

	typedef struct tagPawnStatus
	{
	public:
		tagPawnStatus() = default;
		tagPawnStatus(const _uint& _iLevel ,const _uint& _iHp, const _uint& _iEnergy,
			const _uint& _iPower, const _uint& _iShield, const _uint& _iSpeed, const _float& _fWeight,
			const _float& _fSoul, const _float& _fExp)
			: iLevel(_iLevel), iHealth(_iHp), iEnergy(_iEnergy), iPower(_iPower), iShield(_iShield),
			iSpeed(_iSpeed), fWeight(_fWeight), fSoul(_fSoul), fExp(_fExp)
		{}

		tagPawnStatus operator+=(const tagPawnStatus& _tPawnStatus)
		{
			tagPawnStatus tPawnDesc = *this;

			tPawnDesc.iPower += _tPawnStatus.iPower;
			tPawnDesc.iEnergy += _tPawnStatus.iEnergy;
			tPawnDesc.iShield += _tPawnStatus.iShield;
			tPawnDesc.iSpeed += _tPawnStatus.iSpeed;
			tPawnDesc.fWeight += _tPawnStatus.fWeight;

			return tPawnDesc;
		}

		tagPawnStatus operator + (const tagPawnStatus& _tPawnStatus)
		{
			tagPawnStatus tPawnDesc = *this;

			tPawnDesc.iPower += _tPawnStatus.iPower;
			tPawnDesc.iEnergy += _tPawnStatus.iEnergy;
			tPawnDesc.iShield += _tPawnStatus.iShield;
			tPawnDesc.iSpeed += _tPawnStatus.iSpeed;
			tPawnDesc.fWeight += _tPawnStatus.fWeight;

			return tPawnDesc;
		}

		_uint		iLevel = 0;
		_uint		iHealth = 0;
		_uint		iEnergy = 0;
		_uint		iPower = 0;
		_uint		iShield = 0;
		_uint		iSpeed = 0;
		_float		fWeight = 0.f;
		_float		fSoul = 0.f;
		_float		fExp = 0.f;
	}PAWNSTATUS;
protected:
	explicit CPawn(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CPawn(const CPawn& _rhs);
	virtual ~CPawn() = default;
public:
	const PAWNSTATUS& Get_PawnStatus() const { return m_tPawnStatus; }
	const PAWNTYPE& Get_PawnType() const { return m_ePawnType; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	// Damaged
	virtual HRESULT Damaged( CPawn* _pDamageOwner);
public:
	virtual _bool Is_Collision(CCollider* _pCollider) { return false; }
protected:
	// Ready Component
	virtual HRESULT Ready_Component();
	// SetUp ConstantValue
	virtual HRESULT SetUp_ConstantTable();
protected:
	// Coll Tick
	virtual void Coll_Tick(_matrix matWorld);
#ifdef _DEBUG
	virtual _bool Add_Collider_Render();
#endif
protected:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CModel*			m_pModelCom = nullptr;
	CCollider*			m_pCollCollider = nullptr;
protected:
	PAWNSTATUS  m_tPawnStatus;
	PAWNSTATUS	m_tSaveStatus;
	PAWNTYPE		m_ePawnType;
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
	// Show Pawn Data
	virtual void Extern_PawnData();
	virtual void Control_ColliderData();
#pragma endregion USE_TOOL_METHOD
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END