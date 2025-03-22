#pragma once
#include "Player_KeyBoard_Controller.h"

BEGIN(Engine)
class CGameInstance;
class CTransform;
class CAnimationClip;
END

BEGIN(Client)
class CCamera_Free;
class CPlayer;
class CSabito;

class CSabito_KeyBoard_Controller final : public CPlayer_KeyBoard_Controller
{
private:
	explicit CSabito_KeyBoard_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CSabito_KeyBoard_Controller() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(class CGameObject* _pGameObject) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
public:
	// 데이터를 내부에 집어 넣는 함수
	virtual void Insert_Datas(VOIDDATAS _pData);
	// 데이터를 바깥으로 빼는 함수
	virtual  VOIDDATAS Out_Datas();
protected:
	// Change State
	void Change_State(const _double& _dTimeDelta);
	// Move State
	void Move_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CTransform* _pTargetTransform, CTransform* _pCameraTransform);
	// Judge Charge State 
	void Judge_ChargeState(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CTransform* _pTargetTransform, CTransform* _pCameraTransform);
protected:
	// Find Target Player
	void Find_TargetPlayer(CGameInstance* _pGameInstance);
	// Find Camera Free
	void Find_CameraFree(CGameInstance* _pGameInstance);
protected:
	// Return Idle State
	_bool Judge_Return_IdleState(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CTransform* _pTargetTransform, CTransform* _pCameraTransform);
	// Judge Attack
	_bool Judge_Attack(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CTransform* _pTargetTransform, CTransform* _pCameraTransform);
	// Judge Step
	_bool Judge_Step(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CTransform* _pTargetTransform, CTransform* _pCameraTransform);
	// Judge Guard
	_bool Judge_Gaurd(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CTransform* _pTargetTransform, CTransform* _pCameraTransform);

protected:
	// Judge Step For And Back Situation
	void Judge_StepForAndBack_Situation(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CTransform* _pTargetTransform, CTransform* _pCameraTransform);
protected: 
	CSabito*					m_pSabito = nullptr;
protected:
	CPlayer*					m_pTargetPlayer = nullptr;
	CCamera_Free*		m_pCamera = nullptr;
	// Current Animation Clip
	CAnimationClip*		m_pCurAnimationClip = nullptr;
	// Current Animation Clip
	CAnimationClip*		m_pPrevAnimationClip = nullptr;
	// Cur Next Prev State Data 
	_uint							m_iCurStateData = 0;
	_uint							m_iNextStateData = 0;
	// Direction
	_uint							m_iDir = 0;
	// Cus Timer
	CUSTIMER					m_tChargeStateTimer = CUSTIMER(3.f);
	// Change direction
	_bool							m_bChangeDirection_StepLR = false;
public:
	static CSabito_KeyBoard_Controller* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, class CGameObject* _pGameObject);
	virtual void Free() override;
};

END