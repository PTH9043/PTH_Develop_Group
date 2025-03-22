#pragma once
#include "Client_Defines.h"
#include "AI_CharController.h"

BEGIN(Client)
class CZako;

class CAIController_Zako final : public CAI_CharController
{
public:
	enum STATE 
	{
		STATE_IDLE = 1,
		STATE_MOVE = 2,
		STATE_ATTACK = 3,
		STATE_DASH = 4,
		STATE_DEAD = 5,
		STATE_BEATTACKED = 6,
		STATE_GUARD = 7,
		STATE_JUMP = 8,

		STATE_END = 0
	};

protected:
	explicit CAIController_Zako(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CAIController_Zako(void) = default;
public:
	virtual HRESULT NativeConstruct_Prototype(class CGameObject* _pGameObject) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
protected:
	// Judge State
	virtual void Judge_State(const _double& _dTimeDelta) override;
public:
	static CAIController_Zako* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		class CGameObject* _pGameObject);
	virtual void Free(void) override;
private: 
	CZako* m_pZako = nullptr;
};

END