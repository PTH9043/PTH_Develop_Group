#include "ServerDefines.h"
#include "CAnubisAnimController.h"

namespace Server {

	CAnubisAnimController::CAnubisAnimController(OBJCON_CONSTRUCTOR, SHPTR<APawn> _spPawn,
		const _string& _strFolderPath, const _string& _strFileName, const _float4x4& _PivotMatrix) :
		AAnimController(OBJCON_CONDATA, _spPawn, _strFolderPath, _strFileName, _PivotMatrix)
	{
	}

	void CAnubisAnimController::Tick(const _double& _dTimeDelta)
	{
	}

	void CAnubisAnimController::Free()
	{
	}


}