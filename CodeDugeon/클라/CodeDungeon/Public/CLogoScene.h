#pragma once
#include "UScene.h"
BEGIN(Engine)

END

BEGIN(Client)
class CImageUI;
class CButtonUI;
class CLoadingUI;

class CLogoScene   final : public UScene {
public:
	CLogoScene(CSHPTRREF<UDevice> _spDevice);
	NO_COPY(CLogoScene)
	DESTRUCTOR(CLogoScene)
public:
	// UScene을(를) 통해 상속됨
	virtual void Free() override;
	virtual HRESULT LoadSceneData() override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void LateTick(const _double& _dTimeDelta) override;
private:
	SHPTR<CImageUI>			m_spBackgroundUI;
	SHPTR<CImageUI>			m_spMainTitleUI;
	SHPTR<CImageUI>			m_spMainTitleEffectUI;
	SHPTR<CImageUI>			m_spLineEffectUI;
	SHPTR<CImageUI>			m_spBackEffectUI;

	SHPTR<CImageUI>			m_spLoadingBackgroundUI;
	SHPTR<CLoadingUI>			m_spLoadingFillingUI;

	SHPTR<CImageUI>			m_spLoadingTextUI;
	SHPTR<CLoadingUI>			m_spLoadingDotsUI;
	SHPTR<CLoadingUI>			m_spPleaseWaitTextUI;

	SHPTR<CButtonUI>			m_spEnterButtonUI;
	SHPTR<CButtonUI>			m_spExitButtonUI;
	_float m_fStartSceneLoadingTimer = 0;
	_bool m_bStartSceneForUI = false;
	_bool m_bStartGameForUI = false;
};

END

