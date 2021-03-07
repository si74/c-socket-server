# c-socket-server
Blocking and nonblocking client and server written in C.

There are several elements here for design of the socket server and client.

1. Using blocking vs. non-blocking sockets
2. Using forks vs. threads vs. asynchronous design (ties into whether to use blocking or non-blocking sockets).

Sources:
- [Forks vs. threads vs. asynchronous I/O](https://www.remwebdevelopment.com/blog/overview-of-forks-threads-and-asynchronous-io-133.html)
- [Beej's Guide to Socket Servers](https://beej.us/guide/bgnet/html/)
- [Server Client Example](https://www.binarytides.com/server-client-example-c-sockets-linux/)
