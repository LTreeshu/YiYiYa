/*******************************************************************
 * Copyright 2021-2080 evilbinary
 * 作者: evilbinary on 01/01/20
 * 邮箱: rootdebug@163.com
 ********************************************************************/
#include "serial.h"

static int is_send() { return io_read8(PORT_COM1 + 5) & 0x20; }

static int is_receive() { return io_read8(PORT_COM1 + 5) & 1; }

void serial_write(char a) {
  int i=0;
  while (is_send() == 0||i>1000){
    i++;
  }
  io_write8(PORT_COM1, a);
}

char serial_read() {
  int i=0;
  while (is_receive() == 0||i>1000){
    i++;
  }
  return io_read8(PORT_COM1);
}

void serial_printf(char* fmt, ...) {
  int i;
  char buf[1024];
  va_list args;
  va_start(args, fmt);
  i = vsprintf(buf, fmt, args);
  va_end(args);
  for (int j = 0; j < i; j++) {
    serial_write(buf[j]);
  }
}

static size_t read(device_t* dev, void* buf, size_t len) {
  u32 ret = len;
  for (int i = 0; i < len; i++) {
    ((char*)buf)[i] = serial_read();
  }
  return ret;
}

static size_t write(device_t* dev, void* buf, size_t len) {
  u32 ret = len;
  for (int i = 0; i < len; i++) {
    serial_write(((char*)buf)[i]);
  }
  return ret;
}

int serial_init(void) {
  device_t* dev = kmalloc(sizeof(device_t));
  dev->name = "serial";
  dev->read = read;
  dev->write = write;
  dev->id = DEVICE_SERIAL;
  dev->type = DEVICE_TYPE_CHAR;
  device_add(dev);

  // init
  io_write8(PORT_COM1 + 1, 0x00);
  io_write8(PORT_COM1 + 3, 0x80);
  io_write8(PORT_COM1 + 0, 0x03);
  io_write8(PORT_COM1 + 1, 0x00);
  io_write8(PORT_COM1 + 3, 0x03);
  io_write8(PORT_COM1 + 2, 0xC7);
  io_write8(PORT_COM1 + 4, 0x0B);
  return 0;
}

void serial_exit(void) { kprintf("serial exit\n"); }

module_t serial_module = {
    .name = "serial", .init = serial_init, .exit = serial_exit};
