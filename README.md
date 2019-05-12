# A basic SNTP implementation

## Goal
This is an educational project, aimed at understanding how to interact with
POSIX socket APIs to implement a client-server architecture.

## Features:
* A client that requests timestamp seconds
* A server which sends system time in response
* Both are IPv4 & IPv6 compatible

## Building
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

Requires a C++14 compiler.

## Usage
**Server:**  
`ntp-server [<port number>]`  
E.g. `./ntp-server 8080`

**Client:**  
`ntp-client [<server hostname or address> [<server port>]]`  
E.g. `./ntp-client localhost 8080`, `ntp-client`

## Testing
Tested by hand on Linux 4.12.

## Copyright
Ilya Bizyaev, 2019 (<me@ilyabiz.com>)

Licensed under MIT terms.

