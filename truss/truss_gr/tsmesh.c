/*
    This program plots the mesh with the various
    forms of viewing including stress and strain
    displacement materials, etc.  It works
    with a truss FEM code.

			San Le

 		Last Update 5/14/00

    SLFFEA source file
    Version:  1.1
    Copyright (C) 1999  San Le 

    The source code contained in this file is released under the
    terms of the GNU Library General Public License.
 
 */

#if WINDOWS
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "../truss/tsconst.h"
#include "../truss/tsstruct.h"
#include "tsstrcgr.h"
#include "../../common_gr/control.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

/* graphics globals */

extern choice2, choice3;

/* FEA globals */

extern double *coord, *coord0;
extern int *connecter;
extern int nmat, numnp, numel;
extern GLfloat MeshColor[boxnumber+5][3];
extern GLfloat wire_color[3], black[3], green[3], yellow[3];
extern GLfloat RenderColor[4];
extern ISTRESS *stress_color;
extern ISTRAIN *strain_color;
extern int *U_color, *el_matl_color;
extern int color_choice, input_flag, post_flag;
extern int input_color_flag;
extern int Perspective_flag, Render_flag, AppliedDisp_flag,
        AppliedForce_flag, Material_flag, Node_flag, Element_flag, Axes_flag;
extern int Before_flag, After_flag, Both_flag, Amplify_flag;
extern int stress_flag, strain_flag, disp_flag;
extern int matl_choice, node_choice, ele_choice;

void tsmeshdraw(void)
{
        int i, i2, j, k, dof_el[neqel], ii, check, counter,
		node0, node1;
	int l,m,n;
	int c0,c1;
	int matl_number, node_number;
	int After_gr_flag = 0, Before_gr_flag = 0;
        double coord_el[npel*3], coord0_el[npel*3], fpointx, fpointy, fpointz;
	GLfloat d1[3], d2[3];

        if(post_flag + After_flag > 1) After_gr_flag = 1;
        else After_flag = 0;
        if(input_flag + Before_flag > 1) Before_gr_flag = 1;
        else Before_flag = 0;

	*(wire_color + 2) = 0.0;

        for( k = 0; k < numel; ++k )
        {

/* Calculate element degrees of freedom */

                node0 = *(connecter+k*npel);
                node1 = *(connecter+k*npel+1);

		*(dof_el) = ndof*node0;
		*(dof_el+1) = ndof*node0+1;
		*(dof_el+2) = ndof*node0+2;

		*(dof_el+3) = ndof*node1;
		*(dof_el+4) = ndof*node1+1;
		*(dof_el+5) = ndof*node1+2;

/* Calculate local deformed coordinates */

		if( post_flag )
		{
		    *(coord_el)=*(coord+nsd*node0);
		    *(coord_el+1)=*(coord+nsd*node0+1);
		    *(coord_el+2)=*(coord+nsd*node0+2);

		    *(coord_el+3)=*(coord+nsd*node1);
		    *(coord_el+4)=*(coord+nsd*node1+1);
		    *(coord_el+5)=*(coord+nsd*node1+2);
		}

/* Calculate local undeformed coordinates */

		if( input_flag )
		{
		    *(coord0_el)=*(coord0+nsd*node0);
		    *(coord0_el+1)=*(coord0+nsd*node0+1);
		    *(coord0_el+2)=*(coord0+nsd*node0+2);

		    *(coord0_el+3)=*(coord0+nsd*node1);
		    *(coord0_el+4)=*(coord0+nsd*node1+1);
		    *(coord0_el+5)=*(coord0+nsd*node1+2);
		}

    		/*printf( "%9.5f %9.5f %9.5f \n",*(coord_el+3*j),
			*(coord_el+3*j+1),*(coord_el+3*j+2));*/


/* Calculate element material number */

		matl_number = *(el_matl_color + k);

        	switch (color_choice) {
               	    case 1:
			c0 = strain_color[k].xx;
			c1 = strain_color[k].xx;
               	    break;
               	    case 10:
			c0 = stress_color[k].xx;
			c1 = stress_color[k].xx;
               	    break;
               	    case 19:
			c0 = *(U_color + *(dof_el + ndof*0));
			c1 = *(U_color + *(dof_el + ndof*1));
               	    break;
               	    case 20:
			c0 = *(U_color + *(dof_el + ndof*0 + 1));
			c1 = *(U_color + *(dof_el + ndof*1 + 1));
               	    break;
               	    case 21:
			c0 = *(U_color + *(dof_el + ndof*0 + 2));
			c1 = *(U_color + *(dof_el + ndof*1 + 2));
               	    break;
               	    case 30:
			c0 = 0;
			c1 = 0;
			if( matl_choice == matl_number )
			{
				c0 = 7;
				c1 = 7;
			}
               	    break;
               	    case 31:
			c0 = 0;
			c1 = 0;
               	    break;
               	    case 32:
			c0 = 0;
			c1 = 0;
			if( ele_choice == k )
			{
				c0 = 7;
				c1 = 7;
			}
               	    break;
        	}

/* Draw the mesh after deformation */

    		if( After_gr_flag )
		{
        	    glBegin(GL_LINES);
		  	glColor3fv(MeshColor[c0]);
                   	glVertex3dv((coord_el));
		   	glColor3fv(MeshColor[c1]);
                   	glVertex3dv((coord_el+3));
        	   glEnd();
		}

		if( input_color_flag )
		{
		     c0 = 8;
		     c1 = 8;
		}

/* Draw the mesh before deformation */

    		if( Before_gr_flag )
		{
        	     	glBegin(GL_LINES);
			   	glColor3fv(MeshColor[c0]);
                	   	glVertex3dv((coord0_el));
			   	glColor3fv(MeshColor[c1]);
                 	   	glVertex3dv((coord0_el+3));
        		glEnd();
		}
	}
/* This draws the Node ID node */
	if (color_choice == 31)
	{
	    glPointSize(8);
	    node_number=node_choice;
    	    if( After_gr_flag )
	    {
	    	fpointx = *(coord+nsd*node_number);
	    	fpointy = *(coord+nsd*node_number+1);
	    	fpointz = *(coord+nsd*node_number+2);
              	glBegin(GL_POINTS);
        	    glColor3fv(yellow);
               	    glVertex3f(fpointx, fpointy, fpointz);
    	   	glEnd();
	    }
    	    if( Before_gr_flag )
	    {
	    	fpointx = *(coord0+nsd*node_number);
	    	fpointy = *(coord0+nsd*node_number+1);
	    	fpointz = *(coord0+nsd*node_number+2);
              	glBegin(GL_POINTS);
        	    glColor3fv(yellow);
               	    glVertex3f(fpointx, fpointy, fpointz);
    	   	glEnd();
	    }
	}
	/*return 1;*/
}
