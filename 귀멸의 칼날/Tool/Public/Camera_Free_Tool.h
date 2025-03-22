#pragma once
#include "Tool_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Tool)

class CCamera_Free_Tool final : public CCamera
{
private:
	explicit CCamera_Free_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCamera_Free_Tool(const CCamera_Free_Tool& _rhs);
	virtual ~CCamera_Free_Tool() = default;
public:

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
protected:
	// Move
	void Move(const _double& _dDeltaSeconds);
protected:

public:
	static CCamera_Free_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END