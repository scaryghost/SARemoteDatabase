#include "Core/Common.h"
#include "Database/src/SqliteConnection.h"
#include "Net/TcpListener.h"

#include "CppUtilities/Network/ServerSocket.h"
#include "CppUtilities/Network/Socket.h"

#include <chrono>
#include <memory>
#include <thread>
#ifdef WIN32
#include <windows.h> 
#endif

namespace etsai {
namespace saremotedatabase {
namespace common {

using namespace std::chrono;
using std::shared_ptr;
using std::thread;

Logger* logger;
Connection* dbConn;
static ServerSocket server;

#ifndef WIN32
struct sigaction sigIntHandler;
#else
static HINSTANCE dllHandle;
typedef Connection* (__cdecl *CreateConnType)();
#endif


#ifndef WIN32
static void ctrlHandler(int s) {
    logger->log(Level::INFO, "Shutting down...");
    dbConn->close();
    server.close();
}
#else
static BOOL ctrlHandler(DWORD fdwCtrlType) {  
    switch(fdwCtrlType) { 
        case CTRL_C_EVENT:
        case CTRL_CLOSE_EVENT: 
            logger->log(Level::INFO, "Shutting down...");
            dbConn->close();
            server.close();
            break;
        default: 
            break;
    }
    return FALSE;
} 
#endif

void start(int port, const string& password, int timeout) {
    stringstream msg;

    msg << "Listening on port: " << port;
    common::logger->log(Level::INFO, msg.str());
    server.bind(port);
    while(true) {
        shared_ptr<Socket> client(server.accept());
        shared_ptr<time_point<system_clock> > lastActiveTime(new time_point<system_clock>);

        common::logger->log(Level::INFO, "Received connection from " + client->getAddressPort());
        thread th(tcplistener::handler, client, lastActiveTime, password);
        thread th2(tcplistener::timeout, client, lastActiveTime, timeout);
        th.detach();
        th2.detach();
    }
}

void loadDBLib(const wstring& dbLib) {
    LPCWSTR dbLibWStr= dbLib.c_str();
    CreateConnType connCreator;

    if (dbLib.empty()) {
        dbConn= new SqliteConnection();
        return;
    }

    dllHandle= LoadLibrary(dbLibWStr);
    if (!dllHandle) {
        logger->log(Level::WARNING, "Error!  Cannot load db library ");
        exit(1);
        return;
    }

    connCreator= reinterpret_cast<CreateConnType>(GetProcAddress(dllHandle, "createConnection"));
    if(!connCreator) {
        FreeLibrary(dllHandle);
        logger->log(Level::WARNING, "Error!  Unable to load function createConnection");
        exit(1);
        return;
    }
    dbConn= connCreator();
}

void initCtrlHandler() {
#ifndef WIN32
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler= ctrlHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
#else
    if(!SetConsoleCtrlHandler((PHANDLER_ROUTINE) ctrlHandler, TRUE)) { 
        logger->log(Level::WARNING, "Could not set control handler");
    }
#endif
}

}
}
}
