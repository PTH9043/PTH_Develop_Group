#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Tool)

class CDebug_RenderObj_Tool final : public CGameObject
{
protected:
	explicit CDebug_RenderObj_Tool(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CDebug_RenderObj_Tool(const CDebug_RenderObj_Tool& _rhs);
	virtual ~CDebug_RenderObj_Tool() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderLight() override;

	virtual void Extern_ImGuiClass();
protected:
	virtual HRESULT SetUp_ConstantTable();
protected:
	// SetUp Components
	virtual HRESULT SetUp_Components();
private:
	CRenderer* m_pRenderCom = nullptr;
public:
	static CDebug_RenderObj_Tool* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END