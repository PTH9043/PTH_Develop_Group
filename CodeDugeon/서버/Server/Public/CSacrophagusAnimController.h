#ifndef _SERVERFRAMEWORK_SERVER_PUBLIC_CSACROPHAGUSANIMCONTROLLER_H
#define _SERVERFRAMEWORK_SERVER_PUBLIC_CSACROPHAGUSANIMCONTROLLER_H

#include "AAnimController.h"

BEGIN(Core)
class APawn;
END

BEGIN(Server)

class CSacrophagusAnimController final : public AAnimController {
public:
	CSacrophagusAnimController(OBJCON_CONSTRUCTOR, SHPTR<APawn> _spPawn,
		const _string& _strFolderPath, const _string& _strFileName, const _float4x4& _PivotMatrix = _float4x4::Identity);
	DESTRUCTOR(CSacrophagusAnimController)
public:
	virtual void Tick(const _double& _dTimeDelta) override;
private:
	virtual void Free() override;
private:
	_int				m_iOwnerType;
	_double		m_dSarcophagusOpeningSpeed;
	_double		m_dLyingSarcophagusTimeArcOpenStart;
	_double		m_dLyingSarcophagusTimeArcOpenEnd;
	_double		m_dStandingSarcophagusTimeArcOpenEnd;
};

END

#endif // _SERVERFRAMEWORK_SERVER_PUBLIC_CSACROPHAGUSANIMCONTROLLER_H