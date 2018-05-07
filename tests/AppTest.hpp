#include <cxxtest/TestSuite.h>

#include "../include/Client.hpp"
#include "../include/Server.hpp"

class AppTest : public CxxTest::TestSuite {
    Server testServer;

public:
    void setUp()
    {
        testServer = Server::Server();
    }

    void tearDown()
    {
        delete &testServer;
    }

    void testConnection()
    {
        //
    }
};