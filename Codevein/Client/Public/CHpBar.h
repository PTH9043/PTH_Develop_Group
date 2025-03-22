#pragma once
#include "Client_Defines.h"
#include "CBar.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CHpBar final : public CBar
{
protected:
	explicit CHpBar(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CHpBar(const CHpBar& _rhs);
	virtual ~CHpBar() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
public:
	static CHpBar* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END