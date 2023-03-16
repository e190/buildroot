################################################################################
#
# helloworld
#
################################################################################

LYNQ_FTP_VERSION:= 1.0.0
LYNQ_FTP_SITE:= $(CURDIR)/mobiletek/samples/ftp
LYNQ_FTP_SITE_METHOD:=local
LYNQ_FTP_INSTALL_TARGET:=YES

define LYNQ_FTP_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
            CFLAGS="-L$(CURDIR)/mobiletek/tools/linux64/usr/lib/" \
            -C $(@D) all
endef

define LYNQ_FTP_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/ftp_demo $(TARGET_DIR)/usr/bin
endef

define LYNQ_FTP_PERMISSIONS
    /usr/bin/ftp_demo f 4755 0 0 - - - - -
endef

$(eval $(generic-package))
