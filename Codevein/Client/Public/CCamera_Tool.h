#pragma once
#include "Client_Defines.h"
#include "CCamera.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CCamera_Tool final : public CCamera
{
private:
	explicit CCamera_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCamera_Tool(const CCamera_Tool& _rhs);
	virtual ~CCamera_Tool() = default;

public:
	CCamera::CAMDESC Get_CamDesc();

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
public:
	// Show Cam Control Data
	_bool Show_CamControlData();
protected:
	// Move
	void Move(const _double& _dDeltaSeconds);

protected:
	class CSkyBox* m_pSkyBox = nullptr;
public:
	static CCamera_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END