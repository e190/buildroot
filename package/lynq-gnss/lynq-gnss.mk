################################################################################
#
# helloworld
#
################################################################################

LYNQ_GNSS_VERSION:= 1.0.0
LYNQ_GNSS_SITE:= $(CURDIR)/mobiletek/samples/gnss
LYNQ_GNSS_SITE_METHOD:=local
LYNQ_GNSS_INSTALL_TARGET:=YES

define LYNQ_GNSS_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
            CFLAGS="-L$(CURDIR)/mobiletek/tools/linux64/usr/lib/" \
            LYNQ_DIR="$(CURDIR)/mobiletek/" -C $(@D) all
endef

define LYNQ_GNSS_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/gnssdemo $(TARGET_DIR)/usr/bin
endef

define LYNQ_GNSS_PERMISSIONS
    /usr/bin/gnssdemo f 4755 0 0 - - - - -
endef

$(eval $(generic-package))
