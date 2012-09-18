#include "buffer.h"
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/uaccess.h>

/* Global Variables */
struct buffer *head = NULL;
struct buffer *tail = NULL;
unsigned int bid = 0;
struct semaphore list;

void buffer_init(void)
{
	sema_init(&list, 1);
}

struct buffer *init_buffer(void)
{

	struct buffer **b = &head;
	struct buffer *p = head;

	/* Get lock on list */
	down_interruptible(&list);

	/* Search through looking for last struct */
	while (*b) {
		p = (*b);
		b = &((*b)->next);
	}

	/* Allocate */
	(*b) = (struct buffer *)MALLOC(sizeof(struct buffer));

	/* Set unique identifier */
	(*b)->id = ++bid;

	/* Set next pointer and previous pointers */
	(*b)->prev = p;
	(*b)->next = NULL;

	/* Set read and write pointers */
	(*b)->rp = 0;
	(*b)->wp = 0;
	(*b)->written = 0;

	/* Set read and write fd */
	(*b)->rfd = INVALID_FD;
	(*b)->wfd = INVALID_FD;

	/* Init buffer */
	(*b)->buf = (char *)MALLOC(sizeof(char) * BUF_SIZE);
	memset((*b)->buf, '\0', BUF_SIZE);

	/* Init semaphore */
	sema_init(&(*b)->sem, 1);

	/* Init blocking queue */
	init_waitqueue_head(&(*b)->rq);

	DEBUG("init buffer with id: %d\n", (*b)->id);

	/* Release lock on list */
	up(&list);

	return (*b);

}

void destroy_buffer(struct buffer *b)
{
	/* Assumes callee has lock! */

	/* Previous and Next Buffers */
	struct buffer *prev = b->prev;
	struct buffer *next = b->next;

	/* Disconnecting self */
	if (prev)
		prev->next = next;
	if (next)
		next->prev = prev;
	if (head == b)
		head = b->next;

	DEBUG("destroying buffer with id: %d\n", b->id);

	/* Deallocing self */
	kfree(b->buf);
	kfree(b);

}

struct buffer *get_buffer(unsigned int id)
{
	struct buffer *b = head;

	/* Get a lock on list */
	down_interruptible(&list);

	/* Search through all buffers looking to match id */
	while (b) {
		if (b->id == id) {
			/* Release lock on list */
			up(&list);
			return b;
		}
		b = b->next;
	}

	/* Release lock on list */
	up(&list);

	return NULL;

}

struct buffer *get_buffer_fd(int fd)
{
	struct buffer *b = head;

	/* Get a lock on list */
	down_interruptible(&list);

	/* Search through all buffers looking to match id */
	while (b) {
		if (b->rfd == fd) {
			/* Release lock on list */
			up(&list);
			return b;
		}

		if (b->wfd == fd) {
			/* Release lock on list */
			up(&list);
			return b;
		}

		b = b->next;
	}

	/* Release lock on list */
	up(&list);

	return NULL;

}

void disassociate_fd(int fd)
{
	struct buffer *b = head;

	/* Get a lock on list */
	down_interruptible(&list);

	/* Search through all buffers looking to match read or write fd */
	while (b) {
		if (b->rfd == fd) {

			/* Get lock on buffer */
			down_interruptible(&b->sem);

			/* Disassociate */
			b->rfd = INVALID_FD;

			if (b->wfd == INVALID_FD)
				destroy_buffer(b);

			/* Release lock */
			up(&b->sem);
		}

		if (b->wfd == fd) {

			/* Get lock on buffer */
			down_interruptible(&b->sem);

			/* Disassociate */
			b->wfd = INVALID_FD;

			if (b->rfd == INVALID_FD)
				destroy_buffer(b);

			/* Release lock */
			up(&b->sem);
		}

		b = b->next;
	}

	/* Release lock on list */
	up(&list);
}

ssize_t buffer_size(struct buffer *b)
{
	if (b->wp >= b->rp)
		return b->wp - b->rp;
	return b->wp + (BUF_SIZE - b->rp);
	/*return (BUF_SIZE - b->wp + b->rp) % BUF_SIZE; */
}

ssize_t buffer_read(struct buffer * b, const char *dst, ssize_t maxlen,
		    loff_t * off, struct crypto_smode * cmode,
		    struct cryptodev_state * cstate)
{
	ssize_t tocopy = 0, copied = 0;
	char *start, *end;

	/* Obtain lock on buffer */
	down_interruptible(&b->sem);

	/* Block until there is something to read */
	while (b->rp == b->wp && (b->wfd != INVALID_FD || b->written == 0)) {

		/* Release buffer lock */
		up(&b->sem);

		/* Block on read */
		DEBUG("read(bid:%d): blocking", b->id);
		if (wait_event_interruptible
		    (b->rq, b->rp != b->wp
		     || (b->wfd == INVALID_FD && b->written != 0)))
			return -ERESTARTSYS;

		/* Not expecting more input? -> EOF */
		if (b->rp == b->wp && b->wfd == INVALID_FD) {
			DEBUG("read(bid:%d): EOF", b->id);
			return 0;
		}
		/* Get lock back */
		down_interruptible(&b->sem);

	}

	/* Calculate length to be copied */
	tocopy = MIN(maxlen, buffer_size(b));

	while (tocopy > 0) {

		/* Determine range to be copied */
		start = b->buf + b->rp;
		end = b->buf + MIN(b->rp + tocopy, BUF_SIZE);

		/* Perform decryption if necessary */
		if (cmode && cmode->dir == CRYPTO_READ
		    && cmode->mode != CRYPTO_PASSTHROUGH)
			cryptodev_docrypt(cstate, start, start, end - start);

		/* Perform copy */
		if (copy_to_user((void *)dst, start, end - start)) {
			/* Release lock */
			up(&b->sem);

			/* Return error */
			return -ENOMEM;
		}
		/* Update Pointers */
		b->rp = (b->rp + (end - start)) % BUF_SIZE;
		copied += (end - start);
		tocopy -= (end - start);

	}

	DEBUG("read(bid:%d): %d\n", b->id, copied);

	/* Release lock on buffer */
	up(&b->sem);

	/* Return actual length copied */
	return copied;

}

ssize_t buffer_write(struct buffer * b, const char *src, ssize_t maxlen,
		     loff_t * off, struct crypto_smode * cmode,
		     struct cryptodev_state * cstate)
{

	ssize_t length = 0;
	ssize_t copied = 0;
	ssize_t tocopy = 0;
	ssize_t bytesnotwritten = 0;

	/* Get lock on buffer */
	down_interruptible(&b->sem);

	/* Calculate length to be copied */
	length = MIN(maxlen, BUF_SIZE - buffer_size(b));
	tocopy = length;

	while (copied < length) {

		tocopy = b->wp + tocopy < BUF_SIZE ? tocopy : BUF_SIZE - b->wp;

		if ((bytesnotwritten =
		     copy_from_user(b->buf + b->wp, (void *)src, tocopy)) < 0)
			return bytesnotwritten;	/* Error code was returned */

		/* Perform encryption if necessary */
		if (cmode && cmode->dir == CRYPTO_WRITE
		    && cmode->mode != CRYPTO_PASSTHROUGH)
			cryptodev_docrypt(cstate, b->buf + b->wp,
					  b->buf + b->wp,
					  tocopy - bytesnotwritten);

		/* Update copied and write pointer */
		copied += tocopy - bytesnotwritten;
		b->wp = (b->wp + (tocopy - bytesnotwritten)) % BUF_SIZE;
		tocopy = length - copied;

		if (bytesnotwritten)
			break;	/* Can't copy any more */

	}

	/* Update written count */
	b->written += copied;

	DEBUG("write(bid:%d): %d\n", b->id, copied);

	/* Release lock */
	up(&b->sem);

	/* Wake up sleeping reader */
	wake_up_interruptible(&b->rq);

	return copied;

}
