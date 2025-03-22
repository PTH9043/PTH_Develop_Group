#pragma once

#include "UBase.h"
#include "UProcessedData.h"

BEGIN(Engine)
class UNetworkBaseController;

class UNetworkSender final : public UBase  {
	friend  UNetworkBaseController;
	using SENDQUERY = LIST<UProcessedData>;
public:
	UNetworkSender(SHPTR<UNetworkBaseController> _spNetworkBaseController);
	DESTRUCTOR(UNetworkSender)
public:
	void InsertSendProcessPacketInQuery(const UProcessedData& _ProcceedData);
	void InsertSendTcpPacketInQuery(_char* _pPacket, _short _PacketType, _short _PacketSize);
	void SendDataInQuery();
private:
	virtual void Free() override;
private:
	SOCKET*						m_pClientTcpSocket;
	SENDQUERY					m_SendQuery;
};

END