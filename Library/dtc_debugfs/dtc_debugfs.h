#ifndef _DTC_DEBUGFS_H_
#define _DTC_DEBUGFS_H_

#include <linux/debugfs.h>
#include <linux/ftrace.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <linux/string.h>
#include <linux/timekeeping.h>
#include <linux/ieee80211.h>

/* variables */
extern u32 dtc_debugfs_enable;

extern u32 dtc_debugfs_target_ip[];
extern u16 dtc_debugfs_target_port[];
extern u8 dtc_debugfs_target_count;

extern u32 dtc_debugfs_self_ip[];
extern u16 dtc_debugfs_self_port[];
extern u8 dtc_debugfs_self_count;

/* ---- functions ---- */

int dtc_init_debugfs(char *dirname);
void dtc_cleanup_debugfs(void);

void dtc_debugfs_add_info(char *pInfo);

#endif
