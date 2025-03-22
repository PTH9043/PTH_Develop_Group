#pragma once
#include "CCollision.h"

BEGIN(Engine)

class ENGINE_DLL CCubeCollision : public CCollision
{
public:
	typedef struct tagXYZDir
	{
		_cvector v3Right, v3Up, v3Look;
	}XYZDIR;
protected:
	explicit CCubeCollision(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCubeCollision(const CCubeCollision& _rhs);
	virtual ~CCubeCollision() = default;
public:
	// NativeConstruct Prototype
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	// Collision
	virtual _bool Is_CollisionPos(const _fvector& _v3MousePos) override;
	virtual _bool Is_CollisionOtherComp(CCollision* _pCollision)  override;
	virtual _bool Is_CollisionOtherComp(const TYPE& _eType, const _fvector _v3OtherPos, const _fvector& _v3OtherSize) override;
	_bool				Is_CollisionOBB(CCollision* _pCollision);
	_bool				Is_CollisionOBB(const TYPE& _eType,  class CTransform* _pOtherTransform, const _fvector& _v3Size);
private:
	_bool Is_Check_Axis(const _cvector& _v3Axis, const _cvector& _v3Dist,
		const XYZDIR& _selfDir, const XYZDIR& _otherDir);
public:
	static CCubeCollision* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END