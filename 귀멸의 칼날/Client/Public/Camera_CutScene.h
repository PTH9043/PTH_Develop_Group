#pragma once
#include "Client_Defines.h"
#include "Camera.h"
#include "CutSceneModul.h"

BEGIN(Client)

class CCamera_CutScene final : public CCamera
{
private:
	explicit CCamera_CutScene(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCamera_CutScene(const CCamera_CutScene& _rhs);
	virtual ~CCamera_CutScene() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
public:

public:
	static CCamera_CutScene* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END