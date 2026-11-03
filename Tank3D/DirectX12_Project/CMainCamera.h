#pragma once
#include "CCamera.h"

class CPipeLine;
class CEmptyObject;
class CTransform;

class CMainCamera : public CCamera {
protected:
	explicit CMainCamera() = default;
	explicit CMainCamera(const CMainCamera& _rhs) : CCamera(_rhs) {}
	virtual ~CMainCamera() = default;
public:
	virtual shared_ptr<CActor> Clone(const VOIDDATAS& _tDatas);
	// Free
	virtual void Free() override;

public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas) override;
public:
	virtual HRESULT Tick(const _double& _dTimeDelta);
	virtual HRESULT Late_Tick(const _double& _dTimeDelta);
	virtual HRESULT Render(const HDC& _hDC);
protected:
	// Chase To Posion
	void ChaseToPosition(const _double& _dTimeDelta);
public:
	// Set Move TR
	void Set_MoveTR(const shared_ptr<CTransform>& _pTransform) { this->m_pMoveTR = _pTransform; }
private:
	// Restrict Distance 
	const _float								RESTRIC_DISTANCE_SQ = 100;
	weak_ptr<CTransform>			m_pMoveTR{  };
};

