#pragma once

namespace Engine {

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

	using	CLASSID = _uint;

	using _float = float;
	using _double = double;

	using _bool = bool;
	using _tchar = wchar_t;

	using CAMID = _ushort;

	using _float2 = DirectX::PTH::Vector2;
	using _float3 = DirectX::PTH::Vector3;
	using _float4 = DirectX::PTH::Vector4;

	using _int2 = DirectX::XMINT2;
	using _int3 = DirectX::XMINT3;
	using _int4 = DirectX::XMINT4;

	using _retangle = DirectX::PTH::RECTANGLE;
	using _float4x4 = DirectX::PTH::Matrix;
	using _quaternion = DirectX::PTH::Quaternion;

	using _xmvector = DirectX::XMVECTOR;
	using _xmmatrix = DirectX::XMMATRIX;

	using _wstring = CustomString<wchar_t, std::wstring>;
	using _string = CustomString<char, std::string>;

	namespace PATH {
		using PATHS = std::vector<_wstring>;
	}

	using PATHS = PATH::PATHS;

	namespace RTS {
		using RENDERFORMATS = std::vector<DXGI_FORMAT>;
	}

	using RENDERFORMATS = RTS::RENDERFORMATS;

	template<class T>
	using ComPtr =  Microsoft::WRL::ComPtr<T>;

	using Dx12Device =  ID3D12Device5;
	using Dx12Fence =  ID3D12Fence;
	using Dx12CommandQueue =  ID3D12CommandQueue;
	using Dx12CommandAllocator =  ID3D12CommandAllocator;
	using Dx12GraphicsCommandList =  ID3D12GraphicsCommandList4;
	using Dx12Resource =  ID3D12Resource;
	using Dx12DescriptorHeap =  ID3D12DescriptorHeap;
	using Dx12PipelineState =  ID3D12PipelineState;
	using Dx12RootSignature =  ID3D12RootSignature;
	using DxBlob = ID3DBlob;
	using Dx12StateObject = ID3D12StateObject;
	using DxGIFactory = IDXGIFactory4;
	using DxGISwapChain = IDXGISwapChain3;
	using Dx12QueryHeap = ID3D12QueryHeap;
	// Using
	template<class Type, _uint Num>
	using ARRAY = std::array<Type, Num>;

#pragma region CUSTOM
	template<class T>
	using LIST = std::list<T>;

	template<class T>
	using VECTOR = std::vector<T, UStlAllocator<T>>;

	template<class Type, class Other, class Less = std::less<Type>>
	using MAP = std::map<Type, Other, Less, UStlAllocator<std::pair<const Type, Other>>>;

	template<class Type, class Other, class HASH = std::hash<Type>, class KEYEQ = std::equal_to<Type>>
	using UNORMAP = std::unordered_map<Type, Other, HASH, KEYEQ, UStlAllocator<std::pair<const Type, Other>>>;

	template<class Type, class HASH = std::hash<Type>, class KEYEQ = std::equal_to<Type>>
	using UNORSET = std::unordered_set<Type, HASH, KEYEQ, UStlAllocator<Type>>;

	template<class Type, class Pred = std::less<Type>>
	using SET = std::set<Type, Pred, UStlAllocator<Type>>;

	template<class Type>
	using DEQUE = std::deque<Type, UStlAllocator<Type>>;

	template<class Type, class Container = DEQUE<Type>>
	using QUEUE = std::queue<Type, Container>;

	template<class Type, class Container = VECTOR<Type>, class Pred = std::less<class Container::value_type>>
	using PRIORITYQUEUE = std::priority_queue<Type, Container, Pred>;

	template<class Type, class Container = DEQUE<Type>>
	using STACK = std::stack<Type, Container>;
#pragma endregion CUSTOM
#pragma region ORIGIN
	template<class T>
	using ORILIST = std::list<T>;

	template<class T>
	using ORIVECTOR = std::vector<T>;

	template<class Type, class Other, class HASH = std::hash<Type>, class KEYEQ = std::equal_to<Type>>
	using ORIUNORMAP = std::unordered_map<Type, Other, HASH, KEYEQ>;

	template<class Type, class HASH = std::hash<Type>, class KEYEQ = std::equal_to<Type>>
	using ORIUNORSET = std::unordered_set<Type, HASH, KEYEQ>;

	template<class Type, class Pred = std::less<Type>>
	using ORISET = std::set<Type, Pred>;

	template<class Type>
	using ORIDEQUE = std::deque<Type>;

	template<class Type, class Container = DEQUE<Type>>
	using ORIQUEUE = std::queue<Type, Container>;

	template<class Type, class Container = VECTOR<Type>, class Pred = std::less<class Container::value_type>>
	using ORIPRIORITYQUEUE = std::priority_queue<Type, Container, Pred>;

	template<class Type, class Container = DEQUE<Type>>
	using ORISTACK = std::stack<Type, Container>;
#pragma endregion ORIGIN

	template<class Type>
	using CONQUEUE = concurrency::concurrent_queue<Type>;

	template<class Type>
	using CONVECTOR = concurrency::concurrent_vector<Type>;

	template<class Type>
	using CONPRIORITYQUEUE = concurrency::concurrent_priority_queue<Type>;

	template<class Key, class Type>
	using CONUNOMAP = concurrency::concurrent_unordered_map<Key, Type>;

	namespace VOIDS {
		using VOIDDATAS = std::vector<void*>;
	}

	using VOIDDATAS = VOIDS::VOIDDATAS;

	template<typename T>
	using SHPTR =  std::shared_ptr<T>;

	template <typename T>
	using SHPTRREF = SHPTR<T>&;

	template<typename T>
	using CSHPTR = const SHPTR<T>;

	template<typename T>
	using CSHPTRREF = const SHPTR<T>&;

	template<class T>
	using WKPTR =  std::weak_ptr<T>;

	template<class T>
	using CWKPTR = const WKPTR<T>;

	template<class T>
	using CWKPTRREF = const WKPTR<T>&;

	template<class T>
	using UNPTR = std::unique_ptr<T>;

	template<class T>
	using CUNPTR = const UNPTR<T>;

	template<class T>
	using CUNPTRREF = const UNPTR<T>&;
	/*
	@ Date: 2024-02-02, Writer: 박태현
	@ Explain
	- thread를 등록하기 위한 함수 
	*/
	using THREADFUNC = std::function<void(void*)>;
	/*
	@ Date: 2024-02-10, Writer: 박태현
	@ Explain
	- Key 행동을 등록하기 위한 함수
	*/
	using KEYACTIONFUNC = UDelegate<void, const _double&>;

	/*
	@ Date: 2024-02-10, Writer: 박태현
	@ Explain
	- Animation Event에서 Mouse 혹은 Keyboard가 눌린 함수를 등록하는 변수
	*/
	using MKEVENTFUNC = std::function<_bool(_ubyte)>;
}