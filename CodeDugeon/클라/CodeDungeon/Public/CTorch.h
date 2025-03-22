#pragma once
#include "CModelObjects.h"

BEGIN(Engine)
class UModel;
class UShaderConstantBuffer;
class UFire;
class UParticle;
class UParticleSystem;
END

BEGIN(Client)
/*
@ Date: 2024-05-12, Writer: ÀÌ¼ºÇö
@ Explain
- ¸ÊÀÇ È¶ºÒ
*/
class CTorch final : public CModelObjects{
public:
	struct TORCHDESC
	{
		_float4x4		_Worldm{};
	};

	CTorch(CSHPTRREF<UDevice> _spDevice,
		const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	CTorch(const CTorch& _rhs);
	DESTRUCTOR(CTorch)
	CLONE_MACRO(CTorch, "CTorch::Clone To Failed")
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;

	SHPTR<UFire>& GetFire() { return m_spFire; }
	SHPTR<UParticle>& GetParticle() { return m_spParticle; }

private:

	SHPTR<UFire>					m_spFire;
	FIRENOISEBUFFER*				m_stFireNoiseBuffer;
	FIREDISTORTIONBUFFER*			m_stFireDistortionBuffer;

	SHPTR<UParticle>		m_spParticle;
	PARTICLEPARAM* m_stParticleParam;
	ComputeParticleType* m_stParticleType;

};

END