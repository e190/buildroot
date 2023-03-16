################################################################################
#
# helloworld
#
################################################################################

LYNQ_AUDIO_VERSION:= 1.0.0
LYNQ_AUDIO_SITE:= $(CURDIR)/mobiletek/samples/audio
LYNQ_AUDIO_SITE_METHOD:=local
LYNQ_AUDIO_INSTALL_TARGET:=YES

define LYNQ_AUDIO_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
            CFLAGS="-L$(CURDIR)/mobiletek/tools/linux64/usr/lib/" \
            -C $(@D) all
endef

define LYNQ_AUDIO_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/audiodemo $(TARGET_DIR)/usr/bin
endef

define LYNQ_AUDIO_PERMISSIONS
    /usr/bin/audiodemo f 4755 0 0 - - - - -
endef

$(eval $(generic-package))
