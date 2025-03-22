#include "CoreDefines.h"
#include "ACoreApp.h"
#include "ACoreInstance.h"

namespace Core
{
	ACoreApp::ACoreApp() :
		m_spCoreInstance{ Create<ACoreInstance>() }
	{
#ifdef _WIN32
		_wsetlocale(LC_ALL, L"korean");
#else
		setlocale(LC_ALL, "ko_KR.utf8");
#endif
	}

	ACoreApp::~ACoreApp(){
		m_spCoreInstance.reset();
	}

	_bool ACoreApp::Start()
	{

		return true;
	}
}