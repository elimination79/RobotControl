
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


    static const int HEADERSIZE = 3;

    // Structure for the Header.
    // Contains a 2-byte PktCount and a 1-byte command flag field.
    struct Header {
        unsigned short PktCount;  
        unsigned char Drive : 1;
        unsigned char Status : 1;
        unsigned char Sleep : 1;
        unsigned char Ack : 1;
        unsigned char padding : 4;  
    };

    // Structure for drive command parameters.
    struct DriveBody {
        int Direction;  
        int Duration;   
        int Speed;      
    };

private:
    // Private structure to define a complete command packet.
    struct CmdPacket {
        Header header;  
        char* Data;     
       unsigned char CRC;       
    };

    CmdPacket packet;    
    char* RawBuffer;     
    int dataLength;      

public:
   
    PktDef() : RawBuffer(nullptr), dataLength(0) {
        memset(&packet.header, 0, sizeof(packet.header));
        packet.Data = nullptr;
        packet.CRC = 0;
    }

    // Overloaded constructor that takes a RAW data buffer, parses it,
    // and populates the Header, Body, and CRC of the object.
    
    PktDef(char* rawBuffer) : RawBuffer(nullptr), dataLength(0) {
       
        std::memcpy(&packet.header, rawBuffer, HEADERSIZE);

        
        packet.Data = nullptr;

        packet.CRC = *(reinterpret_cast<unsigned char*>(rawBuffer + HEADERSIZE));
    }

    // sets the command flag in the header based on the CmdType.
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
            packet.header.Status = 1; 
    }

    
    // Allocates the packet's Body field and copies the provided data.
    void SetBodyData(char* data, int size) {
        if (packet.Data) {
            delete[] packet.Data;
        }
        packet.Data = new char[size];
        memcpy(packet.Data, data, size);
        dataLength = size;
    }

    //  sets the PktCount field in the header.
    void SetPktCount(int count) {
        packet.header.PktCount = static_cast<unsigned short>(count);
    }

    //  returns the command type based on the set flag.
    CmdType GetCmd() {
        if (packet.header.Drive == 1)
            return DRIVE;
        else if (packet.header.Sleep == 1)
            return SLEEP;
        else if (packet.header.Status == 1)
            return RESPONSE;
        
        return DRIVE;
    }

    //  returns true if the Ack flag is set, false otherwise.
    bool GetAck() {
        return packet.header.Ack != 0;
    }

    //  returns the total packet length in bytes
    int GetLength() {
        return HEADERSIZE + dataLength + 1;
    }

    //  returns a pointer to the packet's Body field.
    char* GetBodyData() {
        return packet.Data;
    }

    // returns the PktCount value.
    int GetPktCount() {
        return packet.header.PktCount;
    }

    // takes a pointer to a RAW data buffer and its size, calculates the CRC and returns true if the calculated CRC matches the CRC in the buffer.
   
    bool CheckCRC(char* buffer, int size) {
        if (size < HEADERSIZE + 1)
            return false; // Not enough data.
        unsigned char computedCRC = 0;
        for (int i = 0; i < size - 1; ++i) {
            computedCRC ^= static_cast<unsigned char>(buffer[i]);
        }
        return (computedCRC == static_cast<unsigned char>(buffer[size - 1]));
    }

    // calculates the CRC for the current packet
    void CalcCRC() {
        unsigned char crc = 0;
        
        unsigned char* headerBytes = reinterpret_cast<unsigned char*>(&packet.header);
        for (int i = 0; i < HEADERSIZE; ++i) {
            crc ^= headerBytes[i];
        }
        
        for (int i = 0; i < dataLength; ++i) {
            crc ^= static_cast<unsigned char>(packet.Data[i]);
        }
        packet.CRC = crc;
    }

    //allocates the private RawBuffer and serializes the packet into it.
   
    char* GenPacket() {
        if (RawBuffer) {
            delete[] RawBuffer;
        }
        int totalLength = GetLength();
        RawBuffer = new char[totalLength];

        
        memcpy(RawBuffer, &packet.header, HEADERSIZE);
        
        if (dataLength > 0 && packet.Data) {
            memcpy(RawBuffer + HEADERSIZE, packet.Data, dataLength);
        }
       
        RawBuffer[totalLength - 1] = static_cast<char>(packet.CRC);
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

int main() {
    
}



