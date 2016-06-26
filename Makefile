#!/bin/bash

CC := gcc
TARGET := wprime

VPATH := .
vpath %.c .
vpath %.o .

objects = $(notdir $(subst .c,.o,$(wildcard *.c)))
CFLAGS := -O2

all : $(TARGET)
	@echo "compiled" $(TARGET)


$(TARGET) : $(objects)
	@$(CC) $(CFLAGS) $^ -o $@

%.o : %.c
	@$(CC) $(CFLAGS) -c $^ -o $@


.PHONY : install
install : $(TARGET)
	@echo -en "installing wprime to /usr/bin/wprime."
	install -s wprime /usr/bin/

