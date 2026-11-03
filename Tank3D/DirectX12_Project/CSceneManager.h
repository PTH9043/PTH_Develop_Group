#pragma once
#include "CBase.h"

class CScene;

class CSceneManager : public CBase {
protected:
	explicit CSceneManager() = default;
	explicit CSceneManager(const CSceneManager& _rhs) :CBase(_rhs) {};
	virtual ~CSceneManager() = default;
public:
	virtual void Free() override;

public:
	HRESULT NativeConstruct() { return S_OK; }
	// RegisterScene
	void RegisterScene(const shared_ptr<CScene>& _pScene);

	void Tick(const _double& _dTimeDelta);
	void Late_Tick(const _double& _dTimeDelta);
public:
	const shared_ptr<CScene>& Get_CurScene() const { return m_pCurScene; }
private:
	// scene
	shared_ptr<CScene>			m_pCurScene = nullptr;
};

