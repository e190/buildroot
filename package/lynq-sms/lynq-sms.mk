################################################################################
#
# helloworld
#
################################################################################

LYNQ_SMS_VERSION:= 1.0.0
LYNQ_SMS_SITE:= $(CURDIR)/mobiletek/samples/sms
LYNQ_SMS_SITE_METHOD:=local
LYNQ_SMS_INSTALL_TARGET:=YES

define LYNQ_SMS_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
            CFLAGS="-L$(CURDIR)/mobiletek/tools/linux64/usr/lib/" \
            -C $(@D) all
endef

define LYNQ_SMS_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/smsdemo $(TARGET_DIR)/usr/bin
endef

define LYNQ_SMS_PERMISSIONS
    /usr/bin/smsdemo f 4755 0 0 - - - - -
endef

$(eval $(generic-package))
