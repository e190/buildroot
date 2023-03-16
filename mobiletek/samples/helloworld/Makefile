

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
