/*
    This program rereads in the parameters of "brview.dat"
    for the FEM GUI for brick elements.
  
   			Last Update 6/4/00

    SLFFEA source file
    Version:  1.2
    Copyright (C) 1999, 2000, 2001  San Le 

    The source code contained in this file is released under the
    terms of the GNU Library General Public License.
 
 */
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../brick/brconst.h"
#include "../brick/brstruct.h"
#include "brstrcgr.h"
#include "../../common_gr/control.h"

#define init_far0      -2.0

extern int nmat, numnp, numel, dof;
extern double step_sizex, step_sizey, step_sizez;
extern double left, right, top, bottom, near, far, fscale;
extern int control_height, control_width, mesh_height, mesh_width;
extern double ortho_left, ortho_right, ortho_top, ortho_bottom;
extern double left_right, up_down, in_out, left_right0, up_down0, in_out0;
extern double AxisMax_x, AxisMax_y, AxisMax_z,
	AxisMin_x, AxisMin_y, AxisMin_z,
	IAxisMin_x, IAxisMin_y, IAxisMin_z;
extern double AxisLength_x, AxisLength_y, AxisLength_z,
	AxisLength_max;
extern double AxisPoint_step;
extern double amplify_step0;

extern double init_right, init_left, init_top,
	init_bottom, init_near, init_far, true_far, dim_max;
extern SDIM del_stress, del_strain, max_stress, min_stress,
	max_strain, min_strain;
extern double max_Ux, min_Ux, del_Ux, max_Uy, min_Uy, del_Uy,
	max_Uz, min_Uz, del_Uz, absolute_max_U;


void brReGetparameter( void)
{
        int i, j, check;
	int node_Ux_max, node_Ux_min, node_Uy_max, node_Uy_min,
		node_Uz_max, node_Uz_min; 
 	ISDIM max_stress_node, min_stress_node, max_strain_node, min_strain_node;
	char char_dum[20], char_dum2[5], buf[ BUFSIZ ];
	double fdum;
	FILE *brdata;

/*   brdata contains all the parameters and extreme values 
*/
	brdata = fopen( "brview.dat","r" );

/* Read data from the file "brview.dat" */

	fgets( buf, BUFSIZ, brdata );
	fgets( buf, BUFSIZ, brdata );
	fscanf( brdata,"%20s %5s      %d %d   %lf %lf\n", char_dum, char_dum2, &node_Ux_min,
		&node_Ux_max, &min_Ux, &max_Ux);
	fscanf( brdata,"%20s %5s      %d %d   %lf %lf\n", char_dum, char_dum2, &node_Uy_min,
		&node_Uy_max, &min_Uy, &max_Uy);
	fscanf( brdata,"%20s %5s      %d %d   %lf %lf\n", char_dum, char_dum2, &node_Uz_min,
		&node_Uz_max, &min_Uz, &max_Uz);
	fscanf( brdata,"\n");
	fgets( buf, BUFSIZ, brdata );
	fgets( buf, BUFSIZ, brdata );
	fscanf( brdata,"%20s %5s    %d %d  %lf %lf\n", char_dum, char_dum2,
		&min_stress_node.xx, &max_stress_node.xx,
		&min_stress.xx, &max_stress.xx);
	fscanf( brdata,"%20s %5s    %d %d  %lf %lf\n", char_dum, char_dum2,
		&min_stress_node.yy, &max_stress_node.yy,
		&min_stress.yy, &max_stress.yy);
	fscanf( brdata,"%20s %5s    %d %d  %lf %lf\n", char_dum, char_dum2,
		&min_stress_node.zz, &max_stress_node.zz,
		&min_stress.zz, &max_stress.zz);
	fscanf( brdata,"%20s %5s    %d %d  %lf %lf\n", char_dum, char_dum2,
		&min_stress_node.xy, &max_stress_node.xy,
		&min_stress.xy, &max_stress.xy);
	fscanf( brdata,"%20s %5s    %d %d  %lf %lf\n", char_dum, char_dum2,
		&min_stress_node.zx, &max_stress_node.zx,
		&min_stress.zx, &max_stress.zx);
	fscanf( brdata,"%20s %5s    %d %d  %lf %lf\n", char_dum, char_dum2,
		&min_stress_node.yz, &max_stress_node.yz,
		&min_stress.yz, &max_stress.yz);
	fscanf( brdata,"%20s %5s    %d %d  %lf %lf\n", char_dum, char_dum2,
		&min_stress_node.I, &max_stress_node.I,
		&min_stress.I, &max_stress.I);
	fscanf( brdata,"%20s %5s    %d %d  %lf %lf\n", char_dum, char_dum2,
		&min_stress_node.II, &max_stress_node.II,
		&min_stress.II, &max_stress.II);
	fscanf( brdata,"%20s %5s    %d %d  %lf %lf\n", char_dum, char_dum2,
		&min_stress_node.III, &max_stress_node.III,
		&min_stress.III, &max_stress.III);
	fscanf( brdata,"\n");
	fscanf( brdata,"%20s %5s    %d %d  %lf %lf\n", char_dum, char_dum2,
		&min_strain_node.xx, &max_strain_node.xx,
		&min_strain.xx, &max_strain.xx);
	fscanf( brdata,"%20s %5s    %d %d  %lf %lf\n", char_dum, char_dum2,
		&min_strain_node.yy, &max_strain_node.yy,
		&min_strain.yy, &max_strain.yy);
	fscanf( brdata,"%20s %5s    %d %d  %lf %lf\n", char_dum, char_dum2,
		&min_strain_node.zz, &max_strain_node.zz,
		&min_strain.zz, &max_strain.zz);
	fscanf( brdata,"%20s %5s    %d %d  %lf %lf\n", char_dum, char_dum2,
		&min_strain_node.xy, &max_strain_node.xy,
		&min_strain.xy, &max_strain.xy);
	fscanf( brdata,"%20s %5s    %d %d  %lf %lf\n", char_dum, char_dum2,
		&min_strain_node.zx, &max_strain_node.zx,
		&min_strain.zx, &max_strain.zx);
	fscanf( brdata,"%20s %5s    %d %d  %lf %lf\n", char_dum, char_dum2,
		&min_strain_node.yz, &max_strain_node.yz,
		&min_strain.yz, &max_strain.yz);
	fscanf( brdata,"%20s %5s    %d %d  %lf %lf\n", char_dum, char_dum2,
		&min_strain_node.I, &max_strain_node.I,
		&min_strain.I, &max_strain.I);
	fscanf( brdata,"%20s %5s    %d %d  %lf %lf\n", char_dum, char_dum2,
		&min_strain_node.II, &max_strain_node.II,
		&min_strain.II, &max_strain.II);
	fscanf( brdata,"%20s %5s    %d %d  %lf %lf\n", char_dum, char_dum2,
		&min_strain_node.III, &max_strain_node.III,
		&min_strain.III, &max_strain.III);
	fscanf( brdata,"\n");
	fgets( buf, BUFSIZ, brdata );
	fscanf( brdata,"%lf %lf %lf %lf %lf %lf\n", &ortho_right, &ortho_left,
		&ortho_top, &ortho_bottom, &near, &fdum);
	fgets( buf, BUFSIZ, brdata );
	fscanf( brdata,"%d %d\n", &mesh_width, &mesh_height);
	fgets( buf, BUFSIZ, brdata );
	fscanf( brdata,"%lf %lf %lf\n", &step_sizex, &step_sizey, &step_sizez);
	fgets( buf, BUFSIZ, brdata );
	fscanf( brdata,"%lf\n", &amplify_step0);

	fclose( brdata );

	printf( "                            node\n");
	printf( "                          min  max       min            max\n");
	printf("displacement Ux        %5d %5d   %14.6e %14.6e\n", node_Ux_min,
		node_Ux_max, min_Ux, max_Ux);
	printf("displacement Uy        %5d %5d   %14.6e %14.6e\n", node_Uy_min,
		node_Uy_max, min_Uy, max_Uy);
	printf("displacement Uz        %5d %5d   %14.6e %14.6e\n", node_Uz_min,
		node_Uz_max, min_Uz, max_Uz);
	printf("\n");
	printf( "                        el. gauss pt.\n");
	printf( "                        min       max         min           max\n");
	printf("stress xx            %5d     %5d  %14.6e %14.6e\n", min_stress_node.xx,
		max_stress_node.xx, min_stress.xx, max_stress.xx);
	printf("stress yy            %5d     %5d  %14.6e %14.6e\n", min_stress_node.yy,
		max_stress_node.yy, min_stress.yy, max_stress.yy);
	printf("stress zz            %5d     %5d  %14.6e %14.6e\n", min_stress_node.zz,
		max_stress_node.zz, min_stress.zz, max_stress.zz);
	printf("stress xy            %5d     %5d  %14.6e %14.6e\n", min_stress_node.xy,
		max_stress_node.xy, min_stress.xy, max_stress.xy);
	printf("stress zx            %5d     %5d  %14.6e %14.6e\n", min_stress_node.zx,
		max_stress_node.zx, min_stress.zx, max_stress.zx);
	printf("stress yz            %5d     %5d  %14.6e %14.6e\n", min_stress_node.yz,
		max_stress_node.yz, min_stress.yz, max_stress.yz);
	printf("stress I             %5d     %5d  %14.6e %14.6e\n", min_stress_node.I,
		max_stress_node.I, min_stress.I, max_stress.I);
	printf("stress II            %5d     %5d  %14.6e %14.6e\n", min_stress_node.II,
		max_stress_node.II, min_stress.II, max_stress.II);
	printf("stress III           %5d     %5d  %14.6e %14.6e\n", min_stress_node.III,
		max_stress_node.III, min_stress.III, max_stress.III);
	printf("\n");
	printf("strain xx            %5d     %5d  %14.6e %14.6e\n", min_strain_node.xx,
		max_strain_node.xx, min_strain.xx, max_strain.xx);
	printf("strain yy            %5d     %5d  %14.6e %14.6e\n", min_strain_node.yy,
		max_strain_node.yy, min_strain.yy, max_strain.yy);
	printf("strain zz            %5d     %5d  %14.6e %14.6e\n", min_strain_node.zz,
		max_strain_node.zz, min_strain.zz, max_strain.zz);
	printf("strain xy            %5d     %5d  %14.6e %14.6e\n", min_strain_node.xy,
		max_strain_node.xy, min_strain.xy, max_strain.xy);
	printf("strain zx            %5d     %5d  %14.6e %14.6e\n", min_strain_node.zx,
		max_strain_node.zx, min_strain.zx, max_strain.zx);
	printf("strain yz            %5d     %5d  %14.6e %14.6e\n", min_strain_node.yz,
		max_strain_node.yz, min_strain.yz, max_strain.yz);
	printf("strain I             %5d     %5d  %14.6e %14.6e\n", min_strain_node.I,
		max_strain_node.I, min_strain.I, max_strain.I);
	printf("strain II            %5d     %5d  %14.6e %14.6e\n", min_strain_node.II,
		max_strain_node.II, min_strain.II, max_strain.II);
	printf("strain III           %5d     %5d  %14.6e %14.6e\n", min_strain_node.III,
		max_strain_node.III, min_strain.III, max_strain.III);
	printf("\n");
	printf("Orthographic viewport parameters(right, left, top, bootom, near, far)\n ");
	printf("%14.6e %14.6e %14.6e %14.6e %14.6e %14.6e\n", ortho_right, ortho_left,
		ortho_top, ortho_bottom, near, 1000.0);
	printf("Perspective viewport parameters( mesh width and height)\n ");
	printf("%6d %6d\n", mesh_width, mesh_height);
	printf("Step sizes in x, y, z\n ");
	printf("%14.6e %14.6e %14.6e\n",step_sizex, step_sizey, step_sizez);
	printf("Amplification size\n ");
	printf("%14.6e\n",amplify_step0);
}

