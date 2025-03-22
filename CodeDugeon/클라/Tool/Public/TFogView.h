#pragma once

#include "TImGuiView.h"

BEGIN(Engine)
class UFog;
END

BEGIN(Tool)

class TFogView : public TImGuiView {
public:
	TFogView(CSHPTRREF<UDevice> _spDevice);
	NO_COPY(TFogView)
		DESTRUCTOR(TFogView)
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
	void FogView();
	void FogColorTextureSetting();
	void FogScalingSetting();
	void FogPosSetting();
	void FogDensitySetting();
	void FogStartAndEndSetting();
	void FogTypeSetting();
	void SetRTBackColorAndFogColor();
private:
	MAINDESC													m_stMainDesc;
	DOCKDESC												m_stFogView;


	VECTOR<SHPTR<TImGuiView>>			m_CloseImGuies;
	SET<SHPTR<TImGuiView>>					m_OpenImGuies;

	_double														m_dShowDeltaTime;
	_bool															m_isInitSetting;


	
	SHPTR <UFog> m_stFog;


};


END