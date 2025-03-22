#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_USQLDBQUERY_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_USQLDBQUERY_H

#include "ACoreBase.h"


BEGIN(Core)

class CORE_DLL USqlDBQuery final : public ACoreBase {
public:
	USqlDBQuery(const _string& _strQueryName);

private:

};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_USQLDBQUERY_H