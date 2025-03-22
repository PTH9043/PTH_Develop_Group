#pragma once

#include "TImGuiView.h"

BEGIN(Engine)
class UParticle;
class UFire;
END

BEGIN(Tool)

class TParticleView : public TImGuiView {
public:
	TParticleView(CSHPTRREF<UDevice> _spDevice);
	NO_COPY(TParticleView)
		DESTRUCTOR(TParticleView)
public:
	// TImGuiView을(를) 통해 상속됨
	virtual void Free() override;

	virtual HRESULT NativeConstruct() override;
protected:
	virtual HRESULT LoadResource() override;
	virtual HRESULT ReleaseResource() override;
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDetla) override;
	virtual void RenderActive() override;
private:
	void RenderMenu();
	void DockBuildInitSetting();
private:
	//Multiple Particle Setting Functions
	void MultipleParticleView();
	void ResizeMultipleParticleVector(_uint _resizeAmount);

	void MultipleParticleCountSetting();
	void MultipleParticleTimeSetting();
	void MultipleParticleTexSetting();
	void DefaultMultipleParticleSetting();
	void AutomaticMultipleParticleSetting();

	void LoadMultipleParticleResource();
	void ReleaseMultipleParticleResource();

	void SetMultipleParticle(SHPTR<UParticle> _spParticle, int index) { m_MultipleParticle[index] = _spParticle; }
	_uint GetMultipleParticleSize() { return m_iMultipleParticleSize; }
	_uint GetMultipleParticleCurActiveSize() { return m_iCurActiveMultipleParticle; }
private:
	//Single Particle Setting Functions
	void SingleParticleView();
	void ResizeSingleParticleVector(_uint _resizeAmount);

	void SingleParticleCountSetting();
	void SingleParticleTimeSetting();
	void SingleParticleTexSetting();
	void DefaultSingleParticleSetting();
	void AutomaticSingleParticleSetting();

	void LoadSingleParticleResource();
	void ReleaseSingleParticleResource();
	
	void SetSingleParticle(SHPTR<UParticle> _spParticle, int index) { m_SingleParticle[index] = _spParticle; }
	_uint GetSingleParticleSize() { return m_iSingleParticleSize; }

private:
	//Anim Particle Setting Functions
	void AnimParticleView();
	void ResizeAnimParticleVector(_uint _resizeAmount);
	
	void AnimParticleCountSetting();
	void AnimParticleTimeSetting();
	void AnimParticleTexSetting();
	void DefaultAnimParticleSetting();
	void AutomaticAnimParticleSetting();

	void LoadAnimParticleResource();
	void ReleaseAnimParticleResource();


private:
	using PARTICLES = VECTOR<SHPTR<UParticle>>;
	using PARTICLEPARAMS = VECTOR<PARTICLEPARAM*>;
	using PARTICLETYPES = VECTOR<ComputeParticleType*>;
	
private:
	MAINDESC													m_stMainDesc;
	DOCKDESC												m_stSingleParticleView;
	DOCKDESC												m_stMultiParticleView;
	DOCKDESC												m_stAnimParticleView;

	VECTOR<SHPTR<TImGuiView>>			m_CloseImGuies;
	SET<SHPTR<TImGuiView>>					m_OpenImGuies;

	_double														m_dShowDeltaTime;
	_bool															m_isInitSetting;

	
	_bool														m_isResetParticle;

	_int m_iSingleParticleSize;
	PARTICLES m_SingleParticle;
	PARTICLEPARAMS m_SingleParticleParam;
	PARTICLETYPES m_SingleParticleType;

	_int m_iMultipleParticleSize;
	PARTICLES m_MultipleParticle;
	PARTICLEPARAMS m_MultipleParticleParam;
	PARTICLETYPES m_MultipleParticleType;
	_int m_iCurActiveMultipleParticle;

	_int m_iAnimParticleSize;
	PARTICLES m_AnimParticle;
	PARTICLEPARAMS m_AnimParticleParam;
	PARTICLETYPES m_AnimParticleType;

	

};


END