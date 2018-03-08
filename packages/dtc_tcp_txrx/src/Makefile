myProgramName = dtc_tcp_txrx

# Huanle Zhang at UC Davis
# www.huanlezhang.com

main: dtc_sleep.h dtc_sleep.c main.c 
	$(CC) $(LDFLAGS) main.c dtc_sleep.c -o $(myProgramName) -lpthread

clean:
	rm *.o $(myProgramName)
