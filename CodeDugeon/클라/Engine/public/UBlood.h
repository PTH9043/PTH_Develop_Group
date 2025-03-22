#pragma once
#include "UPawn.h"

BEGIN(Engine)

class UTexGroup;
class UVIBufferRect;
class UShaderConstantBuffer;

class UBlood : public UPawn {
public:
	UBlood(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	UBlood(const UBlood& _rhs);
	DESTRUCTOR(UBlood)
public:
	CLONE_MACRO(UBlood, "UBlood::Clone To Failed")
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
	SHPTR<UTexGroup>				m_spBloodTexGroup;

private:
	SHPTR<UShaderConstantBuffer>											m_spTransformConstantBuffer;
	SHPTR< UShaderConstantBuffer>						m_spTimerBuffer;
	TRANSFORMPARAM																	 m_stFinalRenderTransformParam;
	// Cube
	SHPTR< UVIBufferRect>								m_spVIBufferRect;
	BLOODTIMER _bloodTimer;
public:
	CSHPTRREF<UTexGroup> GetTextureGroup() const { return m_spBloodTexGroup; }
	void SetColorTexture(const _wstring& TexName);
	void SetColorTexture(_uint _index);

	_uint ColorTextureIndex = 0;

	void SetTimer(_float _setTime);
	_bool CheckTimeOver();

};

END