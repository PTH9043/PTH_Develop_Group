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
- 모델을 읽어서 불러오는 매니저 객체
*/

BEGIN(Client)

class CModelManager : public UBase
{
public:
	CModelManager();
	NO_COPY(CModelManager)
	DESTRUCTOR(CModelManager)

	// UBase을(를) 통해 상속됨
	void Free() override;
public:
	HRESULT CreateModelProtos(CSHPTRREF<UGameInstance> _spGameInstance, CSHPTRREF<UDevice> _spDevice);
	HRESULT AddModelProtosFromFile(CSHPTRREF<UGameInstance> _spGameInstance, CSHPTRREF<UDevice> _spDevice, CSHPTRREF<FILEGROUP> _spFolder);


};

END