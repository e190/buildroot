################################################################################
#
# helloworld
#
################################################################################

LYNQ_SIM_VERSION:= 1.0.0
LYNQ_SIM_SITE:= $(CURDIR)/mobiletek/samples/sim
LYNQ_SIM_SITE_METHOD:=local
LYNQ_SIM_INSTALL_TARGET:=YES

define LYNQ_SIM_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
            CFLAGS="-L$(CURDIR)/mobiletek/tools/linux64/usr/lib/" \
            -C $(@D) all
endef

define LYNQ_SIM_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/sim_demo $(TARGET_DIR)/usr/bin
endef

define LYNQ_SIM_PERMISSIONS
    /usr/bin/sim_demo f 4755 0 0 - - - - -
endef

$(eval $(generic-package))
