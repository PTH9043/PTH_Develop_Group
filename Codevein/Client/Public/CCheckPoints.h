#pragma once
#include "CWorldObj.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CCheckPoints final : public CWorldObj
{
public:
	void Set_Activation(const _bool& _bIsActivation) { this->m_bActivation = _bIsActivation; }
	const _bool& Is_Activation() const { return m_bActivation; }
protected:
	explicit CCheckPoints(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCheckPoints(const CCheckPoints& _rhs);
	virtual ~CCheckPoints() = default;
public:
	virtual const char* Get_Name() override;
	virtual const _tchar* Get_Proto() override { return PROTO_GOB_CHECKPOINT; }
	virtual const _tchar* Get_Layer() override { return LAYER_CHECKPOINTS; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
protected:
	// Ready Component 
	virtual HRESULT Ready_Component() override;

protected:
	// Find Player
	void Find_Player();
	// Check Collision To Player
	void Check_CollisionToPlayer();
protected:
	// Player 
	class CPlayer*				m_pPlayer = nullptr;
	// Activation
	_bool								m_bActivation = false;
public:
#pragma region USE_TOOL_METHOD
	// Extern EditValue
	virtual _bool Extern_EditValue() override;
	// Extern Hierakey Value
	virtual _bool Extern_HierakeyView(const string& _str) override;
	// Create Self 
	virtual HRESULT Create_Self(CGameObject** _ppOut) override;
	// Delete Self
	virtual HRESULT Delete_Self() override;
	// Save Data 
	virtual HRESULT Save_Data() override;
	// Load Data 
	virtual HRESULT Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData = nullptr) override;
	// Get Picking
	virtual _bool Is_Picking(_float3* _pOut = nullptr) override;
	// Picking Nav 
	_bool Is_PickingNav();
	// Reset Data 
	virtual void Reset_Data() override;
#pragma endregion USE_TOOL_METHOD
protected: 
	// Show Model
	virtual HRESULT Show_Model() override;
public:
	static CCheckPoints* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END