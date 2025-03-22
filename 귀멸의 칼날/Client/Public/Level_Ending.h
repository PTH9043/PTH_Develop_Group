
#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Ending final : public CLevel
{
private:
	CLevel_Ending(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Ending() = default;

public:
	virtual HRESULT NativeConstruct() override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);


public:
	static CLevel_Ending* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END