################################################################################
#
# helloworld
#
################################################################################

LYNQ_LCD_VERSION:= 1.0.0
LYNQ_LCD_SITE:= $(CURDIR)/mobiletek/samples/lcd
LYNQ_LCD_SITE_METHOD:=local
LYNQ_LCD_INSTALL_TARGET:=YES

define LYNQ_LCD_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
            CFLAGS="-L$(CURDIR)/mobiletek/tools/linux64/usr/lib/" \
            -C $(@D) all
endef

define LYNQ_LCD_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/lcddemo $(TARGET_DIR)/usr/bin
endef

define LYNQ_LCD_PERMISSIONS
    /usr/bin/lcddemo f 4755 0 0 - - - - -
endef

$(eval $(generic-package))
