#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_ALOGGER_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_ALOGGER_H

#include "ACoreBase.h"

BEGIN(Core)

/*
@ Date: 2024-04-08, Writer: 박태현
@ Explain: 로그를 찍고 모으는 클래스
*/
class  ALogger final : public ACoreBase  {
public:
	ALogger();
	NO_COPY(ALogger)
	DESTRUCTOR(ALogger)
public:
	void PrintOut(const char* _string);
	void FileOut(const char* _string);
private:
	virtual void Free() override;

private:
	std::ofstream		m_LogFileStream;
};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_ALOGGER_H