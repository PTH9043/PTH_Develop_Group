#pragma once
#include "Level.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
class CTransform;
END

BEGIN(Tool)

class CLevel_Loading_Tool final : public CLevel
{
private:
	explicit CLevel_Loading_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LEVEL& _eLevel);
	virtual ~CLevel_Loading_Tool() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;
private:
	LEVEL							m_eNextLevel = LEVEL_END;
	class CLoader_Tool*	m_pLoader = nullptr;
	CTexture*						m_pUserInterfaceTexture = nullptr;
	CVIBuffer_Rect*			m_pVIBufferBackRect = nullptr;
	CShader*						m_pShaderCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;

public:
	static CLevel_Loading_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LEVEL& _eNextLevel);
	virtual void Free() override;
};

END