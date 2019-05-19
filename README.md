# Sockets

Modified version of echo.

##Usage:

### Commands in current repository:
+ `make` — creates server and client executable files
+ `make server` — creates server executable file
+ `make client` — creates client executable file
+ `make serverRun` — runs the server (creates server executable file, if not done yet)
+ `make clientRun` — runs the client (creates client executable file, if not done yet)
### Server arguments:
+ 0 argument case: server will use default port
+ 1 argument case: port number (integer)
### Client arguments:
+ 1 argument case: client will send your string by default port
+ 2 argument case: client will take first argument as port number (integer) and send second argument (string)
### Expected behavior:
+ server returns reversed copy of received string
+ if sended string is `shutdown` - shuts down server (it returns terminating string and notifies user through the console)
+ client shows taken string and server response
+ client exclusively reacts on server termination
