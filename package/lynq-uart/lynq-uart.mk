################################################################################
#
#               Mbtk OpenLinux
#
################################################################################

LYNQ_UART_VERSION:= 1.0.0
LYNQ_UART_SITE:= $(CURDIR)/mobiletek/samples/uart
LYNQ_UART_SITE_METHOD:=local
LYNQ_UART_INSTALL_TARGET:=YES

define LYNQ_UART_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef

define LYNQ_UART_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/uartdemo $(TARGET_DIR)/usr/bin
endef

define LYNQ_UART_PERMISSIONS
    /usr/bin/uartdemo f 4755 0 0 - - - - -
endef

$(eval $(generic-package))
