#include "CoreDefines.h"
#include "ACoreObject.h"

namespace Core {

	ACoreObject::ACoreObject(SHPTR<ACoreInstance> _spCoreInstance) : 
		m_wpCoreInstance{_spCoreInstance}
	{
	}

	ACoreObject::ACoreObject(const ACoreObject& _rhs) : 
		m_wpCoreInstance{ _rhs.m_wpCoreInstance }
	{
	}

	void ACoreObject::Free()
	{
	}
}