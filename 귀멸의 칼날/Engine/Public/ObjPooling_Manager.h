#pragma once
#include "Base.h"

BEGIN(Engine)
class CObjectPool;
class CGameObject;

class CObjPooling_Manager final : public CBase
{
	DECLARE_SINGLETON(CObjPooling_Manager)
private:
	explicit CObjPooling_Manager();
	virtual ~CObjPooling_Manager() = default;

public:
	HRESULT Add_ObjectPool(const _tchar* _pTag ,const _tchar* _pProtoTag, const _tchar* _pCloneLayer, 
		const _uint& _iLevelIndex, const _uint& _iMaxIndex, void* _Data = nullptr);
	// Effect Add
	HRESULT Add_ObjectPool(const _tchar* _pTag, const _uint& _iLevelIndex, const _uint& _iMaxIndex, const _tchar* _FileName, _uint iSelect);
	// Make Clone List
	HRESULT Make_CloneList(const _tchar* _pTag, const _uint& _iLevelIndex, const _uint& _iMaxIndex, void* _Data);
	// Clear List
	HRESULT Clear_List(const _tchar* _pTag);
	// Active Object
	CGameObject* Active_Object(const _tchar* _pTag);
	// Active Object List
	CGameObject* Active_ObjectList(const _tchar* _pTag, vector<CGameObject*>* _pActiveObject, const _uint& _iActiveCount);
	// Get Object
	vector<CGameObject*>* Get_ObjectList(const _tchar* _pTag);
	// GetCurObject
	CGameObject* Get_CurObject(const _tchar* _pTag);
private:
	// Find Object Pool
	class CObjectPool* Find_ObjectPool(const _tchar* _pTag);
protected:
	// Object Pool Map
	map<const _tchar*, CObjectPool*> m_ObjectPoolingMap;
public:
	// Free 
	virtual void Free() override;
};

END
