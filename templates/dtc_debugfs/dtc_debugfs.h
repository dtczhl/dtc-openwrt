#ifndef _DTC_DEBUGFS_H_
#define _DTC_DEBUGFS_H_

/* 
 *  Each module/package has its own variable space (they do not share)
 *
 *  Huanle Zhang at UC Davis
 *  www.huanlezhang.com
 */

#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/string.h>
#include <linux/timekeeping.h>


/* 
    dirname: main folder name
    pFolderName: a list of string names
    pFolderSize: size for each string name
    nItem: number of pairs
*/
int dtc_enable_debugfs(char *dirname, char **pFolderName, int **pFolderSize, int nItem);

int dtc_disable_debugfs(void);

#endif
