#!/usr/bin/env bash

# ${BUILD_DIR}     output/build
# ${TARGET_DIR}    output/target
# ${BINARIES_DIR}  output/images
red='\e[0;31m' # 红色
green='\e[0;32m' # 绿色
NC='\e[0m' # 没有颜色

function kernel-image ()
{
	cp ${BINARIES_DIR}/zImage ${BINARIES_DIR}/openwrt-mmp-asr1803-zImage
	cat ${BUILD_DIR}/linux/arch/arm/boot/dts/asr1803-p401.dtb >> ${BINARIES_DIR}/openwrt-mmp-asr1803-zImage
	# cat ${BUILD_DIR}/linux/arch/arm/boot/dts/asr1803-ab-p401.dtb >> ${BINARIES_DIR}/openwrt-mmp-asr1803-zImage
}

main()
{
    kernel-image
    cp ${BINARIES_DIR}/rootfs.ubi ${BINARIES_DIR}/buildroot.ubi

    echo -e "${green}************************************ ${NC}"
    echo -e "${green}  Build image success!${NC}"
    echo -e "${green}************************************ ${NC}"

	exit $?
}

main $@
