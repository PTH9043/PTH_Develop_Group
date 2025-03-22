#pragma once

#include "Base.h"

/* 모든 컴포넌트들의 부모다. */

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	explicit CComponent(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	explicit CComponent(const CComponent& Prototype);
	virtual ~CComponent() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct_Clone(void* pArg);

#ifdef _DEBUG
	virtual HRESULT Render_Debug() { return S_OK; }
#endif

protected:
	// Com 객체
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	// Clone
	_bool										m_bCloned = false;
public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free();
};

END