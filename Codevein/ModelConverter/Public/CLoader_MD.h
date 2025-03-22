#pragma once
#include "ModelDefines.h"
#include "CBase.h"

BEGIN(Model)

class CLoader_MD final : public CBase
{
private:
	explicit CLoader_MD(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLoader_MD() = default;
public:
	const _bool& Is_Finished() const { return m_bFinished; }
public:
	HRESULT NativeConstruct(const LEVEL& _eNextLevel);
public:
	// Run Thread
	static _uint APIENTRY Run_Thread(void* _pArg);
	// Running
	void Running();
private:
	// Loading For LogoLevel
	HRESULT Loading_ForLogoLevel();
private:
	// Loading Tool Resources
	void Loading_Tool_Resources();
	// Load Tool Resource
	void Load_Tool_Resource_1();
	// Load Tool Resource 2
	void Load_Tool_Resource_2();
private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	_bool									m_bFinished = false;
	LEVEL								m_eNextLevel = LEVEL_END;
private:
	HANDLE				m_hThread = 0;
	CRITICAL_SECTION	m_CriticalSection;

private:
	_uint m_iThreadCount = 0;
	const _uint LOAD_LOGO_RESOURCESIZE = 1;
	const _uint m_iLoadingCount = 0;
public:
	static CLoader_MD* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LEVEL& _eNextLevel);
	virtual void Free() override;
};

END