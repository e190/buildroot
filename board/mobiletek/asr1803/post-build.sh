#!/usr/bin/env bash

# BOARD_DIR board/mobiletek/asr1803

main()
{
    echo "execute: post-build.sh **************************"
    BOARD_DIR="$(dirname $0)"
	echo "BOARD_DIR ${BOARD_DIR}"
    local cur_dateTime=`date +"%Y-%m-%d %H:%M:%S"`
    echo "Build time: ${cur_dateTime}" > $BOARD_DIR/build_info
    install -m 0644 -D $BOARD_DIR/build_info $TARGET_DIR/etc/build_info
    install -m 0644 -D $BOARD_DIR/mbtk-version $TARGET_DIR/etc/mbtk-version

	exit $?
}

main
