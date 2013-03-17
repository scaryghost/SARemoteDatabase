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
        (*cli).addOption(Option("-tcpport", 0, 1, [&port](const Arguments &args) -> void {
            port= args.asInteger(0);
        }).withArgName("number").withDescription("TCP port to listen on for requests").withRequired(true))
        .addOption(Option("-dburl", 0, 1, [&dbURL](const Arguments &args) -> void {
            dbURL= args.asString(0);
        }).withArgName("url").withDescription("URL to the database storing achievement information").withRequired(true))
        .addOption(Option("-passwd", 0, 1, [&password](const Arguments &args) -> void {
            password= args.asString(0);
        }).withArgName("password").withDescription("Password for connecting to the server").withRequired(true))
        .addOption(Option("-h", [&cli](const Arguments &args) -> void {
            cli->displayUsage();
        }).withLongOpt("--help").withDescription("Displays this help message and exits"))
        .addOption(Option("-dbuser", 0, 1, [&dbUser](const Arguments &args) -> void {
            dbUser= args.asString(0);
        }).withArgName("username").withDescription("Username for logging into the database"))
        .addOption(Option("-dbpwd", 0, 1, [&dbPasswd](const Arguments &args) -> void {
            dbPasswd= args.asString(0);
        }).withArgName("password").withDescription("Password for logging into the database"))
        .addOption(Option("-dblib", 0, 1, [&dbLib](const Arguments &args) -> void {
            dbLib= args.asString(0);
        }).withArgName("libname").withDescription("Custom dll/so to process data in place of sqlite"))
        .addOption(Option("-timeout", 0, 1, [&timeout](const Arguments &args) -> void {
            timeout= args.asInteger(0);
        }).withArgName("sec").withDescription("How long to wait (in sec) before closing idle connection"))
        .addOption(Option("-v", [](const Arguments &args) -> void {
            std::cout << "saremotedatabase " << version << std::endl;
            std::cout << "git page: https://github.com/scaryghost/SARemoteDatabase" << std::endl;
        }).withLongOpt("--version").withDescription("Prints version and exits"))
        .addOption(Option("-logdir", 0, 1, [&logDir](const Arguments &args) -> void {
            logDir= args.asString(0);
        }).withArgName("dir").withDescription("Directory to store log files in"))
        .setUsage("saremotedatabase -dburl <url> -tcpport <number> -passwd <password> [options]");
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
