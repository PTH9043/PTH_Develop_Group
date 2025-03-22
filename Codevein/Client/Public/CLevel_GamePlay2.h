#pragma once
#include "Client_Defines.h"
#include "CLevel.h"

BEGIN(Client)

class CLevel_GamePlay2 final : public CLevel
{
private:
	explicit CLevel_GamePlay2(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_GamePlay2() = default;

public:
	virtual HRESULT NativeConstruct() override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT Ready_Light(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
private:
	LEVEL						m_eNextLevel = LEVEL_END;
	class CLoader*		m_pLoader = nullptr;
public:
	static CLevel_GamePlay2* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContex);
	virtual void Free() override;
};

END