#pragma once
#include "CScene.h"

class CText;
class CTextButton;

class CLogoScene : public CScene {
protected:
	explicit CLogoScene() : CScene(SCENE_LOGO) {}
	CLogoScene(const CLogoScene& _rhs) : CScene(_rhs) {};
	virtual ~CLogoScene() = default;
public:
	virtual void Free() override;
public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT LoadSceneData() override;

	virtual void Tick(const _double& _dTimeDelta);
	virtual void Late_Tick(const _double& _dTimeDelta);

private:
	shared_ptr<CText>				m_pTitleText{ nullptr };
	shared_ptr<CTextButton>	m_pStartButton{ nullptr };
	shared_ptr<CTextButton>	m_pEndButton{ nullptr };
};

