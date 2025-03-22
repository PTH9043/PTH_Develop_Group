#pragma once
#include "CBase.h"

BEGIN(Engine)
class CCollider;
class CGameObject;
class CTransform;

class ENGINE_DLL CRayCast : public CBase
{
	DECLARE_SINGLETON(CRayCast);
private:
	explicit CRayCast();
	virtual ~CRayCast() = default;

public:

protected:

public:
	virtual void Free() override;
};

END