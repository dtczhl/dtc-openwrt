myProgramName = dtc_tcp_txrx

# Huanle Zhang at UC Davis
# www.huanlezhang.com

main: main.o
	$(CC) $(LDFLAGS) main.o -o $(myProgramName)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm *.o $(myProgramName)
