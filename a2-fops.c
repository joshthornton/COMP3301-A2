#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <asm/io.h>

/* a2-fops.c for COMP3301 A2.
 * Written by Sam Kingston, 2010-2012.
 * Version 2012/1.0
 */

/* License */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joshua King, Joshua Thornton");
MODULE_DESCRIPTION("COMP3301 Assignment Two");

/* Local Includes */
#include "ioctl.h"
#include "buffer.h"
#include "fd.h"
#include "a2-fops.h"
#include "miscellaneous.h"
#include "cryptodev-1.0/cryptoapi.h"

/* Constants */
#define MAJORNUM 250
#define BASEMINOR 0
#define COUNT 1
#define NAME "crypto"

/* Global Variables */
dev_t num;

int __init init_module(void)
{
	struct cdev *cdev;

	/* Help separate kernel logs */
	DEBUG("----------------------------------------");

	/* Kernel log */
	DEBUG("Crypto Driver Loaded\n");

	/* Request MAJOR number 250 */
	num = MKDEV(MAJORNUM, BASEMINOR);

	/* Get allocated major region and minor number */
	if (register_chrdev_region(num, COUNT, NAME) < 0) {
		printk(KERN_INFO "could not allocate region\n");
		return -1;

	}
	/* Kernel Log major number */
	printk(KERN_INFO "%s: major=%d minor=%d", NAME, MAJOR(num), MINOR(num));

	/* Attempt Allocation */
	if (!(cdev = cdev_alloc())) {
		printk(KERN_INFO "could not allocate character device\n");
		return -1;
	}
	/* Set my read, write etc. calls */
	cdev->ops = &fops;

	/* Initialise buffers */
	buffer_init();

	/* Don't call "add" until driver is ready, when it returns the driver is "live" */
	if (cdev_add(cdev, num, COUNT) < 0) {
		printk(KERN_INFO "could not add character device\n");
		return -1;
	}

	return 0;

}

void __exit cleanup_module(void)
{
	unregister_chrdev_region(num, COUNT);
	DEBUG("Crypto Driver Unloaded\n");

	/* Help separate kernel logs */
	DEBUG("----------------------------------------");
}

static int device_open(struct inode *inode, struct file *filp)
{

	/* Declarations */
	struct fd *fds;

	try_module_get(THIS_MODULE);	/* increase the refcount of the open module */

	/* Create a fd struct and add it to private data */
	fds = init_fd();
	filp->private_data = fds;

	return 0;
}

static int device_release(struct inode *inode, struct file *filp)
{
	module_put(THIS_MODULE);	/* decrease the refcount of the open module */
	destroy_fd(filp->private_data);
	return 0;
}

static ssize_t device_read(struct file *filp, char *buf, size_t len,
			   loff_t * off)
{
	struct buffer *b;
	struct fd *fd = (struct fd *)filp->private_data;

	/* Get the buffer with this as read fd */
	b = get_buffer_fd(fd->fd);

	if (b)
		return buffer_read(b, buf, len, off, fd->cmode, fd->cstate);

	return -EOPNOTSUPP;
}

static ssize_t device_write(struct file *filp, const char *buf, size_t len,
			    loff_t * off)
{
	struct buffer *b;
	struct fd *fd = (struct fd *)filp->private_data;

	/* Get the buffer with this as write fd */
	b = get_buffer_fd(fd->fd);

	if (b)
		return buffer_write(b, buf, len, off, fd->cmode, fd->cstate);

	return -EOPNOTSUPP;
}

static int device_ioctl(struct inode *inode, struct file *filp,
			unsigned int cmd, unsigned long arg)
{

	struct fd *fd = (struct fd *)filp->private_data;
	struct buffer *b;
	int retval;

	switch (cmd) {
	case CRYPTO_IOCCREATE:
		{
			b = init_buffer();
			if (b) {
				if (!(retval = device_ioctl(inode, filp,
						  CRYPTO_IOCTATTACH, b->id)))
					return b->id;
				else {
					destroy_buffer(b);
					return retval;
				}
			}
			return -ENOMEM;
		}
	case CRYPTO_IOCTDELETE:
		{
			b = get_buffer((unsigned int)arg);
			if (b) {
				destroy_buffer(b);
				return SUCCESS;
			}
			return -EINVAL;
		}
	case CRYPTO_IOCTATTACH:
		{

			/* Check fd is not already attached to something */
			if (get_buffer_fd(fd->fd))
				return -EOPNOTSUPP;

			b = get_buffer((unsigned int)arg);

			if (!b)
				return -EINVAL;

			/* Check read permission */
			if (filp->f_mode & FMODE_READ) {
				if (b->rfd == INVALID_FD)
					b->rfd = fd->fd;
				else
					return -EALREADY;
			}
			/* Check write permission */
			if (filp->f_mode & FMODE_WRITE) {
				if (b->wfd == INVALID_FD)
					b->wfd = fd->fd;
				else
					return -EALREADY;
			}
			return SUCCESS;
		}
	case CRYPTO_IOCDETACH:
		{
			/* This will destroy buffers automatically if necessary */
			disassociate_fd(fd->fd);
			return SUCCESS;
		}
	case CRYPTO_IOCSMODE:
		{
			/* Remember to free memory */
			if (fd->cmode)
				kfree(fd->cmode);

			/* Alloc memory and copy struct */
			fd->cmode = MALLOC(sizeof(struct crypto_smode));
			memcpy(fd->cmode, (struct crpyto_smode *)arg,
			       sizeof(struct crypto_smode));

			/* Initialise cstate struct */
			if (!fd->cstate)
				fd->cstate =
				    MALLOC(sizeof(struct cryptodev_state));
			cryptodev_init(fd->cstate, fd->cmode->key,
				       strlen(fd->cmode->key));

			return SUCCESS;
		}
	default:
		{
			return -EINVAL;
		}
	}
}

static int device_mmap(struct file *filp, struct vm_area_struct *vma)
{
	struct fd *fd = (struct fd *)filp->private_data;
	struct buffer *b = get_buffer_fd(fd->fd);
	int pfn, size, err;
	if (!b)
		return -EOPNOTSUPP;
	pfn = virt_to_phys((void *)b->buf) >> PAGE_SHIFT;
	size = vma->vm_end - vma->vm_start;
	err = 0;
	if (size != BUF_SIZE / 2 && size != BUF_SIZE)
		return -EIO;
	if (size == BUF_SIZE / 2 && vma->vm_pgoff == 1)
		pfn =
		    virt_to_phys((void *)&(b->buf[BUF_SIZE / 2])) >> PAGE_SHIFT;
	else if (size == BUF_SIZE / 2 && vma->vm_pgoff > 1)
		return -EIO;

	err = remap_pfn_range(vma, vma->vm_start, pfn, size, vma->vm_page_prot);
	if (!err)
		return 0;
	return -ENOSYS;
}

struct file_operations fops = {
	.open = device_open,
	.release = device_release,
	.read = device_read,
	.write = device_write,
	.ioctl = device_ioctl,
	.mmap = device_mmap
};
