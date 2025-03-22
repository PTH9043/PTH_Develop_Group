#pragma once
#include "UNetworkBaseController.h"


BEGIN(Client)
class CMainCamera;

class CNetworkClientController final : public UNetworkBaseController{
	enum
	{
		MAINCAMERA_ACTORS_ID = 0,
	};
public:
	CNetworkClientController();
	NO_COPY(CNetworkClientController)
	DESTRUCTOR(CNetworkClientController)
public:
	virtual HRESULT NativeConstruct(const _string& _strIPAddress, const _int _PortNumber) override;
	virtual void MakeActors(const VECTOR<SHPTR<UActor>>& _actorContainer) override;
protected:
	virtual void NativePacket() override;
	virtual void ProcessPacket(_char* _pPacket, PACKETHEAD _PacketHead) override;
private:
#ifdef _ENABLE_PROTOBUFF
	void CreateServerMobData();
	void ConnectSuccessState(_char* _pPacket, const PACKETHEAD& _PacketHead);
	void OtherClientLoginState(_char* _pPacket, const PACKETHEAD& _PacketHead);
	void PlayerAnimState(_char* _pPacket, const PACKETHEAD& _PacketHead);
	void SelfPlayerMoveState(_char* _pPacket, const PACKETHEAD& _PacketHead);
	void MonsterState(_char* _pPacket, const PACKETHEAD& _PacketHead);
#endif
private:
	virtual void Free() override;
private:
};

END