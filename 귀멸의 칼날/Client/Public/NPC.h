
#pragma once
#include "Pawn.h"
#include "NPC_Item.h"

BEGIN(Engine)
class CAnimator;

END

BEGIN(Client)

class CNPC : public CPawn
{
public:
	enum NPCType {NPC_CANDIDATE, NPC_SIT_CANDIDATE, NPC_DEAD_CANDIDATE, NPC_END}; //스토리맵 NPC
	enum RATTANType { NPC_AOI, NPC_SISTER_L, NPC_SISTER_R, NPC_ZENITSU, NPC_KANAWO, RATTAN_CANDIDATE, NPC_GENYA, RATTAN_END }; //등나무 맵 NPC

	enum TRIGGERTYPE {TURN, START, TALK, TALK_END, WALK, STAND, SCARY,NPC_TALK, TRIGGER_END}; //트리거
	
protected:
	explicit CNPC(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CNPC(const CNPC& _rhs);
	virtual ~CNPC() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Extern_ImGuiClass() override;


public:
	void Change_Model(_uint Type);
	void Setting_Positon(_uint Type);
	void Setting_Trigger(const _double& _dTimeDelta);
	void Change_State(_uint TriggerType);

public:
	_bool Get_PopRender() { return m_PopRender; }
	_uint Get_Trigger() { return CurrentState; }
	_uint Get_Type() { return m_iType; }

private:
	_uint m_iType = NPC_END;
	_uint m_RattanType = NPC_END;
	_float3 m_Length = {0.f,0.f,0.f };
	_float3 m_CamLength = { 0.f,0.f,0.f };
	_uint PrevState = TRIGGER_END;
	_uint CurrentState = TRIGGER_END;

	_bool				m_PopText = false;
	_bool				m_PopRender = false;	

	_float				m_Angle = 0.f;
	_bool				m_WalkStop = false;


protected:
	// Set Up Constant
	virtual HRESULT SetUp_ConstantTable() override;
	// SetUp Components
	virtual HRESULT SetUp_Components() override;

private:
	CModel* m_pModel = nullptr;
	CModel* m_pAllAnimModel = nullptr;
	CAnimator* m_pAnimator = nullptr;
	CNPC_Item* m_pItem = nullptr;
public:
	vector <CModel*> m_ModelList;
	vector <CAnimator*> m_AnimatorList;
	vector <CNPC_Item*> m_ItemList;

public:
	static CNPC* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
