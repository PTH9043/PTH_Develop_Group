#pragma once
#include "UPawn.h"

BEGIN(Engine)

class UTexGroup;
class UVIBufferRect;
class UShaderConstantBuffer;

class UDust : public UPawn {
public:
	UDust(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	UDust(const UDust& _rhs);
	DESTRUCTOR(UDust)
public:
	CLONE_MACRO(UDust, "UDust::Clone To Failed")
		virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;
protected:
	// Tick, LateTick, Render
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
protected:
	SHPTR<UTexGroup>				m_spDustTexGroup;

private:

	SHPTR< UShaderConstantBuffer>						m_spTimerBuffer;
	SHPTR< UVIBufferRect>								m_spVIBufferRect;
	BLOODTIMER _bloodTimer;
public:
	CSHPTRREF<UTexGroup> GetTextureGroup() const { return m_spDustTexGroup; }
	void SetColorTexture(const _wstring& TexName);
	void SetColorTexture(_uint _index);

	_uint ColorTextureIndex = 0;
	void SetTimer(_float _setTime);
	_bool CheckTimeOver();
	_float3 ScaleValue;

};

END