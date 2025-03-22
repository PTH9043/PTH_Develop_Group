#pragma once
#include "UBase.h"

BEGIN(Engine)
class UGameInstance;
class UDevice;
class UCommand;
END
/*
@ Date: 2024-04-23, Writer: 이성현
@ Explain
- 네비게이션을 읽어서 불러오는 매니저 객체
*/

BEGIN(Client)

class CNavigationManager : public UBase
{
public:
	CNavigationManager();
	NO_COPY(CNavigationManager)
	DESTRUCTOR(CNavigationManager)

	// UBase을(를) 통해 상속됨
	void Free() override;
public:
	HRESULT LoadNavigation();

};

END