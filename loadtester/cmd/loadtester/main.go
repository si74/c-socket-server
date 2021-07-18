package main

import (
	"bufio"
	"context"
	"fmt"
	"log"
	"net"
	"os"
	"os/signal"
	"sync"
	"syscall"
)

var (
	address    = "localhost:8080"
	maxWorkers = 10
	maxReq     = 10
)

func main() {

	// Create cancellable context for worker goroutines
	ctx, cancel := context.WithCancel(context.Background())

	// Create channel to listen to keyboard interrupts
	c := make(chan os.Signal)
	signal.Notify(c, os.Interrupt, syscall.SIGTERM)
	go func() {
		<-c
		log.Println("keyboard interrupt. ending loadtester...")
		cancel()
		log.Println("exiting...")
		os.Exit(1)
	}()

	// Creating waitgroup for all workers
	var wg sync.WaitGroup

	log.Println("starting workers")
	for i := 0; i < maxWorkers; i++ {
		wg.Add(1)
		go func(ctx context.Context, i int) {
			defer wg.Done()
			fmt.Printf("starting worker: %d\n", i)
			req := 1
			for {
				select {
				case <-ctx.Done():
					fmt.Println("keyboard interrupt received. terminating worker")
					return
				default:
					if req > maxReq {
						fmt.Println("worker finished sending requests")
						return
					}
				}

				req++
				conn, err := net.Dial("tcp", address)
				if err != nil {
					log.Printf("error creating a connection %d: %v", i, err)
					return
				}
				defer conn.Close()

				fmt.Fprintf(conn, "Hi!")
				fmt.Println("sent HI. Awaiting a response")
				status, err := bufio.NewReader(conn).ReadString('!')
				if err != nil {
					log.Printf("unable to read from connection: %v", err)
					return
				}
				log.Printf("received from server: %s", status)
			}
		}(ctx, i)
	}

	// Wait until all workers finish
	wg.Wait()
}
