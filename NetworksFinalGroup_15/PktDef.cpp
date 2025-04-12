#include "PktDef.h"
#include <stdexcept> 
#include <string> 
#include <cstring> 
#include <iostream>
using namespace std;

PktDef::PktDef() : RawBuffer(nullptr), dataLength(0) {
    memset(&packet.header, 0, sizeof(packet.header));
    packet.Data = nullptr;
    packet.CRC = 0;
}

PktDef::PktDef(char* rawBuffer) : RawBuffer(nullptr), dataLength(0) {
    // Parse the header from rawBuffer.
    memcpy(&packet.header, rawBuffer, HEADERSIZE);
    packet.Data = nullptr;
    // Assume the CRC is the byte immediately following the header.
    packet.CRC = *(reinterpret_cast<unsigned char*>(rawBuffer + HEADERSIZE));
}

void PktDef::SetCmd(CmdType cmd) {
    // Clear all command flags.
    packet.header.Drive = 0;
    packet.header.Status = 0;
    packet.header.Sleep = 0;
    // Set the flag corresponding to the command type.
    if (cmd == DRIVE)
        packet.header.Drive = 1;
    else if (cmd == SLEEP)
        packet.header.Sleep = 1;
    else if (cmd == RESPONSE)
        packet.header.Status = 1;
}

void PktDef::SetBodyData(char* data, int size) {
    if (packet.Data)
        delete[] packet.Data; 
    packet.Data = new char[size];
    memcpy(packet.Data, data, size);
    dataLength = size;
}

void PktDef::SetPktCount(int count) {
    packet.header.PktCount = static_cast<unsigned short>(count);
}

PktDef::CmdType PktDef::GetCmd() {
    if (packet.header.Drive == 1)
        return DRIVE;
    else if (packet.header.Sleep == 1)
        return SLEEP;
    else if (packet.header.Status == 1)
        return RESPONSE;
    return DRIVE;  // Default fallback.
}

bool PktDef::GetAck() {
    return packet.header.Ack != 0;
}

int PktDef::GetLength() {
    // Total length = header + body data + 1 byte for CRC.
    return HEADERSIZE + dataLength + 1;
}

char* PktDef::GetBodyData() {
    return packet.Data;
}

int PktDef::GetPktCount() {
    return packet.header.PktCount;
}

bool PktDef::CheckCRC(char* buffer, int size) {
    if (size < HEADERSIZE + 1)
        return false;
    unsigned char computedCRC = 0;
    for (int i = 0; i < size - 1; ++i) {
        computedCRC ^= static_cast<unsigned char>(buffer[i]);
    }
    return (computedCRC == static_cast<unsigned char>(buffer[size - 1]));
}

void PktDef::CalcCRC() {
    unsigned char crc = 0;
    // Process header.
    unsigned char* headerBytes = reinterpret_cast<unsigned char*>(&packet.header);
    for (int i = 0; i < HEADERSIZE; ++i) {
        crc ^= headerBytes[i];
    }
    // Process body data.
    for (int i = 0; i < dataLength; ++i) {
        crc ^= static_cast<unsigned char>(packet.Data[i]);
    }
    packet.CRC = crc;
}

char* PktDef::GenPacket() {
    if (RawBuffer)
        delete[] RawBuffer; 
    int totalLength = GetLength();
    RawBuffer = new char[totalLength];

    // Serialize the header.
    memcpy(RawBuffer, &packet.header, HEADERSIZE);
    // Serialize the body data.
    if (dataLength > 0 && packet.Data)
        memcpy(RawBuffer + HEADERSIZE, packet.Data, dataLength);
    // Append the CRC.
    RawBuffer[totalLength - 1] = static_cast<char>(packet.CRC);
    return RawBuffer;
}

PktDef::~PktDef() {
    if (packet.Data) {
        delete[] packet.Data;
        packet.Data = nullptr;
    }
    if (RawBuffer) {
        delete[] RawBuffer;
        RawBuffer = nullptr;
    }
}

int main()
{
    
    return 0;
}


