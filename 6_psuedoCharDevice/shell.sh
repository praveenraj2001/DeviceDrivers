# make a file main.c and Makefile


#Run make file using CROSSCOMPILE
make -C /home/praveen/DeviceDrivers/psuedoCharDevice/linux-6.1.4/ M=${PWD} ARCH=arm \ CROSS_COMPILE=arm-linux-gnueabi- modules

# make a directory called rootfs in /mnt

# mount rootfs.img (acts as sd card) to /mnt/rootfs
sudo mount -o loop,rw,sync rootfs.img /mnt/rootfs
sudo cp char_driver.ko /mnt/rootfs/home/root/
sudo umount /mnt/rootfs 

# copy .dtb, and zImage to outTree folder

# Run the Target machine(Qemu)
qemu-system-arm -M vexpress-a9 -m 1024 -nographic -kernel zImage -dtb vexpress-v2p-ca9.dtb -sd rootfs.img -append "console=ttyAMA0 root=/dev/mmcblk0 rw"

# now ls to check weather .ko file is there or not in target machine

#now to clear kernal messages
echo dmesg -c

#now to load module
echo insmod main.ko ndevice=20

#now to check all the inserted modules
lsmod

#now to remove module
rmmod main.ko
