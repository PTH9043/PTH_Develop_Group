#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	typedef list<class CGameObject*>	OBJECTS;
private:
	explicit CLayer();
	virtual ~CLayer() = default;
public:
	HRESULT NativeConstruct();
	void Delete_GameObjects();
	void Tick(const _double& _dTimeDelta);
	void LateTick(const _double& _dTimeDelta);
public:
	// Out Delay Time
	void Out_DelayTime(_float*& _pUpdate, _float*& _pLateUpdate);
	// Get Component
	class CComponent* Get_Component(const _tchar* pComponentTag, const _uint& _iIndex = 0);
	// Find GameObject 
	class CGameObject* Find_GameObject(const _uint& _iIndex = 0);
public:
	HRESULT Add_GameObject(class CGameObject*& pGameObject);
	// Add Delete Object 
	void Add_DeleteObject(class CGameObject*& _pGameObject);
	// ObjectList
	list<CGameObject*> Get_ObjectList() { return m_lsObjectList; }
private:
	// ObjList
	OBJECTS			m_lsObjectList;
	// Delete Object List
	OBJECTS			m_lsDelObjList;
	// Delay Time
	_float					m_fUpdateDelayTime = 1.f;
	_float					m_fLateUpdateDelayTime = 1.f;
public:
	static CLayer* Create();
	virtual void Free() override;
};

END