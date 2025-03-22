#pragma once
BEGIN(Engine)
class UGameInstance;
class UDevice;
class UCommand;
class USkyBox;
END

BEGIN(Tool)


class TProtoMaker {
public:
	static HRESULT CreateProtoData(CSHPTRREF<UGameInstance> _spGameInstance,
		CSHPTRREF<UDevice> _spDevice, CSHPTRREF<UCommand> _spCommand);
};

END
