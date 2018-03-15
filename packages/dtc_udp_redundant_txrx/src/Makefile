myProgramName = dtc_udp_redundant_txrx

# Huanle Zhang at UC Davis
# www.huanlezhang.com

main: main.c dtc_sleep.h dtc_sleep.c dtc_write_raw_2_text.h
	$(CC) $(LDFLAGS) main.c dtc_sleep.c -o $(myProgramName) -lpthread

clean:
	rm $(myProgramName)
