#!/usr/bin/env bash

# BOARD_DIR board/mobiletek/asr1803

main()
{
    echo "execute: post-build.sh **************************"
    BOARD_DIR="$(dirname $0)"
	echo "BOARD_DIR ${BOARD_DIR}"

    if [ -d ${BUILD_DIR}/rootfs-mmp ]
    then
       rm -r ${BUILD_DIR}/rootfs-mmp
    fi
    mkdir ${BUILD_DIR}/rootfs-mmp
    tar -zxf $BOARD_DIR/rootfs.tar.gz -C ${BUILD_DIR}/rootfs-mmp
    cp -r $BOARD_DIR/rootfs-overlay/* ${BUILD_DIR}/rootfs-mmp/

	exit $?
}

main
