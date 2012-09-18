#ifndef MISC_H
#define MISC_H

/* Debugging mode */
/* #define DEBUG_ON */

#ifdef DEBUG_ON
	#define DEBUG(...) printk( KERN_INFO __VA_ARGS__ )
#else
	#define DEBUG(...) ;
#endif

/* MIN macro */
#define MIN(a,b) (a<b? a:b)

/* Boolean Types */
#define TRUE 1
#define FALSE 0

/* NULL Definition */
#ifndef NULL
#define NULL 0
#endif

/* Success Definition */
#define SUCCESS 0

/* Malloc Macro */
#define MALLOC(X) kmalloc(X, GFP_KERNEL)

#endif
