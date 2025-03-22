#pragma once
#include "Client_Defines.h"
#include "AI_Controller.h"

BEGIN(Engine)

class CAnimator;
class CAnimationClip;
END

BEGIN(Client)

class CAI_CharController abstract : public CAI_Controller
{
public:
	enum AICONTROLLER_DATA
	{
		AICON_STOP, AICON_END
	};
public:
protected:
	explicit CAI_CharController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CAI_CharController() = default;
public:
	
public:
	virtual HRESULT NativeConstruct_Prototype(class CGameObject* _pGameObject) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	// Fill AI Controller Data
	virtual VOIDDATAS* FillOut_AIControllerData(VOIDDATAS* _pDatas);
protected:
	// Judge State
	virtual void Judge_State(const _double& _dTimeDelta);
	// Find Target
	void Find_Target();
protected:
	_uint Judge_Direction(const _float& _fDot);

protected:
	// 이전 애니메이션 타입
	_string		m_strPrevAnimationType = "";
	// 현재 애니메이션 타입
	_string		m_strCurrentAnimationType = "";
	// Ai Con
	_bool			m_bStopAI = false;
	// Direction
	_float3		m_vDirection = _float3(0.f, 0.f, 0.f);
public:
	virtual void Free() override;
};

END