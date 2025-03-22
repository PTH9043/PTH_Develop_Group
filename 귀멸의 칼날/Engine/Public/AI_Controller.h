#pragma once
#include "Controller.h"

BEGIN(Engine)
class CAnimator;
class CAnimationClip;
struct TRANSITION;

class ENGINE_DLL CAI_Controller abstract : public CController
{
protected:
	explicit CAI_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CAI_Controller() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(class CGameObject* _pGameObject) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	// Target Object
	void Set_TargetObject(class CGameObject* _pGameObject);
	// Fill AI Controller Data
	virtual VOIDDATAS* FillOut_AIControllerData(VOIDDATAS* _pDatas);
protected:
	// Target Object 
	CGameObject*						m_pTargetObject = nullptr;
public:
	virtual void Free() override;
};

END