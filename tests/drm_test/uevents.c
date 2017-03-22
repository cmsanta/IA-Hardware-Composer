#include "uevents.h"

#define UEVENT_MSG_LEN  1024
struct modeset_dev;


int open_uevent_socket()
{
    struct sockaddr_nl addr;
    int sz = 64*1024; // XXX larger? udev uses 16MB!
    int s;
    int data;

    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();
    addr.nl_groups = 0xffffffff;

    s = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if(s < 0) {
        fprintf(stderr, "Couldn't open the domain socket: %d \n", PF_NETLINK);
        return -1;
     }

    setsockopt(s, SOL_SOCKET, SO_RCVBUFFORCE, &sz, sizeof(sz));

    if(bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        close(s);
        fprintf(stderr, "Couldn't bind with socket: %d\n",s);
        return -1;
    }

    return s;
}


int init_worker(const char *name, void *d) {
  pthread_condattr_t cond_attr;
  pthread_condattr_init(&cond_attr);
  pthread_condattr_setclock(&cond_attr, CLOCK_MONOTONIC);
  int ret = pthread_cond_init(&cond_, &cond_attr);
  
  name_= name;

  if (ret) {
    fprintf(stderr, "Failed to int thread %s condition %d", name_, ret);
    return ret;
  }

  ret = pthread_mutex_init(&lock_, NULL);
  
  if (ret) {
    fprintf(stderr, "Failed to init thread %s lock %d", name_, ret);
    pthread_cond_destroy(&cond_);
    return ret;
  }

  ret = pthread_create(&thread_, NULL, internal_routine, d);
  
  if (ret) {
    fprintf(stderr, "Could not create thread %s %d", name_, NULL);
    pthread_mutex_destroy(&lock_);
    pthread_cond_destroy(&cond_);
    return ret;
  }else {
   fprintf(stderr, "Thread worker: %s running... \n",name_);
  }

  return 0;
}

void internal_routine(void *d) {

 priority_ =  HAL_PRIORITY_URGENT_DISPLAY;

  setpriority(PRIO_PROCESS, 0, priority_);

  while (true) {
    int ret = pthread_mutex_lock(&lock_);
    if (ret) {
      fprintf(stderr, "Failed to lock %s thread %d", name_, ret);
      continue;
  }

  bool exit = exit_;

  ret = pthread_mutex_unlock(&lock_);
	
  if (ret) {
      fprintf(stderr, "Failed to unlock %s thread %d", name_, ret);
      break;
  }

  if (exit)
      break;

  uevent_handler(d);
  }
 return;
}


void uevent_handler(void *d){
	char buffer[1024];
	int fd, ret, uevent_fd;
    unsigned int i;
	drmModeRes *res;
	drmModeConnector *conn;
	drmModeEncoder *enc;
	int32_t crtc;	
	
	struct timespec ts;
	uint64_t timestamp = 0;
	ret = clock_gettime(CLOCK_MONOTONIC, &ts);

	struct workerArgs *wa;
	/* Unpack the arguments */
	wa = (struct workerArgs*) d;
	fd = wa->fd;

	uevent_fd = wa->fd_socket;
	fprintf(stderr,"fb : %d\n",wa->fb);
	
	if (!ret)
	  timestamp = ts.tv_sec * 1000 * 1000 * 1000 + ts.tv_nsec;
	else
	  fprintf(stderr, "Failed to get monotonic clock on hotplug %d", ret);
	
	while (true) {
		
	  ret = read(uevent_fd, &buffer, sizeof(buffer));
	  
	  if (ret >= 0) {

		/* retrieve resources */
		res = drmModeGetResources(fd);

		if (!res) {
			fprintf(stderr, "cannot retrieve DRM resources (%d): %m\n",errno);
			break;
		}

		/* iterate all connectors */
		for (i = 0; i < res->count_connectors; ++i) {
			/* get information for each connector */
			conn = drmModeGetConnector(fd, res->connectors[i]);
			if (!conn) {
				fprintf(stderr, "cannot retrieve DRM connector %u:%u (%d): %m\n",i, res->connectors[i], errno);
				continue;
			}
		
			/* check if a monitor is connected */
			if (conn->connection != DRM_MODE_CONNECTED) {
				fprintf(stderr, "DRM_MODE_DISCONNECTED : connector[%u] \n",conn->connector_id);
			}else{
				fprintf(stderr, "DRM_MODE_CONNECTED : connector[%u] \n",conn->connector_id);
			/* first try the currently conected encoder+crtc */
			if (conn->encoder_id) {
				enc = drmModeGetEncoder(fd, conn->encoder_id);
				}
			else
				enc = NULL;
			
			if (enc) {
				if (enc->crtc_id) {
					crtc = enc->crtc_id;
					/* force the modeset */
					fprintf(stderr, "Forcing a modeset...\n");
				    drmModeSetCrtc(fd, crtc, wa->fb, 0, 0, &conn, 1, &conn->modes[0]);
					break;
				}				
			}

			mModeFreeConnector(conn);
			mModeFreeEncoder(enc);
			mModeFreeResources(res);			

			}
			
		}
		//no events to process
		return;
	  }

	 bool exit = exit_;

	 if (exit)
	    break;
	}

}


