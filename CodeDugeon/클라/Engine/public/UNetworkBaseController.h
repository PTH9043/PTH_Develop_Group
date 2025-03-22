#pragma once

#include "ServerUtility.h"
#include "UProcessedData.h"

BEGIN(Engine)

class UActor;

using TOTALBUFFER = ARRAY<_char, MAX_PROCESSBUF_LENGTH>;
using NETWORKINITDATACONTAINER = UNORMAP<_int, NETWORKRECEIVEINITDATA>;

class UActor;
class UNetworkQueryProcessing;
/*
@ Date: 2024-02-03,  Writer: 박태현
@ Explain
- NetworkBaseController로 상속받아서 사용할 Server와 통신 클래스
*/
class UNetworkBaseController abstract : public UBase {
	using NETWORKACTORCONTAINER = CONUNOMAP<_int, SHPTR<UActor>>;
public:
	UNetworkBaseController();
	DESTRUCTOR(UNetworkBaseController)
public:
	virtual HRESULT NativeConstruct(const _string& _strIPAddress, const _int _PortNumber) PURE;
	virtual void MakeActors(const VECTOR<SHPTR<UActor>>& _actorContainer) PURE;
	void AddNetworkInitData(_int _NetworkID, const NETWORKRECEIVEINITDATA& _NetworkInitData);
	void AddCreatedNetworkActor(_int _NetworkID, CSHPTRREF<UActor> _spActor);

	void InsertNetworkInitDataInQuery(const NETWORKRECEIVEINITDATA& _networkInitData);
	SHPTR<UActor> FindNetworkActor(const _int _NetworkID);
	void InsertNetworkProcessInQuery(UProcessedData&& _data);

	void SendTcpData(_char* _pData, short _tag, short _size);
public: /* get set */
	const _llong GetNetworkOwnerID() const { return m_llNetworkOwnerID; }
	const NETWORKACTORCONTAINER& GetNetworkActorContainer() const { return m_NetworkActorContainer; }
	SHPTR<UNetworkQueryProcessing> GetNetworkQueryProcessing() { return m_spNetworkQueryProcessing; }
	const _bool IsNetworkResourceRecvSuccess() const { return m_isNetworkResourceReceiveSuccess; }
	SOCKET* GetClientSocketPointer() { return &m_ClientTcpSocket; }

	void SetSceneID(const _int _iSceneID) { this->m_iSceneID = _iSceneID; }
	void SetNetworkQueryProcessing(CSHPTRREF<UNetworkQueryProcessing> _spNetworkQueryProcessing) { this->m_spNetworkQueryProcessing = _spNetworkQueryProcessing; }
protected:
	void ServerTick();
	virtual void NativePacket() PURE;
	void CombineRecvPacket(UOverExp* _pOverExp, _llong _numBytes);
	virtual void ProcessPacket(_char* _pPacket, PACKETHEAD _PacketHead) PURE;
	void RecvTcpPacket();
	/*
	@ Date: 2023-01-05, Writer: 박태현
	@ Explain
	- ProtocolBuffer를 TCP에 연결된 상대에게 보내기 위한 템플릿 함수이다.
	*/
	template<class T>
		requires CheckProtoType<T>
	void SendProtoData(const T& _data, short _tag)
	{
		static BUFFER Buffer;
		_data.SerializePartialToArray((void*)&Buffer[0], static_cast<int>(_data.ByteSizeLong()));
		short size = static_cast<short>(_data.ByteSizeLong());
		SendTcpData(&Buffer[0], _tag, size);
	}
	void CountingMakeMonsterNum() { ++m_iRealMakeMonsterNum; }
protected: /* get set */
	const SOCKET& GetClientTcpSocket() const { return m_ClientTcpSocket; }
	const SOCKET& GetClientUdpSocket() const { return m_ClientUdpSocket; }
	CSHPTRREF<UNetworkAddress> GetNetworkAddress() const { return m_spNetworkAddress; }
	const NETWORKINITDATACONTAINER& GetNetworkInitDataContainer() const { return m_NetworkInitDataContainer; }
	const _int GetSceneID() const { return m_iSceneID; }

	void SetNetworkOwnerID(const _llong& _llNetworkOwnerID) { this->m_llNetworkOwnerID = _llNetworkOwnerID; }
	void EnableNetworkResourceRecvSuccess() { m_isNetworkResourceReceiveSuccess = true; }
	void SetMakeMonsterNum(_int _MonsterNum) { this->m_iMakeMonsterNum = _MonsterNum; }
private:
	static void ServerThread(void* _pData);
private:
	virtual void Free() override;
private:
	_bool																							m_isNetworkTickRunning;
	HANDLE																						m_IocpHandle;
	UOverExp																					m_RecvTcpOverExp;
	SOCKET																						m_ClientTcpSocket;
	SOCKET																						m_ClientUdpSocket;
	WSADATA																					m_WsaData{};

	TOTALBUFFER																			m_TcpTotalBuffer;
	std::atomic<_llong>																m_CurrentBufferLength;
	std::atomic <_llong>																m_RemainBufferLength;
	_llong																							m_llNetworkOwnerID;

	SHPTR< UNetworkAddress>												m_spNetworkAddress;
	NETWORKACTORCONTAINER												m_NetworkActorContainer;
	NETWORKINITDATACONTAINER											m_NetworkInitDataContainer;
	SHPTR<UNetworkQueryProcessing>									m_spNetworkQueryProcessing;
	_int																								m_iSceneID;
	_bool																							m_isNetworkResourceReceiveSuccess;
	_int																								m_iMakeMonsterNum;
	_int																								m_iRealMakeMonsterNum;

	std::mutex																				m_Lock;
};	

END