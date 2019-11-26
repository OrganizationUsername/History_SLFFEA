/* This program reapplies the boundary conditions for the
   displacement when the conjugate gradient method is used.
   This is for a finite element program which does analysis
   on a quad element.

                        Updated 6/30/00 

    SLFFEA source file
    Version:  1.1
    Copyright (C) 1999  San Le

    The source code contained in this file is released under the
    terms of the GNU Library General Public License.
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "qdconst.h"
#include "qdstruct.h"

int qdBoundary ( double *U, BOUND bc )
{
	int i,dum;

        for( i = 0; i < bc.num_fix[0].x; ++i )
        {
            dum = bc.fix[i].x;
            *(U+ndof*dum) = 0.0;
        }
        for( i = 0; i < bc.num_fix[0].y; ++i )
        {
            dum = bc.fix[i].y;
            *(U+ndof*dum+1) = 0.0;
        }
	return 1;
}