#ifndef BUFFER_H
#define BUFFER_H
#define BUF_SIZE 8192

#include "miscellaneous.h"
#include "fd.h"
#include <linux/types.h>
#include "ioctl.h"
#include "cryptodev-1.0/cryptoapi.h"
#include <linux/semaphore.h>
#include <linux/wait.h>
#include <linux/sched.h>

struct buffer {
	char *buf;
	unsigned int id;
	struct buffer *prev, *next;
	int rfd, wfd;
	int rp, wp;
	int written;
	struct semaphore sem;
	wait_queue_head_t rq;
};

void buffer_init(void);
struct buffer *init_buffer(void);
void destroy_buffer(struct buffer *b);
struct buffer *get_buffer(unsigned int id);
struct buffer *get_buffer_fd(int fd);
void disassociate_fd(int fd);
ssize_t buffer_size(struct buffer *b);
ssize_t buffer_read(struct buffer *b, const char *dst, ssize_t maxlen,
		    loff_t * off, struct crypto_smode *cmode,
		    struct cryptodev_state *cstate);
ssize_t buffer_write(struct buffer *b, const char *src, ssize_t maxlen,
		     loff_t * off, struct crypto_smode *cmode,
		     struct cryptodev_state *cstate);

#endif
