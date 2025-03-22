#pragma once
#include "CModelObjects.h"

BEGIN(Engine)
class UModel;
class UShaderConstantBuffer;
END

BEGIN(Client)
/*
@ Date: 2024-05-12, Writer: ¿Ãº∫«ˆ
@ Explain
- ∏ ¿« √∂¿Â
*/
class CCoreMinotaur final : public CModelObjects{
public:
	struct COREMINOTAURDESC
	{
		_float4x4		_Worldm{};
	};

	CCoreMinotaur(CSHPTRREF<UDevice> _spDevice,
		const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	CCoreMinotaur(const CCoreMinotaur& _rhs);
	DESTRUCTOR(CCoreMinotaur)
	CLONE_MACRO(CCoreMinotaur, "CCoreMinotaur::Clone To Failed")
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