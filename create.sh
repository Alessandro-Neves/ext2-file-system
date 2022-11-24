rm -rf local-ext2.img
dd if=/dev/zero of=./local-ext2.img bs=1024 count=64K
mkfs.ext2 -b 1024 ./local-ext2.img

e2fsck local-ext2.img
sudo mount local-ext2.img /mnt
sudo umount /mnt