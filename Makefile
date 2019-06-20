
TARGET = http_server1

CROSS = arm-linux-gnueabihf-
CXX  = $(CROSS)g++
CC = $(CROSS)gcc
AR  = $(CROSS)ar

CFLAGS += -Wno-write-strings -Wformat=0 -fpermissive -g -O -I include 
VERBOSE = @
COMPILE = $(VERBOSE) $(CXX) $(CFLAGS) -c
LINK = $(VERBOSE) $(CXX)
LINKLIB = $(VERBOSE) $(AR) -r
SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJFILES = $(SOURCES:%.c=%.o)
SQLITE3OBJ = libsqlite3/sqlite3.o
LIBS = RK_SDK.a whitelist_sdk.a libcurl.a libiconv.a

.PHONY: clean

all:	$(TARGET)

$(TARGET):	$(OBJFILES) $(SQLITE3OBJ)
	@echo Linking $@  from $^ $(LIBS)  
	#@echo $(LINK)  $^ $(LIBS) -o $@  -ldl  -lpthread -lm
	$(LINK)  $^ $(LIBS) -o $@  -L . -lpthread  -lm  -ldl -lssl  -lcrypto   
$(OBJFILES):	%.o: %.c $(HEADERS)
	@echo Compiling $@ from $<
	$(COMPILE) $< -o $@ 
$(SQLITE3OBJ):%.o: %.c 
	@echo Compiling $@ from $<
	#@echo $(CC) -c $< -o $@ 
	$(CC) -c $< -o $@ 

clean:
	@echo Removing generated files..
	$(VERBOSE) -$(RM) -rf $(OBJFILES) $(TARGET) $(SQLITE3OBJ) *~ *.d .dep

