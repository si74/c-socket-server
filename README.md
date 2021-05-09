# c-socket-server
Blocking and nonblocking client and server written in C.

## Background

There are several elements here for design of the socket server and client.

1. Using blocking vs. non-blocking sockets
2. Using forks vs. threads vs. asynchronous design (ties into whether to use blocking or non-blocking sockets).

## Testing


## // TODO:

1. Write small go client to test out non-blocking server code to make sure it works
2. Work on nonblocking client based on notes [here](https://docs.google.com/document/d/1qY5_2XeZ2iy2S0uSzuXAz2PiB0bX04LBR3LXjnLyA2w/edit)
3. Change all code to incorporate proper C style (Gnu style: https://man.openbsd.org/style.9, OpenBSD style, Linux style)
4. Start on port scanning code

## Sources

- [Forks vs. threads vs. asynchronous I/O](https://www.remwebdevelopment.com/blog/overview-of-forks-threads-and-asynchronous-io-133.html)
- [Beej's Guide to Socket Servers](https://beej.us/guide/bgnet/html/)
- [Server Client Example](https://www.binarytides.com/server-client-example-c-sockets-linux/)
