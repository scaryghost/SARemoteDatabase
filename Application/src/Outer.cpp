#include "Application/Outer.h"

#include "Core/Global.h"
#include "Data/DataChannel.h"
#include "Data/src/SqliteDataChannel.h"
#include "Net/TcpListener.h"

#include "CppUtilities/Network/ServerSocket.h"
#include "CppUtilities/Network/Socket.h"

#include <chrono>
#include <memory>
#include <thread>
#include <vector>

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

#ifndef WIN32
static struct sigaction sigIntHandler;
typedef DataChannel* (*CreateDataChannelType)();
static void *dllHandle= NULL;
#else
static HINSTANCE dllHandle= NULL;
typedef DataChannel* (__cdecl *CreateDataChannelType)();
using std::wstring;
#endif

#define HANDlER_ACTION(freeLibFunc)\
    if (dllHandle) {\
        freeLibFunc(dllHandle);\
    }\
    global::logger->log(Level::INFO, "Shutting down...");\
    server.close();\
    dataChnl->close()

static ServerSocket server;
static CreateDataChannelType channelCreator= NULL;
static shared_ptr<DataChannel> dataChnl;

#ifndef WIN32
static void ctrlHandler(int s) {
    HANDlER_ACTION(dlclose);
}
#else
static BOOL ctrlHandler(DWORD fdwCtrlType) {  
    switch(fdwCtrlType) { 
        case CTRL_C_EVENT:
        case CTRL_CLOSE_EVENT: 
            HANDlER_ACTION(FreeLibrary);
            break;
        default: 
            break;
    }
    return FALSE;
} 
#endif

void start(Properties &serverProps) {
    global::logger->log(Level::CONFIG, "Achievement data URL: %s", serverProps.dataURL);
    global::logger->log(Level::CONFIG, "Listening on tcp port: %d", port);

    if (!channelCreator) {
        dataChnl.reset(new SqliteDataChannel());
    } else {
        dataChnl.reset(channelCreator());
    }   
    dataChnl->open(serverProps.dataURL, serverProps.dataUser, serverProps.dataPw);

    server.bind(serverProps.port);
    while(true) {
        shared_ptr<Socket> client(server.accept());
        shared_ptr<time_point<system_clock> > lastActiveTime(new time_point<system_clock>);

        global::logger->log(Level::INFO, "Received connection from %s", client->getAddressPort());

        thread th(tcplistener::handler, client, dataChnl, lastActiveTime, serverProps.password);
        thread th2(tcplistener::timeout, client, lastActiveTime, serverProps.timeout);
        th.detach();
        th2.detach();
    }
}

void loadDBLib(const string& dataLib) throw(runtime_error) {
#ifdef WIN32
    wstring temp= wstring(dataLib.begin(), dataLib.end());
    LPCWSTR dataLibWStr= temp.c_str();
#endif

    if (dataLib.empty()) {
        return;
    }
#ifdef WIN32
    dllHandle= LoadLibrary(dataLibWStr);
    if (!dllHandle) {
        throw runtime_error("Error loading library: " + dataLib);
    }
    channelCreator= reinterpret_cast<CreateDataChannelType>(GetProcAddress(dllHandle, "createDataChannel"));
    if(!channelCreator) {
        FreeLibrary(dllHandle);
        throw runtime_error("Error loading function: createDataChannel");
    }
#else
    dllHandle= dlopen(dataLib.c_str(), RTLD_LAZY);
    if (!dllHandle) {
        throw runtime_error(dlerror());
    }
    channelCreator= reinterpret_cast<CreateDataChannelType>(dlsym(dllHandle, "createDataChannel"));
    if(!channelCreator) {
        throw runtime_error(dlerror());
    }
#endif
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

