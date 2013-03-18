SARemoteDatabase
================
View the README in its formatted output here: https://github.com/scaryghost/SARemoteDatabase/blob/master/README.md
## About
This application is a remote database for Server Achievements, written in C++.  It uses sqlite to manage the achievement 
data for each player and communicates with the mutator via a TCP connection.

## Version
1.0.0

## Usage
The application requires 3 command line options to run: the data url, tcp port, and server password.  More options 
are available to provide further configuration.  Run the application with -h (--help) for more information.

    saremotedatabase --data-url <url> --tcp-port <number> --server-pw <password> [options]

A startup script has been provided provided in the "share/bin" directory for convenience.  Copy the script to the root 
directory and edit the configuration variables inside before running.

## OS Info
The Windows x86 version of the application is compiled on Windows 7 with Visual Studio 2012 and the CTP Nov 2012 
compiler update.  
http://www.microsoft.com/en-us/download/details.aspx?id=35515

Both Linux x64 and x86 versions are compiled on openSUSE 12.2 x64 with gcc 4.7.1 (x86 binary with -m32 flag).  If there 
are any compatibility issues, please see the below wiki page for obtaining and compiling the source code:
https://github.com/scaryghost/SARemoteDatabase/wiki/Compiling-on-Linux

## Plugins
If sqlite is not your desired choice for storing data, you can write your own code to manage the data and load it in to 
the application.  See the below page for more details.

https://github.com/scaryghost/SARemoteDatabase/wiki/Creating-Custom-Data-Manager
