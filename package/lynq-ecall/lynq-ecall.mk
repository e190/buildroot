################################################################################
#
# helloworld
#
################################################################################

LYNQ_ECALL_VERSION:= 1.0.0
LYNQ_ECALL_SITE:= $(CURDIR)/mobiletek/samples/ecall
LYNQ_ECALL_SITE_METHOD:=local
LYNQ_ECALL_INSTALL_TARGET:=YES

define LYNQ_ECALL_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
            CFLAGS="-L$(CURDIR)/mobiletek/tools/linux64/usr/lib/" \
            -C $(@D) all
endef

define LYNQ_ECALL_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/ecalldemo $(TARGET_DIR)/usr/bin
endef

define LYNQ_ECALL_PERMISSIONS
    /usr/bin/ecalldemo f 4755 0 0 - - - - -
endef

$(eval $(generic-package))
