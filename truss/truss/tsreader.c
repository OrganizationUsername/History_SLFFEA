/*
    This library function reads in data for a finite element 
    program which does analysis on a truss.

	Updated on 9/30/06

    SLFFEA source file
    Version:  1.5
    Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006  San Le

    The source code contained in this file is released under the
    terms of the GNU Library General Public License.
 
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tsconst.h"
#include "tsstruct.h"

extern int dof, nmat, nmode, numel, numnp; 
extern int stress_read_flag;

int tsreader( BOUND bc, int *connect, double *coord, int *el_matl, double *force,
	MATL *matl, FILE *o1, SDIM *stress, double *U)
{
	int i,j,dum,dum2;
	char *ccheck;
	char buf[ BUFSIZ ];
	char text, stress_dat[30];

	printf( "number of elements:%d nodes:%d materials:%d nmode:%d dof:%d\n",
		numel,numnp,nmat,nmode,dof);
	fgets( buf, BUFSIZ, o1 );
	printf( "\n");

	for( i = 0; i < nmat; ++i )
	{
	   fscanf( o1, "%d ",&dum);
	   printf( "material (%3d) Emod, density, Area ",dum);
	   fscanf( o1, " %lf %lf %lf\n", &matl[dum].E, &matl[dum].rho, &matl[dum].area);
	   printf( " %7.3e %7.3e %7.3e\n", matl[dum].E, matl[dum].rho, matl[dum].area);
	}
	fgets( buf, BUFSIZ, o1 );
	printf( "\n");

	for( i = 0; i < numel; ++i )
	{
	   fscanf( o1,"%d ",&dum);
	   printf( "connectivity for element (%3d) ",dum);
	   for( j = 0; j < npel; ++j )
	   {
		fscanf( o1, "%d",(connect+npel*dum+j));
		printf( "%6d ",*(connect+npel*dum+j));
	   }
	   fscanf( o1,"%d\n",(el_matl+dum));
	   printf( " with matl %3d\n",*(el_matl+dum));
	}
	fgets( buf, BUFSIZ, o1 );
	printf( "\n");

	for( i = 0; i < numnp; ++i )
	{
	   fscanf( o1,"%d ",&dum);
	   printf( "coordinate (%6d) ",dum);
	   printf( "coordinates ");
	   for( j = 0; j < nsd; ++j )
	   {
		fscanf( o1, "%lf ",(coord+nsd*dum+j));
		printf( "%14.6e ",*(coord+nsd*dum+j));
	   }
	   fscanf( o1,"\n");
	   printf( "\n");
	}
	fgets( buf, BUFSIZ, o1 );
	printf( "\n");

	dum= 0;
	fscanf( o1,"%d",&bc.fix[dum].x);
	printf( "node (%6d) has an x prescribed displacement of: ",bc.fix[dum].x);
	while( bc.fix[dum].x > -1 )
	{
		fscanf( o1,"%lf\n%d",(U+ndof*bc.fix[dum].x),
			&bc.fix[dum+1].x);
		printf( "%14.6e\n",*(U+ndof*bc.fix[dum].x));
		printf( "node (%6d) has an x prescribed displacement of: ",
			bc.fix[dum+1].x);
		++dum;
	}
	bc.num_fix[0].x=dum;
	if(dum > numnp) printf( "too many prescribed displacements x\n");
	fscanf( o1,"\n");
	fgets( buf, BUFSIZ, o1 );
	printf( "\n\n");

	dum= 0;
	fscanf( o1,"%d",&bc.fix[dum].y);
	printf( "node (%6d) has an y prescribed displacement of: ",bc.fix[dum].y);
	while( bc.fix[dum].y > -1 )
	{
		fscanf( o1,"%lf\n%d",(U+ndof*bc.fix[dum].y+1),
			&bc.fix[dum+1].y);
		printf( "%14.6e\n",*(U+ndof*bc.fix[dum].y+1));
		printf( "node (%6d) has an y prescribed displacement of: ",
			bc.fix[dum+1].y);
		++dum;
	}
	bc.num_fix[0].y=dum;
	if(dum > numnp) printf( "too many prescribed displacements y\n");
	fscanf( o1,"\n");
	fgets( buf, BUFSIZ, o1 );
	printf( "\n\n");

	dum= 0;
	fscanf( o1,"%d",&bc.fix[dum].z);
	printf( "node (%6d) has an z prescribed displacement of: ",bc.fix[dum].z);
	while( bc.fix[dum].z > -1 )
	{
		fscanf( o1,"%lf\n%d",(U+ndof*bc.fix[dum].z+2),
			&bc.fix[dum+1].z);
		printf( "%14.6e\n",*(U+ndof*bc.fix[dum].z+2));
		printf( "node (%6d) has an z prescribed displacement of: ",
			bc.fix[dum+1].z);
		++dum;
	}
	bc.num_fix[0].z=dum;
	if(dum > numnp) printf( "too many prescribed displacements z\n");
	fscanf( o1,"\n");
	fgets( buf, BUFSIZ, o1 );
	printf( "\n\n");

	dum= 0;
	printf("force vector for node: ");
	fscanf( o1,"%d",&bc.force[dum]);
	printf( "(%6d)",bc.force[dum]);
	while( bc.force[dum] > -1 )
	{
	   for( j = 0; j < ndof; ++j )
	   {
		fscanf( o1,"%lf ",(force+ndof*bc.force[dum]+j));
		printf("%14.6e ",*(force+ndof*bc.force[dum]+j));
	   }
	   fscanf( o1,"\n");
	   printf( "\n");
	   printf("force vector for node: ");
	   ++dum;
	   fscanf( o1,"%d",&bc.force[dum]);
	   printf( "(%6d)",bc.force[dum]);
	}
	bc.num_force[0]=dum;
	if(dum > numnp) printf( "too many forces\n");
	fscanf( o1,"\n");
	fgets( buf, BUFSIZ, o1 );
	printf( "\n\n");

	if(stress_read_flag)
	{
	   printf("stress for ele: ");
	   fscanf( o1,"%d",&dum);
	   printf( "(%6d)",dum);
	   while( dum > -1 )
	   {
		fscanf( o1,"%lf ",&stress[dum].xx);
		printf(" %12.5e",stress[dum].xx);
 		fscanf( o1,"\n");
		printf( "\n");
		printf("stress for ele: ");
		fscanf( o1,"%d",&dum);
		printf( "(%6d)",dum);
	   }
	}
	printf( "\n\n");

	return 1;
}
