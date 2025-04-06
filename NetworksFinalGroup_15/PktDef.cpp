
#include <iostream> 
#include <fstream>
#include <sstream> 
#include <string>  
#include <cstring>
#include <cstdint>


using namespace std;

class PktDef {
public:
    // Enumerated command types.
    enum CmdType { DRIVE, SLEEP, RESPONSE };

    // Constant integer definitions for drive directions.
    static const int FORWARD = 1;
    static const int BACKWARD = 2;
    static const int LEFT = 3;
    static const int RIGHT = 4;
    // HEADERSIZE: calculated by hand.
    // Header contains: unsigned short (2 bytes) + command flags (1 byte) = 3 bytes.
    static const int HEADERSIZE = 3;

    // Structure for the Header.
    // Contains a 2-byte PktCount and a 1-byte command flag field.
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
        int Duration;   // Number of seconds to execute the directive.
        int Speed;      // Motor speed value (typically between 80-100%).
    };

private:
    // Private structure to define a complete command packet.
    struct CmdPacket {
        Header header;  // Packet header.
        char* Data;     // Pointer to the packet body.
        char CRC;       // 1-byte CRC checksum.
    };

    CmdPacket packet;    // The command packet instance.
    char* RawBuffer;     // Buffer to hold the serialized packet.
    int dataLength;      // Length (in bytes) of the packet body.

public:
    // Default constructor.
    // Sets all Header information to zero, the Data pointer to nullptr, and the CRC to zero.
    PktDef() : RawBuffer(nullptr), dataLength(0) {
        std::memset(&packet.header, 0, sizeof(packet.header));
        packet.Data = nullptr;
        packet.CRC = 0;
    }

    // Overloaded constructor that takes a RAW data buffer, parses it,
    // and populates the Header, Body, and CRC of the object.
    
    PktDef(char* rawBuffer) : RawBuffer(nullptr), dataLength(0) {
        // Parse header.
        std::memcpy(&packet.header, rawBuffer, HEADERSIZE);

        // In this example we do not know the body length.
        // We assume no body data (dataLength remains 0) and set Data to nullptr.
        packet.Data = nullptr;

        // Assume the CRC is the byte immediately following the header.
        packet.CRC = *(rawBuffer + HEADERSIZE);
    }

    // Set function: sets the command flag in the header based on the CmdType.
    void SetCmd(CmdType cmd) {
        // Clear all flags.
        packet.header.Drive = 0;
        packet.header.Status = 0;
        packet.header.Sleep = 0;
        // Set the flag corresponding to the command type.
        if (cmd == DRIVE)
            packet.header.Drive = 1;
        else if (cmd == SLEEP)
            packet.header.Sleep = 1;
        else if (cmd == RESPONSE)
            packet.header.Status = 1; // RESPONSE corresponds to the Status flag.
    }

    // Set function: takes a pointer to a RAW data buffer and its size in bytes.
    // Allocates the packet's Body field and copies the provided data.
    void SetBodyData(char* data, int size) {
        if (packet.Data) {
            delete[] packet.Data;
        }
        packet.Data = new char[size];
        std::memcpy(packet.Data, data, size);
        dataLength = size;
    }

    // Set function: sets the PktCount field in the header.
    void SetPktCount(int count) {
        packet.header.PktCount = static_cast<unsigned short>(count);
    }

    // Query function: returns the command type based on the set flag.
    CmdType GetCmd() {
        if (packet.header.Drive == 1)
            return DRIVE;
        else if (packet.header.Sleep == 1)
            return SLEEP;
        else if (packet.header.Status == 1)
            return RESPONSE;
        // Default return (should not happen if flags are set properly).
        return DRIVE;
    }

    // Query function: returns true if the Ack flag is set, false otherwise.
    bool GetAck() {
        return packet.header.Ack != 0;
    }

    // Query function: returns the total packet length in bytes.
    // Total length = header size + body data length + 1 byte for CRC.
    int GetLength() {
        return HEADERSIZE + dataLength + 1;
    }

    // Query function: returns a pointer to the packet's Body field.
    char* GetBodyData() {
        return packet.Data;
    }

    // Query function: returns the PktCount value.
    int GetPktCount() {
        return packet.header.PktCount;
    }

    // Function: takes a pointer to a RAW data buffer and its size, calculates the CRC and returns true if the calculated CRC matches the CRC in the buffer.
   
    bool CheckCRC(char* buffer, int size) {
        if (size < HEADERSIZE + 1)
            return false; // Not enough data.
        char computedCRC = 0;
        for (int i = 0; i < size - 1; ++i) {
            computedCRC ^= buffer[i];
        }
        return (computedCRC == buffer[size - 1]);
    }

    // Function: calculates the CRC for the current packet by XORing all bytes of the header
    // and the body data, then sets the packet's CRC field.
    void CalcCRC() {
        char crc = 0;
        // Process header bytes.
        unsigned char* headerBytes = reinterpret_cast<unsigned char*>(&packet.header);
        for (int i = 0; i < HEADERSIZE; ++i) {
            crc ^= headerBytes[i];
        }
        // Process body data (if any).
        for (int i = 0; i < dataLength; ++i) {
            crc ^= static_cast<unsigned char>(packet.Data[i]);
        }
        packet.CRC = crc;
    }

    // Function: allocates the private RawBuffer and serializes the packet into it.
    // Returns the address of the allocated RawBuffer.
    char* GenPacket() {
        // Free any previously allocated RawBuffer.
        if (RawBuffer) {
            delete[] RawBuffer;
        }
        int totalLength = GetLength();
        RawBuffer = new char[totalLength];

        // Copy header into RawBuffer.
        std::memcpy(RawBuffer, &packet.header, HEADERSIZE);
        // Copy body data (if any) into RawBuffer.
        if (dataLength > 0 && packet.Data) {
            std::memcpy(RawBuffer + HEADERSIZE, packet.Data, dataLength);
        }
        // Copy CRC at the end.
        RawBuffer[totalLength - 1] = packet.CRC;
        return RawBuffer;
    }

    // Destructor: frees any allocated memory.
    ~PktDef() {
        if (packet.Data) {
            delete[] packet.Data;
            packet.Data = nullptr;
        }
        if (RawBuffer) {
            delete[] RawBuffer;
            RawBuffer = nullptr;
        }
    }
};



int main(); 

