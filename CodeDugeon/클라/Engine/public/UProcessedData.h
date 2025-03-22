#pragma once


BEGIN(Engine)
/*
@ Date: 2024-02-03,  Writer: 박태현
@ Explain
- Process를 이용해서 처리된 데이터들을 저장하는 클래스이다.
*/
class UProcessedData {
	using BUFFER = ARRAY<_char, MAX_BUFFER_LENGTH>;
public:
	UProcessedData();
	UProcessedData(const _int _NetworkID, void* _pData, size_t _Size, _int _DataType);
	UProcessedData(void* _pData, size_t _Size, _int _DataType);
	UProcessedData(const UProcessedData& _rhs);
	UProcessedData(UProcessedData&& _rhs) noexcept;
	// Send
	template<class T>
		requires CheckProtoType<T>
	UProcessedData(const T& _data, short _tag) : m_DataType{ _tag },
		 m_DataSize{ 0 }, m_iNetworkID{ -1 }
	{
		::memset(&m_Data[0], 0, MAX_BUFFER_LENGTH);
		_data.SerializePartialToArray((void*)&m_Data[0], static_cast<int>(_data.ByteSizeLong()));
		m_DataSize = static_cast<_int>(_data.ByteSizeLong());
	}
	// Recv
	template<class T>
		requires CheckProtoType<T>
	UProcessedData(const _int _NetworkID, const T& _data, short _tag) : m_DataType{ _tag },
		m_DataSize{ 0 }, m_iNetworkID{ _NetworkID }
	{
		::memset(&m_Data[0], 0, MAX_BUFFER_LENGTH);
		_data.SerializePartialToArray((void*)&m_Data[0], static_cast<int>(_data.ByteSizeLong()));
		m_DataSize = static_cast<_int>(_data.ByteSizeLong());
	}

	// Send
	template<class T>
		requires CheckProtoType<T>
	UProcessedData(const T& _data, short _tag, const size_t _DataSize) : m_DataType{ _tag },
		 m_DataSize{ static_cast<_int>(_DataSize) }, m_iNetworkID{ -1 }
	{
		::memset(&m_Data[0], 0, MAX_BUFFER_LENGTH);
		_data.SerializePartialToArray((void*)&m_Data[0], static_cast<int>(_data.ByteSizeLong()));
	}
	// Recv
	template<class T>
		requires CheckProtoType<T>
	UProcessedData(const _int _NetworkID, const T& _data, short _tag, const size_t _DataSize) : m_DataType{ _tag },
		 m_DataSize{ static_cast<_int>(_DataSize) }, m_iNetworkID{ _NetworkID }
	{
		::memset(&m_Data[0], 0, MAX_BUFFER_LENGTH);
		_data.SerializePartialToArray((void*)&m_Data[0], static_cast<int>(_data.ByteSizeLong()));
	}

	// Recv
	template<class T>
		requires CheckProtoType<T>
	UProcessedData(const _int _NetworkID, T* _data, short _tag) : m_Data{ reinterpret_cast<_char*>(_data) }, m_DataType{ _tag },
		m_DataSize{ static_cast<_int>( sizeof(T)) }, m_iNetworkID{ _NetworkID } { }

	~UProcessedData();

	UProcessedData& operator=(const UProcessedData& _other) noexcept;
	UProcessedData& operator=(UProcessedData&& _other) noexcept;
	template<class T>
	T* ConvertData() { return reinterpret_cast<T*>(m_Data.data()); }
public: /* get Set */
	_int GetDataID() const { return m_iNetworkID.load(); }
	_int GetDataType() const { return m_DataType; }
	_int GetDataSize() const { return m_DataSize; }
	_char* GetData() const { return &m_Data[0]; }
private:
	std::atomic_int			m_iNetworkID;
	mutable BUFFER		m_Data;
	std::atomic_int			m_DataType;
	std::atomic_int			m_DataSize;
};

END

namespace std {
	/*
	@ Date: 2024-02-03,  Writer: 박태현
	@ Explain
	- HASH opeartor() 연산에 대한 UProcessedData 재정의, concurrent_ 컨테이너에 사용하기 위함
	*/
	template <>
	struct std::hash<Engine::UProcessedData> {
		size_t operator()(Engine::UProcessedData&& ptr) const {
			return std::hash<char*>()(ptr.GetData());
		}
	};
	/*
	@ Date: 2024-02-03,  Writer: 박태현
	@ Explain
	- Equal 연산에 대한 UProcessedData 재정의, concurrent_ 컨테이너에 사용하기 위함
	*/
	template <>
	struct std::equal_to<Engine::UProcessedData> {
		bool operator()(Engine::UProcessedData&& lhs, Engine::UProcessedData&& rhs) const {
			return lhs.GetData() == rhs.GetData();
		}
	};
	/*
	@ Date: 2024-02-03,  Writer: 박태현
	@ Explain
	- less 연산에 대한 UProcessedData 재정의, concurrent_ 컨테이너에 사용하기 위함
	*/
	template <>
	struct std::less<Engine::UProcessedData> {
		bool operator()(Engine::UProcessedData&& lhs, Engine::UProcessedData&& rhs) const {
			return lhs.GetData() < rhs.GetData();
		}
	};
}