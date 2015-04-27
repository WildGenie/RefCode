/******************************************************************************
 *
 * vxi11intr_xdr.c,v (This file is best viewed with a tabwidth of 4)
 *
 ******************************************************************************/

/***********************************************************************
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory, and the Regents of the University of
* California, as Operator of Los Alamos National Laboratory, and
* Berliner Elektronenspeicherring-Gesellschaft m.b.H. (BESSY).
* asynDriver is distributed subject to a Software License Agreement
* found in file LICENSE that is included with this distribution.
***********************************************************************/

/*
 *		Types conversions for RPC protocol (intr channel)
 *
 * Current Author: Benjamin Franksen
 *
 ******************************************************************************
 *
 * Notes:
 *
 *	This file was generated by RPCGEN from vxi/RPCL/vxi11intr.rpcl.
 *
 */
#include "osiRpc.h"
#include "vxi11intr.h"

bool_t
xdr_Device_SrqParms( XDR *xdrs, void *arg, ...)
{
    Device_SrqParms *objp = (Device_SrqParms *)arg;
	if (!xdr_bytes(xdrs, (char **)&objp->handle.handle_val, (u_int *)&objp->handle.handle_len, ~0)) {
		return (FALSE);
	}
	return (TRUE);
}

/* 
 * vxi11intr_xdr.c,v
 * Revision 1.3  2003/11/14 15:20:32  mrk
 * fix license
 *
 * Revision 1.2  2003/10/27 13:41:12  mrk
 * interim commit before alp[ha2
 *
 * Revision 1.1  2003/10/01 19:30:03  mrk
 * asynGpib and vxi11 now supported
 *
 * Revision 1.2  2003/04/10 15:17:07  mrk
 * open source license
 *
 * Revision 1.1  2003/02/20 20:42:23  mrk
 * drvHpE2050=>drvVxi11
 *
 * Revision 1.3  2002/10/21 16:15:56  mrk
 * fix  so it does not generate warnings
 *
 * Revision 1.2  2002/10/21 15:59:49  mrk
 * build a single library
 *
 * Revision 1.1  2002/10/18 17:21:16  mrk
 * moved to here
 *
 * Revision 1.2  2001/02/22 19:57:20  norume
 * Many, many R3.14 changes.  Driver tested on RTEMS-gen68360 and Linux.
 *
 * Revision 1.1.1.1  2001/02/15 14:52:51  mrk
 * Import sources
 *
 * Revision 1.1.1.1  2000/03/21 18:06:35  franksen
 * unbundled gpib first version
 */
