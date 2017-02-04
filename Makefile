# @file Makefile
#
# @brief This Makefile for Ascii85 code on posix target
#
# @par
# Copyright © 2017 Doug Currie, Londonderry, NH, USA. All rights reserved.
#

LINT = /usr/local/bin/flexe-lint/flint

TARGET = test

CFLAGS = -std=c99 -Wall -Wextra -Wmissing-prototypes $(TEST_FLAGS)

TEST_OBJS = \
	lcut/lcut.o \
	lcut/xorshift_e.o \
	test_ascii85.o

TARGET_OBJS = \
	ascii85.o

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

# automatic dependency tracking from http://scottmcpeak.com/autodepend/autodepend.html
%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.o
	$(CC) -MM $(CFLAGS) $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

# version.h is changed every time the make is performed

# testapp: version_header $(TARGET)
# 
# .PHONY: version_header
# version_header:
# 	@echo "#define SVN_VERSION \"`svnversion -n .`\"" > version.h

$(TARGET): $(TEST_OBJS) $(TARGET_OBJS) Makefile
	$(CC) $(TEST_OBJS) $(TARGET_OBJS) $(LDFLAGS) -o $(TARGET)

.PHONY: lint
lint:
	$(LINT) lint/em3.lnt $(TARGET_OBJS:.o=.c)

.PHONY: clean
clean:
	rm -f *.o *.d cmsis/*.o cmsis/*.d lcut/*.o lcut/*.d
