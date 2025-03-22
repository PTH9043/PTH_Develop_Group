#pragma once
#include "CManager.h"
#include "CModel_FBX.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Model)
class CModel_FBX;

class CData_Manager_MD final : public CManager
{
	DECLARE_SINGLETON(CData_Manager_MD)
private:
	explicit CData_Manager_MD();
	virtual ~CData_Manager_MD() = default;
public:
	list<CModel_FBX*> Get_ModelList() { return m_AddModelList; }
	list<CModel*>	Get_ShowModelList() { return m_ShowModelList; }

	list<CModel_FBX*> Get_AnimModelList() { return m_AddAnimModelList; }
	list<CModel*>	Get_ShowAnimModelList() { return m_ShowAnimModelList; }
public:
	// NativeConstruct
	HRESULT NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
public:
	// Save Data 
	HRESULT Save_Data_NonAnim();
	// Save Data 
	HRESULT Save_Data_Anim();
	// Load Data FBX
	HRESULT Load_Data_FBX();
	// Load Data  JAB
	HRESULT Load_Data_NonAnim();
	// Load Data Anim
	HRESULT Load_Data_Anim();
public:
	// Load Model Data
	void Load_ModelData_1();
	// Load Model Dta 2
	void Load_ModelData_2();
	// Load Model Data 3
	void Load_ModelData_3();
private:
	// Model List
	list<CModel_FBX*>		m_AddModelList;
	list<CModel*>					m_ShowModelList;
	// Anim List
	list<CModel_FBX*>		m_AddAnimModelList;
	list<CModel*>					m_ShowAnimModelList;
private:
	// Thread Count
	_uint m_iThreadCount = 0;
public:
	virtual void Free() override;
};

END