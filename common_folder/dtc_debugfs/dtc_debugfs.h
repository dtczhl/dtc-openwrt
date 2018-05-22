#ifndef _DTC_DEBUGFS_H_
#define _DTC_DEBUGFS_H_

/* 
*	Note: Each module/package has its own variable space (they do not share).
*
*	Huanle Zhang
*	www.huanlezhang.com
*/

#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <linux/string.h>
#include <linux/timekeeping.h>
#include <linux/ieee80211.h>

/* enable protocol: up to 32 labels */
#define DTC_ENABLE_TCP  ( 1 << 0 )
#define DTC_ENABLE_UDP  ( 1 << 1 )

/* timekeeping location: up to 32 labels */
#define DTC_TIME_SOCK_SEND	( 1 << (0+0) )
#define DTC_TIME_SOCK_RECV	( 1 << (16+0) )
#define DTC_TIME_TCP_SEND	( 1 << (0+1) )
#define DTC_TIME_TCP_RECV	( 1 << (16+1) )
#define DTC_TIME_UDP_SEND	( 1 << (0+2) )
#define DTC_TIME_UDP_RECV	( 1 << (16+2) )
#define DTC_TIME_MAC_SEND	( 1 << (0+3) )
#define DTC_TIME_MAC_RECV	( 1 << (16+3) )
#define DTC_TIME_ATH_SEND	( 1 << (0+4) )
#define DTC_TIME_ATH_RECV	( 1 << (16+4) )
#define DTC_TIME_IP_SEND	( 1 << (0+5) )
#define DTC_TIME_IP_RECV	( 1 << (16+5) )

/* length: decimals to string literals */
#define DTC_DEC_32  10
#define DTC_DEC_64  20

/* variables */
extern u32 dtc_debugfs_enable;		// bind to DTC_ENABLE_
extern u32 dtc_debugfs_time_loc;	// bind to DTC_TIME_
extern u32 dtc_debugfs_target_ip;	
extern u16 dtc_debugfs_target_port;


/* ---- functions ---- */

int dtc_init_debugfs(char *dirname, int buff_num);
void dtc_cleanup_debugfs(void);

/* 
	Two logs should be enough without contention. 
		One for send path
		The other for receive path
*/
void dtc_debugfs_log1(u8 *pData, u32 length);
void dtc_debugfs_log2(u8 *pData, u32 length);

void dtc_debugfs_add_info(char *pInfo);

/* ---- inline functions ---- */

/* point to udp data payload */
static inline unsigned char *dtc_debugfs_skb_udp_payload(const struct sk_buff *skb){
    return skb_transport_header(skb) + 8;
}

/* is ieee80211 data frame ? */
static inline int dtc_debugfs_is_ieee80211_data(const struct sk_buff *skb, const __le16 frame_control){
// udp + ip + ieee80211
#define MIN_FRAME ( 8 + 20 + 10 )
    if (skb->data_len < MIN_FRAME ) return 0;
    return (frame_control & 0x000F) == IEEE80211_FTYPE_DATA;
}


#endif
