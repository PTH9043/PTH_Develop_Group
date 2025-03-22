#pragma once
#include "UPawn.h"

BEGIN(Engine)
class UVIBufferSphere;

class UDefaultSphere : public UPawn{
public:
	UDefaultSphere(CSHPTRREF<UDevice> _spDevice,  const _wstring & _wstrLayer, const CLONETYPE & _eCloneType);
	UDefaultSphere(const UDefaultSphere& _rhs);
	DESTRUCTOR(UDefaultSphere)
public:
	CLONE_MACRO(UDefaultSphere, "UDefaultSphere::Clone To Failed")
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
protected:
	// Tick, LateTick, Render
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
	// Damaged
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;
private:
	// Cube
	SHPTR< UVIBufferSphere>		m_spVIBufferSphere;
};

END