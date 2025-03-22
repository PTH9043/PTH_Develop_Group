#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_CORETLS_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_CORETLS_H

namespace Core
{
	namespace TLS
	{
		/*
		@ Date: 2024.01.23,  Writer: 박태현
		@ Explain
		- 전체 쓰레드가 생성될 수 있는 개수
		*/
		constexpr static _uint									 MAX_THREAD{ 8 };

		/*
		@ Date: 2024.01.23,  Writer: 박태현
		@ Explain
		- 일을 처리할 MAX_THRED의 수
		*/
		constexpr static _uint									 MAX_WORKTHREAD{ 8 };

		/*
		@ Date: 2023-12-26
		@ Writer: 박태현
		@ Explain
		- Thread_Local에서 Thread의 ID를 정의한다. 
		*/
		static  thread_local _uint							g_ThreadID;

		static thread_local _char							g_LogTextBuffer[512];
	}
}


#endif 