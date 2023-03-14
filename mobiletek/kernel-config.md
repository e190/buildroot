# kernel config

       [*] Linux Kernel
             Kernel version (Custom local directory)  --->
           (mobiletek/kernel) URL of custom kernel local directory
           ()    Custom kernel patches
             Kernel configuration (Using a custom (def)config file)  --->
           (board/mobiletek/asr1803/linux_config) Configuration file path
           ()    Additional configuration fragment files
           ()    Custom boot logo file path
             Kernel binary format (zImage)  --->
             Kernel compression format (gzip compression)  --->
       [ ]   Build a Device Tree Blob (DTB)
       [ ]   Install kernel image to /boot in target
       [ ]   Needs host OpenSSL
       [ ]   Needs host libelf
       [ ]   Needs host pahole
             Linux Kernel Extensions  --->
             Linux Kernel Tools  --->



# rootfs config
      [ ] axfs root filesystem
      [ ] btrfs root filesystem
      [ ] cloop root filesystem for the target device
      [ ] cpio the root filesystem (for use as an initial RAM filesystem)
      [ ] cramfs root filesystem
      [ ] erofs root filesystem
      [ ] ext2/3/4 root filesystem
      [ ] f2fs root filesystem
          *** initramfs needs a Linux kernel to be built ***
      [ ] jffs2 root filesystem
      [ ] oci image
      [ ] romfs root filesystem
      [ ] squashfs root filesystem
      [*] tar the root filesystem
            Compression method (no compression)  --->
          ()    other random options to pass to tar
      [*] ubi image containing an ubifs root filesystem
          (0x20000) physical eraseblock size
          (2048) sub-page size
      [*]   Use custom config file
          (board/mobiletek/asr1803/ubinize_rootfs.cfg) Configuration file path
          ()    Additional ubinize options
      -*- ubifs root filesystem
          (0x1f000) logical eraseblock size
          (0x800) minimum I/O unit size
          (2048) maximum logical eraseblock count
            ubifs runtime compression (lzo)  --->
            Compression method (no compression)  --->
          ()    Additional mkfs.ubifs options
      [ ] yaffs2 root filesystem
