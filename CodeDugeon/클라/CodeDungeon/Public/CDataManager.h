#pragma once

#include "UBase.h"

/*
@ Date: 2024-04-24, Writer: 이성현
@ Explain
- 모델, 네비게이션 등 데이터를 관리하고 로드하는 매니저 객체
*/
BEGIN(ENGINE)
class UStageManager;
END
BEGIN(Client)

class CModelManager;

class CDataManager final : public UBase
{
public:
	CDataManager();
	NO_COPY(CDataManager)
	DESTRUCTOR(CDataManager)

	// UBase을(를) 통해 상속됨
	void Free() override;
public:
	HRESULT NativeConstruct();
	HRESULT Load_Data();

private:
	SHPTR<CModelManager>		m_spModelManager;
	SHPTR<UStageManager>		m_spStageManager;
};

END