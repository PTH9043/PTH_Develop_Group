#include "ServerDefines.h"
#include "CChestAnimController.h"

namespace Server {

	CChestAnimController::CChestAnimController(OBJCON_CONSTRUCTOR, SHPTR<APawn> _spPawn, 
		const _string& _strFolderPath, const _string& _strFileName, const _float4x4& _PivotMatrix) :
		AAnimController(OBJCON_CONDATA, _spPawn, _strFolderPath, _strFileName, _PivotMatrix)
	{
	}

	void CChestAnimController::Tick(const _double& _dTimeDelta)
	{
	}

	void CChestAnimController::Free()
	{
	}


}