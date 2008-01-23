/*
 * strcasecmp.h -- string compare
 * 
 * **********************************************
 * CMU ARPA Speech Project
 *
 * Copyright (c) 1996 Carnegie Mellon University.
 * ALL RIGHTS RESERVED.
 * **********************************************
 * 
 * HISTORY
 *
 * 14-Oct-92 Eric Thayer (eht+@cmu.edu) Carnegie Mellon University
 *	added formal declarations for a and
 * 14-Oct-92 Eric Thayer (eht+@cmu.edu) Carnegie Mellon University
 *	installed ulstrcmp() for strcasecmp() because DEC alpha for some reason
 *	seg faults on call to strcasecmp(). (OSF/1 BL8)
 */


#ifndef _STRCASECMP_H_
#define _STRCASECMP_H_

extern int mystrcasecmp (char const *a, char const *b);

#endif  /* _STRCASECMP_H_ */
