Appy to PC Engine APU2


## Download firmware to mSata

1. Decompress firmware

	`gunzip [*]-combined-ext4.img.gz`

2. Insert flash disk, check whch partition it mounted at

	`lsblk`
	
3. Flash firmware to the flash disk

	`sudo dd if=[*]-combined-ext4-img.gz of=/dev/[sdb] bs=1M`
	
4. Config minicom, with 115200 8N1 hard:yes soft:no

	`sudo minicom -s`

5. Insert USB to board, connect board with computer using serial, power on, and select boot option by pressing F10

6. cd to a partition with enough space

	`df -h`
	`cd /tmp`

7. Copy fimware from host comptuer to board

	`scp dtc@ip:/[path_to_*]-combined-ext4.img .`

8. Find mSata by, e.g., partition size

	`fdisk -l`
         `# Disk /dev/[sda]: 14.9 GbiB ...`
 
 9. Flash firmware to mSata
 
 	`dd if=[*]-combined-ext4.img of=/dev/sda bs=1M`
 
 10. Reboot from mSata
 
## Fully allocate disk 
 
1. Check disk space

	`fdisk -l` 

2. Create disk space
```
	fdisk /dev/[sda]
	n Enter -> Enter -> Enter
	w (save)
	reboot
	
	mkfs.ext4 /dev/[sda3] (format newly created partition)
	block detect > /etc/config/fstab
	vi /etc/config/fstab
	(find the 'mount' section of '[sda3]')
	change '/mnt/[sda3]' to '/[your_directory]'
	change enabled '0' to '1'
	reboot
```	

	
	