#pragma once

#include "Base.h"

BEGIN(Engine)
class CGameObject;

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
private:
	explicit CObject_Manager();
	virtual ~CObject_Manager() = default;
public:
	// Get Component
	class CComponent* Get_Component(const _uint& _iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, const  _uint& _iIndex = 0);
	// Reverse Object
	HRESULT Reserve_Objects(const _uint& _iLevelCount);
	// 객체들의 원형 추가
	HRESULT Add_ProtoType(const _tchar*& _pProtoTypeTag, CGameObject*& _pGameObject);
	// 객체들의 사본을 추가
	HRESULT Clone_GameObject(const _uint& _iLevelIndex, const _tchar*& _pLayerTypeTag, const _tchar*& _pProtoTypeTag, void*& _pArg);
	// Clone GameObject
	CGameObject* Clone_GameObject_Add(const _uint& _iLevelIndex, const _tchar*& _pLayerTypeTag, const _tchar*& _pProtoTypeTag, void*& _pArg);
	// Find GameObject
	CGameObject* Find_GameObject(const _uint& _iLevelIndex, const _tchar*& _pLayerTypeTag, const _uint& _iIndex);
	// Find GameObject List
	list<CGameObject*> Find_GameObjectList(const _uint& _iLevelIndex, const _tchar*& _pLayerTypeTag);
	// Delete GameObject
	HRESULT Add_DeleteObject(const _uint& _iLevelIndex, const _tchar*& _pLayerTypeTag, CGameObject*& _pGameObject);
	// Clear
	HRESULT Clear(const _uint& _iLevelIndex);
	// Tick 
	void Tick(const _double& _dTimeDelta);
	// Late Tick
	void Late_Tick(const _double& _dTimeDelta);
	// Out Layer Tags
	void Out_LayerTags(const _uint& _iLevelIndex, list<const _tchar*>* _pOut);
	_uint Cur_LayerSize(const _uint& _iLevel);
	// Level Update Time Control
	void Out_LevelUpdateTime( _float*& _pUpdate, _float*& _fLateUpdate);
	void Out_LayerUpdateTime(const _uint& _iLevel, const _tchar*& _pLayerTypeTag, _float*& _pUpdate, _float*& _fLateUpdate);
	// Find ProtoType
	CGameObject* Find_ProtoType(const _tchar*& _pProtoTypeTag);
private:
	// Find Layer
	class CLayer* Find_Layer(const _uint& _iLevel, const _tchar* _pLayerTag);
private:
	// ProtoType
	map<const _tchar*, class CGameObject*>			m_mPrototypes;
	typedef map<const _tchar*, class CGameObject*>	PROTOTYPES;
private:
	// Layer 
	map<const _tchar*, class CLayer*>* m_pLayers = nullptr;
	typedef map<const _tchar*, class CLayer*>	LAYERS;
	
private:
	list<CGameObject*>			NULL_LIST;
	
private:
	_uint										m_iNumLevels = 0;
	// Delay Time
	_float										m_fUpdateDelayTime = 1.f;
	_float										m_fLateUpdateDelayTime = 1.f;
public:
	virtual void Free() override;
};

END