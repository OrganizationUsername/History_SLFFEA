/*
    This utility function assembles the id array for a finite 
    element program which does analysis on a 4 node plate element.

		Updated 6/20/02

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
#include "plconst.h"
#include "plstruct.h"

extern int dof, neqn;

int plformid( BOUND bc, int *id)
{
/* Assembly of the id array(the matrix which determines
   the degree of feedom by setting fixed nodes = -1) */

	int i, counter;

        counter=0;
        for( i = 0; i < bc.num_fix[0].z; ++i )
	{
           *(id + ndof*bc.fix[i].z) = -1;
	} 
        for( i = 0; i < bc.num_fix[0].phix; ++i )
	{
           *(id + ndof*bc.fix[i].phix + 1) = -1;
	} 
        for( i = 0; i < bc.num_fix[0].phiy; ++i )
	{
           *(id + ndof*bc.fix[i].phiy + 2) = -1;
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
