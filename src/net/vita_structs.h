#ifndef VITA_TCP_H_
#define VITA_TCP_H_

#include <inttypes.h>
#include <psp2/sysmodule.h>
#include <psp2/ctrl.h>
#include <psp2/net/net.h>

/*
	Union needed to correctly access the network packets
	- IcmpPacket for the structure of the packet
	- uin16_t[] for the checksum
	- char[] for the sendto and recvfrom functions
	Taken from : https://github.com/vitasdk/samples/blob/master/socket_ping/src/main.c
*/


/* TCP Flag bits struct */
typedef struct{
	struct{
		unsigned short : 7; /* data offset + reserved, not used in this struct */
		unsigned short ns : 1;
		unsigned short cwr : 1;
		unsigned short ece : 1;
		unsigned short urg : 1;
		unsigned short ack : 1;
		unsigned short psh : 1;
		unsigned short rst : 1;
		unsigned short syn : 1;
		unsigned short fin : 1;
	};
}TcpFlagBits;


/* From wikipedia TCP Header table */
/* TCP header struct */
typedef struct{
	uint16_t src;
	uint16_t dst;
	uint32_t sequence;
	uint32_t ack_num;
	union{
		uint16_t data_flags;
		struct{
			uint16_t data_offset : 4;
			uint16_t reserved : 3;
			uint16_t flags : 9;
		};
		TcpFlagBits bit;
	};
	uint16_t window_size;
	uint16_t checksum;
	uint16_t urgent_ptr;
}TcpHdr;


/* ICMP Packet structure (= icmp header + payload) */
typedef struct{
	SceNetIcmpHeader hdr;
	char payload[32];
}IcmpPacket;

typedef struct{
	TcpHdr hdr;
	IcmpPacket icmp;
}TcpPacket;

typedef union{
	IcmpPacket icmp_struct;
	uint16_t icmp_u16buff[sizeof(IcmpPacket)/sizeof(uint16_t)];
	char icmp_packet[sizeof(IcmpPacket)];
}IcmpUnion;

typedef union{
	TcpPacket tcp_struct;
	uint16_t tcp_u16buff[sizeof(TcpPacket)/sizeof(uint16_t)];
	char tcp_packet[sizeof(TcpPacket)];
}TcpUnion;


#endif