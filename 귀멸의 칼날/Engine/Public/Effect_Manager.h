#pragma once

#include "Base.h"
#include "Effect.h"
#include "EffectObject.h"
#include "ImageEffect.h"
#include "StaticMeshEffect.h"
#include "AnimMeshEffect.h"
#include "InstanceEffect.h"

BEGIN(Engine)

class CGameObject;

class CEffect_Manager final : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager)
private:
	CEffect_Manager();
	virtual ~CEffect_Manager() = default;

public:
	// Get Effect Maps
	map<wstring, CGameObject*> Get_EffectMaps() { return m_mapEffects; }
public:
	HRESULT NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	HRESULT Add_Effect(_uint iLevelIndex, wstring pTag, CGameObject* pEffect);
	CGameObject* Fine_Effect(_uint iLevelIndex, wstring pTag);
	CGameObject* Get_Effect(_uint iLevelIndex, wstring pTag);

public:
	HRESULT Load_Effect();
	CGameObject* Load_Effect2(const _tchar* _FileName, _uint iSelect);
	HRESULT Init_Effects(_uint iSelect);

private:
	map<wstring, CGameObject*> m_mapEffects;

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
public:
	virtual void Free() override;
};

END