
#ifndef _UEVENTS_H
#define _UEVENTS_H

#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <linux/netlink.h>
#include <private/android_filesystem_config.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <stdlib.h>

#include <stdbool.h>

#include <pthread.h>
#include <stdint.h>

#define HAL_PRIORITY_URGENT_DISPLAY     (-8)

struct modeset_dev;

typedef struct modeset_dev{
	struct modeset_dev *next;

	uint32_t width;
	uint32_t height;
	uint32_t stride;
	uint32_t size;
	uint32_t handle;
	uint8_t *map;

	drmModeModeInfo mode;
	uint32_t fb;
	uint32_t conn;
	uint32_t crtc;
	drmModeCrtc *saved_crtc;
}modeset;

struct workerArgs {
	struct workerArgs *next;
	int fd;
	int fd_socket;
	uint32_t fb;  
};
int open_uevent_socket();
int init_worker(const char *name_, void *d);
void internal_routine(void*d );
void uevent_handler(void *d);

struct uevent {
    const char *action;
    const char *path;
    const char *subsystem;
    const char *firmware;
    int major;
    int minor;
};

pthread_t thread_;
pthread_mutex_t lock_;
pthread_cond_t cond_;
const char *name_;
int priority_;
int fd_socket_;

bool exit_;

#endif /* _UEVENTS_H */
