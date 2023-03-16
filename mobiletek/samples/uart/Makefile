#--------------------------------------------------------------
#               Mbtk OpenLinux
#
#--------------------------------------------------------------


TARGET:=uartdemo

OPT    = -O2
DEBUG  = -g
OTHER  = -Wall -Wno-deprecated
CFLAGS = $(OPT) $(OTHER)
INC_DIR = -I.
LIBDIR = -L
LIBS =

CFILES = uart_sample.c

OBJFILE=$(C++FILES:.cpp=.o) $(CFILES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJFILE)
	$(CC) $^ $(CFLAGS) $(LIBDIR) $(INC_DIR) $(LIBS) -o $@

%.o:%.c
	$(CC) -o $@ -c $(CFLAGS) $< $(INC_DIR)

clean:
	rm -f $(TARGET)
