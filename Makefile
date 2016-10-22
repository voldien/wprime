#!/bin/bash


#	
BASE ?= $(call my-dir)
SHELL ?= /bin/bash
DESTDIR=/
MAKE := make
CMAKE := cmake
MKDIR ?=  mkdir -p
TAR := tar
CP ?= cp
RM ?= rm -rf
INSTALL ?= install

#
CLANGCC ?= clang
GCC := gcc
CC ?= $(GCC)
TARGET := wprime

#
VPATH := .
vpath %.c .
vpath %.o .


#	Extracting git tag version.
MAJOR_VERSION := ` git describe | sed -e 's/[^0-9]*//g' | cut -c1-1 `
MINOR_VERSION := ` git describe | sed -e 's/[^0-9].[^0-9]*//g' | cut -c1-1 `
REVISION_VERSION := ` git describe | sed -e 's/[^0-9].[^0-9].[^0-9]*//g' | cut -c1-2`
#
major_version := $(MAJOR_VERSION)
minor_version := $(MINOR_VERSION)
revision_version := $(REVISION_VERSION)

INSTALL_PATH := "/usr/bin/"

objects = $(notdir $(subst .c,.o,$(wildcard *.c)))
CFLAGS := -O3 -DWPRIME_MAJOR_VERSION=$(major_version) -DWPRIME_MINOR_VERSION=$(minor_version) -DWPRIME_REVISION_VERSION=$(revision_version)

all : $(TARGET)
	@echo "Using compiler " $(CC)
	@echo "Compiled" $(TARGET).


$(TARGET) : $(objects)
	@$(CC) $(CFLAGS) $^ -o $@

#	Compile source code to object code.
%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@


.PHONY : install
install : $(TARGET)
	@echo "Installing wprime to $(INSTALL_PATH)$(TARGET)"
	install -s wprime $(INSTALL_PATH)
	gzip -ck wprime.1 > wprime.1.gz
	sudo cp  wprime.1.gz /usr/share/man/man1/


.PHONY : clean
clean : 
	@echo "Removing Object files."
	$(RM) *.o
