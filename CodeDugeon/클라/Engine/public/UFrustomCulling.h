#pragma once
#include "UBase.h"

BEGIN(Engine)
class UCamera;

class UFrustomCulling : public UBase {
	enum PLANE_TYPE : _uint
	{
		PLANE_FRONT,
		PLANE_BACK,
		PLANE_UP,
		PLANE_DOWN,
		PLANE_LEFT,
		PLANE_RIGHT,
		PLANE_END
	};
public:
	UFrustomCulling();
	NO_COPY(UFrustomCulling)
		DESTRUCTOR(UFrustomCulling)
public:
	virtual void Free() override;
	HRESULT NativeConstruct(CSHPTRREF<UCamera> _spCamera);
	void Tick();
	_bool IsContains(const _float3& _vPos, const _float _fRadius);
private:
	ARRAY<_float3, MAX_FRUSTOMSIZE>	m_arrLocalPosArr;
	ARRAY<_float3, MAX_FRUSTOMSIZE>	m_arrChangePosArr;
	ARRAY<_float4, PLANE_END>					m_arrPlaneArr;

	SHPTR<UCamera>										m_spOwnerCamera;
};

END