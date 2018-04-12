#include <cxxtest/TestSuite.h>

#include <dropboxClient.hpp>
#include <dropboxServer.hpp>

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