## Kernel Development

* Compile source for QUILT

	`make target/linxu/{clean,prepare} QUILT=1`
	
* Apply existing patches

	`quilt push -a`

* Create patch for generic platform

	`quilt new platform/[patchname]`
	
* Show the current working patch

	`quilt top`

* Add new files to the patch

	`quilt add [filename]`
	
* Edit files

	`quilt edit [filename]`
	

## Linux Kernel

* Update source tar

	`make target/linux/update`

	
## Patch List

MPTCP patch

* 991-001-dtc-mptcp-v0.93.patch
* 991-002-dtc-mptcp-compatibility.patch
* 991-003-dtc-mptcp-default-config.patch


debugfs patch

* 992-001-dtc-debugfs-sock.patch
* 992-002-dtc-debugfs-mac.patch
* 992-003-dtc-debugfs-ath.patch