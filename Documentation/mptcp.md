
Adapted from official mptcp website
[Configure MPTCP](https://multipath-tcp.org/pmwiki.php/Users/ConfigureMPTCP)

## Configuration

### Read options

	sysctl net.mptcp.[name of the variable]
	
### Write options

	sysctl net.mptcp.[name of the variable]=[value]

# Options

## General

* `net.mptcp.mptcp_enabled`

Disable/Enable MPTCP. Possible values are 0 or 1. (Default 1)

If set to 2, MPTCP will be enabled if the application has set the socket-option MPTCP_ENABLED (value 42) to 1. MPTCP_ENABLED is part of SOL_TCP:
	
```
	int enable = 1;
	setsockopt (fd, SOL_TCP, MPTCP_ENABLED, &enable, sizeof(enable));
```
 
 You can use MPTCP_ENABLED also to get information about whether MPTCP has been successfully negotiated by using `getsockopt`
 
 * `net.mptcp.mptcp_checksum`
 
 Disable/Enable the MPTCP checksum. As described in the draft, both sides (client and server) have to disable DSS-checksums. If one side has it enabled, DSS-checksums will be used. Possible values are 0 or 1. (default 1)
 
 * `net.mptcp.mptcp_syn_retries`
 
 Specifies how often we retransmit a SYN with the MP_CAPABLE-option. After this, the SYN will not contain the MP_CAPABLE-option. This is to handle middleboxes that drop SYNs with unknown TCP options. (Default 3)
 
 ---
 You can also configure different congestion controls. You can change them via the 
 
 `sysctl net.ipv4.tcp_congestion_control`
 
 *  lia (Linked Increase Algorithm)
 * olia (Opportunistic Linked Increase Algorithm)
 * wVegas (Delay-based Congestion Control for MPTCP)
 * balia (Balanced Linked Adaptation Congestion Control Algorithm)

## Path-manager

If you do not select a path-manager, the host will not trigger the creation of new subflows, nor advertise alternative IP-addresses through the ADD_ADDR-option

`sysctl net.mptcp.mptcp_path_manager`

You can also set the path-manager through the socket-option MPTCP_PATH_MANAGER (defined by 44) by doing:

```
	char *pathmanager = "ndiffports";
	setsockopt(fd, SOL_TCP, MPTCP_PATH_MANAGER, pathmanager, sizeof(pathmanager));
```

* default. 
Does not do anything. The host won't announce different IP-addresses nor initiate the creation of new subflows. However, it will accept the passive creation of new subflows.

* fullmesh. 
It will create a full-mesh of subflows among all available subflows. 
It is possible to create multiple subflows for each pair of IP-addresses. Just set /sys/module/mptcp_fullmesh/parameters/num_subflows to a value  > 1. 
If you want to re-create subflows after a timeout (e.g., if the NAT-mapping was lost due to the idle-time), you can set /sys/module/mptcp_fullmesh/parameters/create_on_err to a value > 1.

* ndiffports. 
This one will create X subflows across the same pair of IP-addresses, modifying the source-port. To control the number of subflows (X), you can set /sys/module/mptcp_ndiffports/parameters/num_subflows to a value > 1.

* binder
Using Loose Source Routing from the paper *Binder: a system to aggregate multiple interface internet gateways in community networks*.

## Scheduler

At run-time you can select one of the compiled schedulers through the
 
 `sysctl net.mptcp.mptcp_scheduler`
 
 You can also select the scheduler through the socket-option MPTCP_SCHEDULER (defind as 43) by doing:
 
```
 	char *scheduler = "redundant";
 	setsockopt(fd, SOL_TCP, MPTCP_SCHEDULER, scheduler, sizeof(scheduler));
```
 
 * default
 It will first send data on subflows with the lowest RTT until their congestion-window if full. Then, it will start transmitting on the subflows with the next higher RTT.
 
 * roundrobin
 Transmitting traffic in a round-robin fashion. It is configurable, how many consecutive segments should be sent with the tunable "num_segments" in the sysfs (default 1). 
 Additionally, you can set the boolean tunable "cwnd_limited" to specify whether the scheduler tries to fill the congestion window on all subflows (true) or whether it prefers to leave open space in the congestion window (false) to achieve real round-robin (even if the subflows have very different capacities) (default to true). 
 
 * redundant
 This scheduler will try to transmit the traffic on all available subflows in a redundant way. It is useful when one wants to achieve the lowest possible latency by sacrificing the bandwidth.

 
# Get Detailed Info of Subflows

You can use socket-option MPTCP_INFO (defined as 45), to get detailed information about each subflow (e.g., struct tcp_info):

```
	struct mptcp_info minfo;
	struct mptcp_meta_info meta_info;
	struct tcp_info initial;
	struct tcp_info others[3];
	struct mptcp_sub_info others_info[3];
	
	minfo.tcp_info_len = sizeof(struct tcp_info);
	minfo.sub_len = sizeof(others);
	minfo.meta_len = sizeof(struct mptcp_meta_info);
	minfo.meta_info = &meta_info;
	minfo.initial = &initial;
	minfo.subflows = &others;
	minfo.sub_info_len = sizeof(struct mptcp_sub_info);
	minfo.total_sub_info_len = sizeof(others_info);
	minfo.subflow_info = &others_info;
	
	getsockopt(..., ..., MPTCP_INFO, &minfo, sizeof(minfo));
```

The initial subflow might have disappeared at the time when you call the getsockopt. You can let MPTCP know by using setsockopt on MPTCP_INFO with the flag MPTCP_INFO_FLAG_SAVE_MASTER (defined as 0x01):

```
	#define MPTCP_INFO_FLAG_SAVE_MASTER 0x01
	int val = MPTCP_INFO_FLAG_SAVE_MASTER
	setsockopt(..., ..., MPTCP_INFO, &val, sizeof(val));
```