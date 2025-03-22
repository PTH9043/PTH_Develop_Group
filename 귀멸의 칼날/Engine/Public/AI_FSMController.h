#pragma once
#include "AI_Controller.h"

BEGIN(Engine)
class CAI_State;

class ENGINE_DLL CAI_FSMController abstract : public CAI_Controller
{
	// ex
protected:
		explicit CAI_FSMController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CAI_FSMController() = default;
public:
	CAI_State* Get_CurrentState() { return m_pCurState; }
	CAI_State* Get_PrevState() { return m_pPrevState; }
	const char* Get_CurAnimation_Name();
	_uint Get_CurAnimation_Index();
	_uint Get_State();
public:
	virtual HRESULT NativeConstruct_Prototype(class CGameObject* _pGameObject)  override;
	virtual void Tick(const _double& _dTimeDelta)  override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
public:
	void Change_AIState(const _uint& _iID);
protected:
	// AI State List
	vector<CAI_State*>			m_AIStateList;
	// Current State
	CAI_State*							m_pCurState = nullptr;
	CAI_State*							m_pPrevState = nullptr;
public:
	virtual void Free() override;
};

END
