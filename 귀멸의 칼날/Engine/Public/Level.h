#pragma once

#include "Base.h"

BEGIN(Engine)
class CRenderer;

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	explicit CLevel(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	virtual ~CLevel() = default;
public:
	virtual HRESULT NativeConstruct();
	virtual void Tick(const _double& _dTimeDelta);
	virtual void Late_Tick(const _double& _dTimeDelta);
	virtual HRESULT Render();
protected:
	// Com °´Ã¼
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
public:
	virtual void Free() override;
};

END