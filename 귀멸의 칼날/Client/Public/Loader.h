#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* 생성한 스레드로 */
/* 다음할당될 레벨의 자원들을 로드한다.  */

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	LEVEL Get_NextLevelID() {
		return m_eNextLevel;
	}

	CRITICAL_SECTION* Get_CriticalSection() {
		return &m_CriticalSection;
	}

	const _tchar* Get_LoadingText() {
		return m_szLoading;
	}

	_bool isFinished() {
		return m_isFinished;
	}

public:
	HRESULT NativeConstruct(LEVEL eNextLevel);
public:
	static void Add_Thread(void* _pArg);
	void Loading_Data();
protected:
	HRESULT Load_GamePlay1();
	HRESULT Load_GamePlay2();
	HRESULT Load_GamePlay3();
	HRESULT Load_GamePlay4();
public:
	HRESULT Loading_ForLogoLevel();
	HRESULT Loading_ForModeLevel();
	HRESULT Loading_ForSelectLevel();
	HRESULT Loading_ForBattleLevel();
	HRESULT Loading_ForStoryLevel();
	HRESULT Loading_ForMiniGameLevel();
	HRESULT Loading_ForGamePlayLevel();
	HRESULT Loading_ForRattanLevel();
	HRESULT Loading_ForEndingLevel();
	
private:
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
private:
	_bool										m_isFinished = false;
	LEVEL									m_eNextLevel = LEVEL_END;
	_tchar									m_szLoading[MAX_PATH] = TEXT("");
private:
	HANDLE									m_hThread = 0;
	CRITICAL_SECTION				m_CriticalSection;
	// Thread Count
	_uint										m_iThreadCount = 0;
public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel);
	virtual void Free() override;
};

END