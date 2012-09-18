#ifndef FD_H
#define FD_H
#define INVALID_FD (-1)

#include "miscellaneous.h"
#include "buffer.h"
#include <linux/types.h>
#include "cryptodev-1.0/cryptoapi.h"

struct fd {
	int fd;
	int mode;
	struct crypto_smode *cmode;
	struct cryptodev_state *cstate;
};

struct fd *init_fd(void);
void destroy_fd(struct fd *fd);

#endif
