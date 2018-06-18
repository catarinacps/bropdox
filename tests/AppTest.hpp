#include <cxxtest/TestSuite.h>

#include "client/Client.hpp"
#include "server/Server.hpp"

class AppTest : public CxxTest::TestSuite {
    Server testServer;

public:
    void setUp()
    {
        //testServer = Server();
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