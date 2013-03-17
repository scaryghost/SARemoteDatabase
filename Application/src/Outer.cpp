#include "Application/Outer.h"

#include "Core/Global.h"
#include "Database/src/SqliteConnection.h"
#include "Net/TcpListener.h"

#include "CppUtilities/Network/ServerSocket.h"
#include "CppUtilities/Network/Socket.h"

#include <chrono>
#include <memory>
#include <thread>
#ifdef WIN32
#include <windows.h> 
#else
#include <dlfcn.h>
#include <signal.h>
#endif

namespace etsai {
namespace saremotedatabase {
namespace outer {

using namespace cpputilities;
using namespace std::chrono;
using std::shared_ptr;
using std::thread;

static ServerSocket server;
#ifndef WIN32
static struct sigaction sigIntHandler;
typedef Connection* (*CreateConnType)();
static void *dllHandle;
#else
static HINSTANCE dllHandle;
typedef Connection* (__cdecl *CreateConnType)();
using std::wstring;
#endif

#ifndef WIN32
static void ctrlHandler(int s) {
    global::logger->log(Level::INFO, "Shutting down...");
    global::dbConn->close();
    server.close();
}
#else
static BOOL ctrlHandler(DWORD fdwCtrlType) {  
    switch(fdwCtrlType) { 
        case CTRL_C_EVENT:
        case CTRL_CLOSE_EVENT: 
            global::logger->log(Level::INFO, "Shutting down...");
            global::dbConn->close();
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
    global::logger->log(Level::CONFIG, msg.str());
    server.bind(port);
    while(true) {
        shared_ptr<Socket> client(server.accept());
        shared_ptr<time_point<system_clock> > lastActiveTime(new time_point<system_clock>);

        global::logger->log(Level::INFO, "Received connection from " + client->getAddressPort());
        thread th(tcplistener::handler, client, lastActiveTime, password);
        thread th2(tcplistener::timeout, client, lastActiveTime, timeout);
        th.detach();
        th2.detach();
    }
}

void loadDBLib(const string& dbLib) throw(runtime_error) {
#ifdef WIN32
    LPCWSTR dbLibWStr= wstring(dbLib.begin(), dbLib.end()).c_str();
#endif
    CreateConnType connCreator;

    if (dbLib.empty()) {
        global::dbConn= new SqliteConnection();
        return;
    }
#ifdef WIN32
    dllHandle= LoadLibrary(dbLibWStr);
    if (!dllHandle) {
        throw runtime_error("Error loading library: " + dbLib);
    }
#else
    dllHandle= dlopen(dbLib.c_str(), RTLD_LAZY);
    if (!dllHandle) {
        throw runtime_error(dlerror());
    }
#endif
    
#ifdef WIN32
    connCreator= reinterpret_cast<CreateConnType>(GetProcAddress(dllHandle, "createConnection"));
    if(!connCreator) {
        FreeLibrary(dllHandle);
        throw runtime_error("Error loading function: createConnection");
    }
#else
    connCreator= reinterpret_cast<CreateConnType>(dlsym(dllHandle, "createConnection"));
    if(!connCreator) {
        throw runtime_error(dlerror());
    }
#endif
    global::dbConn= connCreator();
}

void initCtrlHandler() {
#ifndef WIN32
    sigIntHandler.sa_handler= ctrlHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
#else
    if(!SetConsoleCtrlHandler((PHANDLER_ROUTINE) ctrlHandler, TRUE)) { 
        global::logger->log(Level::WARNING, "Could not set control handler");
    }
#endif
}

}   //namespace outer
}   //namespace saremotedatabase
}   //namespace etsai

