#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_COREDEFINES_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_COREDEFINES_H

#pragma warning (disable : 4251)
#pragma warning (disable : 6031)
#pragma warning (disable : 4150)
#pragma warning (disable : 4200)



#define USE_DEBUG 0
//#define USE_STOMP 0

/*
@ Date: 2023-12-30
@ Writer: 박태현
@ Explain
- 현재 실행되는 코드의 상황이 Window OS인지 Linux OS인지 정의한다.
*/

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#else
#include <stdlib.h>
#include <sys/mman.h>
#endif

/*
@ Date: 2023-12-26
@ Writer: 박태현
@ Explain
- DLL 파일 관리하는 매크로, EXPORT 일때와 Exprot 아닐 때 구분하게 해준다.
*/
#ifdef CORE_EXPORTS
#define CORE_DLL   _declspec(dllexport)
#define MATHDLL_ENABLE
#else
#define CORE_DLL  _declspec(dllimport)
#endif

#define STATIC_CONCPP
#define _STOMP

#ifdef _DEBUG
#pragma comment (lib, "redis++_staticd")
#pragma comment (lib, "hiredisd")
#pragma comment(lib, "mysqlcppconn_debug")
#pragma comment(lib, "mysqlcppconn8_debug")
#pragma comment(lib, "ExporterMathLibD")
#else
#pragma comment (lib, "redis++_static")
#pragma comment (lib, "hiredis")
#pragma comment(lib, "mysqlcppconn")
#pragma comment(lib, "mysqlcppconn8")
#pragma comment(lib, "ExporterMathLib")
#endif
#pragma comment (lib, "libcrypto")
#pragma comment (lib, "libssl")
#pragma comment(lib, "liblua54")

#include <iostream>
#include <atomic>
#include <boost/asio.hpp>
#include <oneapi/tbb/concurrent_hash_map.h>
#include <oneapi/tbb/concurrent_priority_queue.h>
#include <oneapi/tbb/concurrent_queue.h>
#include <oneapi/tbb/concurrent_unordered_map.h>
#include <oneapi/tbb/concurrent_unordered_set.h>
#include <oneapi/tbb/concurrent_map.h>
#include <oneapi/tbb/concurrent_vector.h>
#include <oneapi/tbb/concurrent_set.h>
#include <oneapi/tbb/mutex.h>
#include <mutex>
#include <thread>
#include <string>
#include <array>
#include <concepts>
#include <chrono>
#include <random>
#include <queue>
#include <string>
#include <vector>
#include <fstream>
#include <set>
#include <list>
#include <regex>
#include <corecrt_wstring.h>
#include <mysql/mysql_connection.h>
#include <mysql/mysql_driver.h>
#include <mysql/cppconn/prepared_statement.h>
#include <mysql/cppconn/exception.h>
#include <mysql/cppconn/resultset.h>
#include <mysql/cppconn/statement.h>

extern "C" {
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

namespace Asio = boost::asio;

#include "CoreConcept.h"
#include "CoreGrobal.h"
#include "Allocator.h"
#include "CustomString.h"

#include "MathLib/CusMath.h"
#include "CoreTypedef.h"
#include "CoreTypecast.h"
#include "CoreMacro.h"
#include "CoreStatic.h"
#include "Lock.h"
#include "SmartPointer.h"
#include "ALuaLoader.h"

#include "CoreFunction.h"
#include "CorePacket.h"
#include "CoreEnum.h"
#include "CoreStruct.h"
#include "CoreTls.h"
#include "DBDataGroup.h"

#include "LockFree.h"

using namespace DirectX;

#endif 