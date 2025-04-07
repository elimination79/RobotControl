#include "pch.h"
#include "CppUnitTest.h"
#include "PktDef.cpp"
using namespace Microsoft::VisualStudio::CppUnitTestFramework; 


namespace Microsoft {
    namespace VisualStudio {
        namespace CppUnitTestFramework {
            template<>

            wstring ToString<PktDef::CmdType>(const PktDef::CmdType& t)
            {
                switch (t)
                {
                case PktDef::DRIVE:    return L"DRIVE";
                case PktDef::SLEEP:    return L"SLEEP";
                case PktDef::RESPONSE: return L"RESPONSE";
                default:               return L"Unknown CmdType";
                }
            }
        }
    }
}
namespace UnitTestPktDef
{
	TEST_CLASS(UnitTestPktDef)
	{
	public:
        // Test that the default constructor sets the object to a safe state.
        TEST_METHOD(DefaultConstructorTest)
        {
            PktDef pkt;
            // Default PktCount should be 0.
            Assert::AreEqual(0, pkt.GetPktCount(), L"Default packet count is not 0");
            // GetAck should be false.
            Assert::IsFalse(pkt.GetAck(), L"Default Ack flag should be false");
            // With no body data, total length = HEADERSIZE + 1 (for CRC)
            Assert::AreEqual(PktDef::HEADERSIZE + 1, pkt.GetLength(), L"Default packet length is incorrect");
        }

        // Test setting and getting the packet count.
        TEST_METHOD(SetAndGetPktCountTest)
        {
            PktDef pkt;
            pkt.SetPktCount(123);
            Assert::AreEqual(123, pkt.GetPktCount(), L"PktCount was not set correctly");
        }

        // Test setting command flag via SetCmd() and retrieving it with GetCmd().
        TEST_METHOD(SetCmdTest)
        {
            PktDef pkt;

            pkt.SetCmd(PktDef::DRIVE);
            Assert::AreEqual(PktDef::DRIVE, pkt.GetCmd(), L"Command type should be DRIVE");

            pkt.SetCmd(PktDef::SLEEP);
            Assert::AreEqual(PktDef::SLEEP, pkt.GetCmd(), L"Command type should be SLEEP");

            pkt.SetCmd(PktDef::RESPONSE);
            Assert::AreEqual(PktDef::RESPONSE, pkt.GetCmd(), L"Command type should be RESPONSE");
        }

        // Test setting and retrieving body data.
        TEST_METHOD(SetAndGetBodyDataTest)
        {
            PktDef pkt;
            const char testData[] = "testdata";
            pkt.SetBodyData(const_cast<char*>(testData), sizeof(testData));

            char* bodyData = pkt.GetBodyData();
            Assert::IsNotNull(bodyData, L"Body data pointer should not be null");
            Assert::AreEqual(0, std::memcmp(bodyData, testData, sizeof(testData)), L"Body data does not match");

            // Expected total length: HEADERSIZE + body size + 1 (CRC)
            Assert::AreEqual(PktDef::HEADERSIZE + static_cast<int>(sizeof(testData)) + 1, pkt.GetLength(), L"Packet length is incorrect");
        }

        // Test CRC calculation, packet generation, and CRC check functions.
        TEST_METHOD(CalcCRCAndGenPacketTest)
        {
            PktDef pkt;
            pkt.SetPktCount(200);
            pkt.SetCmd(PktDef::DRIVE);

            const char testData[] = "abc";
            pkt.SetBodyData(const_cast<char*>(testData), sizeof(testData));

            // Calculate CRC and generate the raw packet.
            pkt.CalcCRC();
            char* rawPacket = pkt.GenPacket();
            int len = pkt.GetLength();

            // Check that the generated packet passes the CRC check.
            Assert::IsTrue(pkt.CheckCRC(rawPacket, len), L"CRC check failed on generated packet");
        }

        // Test the overloaded constructor that parses a raw data buffer.
        TEST_METHOD(OverloadedConstructorTest)
        {
            // Build a raw buffer simulating a packet with only header and CRC.
           
            char rawBuffer[4] = { 0 };
            unsigned short pktCount = 50;
            memcpy(rawBuffer, &pktCount, sizeof(pktCount));
           
            // Set CRC byte to 0xAA (170) at rawBuffer[3]
            rawBuffer[3] = 0xAA;

            PktDef pkt(rawBuffer);
            Assert::AreEqual(50, pkt.GetPktCount(), L"Overloaded constructor did not parse PktCount correctly");
            // With no body data, length should be HEADERSIZE + 1 = 4.
            Assert::AreEqual(4, pkt.GetLength(), L"Overloaded constructor packet length is incorrect");

            // Generate packet from the object and verify the CRC byte.
            char* generatedPacket = pkt.GenPacket();
            Assert::AreEqual(170, static_cast<int>(static_cast<unsigned char>(generatedPacket[3])), L"CRC value from overloaded constructor is incorrect");
        }
	};
}
