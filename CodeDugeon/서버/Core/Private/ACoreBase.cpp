#include "CoreDefines.h"
#include "ACoreBase.h"

namespace Core
{
	ACoreBase::ACoreBase() : Core::AEnableSharedFromThis<ACoreBase>()
	{}

	ACoreBase::ACoreBase(const ACoreBase& _rhs) 
	{

	}

	ACoreBase::~ACoreBase()
	{

	}
}