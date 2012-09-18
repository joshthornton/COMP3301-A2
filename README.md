COMP3301-A2
===========

COMP3301 Assignment Two

Overview
--------
- The assignment is to create a Linux device driver interface to an encryption coprocessor. You will be provided with a pseudo-hardware device in the form of a kernel module that you will use to perform the cryptography computations. The encryption scheme that the device implements is RC4.
- The device node is located at /dev/crypto.
- After opening the device, processes use ioctl() calls to create or attach to one or more encryption buﬀers, and to initialise the encryption coprocessor.
- Once a file descriptor is attached to a buﬀer, read() and write() operations are used to send and receive plaintext/ciphertext to and from the device. You will perform crypto operations on the encryption coprocessor via its API.
- The mmap() operation is supported so that processes may inspect a buﬀer's contents directly.
- After completing the cryptography operations, the process detaches from and/or destroys the buﬀers using ioctl() calls, before ĕnally closing the ĕle descriptor.
