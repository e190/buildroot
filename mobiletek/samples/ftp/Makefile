#--------------------------------------------------------------
#               LYNQ OpenLinux
#
#--------------------------------------------------------------

TARGET:=ftp_demo

OPT    = -O2
DEBUG  = -g
OTHER  = -Wall -Wno-deprecated

LOCAL_C_FLAGS= -march=armv7-a -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard \
			   -Wno-unused-function -pthread

CFLAGS += $(OPT) $(OTHER) $(LOCAL_C_FLAGS)
INC_DIR = -I.
LIBDIR = -L

USR_LIB= -lmbtk_lib

STD_LIB=-lstdc++ -llog -lubus -lprop2uci -lubox -luci -lrilutil -lpolarssl

LIBS = $(STD_LIB) $(USR_LIB)

CFILES = mbtk_ftp.c

OBJFILE=$(C++FILES:.cpp=.o) $(CFILES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJFILE)
	$(CC) $^ $(CFLAGS) $(LIBDIR) $(INC_DIR) $(LIBS) -o $@

%.o:%.c
	$(CC) -o $@ -c $(CFLAGS) $< $(INC_DIR)

clean:
	rm -f $(TARGET)
