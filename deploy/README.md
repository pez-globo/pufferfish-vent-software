# Pufferfish-Vent-Software Deployment

### Table of Contents

1. [Prerequisites](#prerequisites)
2. [Installation (Development Environment)](#installation-(development-environment))
3. [Deployment (Production Environment)](#deployment-(production-environment))
4. [Cloning Raspberry Pi SD Card](#cloning-raspberry-pi-sd-card)
    1. [Cloning](#cloning)
    2. [Pishrink](#pishrink)
    3. [Writing img to SD Card](#writing-img-to-sd-card)
5. [References](#references)

>To start with a fresh setup, download the standard Pufferfish OS image and flash it to a SD Card by following the steps [here](#writing-img-to-sd-card)

---

### Prerequisites
- [Pyenv](https://github.com/pyenv/pyenv)
- [Poetry](https://python-poetry.org/)
- [Pigpio](http://abyz.me.uk/rpi/pigpio/)
- [Node](https://nodejs.org/en/)
- [Yarn](https://yarnpkg.com/)
- [Nginx](https://www.nginx.com/)
- USB Device for Logging (Production Environment)

---

### Installation (Development Environment)
All the software components can be installed by running `setup.sh` on a terminal.

```sh
$ ./install.sh
```

This will install the required components for pufferfish-vent-software in the given order.  
- Pyenv
- Poetry
- Pigpio
- Node
- Roboto fonts
- Yarn

---

### Deployment (Production Environment)
These instructions are to setup Raspberry Pi with Pufferfish-Vent-Software in a production environment.

> **Note:** USB Mass Storage Device with Label **LOGS** is needed for logging in production environment.
>
> To set Label of a USB Mass Storage Device follow the procedure below
> - Format the USB Mass Storage Device
> - Select FAT32 as the storage format
> - Enter **LOGS** in the name section to set the Label for the device

All the components required for the production environment can be setup by running `deploy.sh` on a terminal.

```sh
$ ./deploy.sh
```

This will setup required components for pufferfish-vent-software in the given order.  
- Pyenv
- Poetry
- Pigpio
- Node
- Roboto fonts
- Yarn
- Nginx web-server
- Custom Pufferfish boot screen
- Python backend systemd service
- Disables screen blanking and hides cursor
- Chromium browser in kiosk mode
- USB Logging
- Live USB Boot
- Internal Watchdog
- Read-only filesystem (Overlayfs)
- Disables unnecessary background services
- Network Security
- Data Tampering Security
- User Security

---

### Cloning Raspberry Pi SD Card

#### Cloning

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

#### Pishrink

If you are a linux user you can shrink this cloned img file using  
```sh
$ sudo ./scripts/pishrink.sh -vd /path/to/saved/rpi.img /path/to/save/pishrink_rpi.img
```

#### Writing img to SD Card

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
---

### References
Pishrink - https://github.com/Drewsif/PiShrink
Overlayfs - https://github.com/ghollingworth/overlayfs