# c-socket-server
Blocking and nonblocking client and server written in C.

## Background

There are several elements here for design of the socket server and client.

1. Using blocking vs. non-blocking sockets
2. Using forks vs. threads vs. asynchronous design (ties into whether to use blocking or non-blocking sockets).

## Testing

### Load-Testing

1. Test blocking-server:

```
$ gcc blocking/server.c -o server
$ ./server
```

Running the load-testing client:

```
$ time ./loadtester  0.02s user 0.03s system 4% cpu 1.030 total
```

2. Testing non-blocking server:

```
$ gcc nonblocking/server.c -o server
$ ./server
```

Running the load-testing client:

```
$ cd loadtester/cmd/ && go build .
$ time ./loadtester  
...
$ time ./loadtester  0.02s user 0.03s system 156% cpu 0.028 total
```

Meaning of the output for `time`:

- [0.02s] user -> CPU time in user mode
- [0.03s] system -> CPU time in system mode
- 156% cpu -> % of cpu that this job got
- [0.028] total -> time from start to finish of call

## // TODO:

1. Work on nonblocking client based on notes [here](https://docs.google.com/document/d/1qY5_2XeZ2iy2S0uSzuXAz2PiB0bX04LBR3LXjnLyA2w/edit)
2. Change all code to incorporate proper C style (Gnu style: https://man.openbsd.org/style.9, OpenBSD style, Linux style)
3. Start on port scanning code

## Sources

- [Forks vs. threads vs. asynchronous I/O](https://www.remwebdevelopment.com/blog/overview-of-forks-threads-and-asynchronous-io-133.html)
- [Beej's Guide to Socket Servers](https://beej.us/guide/bgnet/html/)
- [Server Client Example](https://www.binarytides.com/server-client-example-c-sockets-linux/)
