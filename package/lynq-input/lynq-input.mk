################################################################################
#
# helloworld
#
################################################################################

LYNQ_INPUT_VERSION:= 1.0.0
LYNQ_INPUT_SITE:= $(CURDIR)/mobiletek/samples/input
LYNQ_INPUT_SITE_METHOD:=local
LYNQ_INPUT_INSTALL_TARGET:=YES

define LYNQ_INPUT_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
            CFLAGS="-L$(CURDIR)/mobiletek/tools/linux64/usr/lib/" \
            -C $(@D) all
endef

define LYNQ_INPUT_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/key_demo $(TARGET_DIR)/usr/bin
endef

define LYNQ_INPUT_PERMISSIONS
    /usr/bin/key_demo f 4755 0 0 - - - - -
endef

$(eval $(generic-package))
