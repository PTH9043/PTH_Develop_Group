#pragma once

#include "Client_Defines.h"
#include "CCamera.h"

BEGIN(Client)

class CCamera_Minimap final : public CCamera
{
private:
	explicit CCamera_Minimap(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCamera_Minimap(const CCamera_Minimap& _rhs);
	virtual ~CCamera_Minimap() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;

protected:
	// Cam Setting
	HRESULT InitializeCamSetting();
protected:
	// Player
	class CPlayer* m_pPlayer = nullptr;
public:
	static CCamera_Minimap* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

