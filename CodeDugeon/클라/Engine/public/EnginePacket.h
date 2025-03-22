#pragma once

namespace Engine
{
	enum
	{
		TCP_PORT_NUM = 9998,
		UDP_PORT_NUM = 9999,
		PACKET_SIZE_CNT = 2
	};
	/*
	@ Data: 2024-02-03, Writer: 박태현
	@ Explain
	- Packet의 앞쪽 부분을 담당하는 Head 부분이다.
	*/
	struct PACKETHEAD {
		short		PacketSize;
		short		PacketType;

		PACKETHEAD() : PacketSize{ 0 }, PacketType{ 0 } {}
		PACKETHEAD(const short _size, const short _type) : PacketSize(_size), PacketType(_type) {}
	};

	constexpr static unsigned int PACKETHEAD_SIZE{ sizeof(PACKETHEAD) };
	constexpr static unsigned int PACKETSIZE_SIZE{ sizeof(short) };
}
