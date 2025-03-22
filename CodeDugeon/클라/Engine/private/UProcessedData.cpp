#include "EngineDefine.h"
#include "UProcessedData.h"

UProcessedData::UProcessedData() : m_Data{  },
m_DataType{ 0 }, m_DataSize{ 0 }, m_iNetworkID{ -1 }
{
}

UProcessedData::UProcessedData(const _int _NetworkID, void* _pData, size_t _Size, _int _DataType) :
	m_Data{  }, m_DataType{ _DataType }, m_DataSize{ static_cast<_int>(_Size) }, m_iNetworkID{_NetworkID}
{
	::memcpy(&m_Data[0], _pData, _Size);
}

UProcessedData::UProcessedData(void* _pData, size_t _Size, _int _DataType) :
	m_Data{  }, m_DataType{ _DataType }, m_DataSize{ static_cast<_int>(_Size) }, m_iNetworkID{ 0 }
{
	::memcpy(&m_Data[0], _pData, _Size);
}

UProcessedData::UProcessedData(const UProcessedData& _rhs) : m_Data{ _rhs.m_Data },
m_DataType{ _rhs.m_DataType.load() },  m_DataSize{ _rhs.m_DataSize.load() }, m_iNetworkID{_rhs.m_iNetworkID.load() }
{
}

UProcessedData::UProcessedData(UProcessedData&& _rhs) noexcept : m_Data{_rhs.m_Data },
m_DataType{_rhs.m_DataType.load() },  m_DataSize{ _rhs.m_DataSize.load() }, m_iNetworkID{ _rhs.m_iNetworkID.load() }
{
}

UProcessedData::~UProcessedData()
{
}
 
UProcessedData& UProcessedData::operator=(const UProcessedData& _other) noexcept
{
	m_Data = _other.m_Data;
	m_DataType = _other.m_DataType.load();
	m_iNetworkID = _other.m_iNetworkID.load();
	m_DataSize = _other.m_DataSize.load();
	return *this;
}

UProcessedData& UProcessedData::operator=(UProcessedData&& _other) noexcept
{
	m_Data = _other.m_Data;
	m_DataType = _other.m_DataType.load();
	m_iNetworkID = _other.m_iNetworkID.load();
	m_DataSize = _other.m_DataSize.load();
	return *this;
}
