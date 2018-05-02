/*
 * Copyright (C) 2018 The University of Notre Dame This software is
 * distributed under the GNU General Public License. See the file LICENSE
 * for details. 
 */

#include "console.h"
#include "kobject.h"
#include "kmalloc.h"
#include "fs.h"
#include "device.h"

struct kobject *kobject_create_file(struct fs_file *f) {
    struct kobject *k = kmalloc(sizeof(*k));
    k->type = KOBJECT_FILE;
    k->rc = 1;
    k->data.file = f;
    return k;
}

struct kobject *kobject_create_device(struct device *d){ 
    struct kobject *k = kmalloc(sizeof(*k));
    k->type = KOBJECT_DEVICE;
    k->rc = 1;
    k->data.device = d;
    return k;
}

struct kobject *kobject_create_graphics(struct graphics *g){ 
    struct kobject *k = kmalloc(sizeof(*k));
    k->type = KOBJECT_GRAPHICS;
    k->rc = 1;
    k->data.graphics = g;
    return k;
}

struct kobject *kobject_create_pipe(struct pipe *p){ 
    struct kobject *k = kmalloc(sizeof(*k));
    k->type = KOBJECT_PIPE;
    k->rc = 1;
    k->data.pipe = p;
    return k;
}

int kobject_read(struct kobject *kobject, void *buffer, int size) {
    switch (kobject->type) {
        case KOBJECT_INVALID: return 0;
        case KOBJECT_GRAPHICS: return 0;
        case KOBJECT_FILE:
            return fs_file_read(kobject->data.file, (char*)buffer, (uint32_t)size);
        case KOBJECT_DEVICE:
            return device_read(kobject->data.device, buffer, size/kobject->data.device->block_size, 0);
        case KOBJECT_PIPE:
            return pipe_read(kobject->data.pipe, buffer, size);
    }
    return 0;
}

int kobject_write(struct kobject *kobject, void *buffer, int size) {
    switch (kobject->type) {
        case KOBJECT_INVALID: return 0;
        case KOBJECT_GRAPHICS:
            return graphics_object_write((struct graphics_command*)buffer, kobject->data.graphics);
        case KOBJECT_FILE:
            return fs_file_write(kobject->data.file, (char*)buffer, (uint32_t)size);
        case KOBJECT_DEVICE:
            return device_write(kobject->data.device, buffer, size/kobject->data.device->block_size, 0);
        case KOBJECT_PIPE:
            return pipe_write(kobject->data.pipe, buffer, size);
    }
    return 0;
}

int kobject_close(struct kobject *kobject) {
    int ret;
    if (--kobject->rc <= 0) {
        switch (kobject->type) {
            case KOBJECT_INVALID: return 0;
            case KOBJECT_GRAPHICS: return 0;
            case KOBJECT_FILE:
                ret = fs_file_close(kobject->data.file);
                kfree(kobject);
                return ret;
            case KOBJECT_DEVICE: return 0;
            case KOBJECT_PIPE:
                pipe_close(kobject->data.pipe);
                return 1;
        }
    } else if (kobject->rc == 1) {
        switch (kobject->type) {
            case KOBJECT_INVALID: return 0;
            case KOBJECT_GRAPHICS: return 0;
            case KOBJECT_FILE: return 0;
            case KOBJECT_DEVICE: return 0;
            case KOBJECT_PIPE:
                pipe_flush(kobject->data.pipe);
                return 0;
        }
    }
    return 0;
}

int kobject_set_blocking(struct kobject *kobject, int b) {
    switch (kobject->type) {
        case KOBJECT_INVALID: return 0;
        case KOBJECT_GRAPHICS: return 0;
        case KOBJECT_FILE: return 0;
        case KOBJECT_DEVICE:
            return device_set_blocking(kobject->data.device, b);
        case KOBJECT_PIPE:
            return pipe_set_blocking(kobject->data.pipe, b);
    }
    return 0;
}

