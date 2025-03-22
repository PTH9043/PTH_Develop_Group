#pragma once
#include "CModelObjects.h"

BEGIN(Engine)
class UModel;
class UShaderConstantBuffer;
END

BEGIN(Client)
/*
@ Date: 2024-07-21, Writer: 이성현
@ Explain
- 맵의 석상
*/
class CStatue final : public CModelObjects{
public:
	struct STATUEDESC
	{
		_float4x4		_Worldm{};
	};
	CStatue(CSHPTRREF<UDevice> _spDevice,
		const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	CStatue(const CStatue& _rhs);
	DESTRUCTOR(CStatue)
	CLONE_MACRO(CStatue, "CStatue::Clone To Failed")
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;

private:
protected:
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;
};

END