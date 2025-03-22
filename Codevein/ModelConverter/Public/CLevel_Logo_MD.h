#pragma once
#include "ModelDefines.h"
#include "CLevel.h"


BEGIN(Model)

class CLevel_Logo_MD final : public CLevel
{
private:
	explicit CLevel_Logo_MD(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Logo_MD() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual void Tick(const _double& _dDeltaSeconds)override;
	virtual void Late_Tick(const _double& _dDeltaSeconds)override;
	virtual HRESULT Render()override;
private:
	HRESULT Ready_LightDesc(const _tchar* _pLayerTag);
	HRESULT Ready_Camera(const _tchar* _pLayerTag);

public:
	static CLevel* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END