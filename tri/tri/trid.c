/* 
    This utility function assembles the id array for a finite 
    element program which does analysis on a 3 node triangle element.

		Updated 6/20/02

    SLFFEA source file
    Version:  1.3
    Copyright (C) 1999, 2000, 2001, 2002  San Le 

    The source code contained in this file is released under the
    terms of the GNU Library General Public License.
 
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "trconst.h"
#include "trstruct.h"

extern int dof, neqn;

int trformid( BOUND bc, int *id)
{
/* Assembly of the id array(the matrix which determines
   the degree of feedom by setting fixed nodes = -1) */

	int i, counter;

        counter=0;
        for( i = 0; i < bc.num_fix[0].x; ++i )
	{
           *(id + ndof*bc.fix[i].x) = -1;
	} 
        for( i = 0; i < bc.num_fix[0].y; ++i )
	{
           *(id + ndof*bc.fix[i].y + 1) = -1;
	} 
        for( i = 0; i < dof; ++i )
	{
           if( *(id + i) != -1  )
           {
                *(id + i) = counter;
                ++counter;
           }
	}
        neqn=counter;
	return 1;
}

