#ifndef _SERVERFRAMEWORK_SERVER_PUBLIC_CCHESTANIMCONTROLLER_H
#define _SERVERFRAMEWORK_SERVER_PUBLIC_CCHESTANIMCONTROLLER_H

#include "AAnimController.h"

BEGIN(Server)

class CChestAnimController final : public AAnimController {
public:
	CChestAnimController(OBJCON_CONSTRUCTOR, SHPTR<APawn> _spPawn,
		const _string& _strFolderPath, const _string& _strFileName, const _float4x4& _PivotMatrix = _float4x4::Identity);
	DESTRUCTOR(CChestAnimController)
public:
	virtual void Tick(const _double& _dTimeDelta) override;
private:
	virtual void Free() override;
private:
};

END

#endif // _SERVERFRAMEWORK_SERVER_PUBLIC_CCHESTANIMCONTROLLER_H
