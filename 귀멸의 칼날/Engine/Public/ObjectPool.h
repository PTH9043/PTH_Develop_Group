#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameObject;

class CObjectPool final : public CBase
{
public:
	typedef vector<CGameObject*> OBJLIST;
private:
	explicit CObjectPool();
	virtual ~CObjectPool() = default;
public:
	OBJLIST* Get_ObjectList() { return &m_ObjectList; }
public:
	// 초기화
	HRESULT NativeConstruct(const _tchar* _pProtoTag, const _tchar* _pCloneLayer, const _uint& _iLevelIndex, const _uint& _iMaxIndex, void* _Data);
	// Effect 추가
	HRESULT NativeConstruct(const _uint& _iLevelIndex, const _uint& _iMaxIndex, const _tchar* _FileName, _uint iSelect);
	// Make Clone List
	HRESULT Make_CloneList( const _uint& _iLevelIndex, const _uint& _iMaxIndex, void* _Data);
	// Clear List
	HRESULT Clear_List();
	// Active Object
	CGameObject* Active_Object();
	// Active Object List
	CGameObject* Active_ObjectList(vector<CGameObject*>* _pActiveObject, const _uint& _iActiveCount);
	// CurObject
	CGameObject* Cur_Object();
private:
	OBJLIST					m_ObjectList;
private:
	const _tchar*		m_pProtoTag = nullptr;
	const _tchar*		m_pCloneLayer = nullptr;
	_uint						m_iObjectLevelIndex = 0;
	_uint						m_iActiveIndex = 0;
	_uint						m_iMaxIndex = 0;
public:
	static CObjectPool* Create(const _tchar* _pProtoTag, const _tchar* _pCloneLayer, const _uint& _iLevelIndex, const _uint& _iMaxIndex, void* _pArg);
	static CObjectPool* Create(const _uint& _iLevelIndex, const _uint& _iMaxIndex, const _tchar* _FileName, _uint iSelect);
	virtual void Free() override;
};

END