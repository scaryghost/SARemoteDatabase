#include "CppUtilities/Cli/cli.h"

#include "Core/Common.h"

#include <signal.h>

using namespace etsai::cpputilities;
using namespace etsai::saremotedatabase;

void ctrlHandler(int s) {
    common::logger->log(Level::INFO, "Shutting down...");
    common::dbConn->close();
}

int main(int argc, char **argv) {
    int port;
    string dbURL, dbUser, dbPasswd, password;

    common::logger.reset(Logger::getLogger("saremotedatabase"));

    CliBuilder *cli= CliBuilder::getBuilder();
    (*cli).addOption(Option("-tcpport", 0, 1, [&port](const Arguments &args) -> void {
        port= args.asInteger(0);
    }).withArgName("number").withDescription("TCP port to listen on for requests").withRequired(true))
    .addOption(Option("-dburl", 0, 1, [&dbURL](const Arguments &args) -> void {
        dbURL= args.asString(0);
    }).withArgName("url").withDescription("URL to the database storing achievement information").withRequired(true))
    .addOption(Option("-passwd", 0, 1, [&password](const Arguments &args) -> void {
        password= args.asString(0);
    }).withArgName("value").withDescription("Password for connecting to the server").withRequired(true))
    .addOption(Option("-h", [&cli](const Arguments &args) -> void {
        cli->displayUsage();
    }).withLongOpt("--help").withDescription("Displays this help message and exits"))
    .addOption(Option("-dbuser", 0, 1, [&dbUser](const Arguments &args) -> void {
        dbUser= args.asString(0);
    }).withArgName("user name").withDescription("Username for logging into the database"))
    .addOption(Option("-dbpwd", 0, 1, [&dbPasswd](const Arguments &args) -> void {
        dbPasswd= args.asString(0);
    }).withArgName("password").withDescription("Password for logging into the database"))
    .setUsage("saremotedatabase -dburl <url> -tcpport <number> -passwd <value> [options]");
    cli->parse(argc, argv);

    common::logger->addHandler(new ConsoleHandler());
    common::logger->addHandler(new FileHandler());

    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler= ctrlHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    common::initDbConnection(dbURL, dbUser, dbPasswd);
    common::start(port, password);

    return 0;
}
