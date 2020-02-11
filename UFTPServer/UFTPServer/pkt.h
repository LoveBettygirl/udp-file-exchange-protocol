#pragma once
#include <Windows.h>
#pragma pack(1)
struct Pkt //opcode=1||2 or others
{
	BYTE opcode;
	BYTE ack;
};

struct FileListPkt	//opcode=3
{
	BYTE opcode;
	BYTE ack;
	DWORD len;
	BYTE filelistdata[1];
};

struct LoadFilePkt	 //opcode=4||5
{
	BYTE opcode;
	BYTE ack;
	DWORD filenamelen;
	BYTE filename[1];
};

struct TransDataPkt	 //opcode=6||7
{
	BYTE opcode;
	BYTE ack;
	DWORD block;
	DWORD datalen;
	BYTE data[1];
};
#pragma pack()