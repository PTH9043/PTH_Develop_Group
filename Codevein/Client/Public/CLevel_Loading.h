#pragma once

#include "Client_Defines.h"
#include "CLevel.h"

BEGIN(Engine)
class CTexture;
class CShader;
class CRectTransform;
class CVIBuffer_Rect;
class CTransform;
END

BEGIN(Client)
class CUI_LoadImg;

class CLevel_Loading final : public CLevel
{
private:
	explicit CLevel_Loading(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LEVEL& _eLevel);
	virtual ~CLevel_Loading() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	LEVEL							m_eNextLevel = LEVEL_END;
	class CLoader*			m_pLoader = nullptr;
	CTexture*						m_pUserInterfaceTexture = nullptr;
	CRectTransform*		m_pRectTranform = nullptr;
	CVIBuffer_Rect*			m_pVIBufferBackRect = nullptr;
	CShader*						m_pShaderCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	// Loading
	vector<CUI_LoadImg*> m_LoadingImgList;
private:
	_uint m_iInitIndex = 0;
	_float m_fDelayTime = 0.f;
public:
	static CLevel_Loading* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LEVEL& _eNextLevel);
	virtual void Free() override;
};

END