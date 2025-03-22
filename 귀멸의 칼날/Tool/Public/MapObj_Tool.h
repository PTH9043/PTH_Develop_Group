#pragma once
#include"Tool_Defines.h"
#include "GameObject_Tool.h"

BEGIN(Tool)
class CMapObj_Tool final : public CGameObject_Tool
{
private:
	CMapObj_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMapObj_Tool(const CMapObj_Tool& _rhs);
	virtual ~CMapObj_Tool(void) = default;
public:
	static CMapObj_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
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
	virtual const _tchar* Get_Proto() override  { return PROTO_GOB_MAPOBJECT; }
	virtual const _tchar* Get_Layer() override  { return LAYER_MAPOBJECT; }
	virtual HRESULT		  Delete_Self(void);
	void				  Set_RotationXYZ(_float3 _fXYZ);
	_float3*			  Get_RotationXYZ(void) { return &m_tGobDesc.v3Rotate; }
public:
	virtual void Free(void) override;
private:
	//_float3 m_vRotationXYZ{};
};
END