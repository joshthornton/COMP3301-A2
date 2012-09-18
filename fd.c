#include "fd.h"
#include <linux/kernel.h>
#include <linux/module.h>
#include "buffer.h"
#include "miscellaneous.h"

/* Global varaibles */
static int fid = 0;

struct fd *init_fd(void)
{
	/* Alloc space */
	struct fd *fd = MALLOC(sizeof(struct fd));

	/* Set id */
	fd->fd = ++fid;

	/* Default cmode pointer */
	fd->cmode = NULL;
	fd->cstate = NULL;

	DEBUG("Created fd struct with id %d\n", fid);

	return fd;

}

void destroy_fd(struct fd *fd)
{
	DEBUG("destroying fd struct with id %d\n", fd->fd);

	disassociate_fd(fd->fd);

	/* Check crypto mode */
	if (fd->cmode) {
		kfree(fd->cmode);
		fd->cmode = NULL;
	}
	/* Check crypto state */
	if (fd->cstate) {
		kfree(fd->cstate);
		fd->cstate = NULL;
	}

	kfree(fd);
}
