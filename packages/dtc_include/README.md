
Common functions for other packages


* `dtc_sleep.h`, `dtc_sleep.c`

	* `void dtc_sleep(struct timespec, struct timespec)`: sleep for high-resolution
	
* `dtc_write_raw_2_text.h`

	* `inline void dtcWriteRaw2Text(FILE*, char*, int)`: save to file byte-wise