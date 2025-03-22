#pragma once

#include "UBase.h"

/*
@ Date: 2024-02-03, Writer: 이성현
@ Explain
- Imgui를 관리하는 Manager
*/

BEGIN(Engine)
class UDevice;
class UGpuCommand;
class UTableDescriptor;
END


BEGIN(Tool)
class TImGuiView;
class TMainView;

class TImGuiManager : public UBase {
	DECLARE_SINGLETON(TImGuiManager)
public:
	TImGuiManager();
	DESTRUCTOR(TImGuiManager)
public:
	const _bool IsResetScene() const { return m_isResetScene; }
	void SetResetScene(const _bool _isResetScene) { this->m_isResetScene = _isResetScene; }
public:
	virtual void Free() override;

	HRESULT ReadyManager(const GRAPHICDESC& _stGraphicDesc, const OUTPUTDATA& _stOutputData);
	SHPTR<TImGuiView> GetImGuiObject(const IMGTAG _eImGuiTag);

	void Tick(const _double& _dDeltaTime);
	void LateTick(const _double& _dDeltaTime);
	void Render();
private:
	HRESULT ReadyImGuiClass();
private:
	using IMGOBJECTS = UNORMAP<IMGTAG, SHPTR<TImGuiView>>;
	// IMGUI Objects 
	IMGOBJECTS												m_ImGuiObjectContainer;
	SHPTR<TMainView>								m_spMainView;


	SHPTR<UDevice>										m_spDevice;
	SHPTR<UGpuCommand>						m_spGpuCommand;
	SHPTR<UTableDescriptor>					m_spTableDesciptor;

	_bool															m_isResetScene{ false };
};

END