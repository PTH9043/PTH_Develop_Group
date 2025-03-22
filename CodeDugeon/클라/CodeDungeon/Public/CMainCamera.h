#pragma once

#include "UCamera.h"
#include "UNavigation.h"

BEGIN(Client)

class CMainCamera : public UCamera {
public:
	CMainCamera(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	CMainCamera(const CMainCamera& _rhs);
	DESTRUCTOR(CMainCamera)
public:
	CLONE_MACRO(CMainCamera, "CMainCamera::Clone To Failed")
		virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
public: /* get set */
	const _bool IsMoveState() const { return m_isMoveState; }
	void SetMoveState(const _bool _isMoveState) { this->m_isMoveState = _isMoveState; }
	SHPTR<UNavigation> GetCurrentNavi() const { return m_spCurNavi; }
protected:
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;

private:
	_bool				m_isMoveState;
	_float3				m_vPrevPos;

	SHPTR<UNavigation> m_spCurNavi;

};

END