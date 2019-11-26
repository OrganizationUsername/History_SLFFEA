/*
     SLFFEA source file
     Version:  1.1
     Copyright (C) 1999, 2000  San Le

     The source code contained in this file is released under the
     terms of the GNU Library General Public License.
*/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "plconst.h"
#include "plstruct.h"

int dotX(double *, double *, double *, int );

int plshl( double g, SH shl, double *w)
{
/* 
     This subroutine calculates the local shape function derivatives for
     a plate element at the gauss points.

     It is based on the subroutine QDCSHL from the book
     "The Finite Element Method" by Thomas Hughes, page 784.

			Updated 5/5/99
*/
        double ra[]={-0.50, 0.50, 0.50,-0.50, 0.00};
        double sa[]={-0.50,-0.50, 0.50, 0.50, 0.00};
	double temps,tempr,r,s;
	int i,j,k;

        for( k = 0; k < num_int; ++k )
        {
/* 
   calculating the weights and local dN/ds,dr matrix for 2X2 
   integration
*/

		*(w+k)=1.0;

        	r=g*(*(ra+k));
        	s=g*(*(sa+k));
        	for( i = 0; i < npel; ++i )
        	{
		   tempr = pt5 + *(ra+i)*r;
		   temps = pt5 + *(sa+i)*s;
        	   shl.bend[npel*(nsd+1)*k+i]=*(ra+i)*temps;
        	   shl.bend[npel*(nsd+1)*k+npel*1+i]=tempr*(*(sa+i));
        	   shl.bend[npel*(nsd+1)*k+npel*2+i]=tempr*temps;
		}
               	/*printf("\n");*/
	}
/* 
   calculating the weights and local dN/ds,dr matrix for 1 
   point integration
*/
        r=g*(*(ra+num_int));
        s=g*(*(sa+num_int));
	*(w+num_int)=1.0; /* Actually, 4.0 but I'm adding 4 times in plKassemble */
        for( i = 0; i < npel; ++i )
        {
	   tempr = pt5 + *(ra+i)*r;
	   temps = pt5 + *(sa+i)*s;
           shl.shear[i]=*(ra+i)*temps;
           shl.shear[npel*1+i]=tempr*(*(sa+i));
           shl.shear[npel*2+i]=tempr*temps;
	}
        return 1;
}

int plshg( double *det, int el, SH shl, SH shg, double *xl, double *Area)
{
/*
     This subroutine calculates the global shape function derivatives for
     a plate element at the gauss points.

     It is based on the subroutine QDCSHG from the book
     "The Finite Element Method" by Thomas Hughes, page 783.

 ....  CALCULATE GLOBAL DERIVATIVES OF SHAPE FUNCTIONS AND
       JACOBIAN DETERMINANTS FOR A FOUR-NODE QUADRALATERAL ELEMENT

       *(xl+j+npel*i) = GLOBAL COORDINATES

       FOR 2X2 PT. GAUSS
       *(det+k)  = JACOBIAN DETERMINANT
       shl.bend[npel*(nsd+1)*k+i] = LOCAL ("XI") DERIVATIVE OF SHAPE FUNCTION
       shl.bend[npel*(nsd+1)*k+npel*1+i] = LOCAL ("ETA") DERIVATIVE OF SHAPE FUNCTION
       shl.bend[npel*(nsd+1)*k+npel*2+i] = LOCAL SHAPE FUNCTION
       shg.bend[npel*(nsd+1)*k+i] = X-DERIVATIVE OF SHAPE FUNCTION
       shg.bend[npel*(nsd+1)*k+npel*1+i] = Y-DERIVATIVE OF SHAPE FUNCTION
       shg.bend[npel*(nsd+1)*k+npel*2+i] = shl.bend[npel*(nsd+1)*k+npel*2+i]

       FOR 1X1 PT. GAUSS
       shl.shear[i] = LOCAL ("XI") DERIVATIVE OF SHAPE FUNCTION
       shl.shear[npel*1+i] = LOCAL ("ETA") DERIVATIVE OF SHAPE FUNCTION 
       shl.shear[npel*2+i] = LOCAL SHAPE FUNCTION 
       shg.shear[i] = X-DERIVATIVE OF SHAPE FUNCTION 
       shg.shear[npel*1+i] = Y-DERIVATIVE OF SHAPE FUNCTION 
       shg.shear[npel*2+i] = shl.shear[npel*2+i]
       *(xs+2*j+i) = JACOBIAN MATRIX
          i    = LOCAL NODE NUMBER OR GLOBAL COORDINATE NUMBER
          j    = GLOBAL COORDINATE NUMBER
          k    = INTEGRATION-POINT NUMBER
       num_int    = NUMBER OF INTEGRATION POINTS, EQ. 1 
		
			Updated 2/1/00
*/
        double xs[4],temp;
	int check,i,j,k;

	memcpy(shg.shear,shl.shear,soshs*sizeof(double));
	memcpy(shg.bend,shl.bend,soshb*sizeof(double));

/* initialize the Area */

	*(Area)=0.0;

/* 
   calculating the dN/dx,dy matrix for 2X2 
   integration
*/
        for( k = 0; k < num_int; ++k )
	{

/* The jacobian, dx/dc, is calculated below */

           for( j = 0; j < nsd; ++j )
	   {
        	for( i = 0; i < nsd; ++i )
        	{
	     	   check=dotX((xs+nsd*i+j),(shg.bend+npel*(nsd+1)*k+npel*j),
				(xl+npel*i),npel);
		}
	   }
           *(det+k)=*(xs)*(*(xs+3))-*(xs+2)*(*(xs+1));
	   /*printf(" %9.5f %9.5f\n %9.5f %9.5f\n",
        	*(xs),*(xs+1),*(xs+nsd*1),*(xs+nsd*1+1));
           printf("%d %f\n", k, *(det+k)); */

/* Calculate the Area from determinant of the Jacobian */

	   *Area+=*(det+k);

           if(*(det+k) <= 0 ) 
	   {
                printf("the element (%d) is inverted; det:%f; integ pt.:%d\n",
                        el,*(det+k),k);
		return 0;
	   }

/* The inverse of the jacobian, dc/dx, is calculated below */

           temp=*(xs);
           *(xs)=*(xs+3)/(*(det+k));
           *(xs+1)*=-1./(*(det+k));
           *(xs+2)*=-1./(*(det+k));
           *(xs+3)=temp/(*(det+k));

           for( i = 0; i < npel; ++i )
	   {
                shg.bend[npel*(nsd+1)*k+i]=*(xs)*shl.bend[npel*(nsd+1)*k+i]+
                        *(xs+2)*shl.bend[npel*(nsd+1)*k+npel*1+i];
                shg.bend[npel*(nsd+1)*k+npel*1+i]=*(xs+1)*shl.bend[npel*(nsd+1)*k+i]+
                        *(xs+3)*shl.bend[npel*(nsd+1)*k+npel*1+i];
               	/*printf("%d %d %f %f %f\n", k, i, shg.bend[npel*(nsd+1)*k+i],
			shg.bend[npel*(nsd+1)*k+npel*1+i], 
			shg.bend[npel*(nsd+1)*k+npel*2+i]);*/
	   }
           /*printf("\n");*/
	}
/* 
   calculating the dN/dx,dy matrix for 1 
   point integration
*/
        for( j = 0; j < nsd; ++j )
	{
        	for( i = 0; i < nsd; ++i )
        	{
	     	   check=dotX((xs+nsd*i+j),(shg.shear+npel*j),
			(xl+npel*i),npel);
		}
	}
        *(det+num_int)=*(xs)*(*(xs+3))-*(xs+2)*(*(xs+1));

	/*printf(" %9.5f %9.5f\n %9.5f %9.5f\n",
        	*(xs),*(xs+1),*(xs+nsd*1),*(xs+nsd*1+1));
        printf("%d %f\n", num_int, *(det+num_int)); */

/* Calculate the Area from 1X1 point Gauss using the determinant of the Jacobian */

	/*   *Area+=*(det+num_int); */

        if(*(det+num_int) <= 0 ) 
	{
                printf("the element (%d) is inverted; det:%f; 1X1 integ pt.%d\n",
                        el,*(det+k));
		return 0;
	}

/* The inverse of the jacobian, dc/dx, is calculated below */

        temp=*(xs);
        *(xs)=*(xs+3)/(*(det+num_int));
        *(xs+1)*=-1./(*(det+num_int));
        *(xs+2)*=-1./(*(det+num_int));
        *(xs+3)=temp/(*(det+num_int));

        for( i = 0; i < npel; ++i )
	{
           	shg.shear[i]=*(xs)*shl.shear[i] + *(xs+2)*shl.shear[npel*1+i];
           	shg.shear[npel*1+i]=*(xs+1)*shl.shear[i] + *(xs+3)*shl.shear[npel*1+i];
               	/*printf("%d %f %f %f\n", i, shg.shear[i], shg.shear[npel*1+i], 
			shg.shear[npel*2+i]);*/
	}
        return 1; 
}

int plshg_mass( double *det, int el, SH shg, double *xl, double *Area)
{
/*
     This subroutine calculates the determinant for
     a plate element at the gauss points for the calculation of
     the global mass matrix.  Unlike plshg, we do not have to
     calculate the shape function derivatives with respect to
     the global coordinates x, y, z which are only needed for
     strains and stresses.

     It is based on the subroutine QDCSHG from the book
     "The Finite Element Method" by Thomas Hughes, page 783.
     This subroutine calculates the global shape function derivatives for
     a plate element at the gauss points.

 ....  CALCULATE GLOBAL DERIVATIVES OF SHAPE FUNCTIONS AND
       JACOBIAN DETERMINANTS FOR A FOUR-NODE QUADRALATERAL ELEMENT

       *(xl+j+npel*i) = GLOBAL COORDINATES
       FOR 2X2 PT. GAUSS
       *(det+k)  = JACOBIAN DETERMINANT
       shg.bend[npel*(nsd+1)*k+i] = LOCAL ("XI") DERIVATIVE OF SHAPE FUNCTION
       shg.bend[npel*(nsd+1)*k+npel*1+i] = LOCAL ("ETA") DERIVATIVE OF SHAPE FUNCTION
       shg.bend[npel*(nsd+1)*k+npel*2+i] = shl.bend[npel*(nsd+1)*k+npel*2+i] 
       *(xs+2*j+i) = JACOBIAN MATRIX
          i    = LOCAL NODE NUMBER OR GLOBAL COORDINATE NUMBER
          j    = GLOBAL COORDINATE NUMBER
          k    = INTEGRATION-POINT NUMBER
       num_int    = NUMBER OF INTEGRATION POINTS, EQ. 1 

			Updated 7/27/00
*/

        double xs[4],temp;
	int check,i,j,k;

/* initialize the Area */

	*(Area)=0.0;

/* 
   calculating the dN/dx,dy matrix for 2X2 
   integration
*/
        for( k = 0; k < num_int; ++k )
	{

/* The jacobian, dx/dc, is calculated below */

           for( j = 0; j < nsd; ++j )
	   {
        	for( i = 0; i < nsd; ++i )
        	{
	     	   check=dotX((xs+nsd*i+j),(shg.bend+npel*(nsd+1)*k+npel*j),
				(xl+npel*i),npel);
		}
	   }
           *(det+k)=*(xs)*(*(xs+3))-*(xs+2)*(*(xs+1));
	   /*printf(" %9.5f %9.5f\n %9.5f %9.5f\n",
        	*(xs),*(xs+1),*(xs+nsd*1),*(xs+nsd*1+1));
           printf("%d %f\n", k, *(det+k)); */

/* Calculate the Area from determinant of the Jacobian */

	   *Area+=*(det+k);

           if(*(det+k) <= 0 ) 
	   {
                printf("the element (%d) is inverted; det:%f; integ pt.:%d\n",
                        el,*(det+k),k);
		return 0;
	   }

	}
        return 1; 
}
