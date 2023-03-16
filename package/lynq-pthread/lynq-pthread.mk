################################################################################
#
# helloworld
#
################################################################################

LYNQ_PTHREAD_VERSION:= 1.0.0
LYNQ_PTHREAD_SITE:= $(CURDIR)/mobiletek/samples/pthread
LYNQ_PTHREAD_SITE_METHOD:=local
LYNQ_PTHREAD_INSTALL_TARGET:=YES

define LYNQ_PTHREAD_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
            CFLAGS="-L$(CURDIR)/mobiletek/tools/linux64/usr/lib/" \
            -C $(@D) all
endef

define LYNQ_PTHREAD_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/pthreaddemo $(TARGET_DIR)/usr/bin
endef

define LYNQ_PTHREAD_PERMISSIONS
    /usr/bin/pthreaddemo f 4755 0 0 - - - - -
endef

$(eval $(generic-package))
