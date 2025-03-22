#pragma once

#include "ModelDefines.h"
#include "CLevel.h"

BEGIN(Model)

class CLevel_Loading_MD final : public CLevel
{
private:
	explicit CLevel_Loading_MD(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LEVEL& _eLevel);
	virtual ~CLevel_Loading_MD() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	LEVEL						m_eNextLevel = LEVEL_END;
	class CLoader_MD*		m_pLoader = nullptr;
public:
	static CLevel_Loading_MD* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LEVEL& _eNextLevel);
	virtual void Free() override;
};

END