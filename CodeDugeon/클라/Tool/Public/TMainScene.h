#pragma once

#include "UScene.h"

BEGIN(Engine)
class UGrid;
END

BEGIN(Tool)

class TMainCamera;
class TTestObject;
class TGuizmoManager;

class TMainScene : public UScene {
public:
	TMainScene(CSHPTRREF<UDevice> _spDevice);
	NO_COPY(TMainScene)
		DESTRUCTOR(TMainScene)
public:
	// UScene을(를) 통해 상속됨
	virtual void Free() override;
	virtual HRESULT LoadSceneData() override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void LateTick(const _double& _dTimeDelta) override;

	SHPTR<UGrid>		GetGrid() { return		m_spGrid; }
private:
	// Main
	SHPTR<TMainCamera>			m_spMainCamera;
	SHPTR<UGrid>				m_spGrid;


};

END
