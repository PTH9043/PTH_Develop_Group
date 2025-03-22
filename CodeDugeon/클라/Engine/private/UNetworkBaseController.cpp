#include "EngineDefine.h"
#include "UNetworkBaseController.h"
#include "UGameInstance.h"
#include "UActor.h"
#include "UProcessedData.h"
#include "UNetworkQueryProcessing.h"
#include "UMethod.h"

UNetworkBaseController::UNetworkBaseController() : 
	m_isNetworkTickRunning{true},
	m_IocpHandle{NULL},
	m_RecvTcpOverExp{},
	m_ClientTcpSocket{NULL}, 
	m_ClientUdpSocket{NULL},
	m_WsaData{},
	m_llNetworkOwnerID{0},
	m_TcpTotalBuffer{},
	m_RemainBufferLength{0},
	m_spNetworkAddress{nullptr},
	m_iSceneID{0},
	m_isNetworkResourceReceiveSuccess{false}, 
	m_iMakeMonsterNum{0},
	m_iRealMakeMonsterNum{0}
{
}

HRESULT UNetworkBaseController::NativeConstruct(const _string& _strIPAddress, const _int _PortNumber)
{
	// Create Socket
	m_spNetworkAddress = Create<UNetworkAddress>(_strIPAddress, _PortNumber);
	// Wsa Connect 
	RETURN_CHECK(UServerMethods::ReadyConnectToServer(&m_WsaData), E_FAIL);	
	m_ClientTcpSocket = UServerMethods::CreateTcpSocket();
	RETURN_CHECK(UServerMethods::ServerToConnect(m_ClientTcpSocket, m_spNetworkAddress), E_FAIL);
	m_IocpHandle = UServerMethods::CreateIocpHandle();
	UServerMethods::RegisterIocpToSocket(m_ClientTcpSocket, m_IocpHandle);

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	spGameInstance->RegisterFuncToRegister(ServerThread, this);
	return S_OK;
}

void UNetworkBaseController::MakeActors(const VECTOR<SHPTR<UActor>>& _actorContainer)
{
	m_NetworkInitDataContainer.clear();
}

void UNetworkBaseController::AddNetworkInitData(_int _NetworkID, const NETWORKRECEIVEINITDATA& _NetworkInitData)
{
	// Insert To Data 
	m_NetworkInitDataContainer.insert(MakePair(_NetworkID, _NetworkInitData));
}

void UNetworkBaseController::AddCreatedNetworkActor(_int _NetworkID, CSHPTRREF<UActor> _spActor)
{
	if (nullptr != m_spNetworkQueryProcessing)
	{
		m_spNetworkQueryProcessing->AddCreatedNetworkActor(_NetworkID, _spActor);
		_spActor->SetNetworkID(_NetworkID);
		m_NetworkActorContainer.insert(MakePair(_NetworkID, _spActor));
	}
}

void UNetworkBaseController::InsertNetworkInitDataInQuery(const NETWORKRECEIVEINITDATA& _networkInitData)
{
	GetNetworkQueryProcessing()->InsertNetworkInitData(_networkInitData);
}

SHPTR<UActor> UNetworkBaseController::FindNetworkActor(const _int _NetworkID)
{
	const auto& iter = m_NetworkActorContainer.find(_NetworkID);
	RETURN_CHECK(m_NetworkActorContainer.end() == iter, nullptr);
	return iter->second;
}

void UNetworkBaseController::InsertNetworkProcessInQuery(UProcessedData&& _data)
{
	if (nullptr != m_spNetworkQueryProcessing)
	{
		m_spNetworkQueryProcessing->InsertQueryData(std::move(_data));
	}
}

void UNetworkBaseController::SendTcpData(_char* _pData, short _tag, short _size)
{
	UOverExp* pOverExp = Make::xnew<UOverExp>(_pData, _tag, _size);
	UServerMethods::SendTcpPacket(m_ClientTcpSocket, pOverExp);
}

void UNetworkBaseController::ServerTick()
{
	DWORD num_bytes;
	ULONG_PTR key;
	WSAOVERLAPPED* over = nullptr;
	BOOL ret = GetQueuedCompletionStatus(m_IocpHandle, &num_bytes, &key, &over, INFINITE);
	if (SERVER_END == num_bytes)
		return;
	UOverExp* ex_over = reinterpret_cast<UOverExp*>(over);

	if (FALSE == ret) {
		assert("Failed Connect");
		if (ex_over->GetCompType() < OP_SEND_END)
			Make::xdelete(ex_over);
	}

	if ((0 == num_bytes) && ((ex_over->GetCompType() == OP_TCP_RECV) || (ex_over->GetCompType() == OP_TCP_SEND))) {
		assert("Failed Connect");
		if (ex_over->GetCompType() < OP_SEND_END)
			Make::xdelete(ex_over);
	}

	switch (ex_over->GetCompType()) {
	case OP_TCP_SEND:
	case OP_UDP_SEND:
		Make::xdelete(ex_over);
		break;
	case OP_TCP_RECV:
		CombineRecvPacket(ex_over, num_bytes);
		RecvTcpPacket();
		break;
	}
}

void UNetworkBaseController::NativePacket()
{
	RecvTcpPacket();
}

void UNetworkBaseController::CombineRecvPacket(UOverExp* _pOverExp, _llong _numBytes)
{
	std::lock_guard Lock{ m_Lock };
	// 버퍼를 조합한다. 
	{
		::memcpy(&m_TcpTotalBuffer[m_RemainBufferLength], _pOverExp->GetBufferAddress(0), _numBytes);
	}
	short moveBuffer{ 0 };
	char* pBufferMove = &m_TcpTotalBuffer[0];
	// 만약 BufferLocation이 존재할 때
	while (_numBytes != 0)
	{
		// PacketSize를 구한다. 
		PACKETHEAD PacketHead;
		memcpy(&PacketHead, pBufferMove, PACKETHEAD_SIZE);
		short CurrPacket = PacketHead.PacketSize + PACKETHEAD_SIZE;
		// 패킷의 현재 위치가 음수가 되는 경우면 
		if ((_numBytes - CurrPacket) < 0)
		{
			::memcpy(&m_TcpTotalBuffer[0], pBufferMove, _numBytes);
			m_RemainBufferLength += _numBytes;
			break;
		}
		ProcessPacket(&pBufferMove[PACKETHEAD_SIZE], PacketHead);
		// Buffer의 위치를 옮긴다. 
		_numBytes -= CurrPacket;
		pBufferMove += CurrPacket;
		moveBuffer += CurrPacket;
	}
	m_RemainBufferLength = 0;
}

void UNetworkBaseController::RecvTcpPacket()
{
	UServerMethods::RecvTcpPacket(m_ClientTcpSocket, REF_OUT m_RecvTcpOverExp);
}

void UNetworkBaseController::ServerThread(void* _pData)
{
	UNetworkBaseController* pNetworkBaseController = static_cast<UNetworkBaseController*>(_pData);
	RETURN_CHECK(nullptr == pNetworkBaseController, ;);
	pNetworkBaseController->NativePacket();
	g_threadID = 1;
	while (pNetworkBaseController->m_isNetworkTickRunning)
	{
		pNetworkBaseController->ServerTick();
	}
	CloseHandle(pNetworkBaseController->m_IocpHandle);
}

void UNetworkBaseController::Free()
{
	std::atomic_thread_fence(std::memory_order_seq_cst);
	m_isNetworkTickRunning = false;
	PostQueuedCompletionStatus(m_IocpHandle, SERVER_END, 0, 0);
	WaitForSingleObject(m_IocpHandle, INFINITE);
	closesocket(m_ClientTcpSocket);
	WSACleanup();
}
