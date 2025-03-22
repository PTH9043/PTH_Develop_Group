#pragma once

#include "TImGuiView.h"

/*
@ Date: 2024-02-03, Writer: 이성현
@ Explain
- Imgui의 메인 뷰
*/
BEGIN(Tool)
class TCameraView : public TImGuiView {
public:
	TCameraView(CSHPTRREF<UDevice> _spDevice);
	NO_COPY(TCameraView)
	DESTRUCTOR(TCameraView)
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

	void CameraView();
private:
	MAINDESC													m_stMainDesc;
	DOCKDESC												m_stCameraView;

	VECTOR<SHPTR<TImGuiView>>			m_CloseImGuies;
	SET<SHPTR<TImGuiView>>					m_OpenImGuies;

	_double														m_dShowDeltaTime;
	_bool															m_isInitSetting;
};
END