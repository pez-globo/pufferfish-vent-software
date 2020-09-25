# Cloning Raspberry Pi SD Card

### Cloning

Mount the SD Card using a card reader  
Execute the respective command to find the mount point of your SD Card  
<br/>
**Linux**
```sh
$ sudo fdisk -l
```
**MacOS**
```sh
$ diskutil list
```

Use `dd` command to clone the SD Card  
Assuming `/dev/sdb` as the mount point for your SD Card. Replace it with the mounting point on your system.  

```sh
$ sudo dd if=/dev/sdb of=/path/to/save/rpi.img
```
This will create an `img` file which can be used to flash to a brand new SD Card  

### Pishrink

If you are a linux user you can shrink this cloned img file using  
```sh
$ sudo ./scripts/pishrink.sh -vd /path/to/saved/rpi.img /path/to/save/pishrink_rpi.img
```

### Writing img to SD Card

Mount the SD Card using a card reader  
Execute the respective command to find the mount point of your SD Card  
<br/>
**Linux**
```sh
$ sudo fdisk -l
```
**MacOS**
```sh
$ diskutil list
```

If your SD Card already has an OS installed you might have more than one partition for your SD Card such as `/dev/sdb1` and `/dev/sdb2`  
In that case you'll have to unmount the partitions before flashing a new OS.  
Unmount the partitions using command below with respective mount points  
```sh
$ sudo umount /dev/sdb1
```

Now you can flash the SD Card with your cloned OS using `dd`  
```sh
$ sudo dd bs=4M if=/path/to/saved/rpi.img of=/dev/sdb
```

### References
Pishrink - https://github.com/Drewsif/PiShrink