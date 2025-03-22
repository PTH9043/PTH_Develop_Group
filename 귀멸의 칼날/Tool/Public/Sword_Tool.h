#pragma once
#include "Item_Tool.h"


BEGIN(Tool)


class CSword_Tool : public CItem_Tool
{
protected:
	explicit CSword_Tool(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CSword_Tool(const CSword_Tool& _rhs);
	virtual ~CSword_Tool() = default;
public:
	virtual const char* Get_Name() { return "Sword"; }
	virtual const _tchar* Get_Proto() { return PROTO_GOB_SWORD; }
	virtual const _tchar* Get_Layer() { return LAYER_SWORD; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
public:
	static CSword_Tool* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END