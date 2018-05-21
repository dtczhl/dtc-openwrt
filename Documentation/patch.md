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