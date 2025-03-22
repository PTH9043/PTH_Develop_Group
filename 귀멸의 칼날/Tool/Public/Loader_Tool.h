#pragma once
#include "Base.h"
#include "Tool_Defines.h"

BEGIN(Tool)

class CLoader_Tool  final : public CBase
{
private:
	explicit CLoader_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLoader_Tool() = default;
public:
	const _bool& Is_Finished() const { return m_bFinished; }
public:
	HRESULT NativeConstruct(const LEVEL& _eNextLevel);
public:
	// Run Thread
	static _uint APIENTRY Run_Thread(void* _pArg);
	void Running();

	static void Add_Thread(void* _pArg);
	void Loading_Data();
protected:
	void Load_1();
	void Load_2();
private:
	// Loading for Tool Level
	HRESULT Loading_ForToolLevel();
private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	_bool									m_bFinished = false;
	LEVEL								m_eNextLevel = LEVEL_END;
	// Thread Count
	_uint									m_iThreadCount = 0;
private:
	HANDLE								m_hThread = 0;
	CRITICAL_SECTION			m_CriticalSection;
public:
	static CLoader_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LEVEL& _eNextLevel);
	virtual void Free() override;
};

END