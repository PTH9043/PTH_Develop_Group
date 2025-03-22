#pragma once

#include "Renderer.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Cube.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "Model.h"
#include "Renderer.h"
#include "Collider.h"
#include "Navigation.h"
#include "RectTransform.h"
#include "VIBuffer_Effect_Instance.h"
#include "Animator.h"
#include "Model_Instance.h"
#include "VIBuffer_Trail.h"

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

	map<const _tchar*, CComponent*>* m_pPrototypes = nullptr;
	typedef map<const _tchar*, CComponent*>		PROTOTYPES;

private:
	CComponent* Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);

public:
	virtual void Free() override;
};

END