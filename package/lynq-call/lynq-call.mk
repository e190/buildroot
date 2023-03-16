################################################################################
#
# helloworld
#
################################################################################

LYNQ_CALL_VERSION:= 1.0.0
LYNQ_CALL_SITE:= $(CURDIR)/mobiletek/samples/call
LYNQ_CALL_SITE_METHOD:=local
LYNQ_CALL_INSTALL_TARGET:=YES

define LYNQ_CALL_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
            CFLAGS="-L$(CURDIR)/mobiletek/tools/linux64/usr/lib/" \
            -C $(@D) all
endef

define LYNQ_CALL_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/calldemo $(TARGET_DIR)/usr/bin
endef

define LYNQ_CALL_PERMISSIONS
    /usr/bin/calldemo f 4755 0 0 - - - - -
endef

$(eval $(generic-package))
