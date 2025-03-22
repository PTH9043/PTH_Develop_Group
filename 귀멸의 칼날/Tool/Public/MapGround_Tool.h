#pragma once
#include"Tool_Defines.h"
#include "GameObject_Tool.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Tool)
class CMapGround_Tool final : public CGameObject_Tool
{
private:
	explicit CMapGround_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CMapGround_Tool(const CMapGround_Tool& _rhs);
	virtual ~CMapGround_Tool(void) = default;
public:
	static CMapGround_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
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
	virtual const char* Get_Name() override { return "MapGround"; } 
	virtual const _tchar* Get_Proto() override  { return PROTO_GOB_MAPGROUND; }
	virtual const _tchar* Get_Layer() override  { return LAYER_MAPGROUND; }
	// Extern Hierakey Value
	virtual _bool Extern_HierakeyView(const string& _str);
	virtual HRESULT Delete_Self(void);
public:
	virtual void Free(void) override;
private:
	_uint		 m_iObjType = 0; // 0: ground 1: sky 
	CTexture* m_pTextureCom = nullptr;
	_tchar		m_GroundName[MAX_PATH] = L"";
	_bool		m_bImageChangeCheck = false;
};
END