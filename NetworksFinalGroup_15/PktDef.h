#pragma once
#include <cstring>

class PktDef {
public:
    // Enumeration for command types.
    enum CmdType { DRIVE, SLEEP, RESPONSE };

    // Constant integer definitions for drive directions.
    static const int FORWARD = 1;
    static const int BACKWARD = 2;
    static const int LEFT = 3;
    static const int RIGHT = 4;
    // HEADERSIZE is manually calculated as 3 bytes (2 bytes for PktCount + 1 byte for flags).
    static const int HEADERSIZE = 3;

    // Header structure: holds a 2-byte packet count and a 1-byte field for command flags.
    struct Header {
        unsigned short PktCount;  // 2 bytes.
        // Bit-fields for command flags.
        unsigned char Drive : 1;
        unsigned char Status : 1;
        unsigned char Sleep : 1;
        unsigned char Ack : 1;
        unsigned char padding : 4;  // 4 bits of padding to complete 1 byte.
    };

    // Structure for drive command parameters.
    struct DriveBody {
        int Direction;  // e.g., FORWARD, BACKWARD, etc.
        int Duration;   // Number of seconds for the directive.
        int Speed;      // Motor speed value (80-100%).
    };

    // Constructors
    PktDef();
    PktDef(char* rawBuffer);

    // Setters
    void SetCmd(CmdType cmd);
    void SetBodyData(char* data, int size);
    void SetPktCount(int count);

    // Getters
    CmdType GetCmd();
    bool GetAck();
    int GetLength();
    char* GetBodyData();
    int GetPktCount();

    // CRC functions
    bool CheckCRC(char* buffer, int size);
    void CalcCRC();
    char* GenPacket();

    // Destructor
    ~PktDef();

private:
    // Private structure defining the complete packet.
    struct CmdPacket {
        Header header;
        char* Data;
        unsigned char CRC; // Using unsigned char for consistency.
    };

    CmdPacket packet;    // The complete command packet.
    char* RawBuffer;     // Serialized form of the packet.
    int dataLength;      // Number of bytes in the packet body. 
};