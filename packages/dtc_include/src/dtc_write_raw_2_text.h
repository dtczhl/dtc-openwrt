#ifndef _DTC_WRITE_RAW_2_TEXT_H_
#define _DTC_WRITE_RAW_2_TEXT_H_

static inline 
void dtcWriteRaw2Text(FILE *fp, char *buf, int numberToLog)
{
    for (int i = 0; i < numberToLog; i++) {
        fprintf (fp, "%u-", (unsigned char) buf[i]);
    }
}

#endif
