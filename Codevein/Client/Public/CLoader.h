#pragma once
#include "Client_Defines.h"
#include "CBase.h"

BEGIN(Client)

class CLoader final : public CBase
{
private:
	explicit CLoader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLoader() = default;
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
	// Loading For Tool_Level
	HRESULT Loading_ForToolLevel();
	// Loading For LogoLevel
	HRESULT Loading_ForLogoLevel();
	// Loading For GamePlayLevel
	HRESULT Loading_ForGamePlayLevel();
	// Loading For GamePlayLevel 2
	HRESULT Loading_ForGamePlayLevel2();
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
	static CLoader* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LEVEL& _eNextLevel);
	virtual void Free() override;
};

END