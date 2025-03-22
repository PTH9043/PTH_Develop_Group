#pragma once

BEGIN(Engine)
class UGameInstance;
class UDevice;
class UCommand;
class USkyBox;
END

BEGIN(Client)

class CProtoMaker {
public:
	static HRESULT CreateProtoData(CSHPTRREF<UGameInstance> _spGameInstance,
		CSHPTRREF<UDevice> _spDevice, CSHPTRREF<UCommand> _spCommand);

	static HRESULT CreateMainSceneProtoData(CSHPTRREF<UGameInstance> _spGameInstance,
		CSHPTRREF<UDevice> _spDevice, CSHPTRREF<UCommand> _spCommand);
};

END

