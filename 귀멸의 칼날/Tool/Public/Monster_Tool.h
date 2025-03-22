#pragma once
#include"Tool_Defines.h"
#include "GameObject_Tool.h"

BEGIN(Tool)
class CMonster_Tool final : public CGameObject_Tool
{
private:
	CMonster_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMonster_Tool(const CMonster_Tool& _rhs);
	virtual ~CMonster_Tool(void) = default;
public:
	static CMonster_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg);
public:
	virtual HRESULT NativeConstruct_Prototype(void) override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void Tick(const _double& _dTimeDelta) override;
	void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render(void) override;
protected:
	HRESULT SetUp_ConstantTable(void);
public:
	virtual const char* Get_Name() override { return "CMapObj"; }
	virtual const _tchar* Get_Proto() override { return PROTO_GOB_MONSTER; }
	virtual const _tchar* Get_Layer() override { return LAYER_MONSTER; }
	virtual HRESULT		  Delete_Self(void);
	_float3* Get_RotationXYZ(void) { return &m_tGobDesc.v3Rotate; }
public:
	virtual void Free(void) override;
};
END