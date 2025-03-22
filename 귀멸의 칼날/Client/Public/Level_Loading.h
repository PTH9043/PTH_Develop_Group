#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT NativeConstruct(LEVEL eLevel);
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	

private:
	LEVEL			m_eNextLevel = LEVEL_END;
	class CLoader*	m_pLoader = nullptr;


public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel);
	virtual void Free() override;
};

END