SARemoteDatabase
================
## About
This application is a remote database for Server Achievements, written in C++.  It uses sqlite to manage the achievement 
data for each player and communicates with the mutator via a TCP connection.

## Version
1.0.0

## Usage
The application requires 3 command line options to run: the database url, tcp port, and server password.  The usage 
format is given below.

    saremotedatabase -dburl <url> -tcpport <number> -passwd <password> [options]

More options are available to provide further configuration.  Run the application with -h (--help) for more information.

## Plugins
If sqlite is not your desired choice for storing data, you can write your own code to manage the data and load it in to 
the application.  See the below page for more details.

https://github.com/scaryghost/SARemoteDatabase/wiki/Creating-Custom-Data-Manager
