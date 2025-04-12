#include "pch.h"               
#include "CppUnitTest.h"
#include "MySocket.h"   
#include "MySocket.cpp"
#include <string>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace Microsoft {
    namespace VisualStudio {
        namespace CppUnitTestFramework {

            template<>
            wstring ToString<SocketType>(const SocketType& t)
            {
                switch (t)
                {
                case CLIENT:
                    return L"CLIENT";
                case SERVER:
                    return L"SERVER";
                default:
                    return L"Unknown SocketType";
                }
            }

            template<>
            wstring ToString<ConnectionType>(const ConnectionType& t)
            {
                switch (t)
                {
                case TCP:
                    return L"TCP";
                case UDP:
                    return L"UDP";
                default:
                    return L"Unknown ConnectionType";
                }
            }
        }
    }
}
namespace MySocketTests
{
    TEST_CLASS(MySocketUnitTests)
    {
    public:
        // Test construction of a UDP client.
        TEST_METHOD(ConstructorUDPClientTest)
        {
            // Create a UDP client with a custom buffer size.
            MySocket socket(CLIENT, "127.0.0.1", 8080, UDP, 2048);
            Assert::AreEqual(string("127.0.0.1"), socket.GetIPAddr());
            Assert::AreEqual(8080, socket.GetPort());
            Assert::AreEqual(CLIENT, socket.GetType());
        }

        // Test construction of a TCP server.
        TEST_METHOD(ConstructorTCPServerTest)
        {
            // If an invalid size (zero) is provided, DEFAULT_SIZE is used.
            MySocket socket(SERVER, "127.0.0.1", 9090, TCP, 0);
            Assert::AreEqual(string("127.0.0.1"), socket.GetIPAddr());
            Assert::AreEqual(9090, socket.GetPort());
            Assert::AreEqual(SERVER, socket.GetType());
        }

        // Test that IP address can be set before any TCP connection is established.
        TEST_METHOD(SetIPAddrTest)
        {
            MySocket socket(CLIENT, "127.0.0.1", 8080, UDP, 1024);
            socket.SetIPAddr("192.168.1.100");
            Assert::AreEqual(string("192.168.1.100"), socket.GetIPAddr());
        }

        // Test that the port can be set and then retrieved.
        TEST_METHOD(SetAndGetPortTest)
        {
            MySocket socket(CLIENT, "127.0.0.1", 8080, UDP, 1024);
            socket.SetPort(9090);
            Assert::AreEqual(9090, socket.GetPort());
        }

        // Test setting and retrieving the socket type.
        TEST_METHOD(SetAndGetTypeTest)
        {
            MySocket socket(CLIENT, "127.0.0.1", 8080, UDP, 1024);
            Assert::AreEqual(CLIENT, socket.GetType());
            socket.SetType(SERVER);
            Assert::AreEqual(SERVER, socket.GetType());
        }
    };
}