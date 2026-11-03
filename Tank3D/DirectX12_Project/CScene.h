#pragma once
#include "CBase.h"

class CMonster;
class CCharacter;
enum MONTYPE;

typedef vector<shared_ptr<CMonster>> MONVECTOR;
typedef unordered_map< MONTYPE, MONVECTOR> MONCONTAINER;

class CScene abstract : public CBase{
protected:
	explicit CScene() = default;
	explicit CScene(const SCENE _eScene) : m_eSceneNum(_eScene){}
	CScene(const CScene& _rhs) :CBase(_rhs) {};
	virtual ~CScene() = default;

public:
	virtual void Free() override;
public:
	virtual HRESULT NativeConstruct();
	virtual HRESULT LoadSceneData();

	virtual void Tick(const _double& _dTimeDelta);
	virtual void Late_Tick(const _double& _dTimeDelta);
public:
	// scene Index 
	const SCENE Get_SceneIndex() const { return m_eSceneNum; }
private:
	SCENE						m_eSceneNum{ SCENE_NON };
};

