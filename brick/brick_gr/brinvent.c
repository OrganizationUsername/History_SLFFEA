/*
    This program converts a FEM brick mesh data file 
    into an Open Inventor readable data file.

        Updated 10/2/00

    SLFFEA source file
    Version:  1.3
    Copyright (C) 1999, 2000, 2001, 2002  San Le 

    The source code contained in this file is released under the
    terms of the GNU Library General Public License.
 
*/


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "../brick/brconst.h"
#include "../brick/brstruct.h"

int normcrossX(double *, double *, double *);

int brreader( BOUND , int *, double *, int *, double *, MATL *, char *,
	FILE *, STRESS *, SDIM *, double *);

int numnp, numel, nmat, nmode, dof, neqn, flag;
int element_stress_read_flag, stress_read_flag;
int sof;
double Emod, G, K, nu;

int crossX(double *coord, int tri[3],FILE *o3)
{
	int check;
	double coord_el[nsd*nsd],d1[nsd],d2[nsd],norm[nsd];

        *(coord_el)=*(coord+nsd*(*(tri)));
        *(coord_el+1)=*(coord+nsd*(*(tri))+1);
        *(coord_el+2)=*(coord+nsd*(*(tri))+2);
        *(coord_el+nsd*1)=*(coord+nsd*(*(tri+1)));
        *(coord_el+nsd*1+1)=*(coord+nsd*(*(tri+1))+1);
        *(coord_el+nsd*1+2)=*(coord+nsd*(*(tri+1))+2);
        *(coord_el+nsd*2)=*(coord+nsd*(*(tri+2)));
        *(coord_el+nsd*2+1)=*(coord+nsd*(*(tri+2))+1);
        *(coord_el+nsd*2+2)=*(coord+nsd*(*(tri+2))+2);

        *(d1)=*(coord_el)-*(coord_el+nsd*1);
        *(d1+1)=*(coord_el+1)-*(coord_el+nsd*1+1);
        *(d1+2)=*(coord_el+2)-*(coord_el+nsd*1+2);
        *(d2)=*(coord_el)-*(coord_el+nsd*2);
        *(d2+1)=*(coord_el+1)-*(coord_el+nsd*2+1);
        *(d2+2)=*(coord_el+2)-*(coord_el+nsd*2+2);

       	check = normcrossX(d1, d2, norm);
       	fprintf( o3,"%9.5f %9.5f %9.5f \n",*(norm),*(norm+1),*(norm+2));
	return 1;
}

int writer (int *connect, double *coord)
{
	FILE *o2,*o3,*o4;
	int i,i1,tri[3],j,check;
	double fpointx, fpointy, fpointz,nvec[nsd];
	double coord_el[24];

        o2 = fopen( "brick.iv","w" );
        o3 = fopen( "norms","w" );

        fprintf(o2,"#Inventor V2.0 ascii\n\n");
        fprintf(o2,"Separator {\n    NormalBinding {\n    }\n  ");
        fprintf(o2,"    RotationXYZ {\n        axis    X\n        angle   0\n    }\n");
        fprintf(o2,"    Separator {\n        Label {\n");
	fprintf(o2,"            label       \"lpremol1\"\n        }\n");
	fprintf(o2,"        Material {\n");
	fprintf(o2,"            ambientColor        0.25 0.25 0.25\n");
	fprintf(o2,"            diffuseColor        0.666667 1 1\n        }\n");
	fprintf(o2,"        Separator {\n            Coordinate3 {\n                point   [");
        for( i = 0; i < numnp - 1; ++i )
        {
                fpointx = *(coord+nsd*i);
                fpointy = *(coord+nsd*i+1);
                fpointz = *(coord+nsd*i+2);
                fprintf(o2,"\n		   %14.8f %14.8f %14.8f,",fpointx,fpointy,fpointz);
        }
        fpointx=*(coord+nsd*(numnp-1));
        fpointy=*(coord+nsd*(numnp-1)+1);
        fpointz=*(coord+nsd*(numnp-1)+2);
        fprintf(o2,"\n   %14.8f %14.8f %14.8f ]\n            }\n",fpointx,fpointy,fpointz);
	fprintf(o2,"            ShapeHints {\n");
	fprintf(o2,"                vertexOrdering  COUNTERCLOCKWISE\n");
	fprintf(o2,"                faceType        UNKNOWN_FACE_TYPE\n}\n");
	fprintf(o2,"            IndexedFaceSet {\n                coordIndex      [\n");

        for( j = 0; j < numel; ++j )
        {

		*(tri)=*(connect+npel*j);
                *(tri+1)=*(connect+npel*j+2);
                *(tri+2)=*(connect+npel*j+1);
		check = crossX(coord, tri, o3);

		if( j == 0 )
		{
    		   fprintf(o2, "		%d, %d, %d, ",
			*(tri),*(tri+1),*(tri+2));
		}
		else
		{
    		   fprintf(o2, "		%d, %d, %d, %d, ",-1,
			*(tri),*(tri+1),*(tri+2));
		}

		*(tri)=*(connect+npel*j);
                *(tri+1)=*(connect+npel*j+3);
                *(tri+2)=*(connect+npel*j+2);
		check = crossX(coord, tri, o3);
    		fprintf(o2, "%d, %d, %d, %d, \n",-1,
			*(tri),*(tri+1),*(tri+2));

		*(tri)=*(connect+npel*j+4);
                *(tri+1)=*(connect+npel*j+5);
                *(tri+2)=*(connect+npel*j+6);
		check = crossX(coord, tri, o3);
    		fprintf(o2, "		%d, %d, %d, %d, ",-1,
			*(tri),*(tri+1),*(tri+2));

		*(tri)=*(connect+npel*j+4);
                *(tri+1)=*(connect+npel*j+6);
                *(tri+2)=*(connect+npel*j+7);
		check = crossX(coord, tri, o3);
    		fprintf(o2, "%d, %d, %d, %d, \n",-1,
			*(tri),*(tri+1),*(tri+2));

        	for( i1 = 0; i1 < 3; ++i1 )
        	{
			*(tri)=*(connect+npel*j+i1);
                        *(tri+1)=*(connect+npel*j+i1+5);
                        *(tri+2)=*(connect+npel*j+i1+4);
			check = crossX(coord, tri, o3);
    			fprintf(o2, "		%d, %d, %d, %d, ",-1,
				*(tri),*(tri+1),*(tri+2));
			*(tri)=*(connect+npel*j+i1);
                        *(tri+1)=*(connect+npel*j+i1+1);
                        *(tri+2)=*(connect+npel*j+i1+5);
			check = crossX(coord, tri, o3);
    			fprintf(o2, "%d, %d, %d, %d, \n ",-1,
				*(tri),*(tri+1),*(tri+2));
		}
		*(tri)=*(connect+npel*j+3);
                *(tri+1)=*(connect+npel*j+4);
                *(tri+2)=*(connect+npel*j+7);
		check = crossX(coord, tri, o3);
    		fprintf(o2, "		%d, %d, %d, %d, ",-1,
			*(tri),*(tri+1),*(tri+2));
		*(tri)=*(connect+npel*j+3);
                *(tri+1)=*(connect+npel*j);
                *(tri+2)=*(connect+npel*j+4);
		check = crossX(coord, tri, o3);

		if( j == numel -1 )
		{
    		   fprintf(o2, "%d, %d, %d, %d ] \n            }\n",-1,
			*(tri),*(tri+1),*(tri+2));
		}
		else
		{
    		   fprintf(o2, "%d, %d, %d, %d,  \n",-1,
			*(tri),*(tri+1),*(tri+2));
		}
	}
	fclose(o3);
        o4 = fopen( "norms","r" );
	fprintf(o2,"            Normal {\n                vector  [");
        for( j = 0; j < numel; ++j )
        {
        	for( i1 = 0; i1 < 12; ++i1 )
        	{
    			   fscanf(o4, "%lf %lf %lf  \n",(nvec),(nvec+1),(nvec+2));
			   if( j == numel -1 && i1 == 11 )
			   {
    			   	fprintf(o2, "                %9.5f %9.5f %9.5f  \n",
					*(nvec),*(nvec+1),*(nvec+2));
			   }
			   else
			   {
    			   	fprintf(o2, "                %9.5f %9.5f %9.5f,  \n",
					*(nvec),*(nvec+1),*(nvec+2));
			   }
		}
	}
	fprintf(o2,"            ]\n            }\n");
	fprintf(o2,"            NormalBinding {\n");
	fprintf(o2,"                value   PER_FACE\n            }\n");
	fprintf(o2,"        }\n    }\n}\n");

	return 1;
}

int main(int argc, char** argv)
{
	int i, j;
        int *id, *lm, *idiag, check;
	XYZI *mem_XYZI;
	int *mem_int, sofmi, sofmf, sofmXYZI, sofmSDIM, ptr_inc;
	MATL *matl;
	double *mem_double;
        double fpointx, fpointy, fpointz;
	int *connect, *el_matl, dum;
	double *coord, *force, *U;
	char name[30], buf[ BUFSIZ ];
        FILE *o1;
	BOUND bc;
	STRESS *stress;
	STRAIN *strain;
	SDIM *stress_node, *mem_SDIM;
	double g;
	long timec;
	long timef;

        sof = sizeof(double);

	memset(name,0,30*sizeof(char));
	
    	printf("What is the name of the file containing the \n");
    	printf("brick structural data? \n");
    	scanf( "%30s",name);

/*   o1 contains all the structural data */

        o1 = fopen( name,"r" );

	if(o1 == NULL ) {
		printf("error on open\n");
		exit(1);
	}
        fgets( buf, BUFSIZ, o1 );
        fscanf( o1, "%d %d %d %d\n ",&numel,&numnp,&nmat,&nmode);
        dof=numnp*ndof;

/*   Begin allocation of meomory */

/* For the doubles */
	sofmf=numnp*nsd+2*dof;
	mem_double=(double *)calloc(sofmf,sizeof(double));

        if(!mem_double )
        {
                printf( "failed to allocate memory for doubles\n ");
		exit(1);
        }
					ptr_inc=0; 
	coord=(mem_double+ptr_inc); 	ptr_inc += numnp*nsd;
	force=(mem_double+ptr_inc); 	ptr_inc += dof;
	U=(mem_double+ptr_inc);    	ptr_inc += dof;

/* For the materials */
        matl=(MATL *)calloc(nmat,sizeof(MATL));
        if(!matl )
        {
                printf( "failed to allocate memory for matl doubles\n ");
                exit(1);
        }

/* For the STRESS doubles */

	stress=(STRESS *)calloc(1,sizeof(STRESS));
        if(!stress )
        {
                printf( "failed to allocate memory for stress doubles\n ");
		exit(1);
        }

/* For the STRAIN doubles */

	strain=(STRAIN *)calloc(1,sizeof(STRAIN));
        if(!strain )
        {
                printf( "failed to allocate memory for strain doubles\n ");
		exit(1);
        }

/* For the integers */
	sofmi= numel*npel+2*dof+numel*npel*ndof+numel+numnp+1;
	mem_int=(int *)calloc(sofmi,sizeof(int));
        if(!mem_int )
        {
                printf( "failed to allocate memory for integers\n ");
		exit(1);
        }
						ptr_inc = 0; 
	connect=(mem_int+ptr_inc);	 	ptr_inc += numel*npel; 
        id=(mem_int+ptr_inc);                   ptr_inc += dof;
        idiag=(mem_int+ptr_inc);                ptr_inc += dof;
        lm=(mem_int+ptr_inc);                   ptr_inc += numel*npel*ndof;
        el_matl=(mem_int+ptr_inc);              ptr_inc += numel;
	bc.force =(mem_int+ptr_inc);            ptr_inc += numnp;
	bc.num_force=(mem_int+ptr_inc);         ptr_inc += 1;

/* For the XYZI integers */
	sofmXYZI=numnp+1;
	mem_XYZI=(XYZI *)calloc(sofmXYZI,sizeof(XYZI));
        if(!mem_XYZI )
        {
                printf( "failed to allocate memory for XYZI integers\n ");
		exit(1);
        }
					   ptr_inc = 0; 
	bc.fix =(mem_XYZI+ptr_inc);        ptr_inc += numnp;
	bc.num_fix=(mem_XYZI+ptr_inc);     ptr_inc += 1;

/* For the SDIM doubles */
	sofmSDIM=1;
	mem_SDIM=(SDIM *)calloc(sofmSDIM,sizeof(SDIM));
        if(!mem_SDIM )
        {
                printf( "failed to allocate memory for SDIM integers\n ");
		exit(1);
        }
                                                ptr_inc = 0;
        stress_node=(mem_SDIM+ptr_inc);         ptr_inc += 1;

	timec = clock();
	timef = 0;

	stress_read_flag = 0;
	element_stress_read_flag = 0;
	check = brreader( bc, connect, coord, el_matl, force, matl, name, o1,
		stress, stress_node, U);
        if(!check) printf( " Problems with reader \n");

        printf(" \n\n");

        check = writer( connect, coord );
        if(!check) printf( " Problems with writer \n");

    	timec = clock();
        printf("\n elapsed CPU = %lf\n\n",( (double)timec)/800.);

	free(mem_double);
	free(strain);
	free(stress);
	free(mem_int);
	free(mem_XYZI);
	free(mem_SDIM);
}



