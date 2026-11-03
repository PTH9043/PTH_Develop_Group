#pragma once
#include "CBase.h"

class CPipeLine;
class CActor;
class CTransform;

class CPicking : public CBase {
public:
	typedef struct tagPickDesc {
		weak_ptr<CActor>				pActor{ };
		weak_ptr<CTransform>	pTransform{ };
		_float										fDist{ 0.f };
	}PICKDESC;
protected:
	explicit CPicking() = default;
	virtual ~CPicking() = default;

public:
	virtual void Free() override;
public:
	HRESULT NativeConstruct(const RECT& _tDesc);
	// Tick
	void Tick_RayInWorldSpace(class CGameInstance* _pGameInstance);

private:
	// Pipe Line
	shared_ptr<CPipeLine>	m_pPipeLine{ nullptr };
	// Window Rect
	const RECT&						m_tWindowRect;
	// Ray Dir, Pos
	_float3								m_vRayDir{};
	_float3								m_vRayPos{};
};

