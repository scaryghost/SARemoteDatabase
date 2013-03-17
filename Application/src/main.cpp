#include "CppUtilities/Core/CppUtilitiesException.h"
#include "CppUtilities/Cli/cli.h"

#include "Core/Global.h"
#include "Application/Outer.h"

#include <exception>
#include <iostream>

using namespace etsai::cpputilities;
using namespace etsai::saremotedatabase;

const char* version= "1.0.0";

int main(int argc, char **argv) {
    int port, timeout= 60;
    string dbURL, dbUser, dbPasswd, dbLib, password, logDir("log");

    try {
        global::logger= Logger::getLogger("saremotedatabase");
        global::logger->addHandler(new ConsoleHandler());

        CliBuilder *cli= CliBuilder::getBuilder();
        (*cli).addOption(Option("--tcp-port", 0, 1, [&port](const Arguments &args) -> void {
            port= args.asInteger(0);
        }).withArgName("number").withDescription("TCP port to listen on for requests").withRequired(true))
        .addOption(Option("--data-url", 0, 1, [&dbURL](const Arguments &args) -> void {
            dbURL= args.asString(0);
        }).withArgName("url").withDescription("URL to where the achievement data is stored").withRequired(true))
        .addOption(Option("--server-pw", 0, 1, [&password](const Arguments &args) -> void {
            password= args.asString(0);
        }).withArgName("password").withDescription("Password for connecting to the server").withRequired(true))
        .addOption(Option("-h", [&cli](const Arguments &args) -> void {
            cli->displayUsage();
        }).withLongOpt("--help").withDescription("Displays this help message and exits"))
        .addOption(Option("--data-user", 0, 1, [&dbUser](const Arguments &args) -> void {
            dbUser= args.asString(0);
        }).withArgName("username").withDescription("Username to access achievement data"))
        .addOption(Option("--data-pw", 0, 1, [&dbPasswd](const Arguments &args) -> void {
            dbPasswd= args.asString(0);
        }).withArgName("password").withDescription("Password to access achievement data"))
        .addOption(Option("--data-lib", 0, 1, [&dbLib](const Arguments &args) -> void {
            dbLib= args.asString(0);
        }).withArgName("libname").withDescription("Custom dll/so to process data instead of sqlite"))
        .addOption(Option("--connect-timeout", 0, 1, [&timeout](const Arguments &args) -> void {
            timeout= args.asInteger(0);
        }).withArgName("sec").withDescription("Max wait time before closing an idle connection"))
        .addOption(Option("-v", [](const Arguments &args) -> void {
            std::cout << "saremotedatabase " << version << std::endl;
            std::cout << "git page: https://github.com/scaryghost/SARemoteDatabase" << std::endl;
        }).withLongOpt("--version").withDescription("Prints version and exits"))
        .addOption(Option("--log-dir", 0, 1, [&logDir](const Arguments &args) -> void {
            logDir= args.asString(0);
        }).withArgName("dir").withDescription("Directory to store log files in"))
        .setUsage("saremotedatabase --data-url <url> --tcp-port <number> --server-pw <password> [options]");
        cli->parse(argc, argv);

        global::logger->addHandler(new FileHandler(logDir));
        outer::loadDBLib(dbLib);
        global::dataChannel->open(dbURL, dbUser, dbPasswd);
        outer::initCtrlHandler();
        outer::start(port, password, timeout);
    } catch (CppUtilitiesException &ex) {
        global::logger->log(Level::SEVERE, ex.what());
        return ex.getStatus();
    } catch (std::exception &ex) {
        global::logger->log(Level::SEVERE, ex.what());
        return 1;
    }

    return 0;
}
