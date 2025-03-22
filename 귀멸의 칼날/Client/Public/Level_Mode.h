
#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Mode final : public CLevel
{
private:
	CLevel_Mode(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Mode() = default;

public:
	virtual HRESULT NativeConstruct() override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);


public:
	static CLevel_Mode* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END