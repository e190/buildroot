################################################################################
#
# helloworld
#
################################################################################

LYNQ_AT_VERSION:= 1.0.0
LYNQ_AT_SITE:= $(CURDIR)/mobiletek/samples/at
LYNQ_AT_SITE_METHOD:=local
LYNQ_AT_INSTALL_TARGET:=YES

define LYNQ_AT_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
            CFLAGS="-L$(CURDIR)/mobiletek/tools/linux64/usr/lib/" \
            -C $(@D) all
endef

define LYNQ_AT_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/at_demo $(TARGET_DIR)/usr/bin
endef

define LYNQ_AT_PERMISSIONS
    /usr/bin/at_demo f 4755 0 0 - - - - -
endef

$(eval $(generic-package))
