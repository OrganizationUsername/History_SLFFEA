/*
    This program contains the mesh display routine for the FEM GUI
    for triangle elements.
  
   			Last Update 2/4/02

    SLFFEA source file
    Version:  1.3
    Copyright (C) 1999, 2000, 2001, 2002  San Le 

    The source code contained in this file is released under the
    terms of the GNU Library General Public License.
 
 */

#if WINDOWS
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "../tri/trconst.h"
#include "../tri/trstruct.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

void trmeshdraw(void);
void trrender(void);
void trdisp_vectors(BOUND , double *);
void trforce_vectors(BOUND , double *, XYF *);

extern double *coord;
extern BOUND bc;

extern double left_right, up_down, in_out, left_right0,
	up_down0, in_out0, xAngle, yAngle, zAngle;
extern GLuint AxesList, DispList, ForceList;   /* Display lists */
extern XYF *force_vec;
extern int Render_flag, AppliedDisp_flag, AppliedForce_flag,
    Axes_flag, Before_flag, After_flag; 
extern int CrossSection_flag;

void AxesNumbers2(void);

void AxesNumbers(void);

void AxesLabel(void);

void CrossSetionPlaneDraw(void);

void trMeshDisplay(void)
{
    	glClear (GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    	glLoadIdentity ();  /*  clear the matrix    */

        glTranslatef (left_right, up_down, in_out);
        glRotatef (xAngle, 1, 0, 0);
        glRotatef (yAngle, 0, 1, 0);
        glRotatef (zAngle, 0, 0, 1);

    	glPointSize(8);
	if(Axes_flag)
		glCallList(AxesList);
	if(AppliedDisp_flag)
	{
		if(Before_flag )
			glCallList(DispList);
		if(After_flag )
  			trdisp_vectors(bc, coord);
	}
	if(CrossSection_flag)
	{
		CrossSetionPlaneDraw();
	}
	if(AppliedForce_flag)
	{
		if(Before_flag )
			glCallList(ForceList);
		if(After_flag )
  			trforce_vectors(bc, coord, force_vec);
	}
	glLineWidth (2.0);
	if(Render_flag)
		trrender();
    	else
		trmeshdraw();
	if(Axes_flag)
	{
		AxesNumbers();
		/*AxesNumbers2();*/
	}
  	glutSwapBuffers();
}

