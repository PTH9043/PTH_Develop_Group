#include "CoreDefines.h"
#include "ALogger.h"

namespace Core {

	ALogger::ALogger() : 
		m_LogFileStream{"..\\Bin\\Log.txt",  std::ofstream::out | std::ofstream::app }
	{
	}

	void ALogger::PrintOut(const char* _string)
	{
		printf_s("%s", _string);
	}

	void ALogger::FileOut(const char* _string)
	{
		m_LogFileStream << _string;
		m_LogFileStream.flush();
	}

	void ALogger::Free()
	{
		m_LogFileStream.close();
	}
}
