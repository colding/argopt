#  
#  argopt, a library to pass command line options.
#  Copyright (C) 2003-2012 Jules Colding <jcolding@gmail.com>
#
#  This file is part of argopt.
#
#  argopt is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as
#  published by the Free Software Foundation, either version 3 of 
#  the License, or (at your option) any later version.
#
#  argopt is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General 
#  Public License along with argopt. If not, 
#  see <http://www.gnu.org/licenses/>.

VERSION = 0.3.0

SHELL = /bin/sh
LDCONFIG = /sbin/ldconfig
INSTALL = install
GZIP = /usr/bin/gzip
CP = /bin/cp
RM = /bin/rm
LN = /bin/ln
MV = /bin/mv
AR = ar
CC = gcc

PREFIX = /usr/local
MANDIR = $(PREFIX)/share/man/man3

CFLAGS = -Wall -Os

LIB_SOURCE = argopt.c
LIB_HEADER = argopt.h
LIB_OBJ = argopt.o
LIB = libargopt

TEST_SOURCE = test.c
TEST_OBJ = test.o
STATIC_BIN = static-opt
SHARED_BIN = shared-opt
SOURCE_BIN = source-opt

all: lib static-test shared-test source-test

test:  static-test shared-test source-test

install: lib
	$(INSTALL) -m 644 -g root -o root $(LIB_HEADER) $(PREFIX)/include
	$(INSTALL) -m 644 -g root -o root libargopt.a $(PREFIX)/lib
	$(INSTALL) -m 755 -g root -o root libargopt.so.$(VERSION) $(PREFIX)/lib
	$(LDCONFIG) -v
	cd $(PREFIX)/lib && $(LN) -sf libargopt.so.0 libargopt.so
	$(CP) argopt.3 $(MANDIR)/ && $(GZIP) $(MANDIR)/argopt.3 

uninstall:
	$(RM) -f $(PREFIX)/lib/libargopt.*
	$(RM) -f $(PREFIX)/include/argopt.h
	$(RM) -f $(MANDIR)/argopt.*
	$(LDCONFIG) -v

static-test: static $(TEST_OBJ)
	@echo
	@echo "Creating static library test binary.."
	$(CC) $(CFLAGS) -static -o $(STATIC_BIN) $(TEST_OBJ) -L. -largopt

shared-test: shared $(TEST_OBJ)
	@echo
	@echo "Creating shared library test binary.."
	$(CC) $(CFLAGS) -Wl,-rpath,$(.) -o $(SHARED_BIN) $(TEST_OBJ) -L. -largopt

source-test: 
	@echo
	@echo "Creating ordinary test binary.."
	$(CC) $(CFLAGS) $(LIB_SOURCE) $(TEST_SOURCE) -o $(SOURCE_BIN)

lib: static shared

shared:
	@echo
	@echo "Creating shared library.."
	$(CC) $(CFLAGS) -fPIC -c $(LIB_SOURCE) -o $(LIB_OBJ)
	$(CC) -shared -Wl,-soname,libargopt.so.0 -o libargopt.so.$(VERSION) $(LIB_OBJ) -lc
	$(LDCONFIG) -n .
	$(LN) -sf libargopt.so.0 libargopt.so

static: $(LIB_OBJ)
	@echo
	@echo "Creating static library.."
	$(AR) rsc $(LIB).a $?

$(LIB_OBJ): $(LIB_SOURCE)
	$(CC) $(CFLAGS) -c $?

$(TEST_OBJ): $(TEST_SOURCE)
	$(CC) $(CFLAGS) -c $?

clean:
	$(RM) -f *~ *.o* *.a* *.so* $(STATIC_BIN) $(SHARED_BIN) $(SOURCE_BIN)
