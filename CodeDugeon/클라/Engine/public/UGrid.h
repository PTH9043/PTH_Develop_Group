#pragma once

#include "UPawn.h"

BEGIN(Engine)

class UVIBuffer;
class UShaderConstantBuffer;
class UParticle;



class UGrid : public UPawn {
public:
	UGrid(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	UGrid(const UGrid& _rhs);
	DESTRUCTOR(UGrid)
public:
	CLONE_MACRO(UGrid, "UGrid::Clone To Failed")
		virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;

	CSHPTRREF<UCollider> GetCollider() { return m_spCollider; }
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
	SHPTR< UVIBuffer>								m_spVIBufferGrid;
	SHPTR<UCollider>								m_spCollider;
	//SHPTR<UParticle>		m_spParticle;


};

END
