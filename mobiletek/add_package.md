添加自定义package
=================
## 1.源码 hello.c和Makefile一起放在了 buildroot/mobiletek/helloworld 文件夹
-----------------
```c
#include <stdio.h>

int main()
{
    printf("buildroot helloworld\n");
    return 0;
}
```
```makefile
TARGET:=helloworld

OPT    = -O2
DEBUG  = -g
OTHER  = -Wall -Wno-deprecated
CFLAGS = $(OPT) $(OTHER)
INC_DIR = -I.
LIBDIR = -L
LIBS =

CFILES = hello.c

OBJFILE=$(C++FILES:.cpp=.o) $(CFILES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJFILE)
	$(CC) $^ $(CFLAGS) $(LIBDIR) $(INC_DIR) $(LIBS) -o $@
	@echo "-------build module end------"

%.o:%.c
	$(CC) -o $@ -c $(CFLAGS) $< $(INC_DIR)

clean:
	rm -f $(TARGET)
```

## 2.加menuconfig项目
buildroot/package/Config.in
```
……
menu "Text editors and viewers"
    source "package/ed/Config.in"
    source "package/joe/Config.in"
    source "package/less/Config.in"
    source "package/mc/Config.in"
    source "package/most/Config.in"
    source "package/nano/Config.in"
    source "package/uemacs/Config.in"
    source "package/vim/Config.in"
endmenu

menu "myown helloworld"
    source "package/helloworld/Config.in"
endmenu

endmenu
```
注意要在最后这个 endmenu 前插入。

## 3.新建 buildroot/package/helloworld/Config.in
```
config BR2_PACKAGE_HELLOWORLD
bool "helloworld"
help
  This is a demo to add myown helloworld.
```
## 4.新建 buildroot/package/helloworld/helloworld.mk
```makefile
################################################################################
#
# helloworld
#
################################################################################

HELLOWORLD_VERSION:= 1.0.0
HELLOWORLD_SITE:= $(CURDIR)/dl/helloworld
HELLOWORLD_SITE_METHOD:=local
HELLOWORLD_INSTALL_TARGET:=YES

define HELLOWORLD_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef

define HELLOWORLD_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/helloworld $(TARGET_DIR)/bin
endef

define HELLOWORLD_PERMISSIONS
    /bin/helloworld f 4755 0 0 - - - - -
endef

$(eval $(generic-package))
```
make menuconfig 使能helloworld包，然后make。helloworld会自动安装到根文件系统的/bin目录下。
