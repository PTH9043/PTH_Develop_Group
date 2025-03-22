#pragma once

#include "CBase.h"

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
	void Tick(const _double& _dTimeDelta);
	void LateTick(const _double& _dTimeDelta);
public:
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
public:
	static CLayer* Create();
	virtual void Free() override;
};

END