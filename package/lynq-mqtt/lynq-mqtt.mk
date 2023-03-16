################################################################################
#
# helloworld
#
################################################################################

LYNQ_MQTT_VERSION:= 1.0.0
LYNQ_MQTT_SITE:= $(CURDIR)/mobiletek/samples/mqtt
LYNQ_MQTT_SITE_METHOD:=local
LYNQ_MQTT_INSTALL_TARGET:=YES

define LYNQ_MQTT_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
            CFLAGS="-L$(CURDIR)/mobiletek/tools/linux64/usr/lib/" \
            -C $(@D) all
endef

define LYNQ_MQTT_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/mqtt_demo $(TARGET_DIR)/usr/bin
endef

define LYNQ_MQTT_PERMISSIONS
    /usr/bin/mqtt_demo f 4755 0 0 - - - - -
endef

$(eval $(generic-package))
