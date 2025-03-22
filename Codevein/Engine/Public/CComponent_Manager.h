#pragma once

#include "CComponent.h"
#include "CRenderer.h"
#include "CVIBuffer_Rect.h"
#include "CVIBuffer_Terrain.h"
#include  "CVIBuffer_Cube.h"
#include "CShader.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CRectTransform.h"
#include "CModel.h"
#include "CCollider.h"
#include "CNavigation.h"
#include "CVIBuffer_Point_Instance.h"
#include "CVIBuffer_Rect_Instance.h"
#include "CVIBuffer_Cube_Instance.h"

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Reserve_Container(const _uint& iNumLevels);
	HRESULT Add_Prototype(const _uint& iLevelIndex, const _tchar* pPrototypeTag, CComponent* pPrototype);
	CComponent* Clone_Component(const _uint& iLevelIndex, const _tchar* pPrototypeTag, void* pArg);
	// Find Proto Comp
	CComponent* Get_ProtoComp(const _uint& _iLevelIndex, const _tchar* _pPrototypeTag);
public:
	HRESULT Clear(_uint iLevelIndex);

private:
	_uint										m_iNumLevels = 0;

	map<const _tchar*, CComponent*>*			m_pPrototypes = nullptr;
	typedef map<const _tchar*, CComponent*>		PROTOTYPES;

private:
	CComponent* Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);

public:
	virtual void Free() override;
};

END