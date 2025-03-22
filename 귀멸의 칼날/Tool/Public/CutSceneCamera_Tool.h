#pragma once
#include "Tool_Defines.h"
#include "Camera.h"


BEGIN(Engine)
class CGameInstance;
class CHierarchyNode;
END

BEGIN(Tool)

class CCutSceneCamera_Tool final : public CCamera
{
private:
	explicit CCutSceneCamera_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCutSceneCamera_Tool(const CCutSceneCamera_Tool& _rhs);
	virtual ~CCutSceneCamera_Tool() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;



public:
	static CCutSceneCamera_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END