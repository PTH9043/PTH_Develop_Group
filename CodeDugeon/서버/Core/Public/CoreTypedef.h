#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_CORETYPEDEF_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_CORETYPEDEF_H

namespace Core
{

	/*
	@ Date: 2023-12-26
	@ Writer: 박태현
	@ Explain
	- 서버 프레임워크 구축에서 사용할 변수 타입들 정의
	*/
	using _ubyte = unsigned char;
	using _byte = signed char;
	using  _char = char;

	using _ushort = unsigned short;
	using _short = signed short;

	using _uint = unsigned int;
	using _int = signed int;

	using _ulong = unsigned long;
	using _long = signed long;
	using _ullong = unsigned long long;
	using _llong = signed long long;

	using	ACTORID = _llong;
	using CAMID = unsigned short;

	using SESSIONID = _llong;
	using ROOMID = _llong;

	using _float = float;
	using _double = double;

	using _bool = bool;
	using _tchar = wchar_t;

	/*
	GLM (OpenGl용 수학 라이브러리)
	*/
	using Vector2 = DirectX::PTH::Vector2;
	using Vector3 = DirectX::PTH::Vector3;
	using Vector4 = DirectX::PTH::Vector4;

	using _int2 = DirectX::XMINT2;
	using _int3 = DirectX::XMINT3;
	using _int4 = DirectX::XMINT4;

	using _float4x4 = DirectX::PTH::Matrix;
	using _quaternion = DirectX::PTH::Quaternion;

	using _wstring = CustomString<wchar_t, std::wstring>;
	using _string = CustomString<char, std::string>;

	using _xmvector = DirectX::XMVECTOR;
	using _xmmatrix = DirectX::XMMATRIX;

	template<class T>
	using _atomic = std::atomic<T>;

	using _mutex = std::mutex;
	using SMART_LOCK = std::lock_guard<_mutex>;
	using UNQUE_LOCK = std::unique_lock<_mutex>;

	namespace PATH {
		using PATHS = std::vector<std::wstring>;
	}

	using PATHS = PATH::PATHS;

	namespace VOIDS {
		using VOIDDATAS = std::vector<void*>;
	}

	using VOIDDATAS = VOIDS::VOIDDATAS;

	template<class Type, _uint Num>
	using ARRAY = std::array<Type, Num>;

	template<class Type, class Other>
	using CONUNORMAP = tbb::concurrent_unordered_map<Type, Other>;

	template<class Type, class Other, class Compare = std::less<Type>>
	using CONMULTIMAP = tbb::concurrent_multimap<Type, Other, Compare>;

	template<class Type, class Other>
	using CONMAP = tbb::concurrent_map<Type, Other>;

	template<class Type, class Other, class Compare = tbb::tbb_hash_compare<Type>>
	using CONHASHMAP = tbb::concurrent_hash_map<Type, Other, Compare>;

	template<class Type>
	using CONUNORSET = tbb::concurrent_unordered_set<Type>;

	template<class Type>
	using CONVECTOR = tbb::concurrent_vector<Type>;

	template<class Type>
	using CONQUEUE = tbb::concurrent_queue<Type>;

	template<class Type>
	using CONPRIORITYQUEUE = tbb::concurrent_priority_queue<Type>;

	template<class Type>
	using CONSET = tbb::concurrent_set<Type>;

	template<class Type, class Compare = std::less<Type>>
	using CONMULTISET = tbb::concurrent_multiset<Type, Compare>;

	template<typename Type>
	using Deque = std::deque <Type, USTLAllocator<Type>>;

	template<class Type>
	using QUEUE = std::queue<Type, Deque<Type>>;

	template<class Type>
	using VECTOR = std::vector<Type, USTLAllocator<Type>>;

	template<class Type>
	using LIST = std::list<Type, USTLAllocator<Type>>;

	template<class Type, class Pred = std::less<Type>>
	using SET = std::set<Type, Pred, USTLAllocator<Type>>;

	using string = CustomString<char, std::string>;
	using wstring = CustomString<_tchar, std::wstring>;

	using IOContext = boost::asio::io_context;
	using IOService = boost::asio::io_service;

	using TIMEPOINT = std::chrono::system_clock::time_point;

	using SendReceiveFunc = std::function<void(const boost::system::error_code& _error, std::size_t _Size)>;
	using AcceptFunc = std::function<void(const boost::system::error_code& _error)>;

	template<class T>
	using ATOMIC = std::atomic<T>;

	template<class T>
	using LOCKGUARD = std::lock_guard<T>;

	// TBB Mutex
	using MUTEX = tbb::mutex;

	using THREADFUNC = std::function<void(void*)>;

	using SENDRECVFUNC = std::function<void(const boost::system::error_code& _error, std::size_t _Size)>;
	using ACCEPTFUNC = std::function<void(const boost::system::error_code& _error)>;

	using TCPACCEPTOR = Asio::ip::tcp::acceptor;
	using TCPSOCKET = Asio::ip::tcp::socket;
	using UDPSOCKET = Asio::ip::udp::socket;
}

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_CORETYPEDEF_H