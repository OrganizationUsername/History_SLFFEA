/*
    This utility function assembles the stiffness matrix for a finite 
    element program which does analysis on a shell element.  

		Updated 9/25/06

    SLFFEA source file
    Version:  1.1
    Copyright (C) 1999  San Le 

    The source code contained in this file is released under the
    terms of the GNU Library General Public License.
 
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "shconst.h"
#include "shstruct.h"

extern int analysis_flag, dof, integ_flag, numel, numnp, neqn, sof;
extern int gauss_stress_flag;
extern int lin_algebra_flag, numel_K, numel_P;
extern SH shg, shg_node, shl, shl_node;
extern ROTATE rotate, rotate_node;
extern double w[num_int];

int brcubic( double *);

int globalConjKassemble(double *, int *, int , double *,
        double *, int , int , int );

int globalKassemble(double *, int *, double *, int *, int );

int normal(double *);

int matX( double *,double *,double *, int ,int ,int );

int matXT(double *, double *, double *, int, int, int);

int shellB1ptT(double *, double *,double *, double *, double *, double *);

int shellB1ptM(double *, double *, double *, double *);

int shellB4pt(double *, double *,double *, double *, double *, double *);

int shshg( double *, int , SH , SH , XL , double *, double *, double *,
	double *, ROTATE , double *);

int normcrossX(double *, double *, double *);

int shKassemble(double *A, int *connect, double *coord, int *el_matl, double *force,
	int *id, int *idiag, double *K_diag, int *lm, MATL *matl,
	double *node_counter, STRAIN *strain, SDIM *strain_node, STRESS *stress,
	SDIM *stress_node, double *U, double *Voln)
{
        int i, i1, i2, i3, i4, i5, j, k, dof_el[neqel], sdof_el[npel*nsd];
	int check, counter, node;
	int matl_num;
	double Emod, Pois, G1, G2, G3, shearK, const1, const2, fdum1, fdum2, fdum3, fdum4;
	double D11,D12,D21,D22;
        double B[soB], DB[soB];
        double K_temp[neqlsq], K_el[neqlsq];
	double force_el[neqel], U_el[neqel];
        double coord_el_trans[npel*nsd], zm1[npell], zp1[npell],
		znode[npell*num_ints], dzdt_node[npell];
	double stress_el[sdim], strain_el[sdim], xxaddyy, xxsubyy, xysq, invariant[nsd],
                yzsq, zxsq, xxyy;
        double vec_dum[nsd];
        double det[num_int+num_ints];
	XL xl;

        for( k = 0; k < numel; ++k )
        {

		matl_num = *(el_matl+k);
		Emod = matl[matl_num].E;
		Pois = matl[matl_num].nu;
		shearK = matl[matl_num].shear;

/* The constants below are for plane stress */

		const1 = Emod/(1.0-Pois*Pois);
        	const2 = .5*(1.0-Pois);

		/*printf("Emod, Pois %f %f \n", Emod, Pois);*/

                D11 = const1;
		D12 = Pois*const1;
                D21 = Pois*const1;
		D22 = const1;

                G1 = const1*const2;
                G2 = const1*const2*shearK;
                G3 = const1*const2*shearK;

/* Create the coord transpose vector and other variables for one element */

                for( j = 0; j < npell; ++j )
                {
			node = *(connect+npell*k+j);

                        *(sdof_el+nsd*j)=nsd*node;
                        *(sdof_el+nsd*j+1)=nsd*node+1;
                        *(sdof_el+nsd*j+2)=nsd*node+2;

                        *(sdof_el+nsd*npell+nsd*j)=nsd*(node+numnp);
                        *(sdof_el+nsd*npell+nsd*j+1)=nsd*(node+numnp)+1;
                        *(sdof_el+nsd*npell+nsd*j+2)=nsd*(node+numnp)+2;

                        *(dof_el+ndof*j) = ndof*node;
                        *(dof_el+ndof*j+1) = ndof*node+1;
                        *(dof_el+ndof*j+2) = ndof*node+2;
                        *(dof_el+ndof*j+3) = ndof*node+3;
                        *(dof_el+ndof*j+4) = ndof*node+4;

/* Count the number of times a particular node is part of an element */

			if(analysis_flag == 1)
			{
				*(node_counter + node) += 1.0;
				*(node_counter + node + numnp) += 1.0;
			}

/* Create the coord -/+*/

                        *(coord_el_trans+j) =
				*(coord+*(sdof_el+nsd*j));
                        *(coord_el_trans+npel*1+j) =
				*(coord+*(sdof_el+nsd*j+1));
                        *(coord_el_trans+npel*2+j) =
				*(coord+*(sdof_el+nsd*j+2));

                        *(coord_el_trans+npell+j) =
				*(coord+*(sdof_el+nsd*npell+nsd*j));
                        *(coord_el_trans+npel*1+npell+j) =
				*(coord+*(sdof_el+nsd*npell+nsd*j+1));
                        *(coord_el_trans+npel*2+npell+j) =
				*(coord+*(sdof_el+nsd*npell+nsd*j+2));

/* Create the coord_bar and coord_hat vector for one element */

                        xl.bar[j]=.5*( *(coord_el_trans+j)*(1.0-zeta)+
				*(coord_el_trans+npell+j)*(1.0+zeta));
                        xl.bar[npell*1+j]=.5*( *(coord_el_trans+npel*1+j)*(1.0-zeta)+
				*(coord_el_trans+npel*1+npell+j)*(1.0+zeta));
                        xl.bar[npell*2+j]=.5*( *(coord_el_trans+npel*2+j)*(1.0-zeta)+
				*(coord_el_trans+npel*2+npell+j)*(1.0+zeta));

                        xl.hat[j]=*(coord_el_trans+npell+j)-*(coord_el_trans+j);
                        xl.hat[npell*1+j]=*(coord_el_trans+npel*1+npell+j)-
				*(coord_el_trans+npel*1+j);
                        xl.hat[npell*2+j]=*(coord_el_trans+npel*2+npell+j)-
				*(coord_el_trans+npel*2+j);

			fdum1=fabs(xl.hat[j]);
			fdum2=fabs(xl.hat[npell*1+j]);
			fdum3=fabs(xl.hat[npell*2+j]);
			fdum4=sqrt(fdum1*fdum1+fdum2*fdum2+fdum3*fdum3);
                        xl.hat[j] /= fdum4;
                        xl.hat[npell*1+j] /= fdum4;
                        xl.hat[npell*2+j] /= fdum4;
			*(zp1+j)=.5*(1.0-zeta)*fdum4;
			*(zm1+j)=-.5*(1.0+zeta)*fdum4;
/*
   Calculating rotation matrix for the fiber q[i,j] matrix.

   The algorithm below is taken from "The Finite Element Method" by Thomas Hughes,
   page 388.  The goal is to find the local shell coordinates which come closest
   to the global x, y, z coordinates.  In the algorithm below, vec_dum is set to either
   the global x, y, or z basis vector based on the one of the 2 smaller components of
   xl.hat.  xl.hat itself is the local z direction of that node.  Once set, the cross
   product of xl.hat and vec_dum produces the local y fiber direction.  This local y is
   then crossed with xl.hat to produce local x.
*/
        	       	memset(vec_dum,0,nsd*sof);
			i2=1;
			if( fdum1 > fdum3)
			{
				fdum3=fdum1;
				i2=2;
			}
			if( fdum2 > fdum3) i2=3;
			*(vec_dum+(i2-1))=1.0;
                	rotate.f_shear[nsdsq*j+2*nsd]=xl.hat[j];
                	rotate.f_shear[nsdsq*j+2*nsd+1]=xl.hat[npell*1+j];
                	rotate.f_shear[nsdsq*j+2*nsd+2]=xl.hat[npell*2+j];
                	check = normcrossX((rotate.f_shear+nsdsq*j+2*nsd),
			    vec_dum,(rotate.f_shear+nsdsq*j+1*nsd));
                	if(!check) printf( "Problems with normcrossX \n");
                	check = normcrossX((rotate.f_shear+nsdsq*j+1*nsd),
			    (rotate.f_shear+nsdsq*j+2*nsd),(rotate.f_shear+nsdsq*j));
                	if(!check) printf( "Problems with normcrossX \n");
                }

		memcpy(rotate_node.f_shear,rotate.f_shear,sorfs*sizeof(double));

/* Assembly of the shg matrix for each integration point */

		check=shshg( det, k, shl, shg, xl, zp1, zm1, znode,
			dzdt_node, rotate, (Voln + k));
		if(!check) printf( "Problems with shshg \n");

		memset(U_el,0,neqel*sof);
		memset(K_el,0,neqlsq*sof);
		memset(force_el,0,neqel*sof);

/* The loop over i4 below calculates the 2 fiber points of the gaussian integration */

		for( i4 = 0; i4 < num_ints; ++i4 )
                {

/* The loop over j below calculates the 2X2 points of the gaussian integration
   over the lamina for several quantities */

                   for( j = 0; j < num_intb; ++j )
                   {
        	       	memset(B,0,soB*sof);
        	       	memset(DB,0,soB*sof);
        		memset(K_temp,0,neqlsq*sof);

/* Assembly of the B matrix */

                    	check =
			   shellB4pt((shg.bend+npell*(nsd+1)*num_intb*i4+npell*(nsd+1)*j),
                    	   (shg.bend_z+npell*(nsd)*num_intb*i4+npell*(nsd)*j),
			   (znode+npell*i4),B,(rotate.l_bend+nsdsq*num_intb*i4+nsdsq*j),
			   rotate.f_shear);
                        if(!check) printf( "Problems with shellB4pt \n");

                       	if( integ_flag == 0 || integ_flag == 2 ) 
		        {
/* Calculate the membrane shear terms in B using 1X1 point gaussian integration in lamina */

                       	    check = shellB1ptM((shg.shear+npell*(nsd+1)*i4),
				(B+(sdim-2)*neqel),
				(rotate.l_shear+nsdsq*i4), rotate.f_shear);
                       	    if(!check) printf( "Problems with shellB1pt \n");
			}

                       	if( integ_flag == 1 || integ_flag == 2 ) 
		        {
/* Calculate the transverse shear terms in B using 1X1 point gaussian integration in lamina */

                       	    check = shellB1ptT((shg.shear+npell*(nsd+1)*i4),
                    	    	(shg.bend_z+npell*(nsd)*num_intb*i4+npell*(nsd)*j),
				(znode+npell*i4),(B+(sdim-2)*neqel),
				(rotate.l_shear+nsdsq*i4), rotate.f_shear);
                       	    if(!check) printf( "Problems with shellB1pt \n");
			}

                    	for( i1 = 0; i1 < neqel; ++i1 )
                    	{
                        	*(DB+i1)=*(B+i1)*D11 +
					*(B+neqel*1+i1)*D12;
                        	*(DB+neqel*1+i1)=*(B+i1)*D21 +
					*(B+neqel*1+i1)*D22;
                        	*(DB+neqel*2+i1)=*(B+neqel*2+i1)*G1;
                            	*(DB+neqel*3+i1)=*(B+neqel*3+i1)*G2;
                            	*(DB+neqel*4+i1)=*(B+neqel*4+i1)*G3;
		       	}
#if 0
                	fprintf(oB,"\n\n\n");
                	fprintf(oS,"\n\n\n");
                	for( i3 = 0; i3 < 3; ++i3 )
                	{
                   	    fprintf(oB,"\n ");
                   	    fprintf(oS,"\n ");
                   	    for( i2 = 0; i2 < neqel; ++i2 )
                   	    {
                            	fprintf(oB,"%14.5e ",*(B+neqel*i3+i2));
                            	fprintf(oS,"%14.5e ",*(DB+neqel*i3+i2));
                   	    }
                	}
#endif
                    	check=matXT(K_temp, B, DB, neqel, neqel, sdim);
                    	if(!check) printf( "Problems with matXT \n");

                    	for( i2 = 0; i2 < neqlsq; ++i2 )
                    	{
                           *(K_el+i2) +=
				*(K_temp+i2)*(*(w+num_intb*i4+j))*(*(det+num_intb*i4+j));
                    	}
                   }
                }

                for( j = 0; j < neqel; ++j )
                {
                        *(U_el + j) = *(U + *(dof_el+j));
                }

                check = matX(force_el, K_el, U_el, neqel, 1, neqel);
                if(!check) printf( "Problems with matX \n");

                if(analysis_flag == 1)
                {

/* Compute the equivalant nodal forces based on prescribed displacements */

			for( j = 0; j < neqel; ++j )
			{
				*(force + *(dof_el+j)) -= *(force_el + j);
			}

/* Assembly of either the global skylined stiffness matrix or numel_K of the
   element stiffness matrices if the Conjugate Gradient method is used */

			if(lin_algebra_flag)
			{
			    check = globalKassemble(A, idiag, K_el, (lm + k*neqel),
				neqel);
			    if(!check) printf( "Problems with globalKassemble \n");
			}
			else
			{
			    check = globalConjKassemble(A, dof_el, k, K_diag, K_el,
				neqel, neqlsq, numel_K);
			    if(!check) printf( "Problems with globalConjKassemble \n");
			}
                }
		else
		{
/* Calculate the element reaction forces */

			for( j = 0; j < neqel; ++j )
                        {
				*(force + *(dof_el+j)) += *(force_el + j);
			}

/* Calculate the element stresses */

/* Assembly of the shstress_shg matrix for each nodal point */

/*
   shstress_shg calculates shg at the nodes.  It is more efficient than shshg
   because it removes all the zero multishications from the calculation of shg. 
   You can use either function when calculating shg at the nodes.  Because stress
   is calculated at the Gauss points, shshg is used.

			check=shshg( det, k, shl_node, shg, xl, zp1, zm1, znode,
				dzdt_node, rotate, (Voln + k));
			check=shstress_shg(det, k, shl_node2, shg, coord_el_trans);
*/

			if(gauss_stress_flag)
			{
/* Calculate shg at integration point */
			    check=shshg( det, k, shl, shg, xl, zp1, zm1, znode,
				dzdt_node, rotate, (Voln + k));
			    if(!check) printf( "Problems with shshg \n");
			}
			else
			{
/* Calculate shg at nodal point */
			    check=shshg( det, k, shl_node, shg_node, xl, zp1, zm1, znode,
				dzdt_node, rotate_node, (Voln + k));
			    if(!check) printf( "Problems with shshg \n");
			}


/* The loop over i4 below calculates the 2 fiber points of the gaussian integration */

			for( i4 = 0; i4 < num_ints; ++i4 )
                	{

/* The loop over j below calculates the 2X2 points of the gaussian integration
   over the lamina for several quantities */

                   	    for( j = 0; j < num_intb; ++j )
                   	    {
        	       		memset(B,0,soB*sof);
                           	memset(stress_el,0,sdim*sof);
                           	memset(strain_el,0,sdim*sof);

				node = *(connect+npell*k+j) + i4*numnp;

/* Assembly of the B matrix */

				if(gauss_stress_flag)
				{
				   check =
					shellB4pt(
					(shg.bend+npell*(nsd+1)*num_intb*i4+npell*(nsd+1)*j),
					(shg.bend_z+npell*(nsd)*num_intb*i4+npell*(nsd)*j),
					(znode+npell*i4),B,
					(rotate.l_bend+nsdsq*num_intb*i4+nsdsq*j),
					rotate.f_shear);
				   if(!check) printf( "Problems with shellB4pt \n");

				   if( integ_flag == 0 || integ_flag == 2 )
				   {
/* Calculate the membrane shear terms in B using 1X1 point gaussian integration in lamina */

					check = shellB1ptM((shg.shear+npell*(nsd+1)*i4),
					    (B+(sdim-2)*neqel),
					    (rotate.l_shear+nsdsq*i4), rotate.f_shear);
					if(!check) printf( "Problems with shellB1pt \n");
				   }

				   if( integ_flag == 1 || integ_flag == 2 )
				   {
/* Calculate the transverse shear terms in B using 1X1 point gaussian integration in lamina */

					check = shellB1ptT((shg.shear+npell*(nsd+1)*i4),
					    (shg.bend_z+npell*(nsd)*num_intb*i4+npell*(nsd)*j),
					    (znode+npell*i4),(B+(sdim-2)*neqel),
					    (rotate.l_shear+nsdsq*i4), rotate.f_shear);
					if(!check) printf( "Problems with shellB1pt \n");
				   }
				}
				else
				{
/* Calculate B matrix at nodal point */
				   check =
					shellB4pt(
					(shg_node.bend+npell*(nsd+1)*num_intb*i4+npell*(nsd+1)*j),
					(shg_node.bend_z+npell*(nsd)*num_intb*i4+npell*(nsd)*j),
					(znode+npell*i4),B,
					(rotate_node.l_bend+nsdsq*num_intb*i4+nsdsq*j),
					rotate_node.f_shear);
					if(!check) printf( "Problems with shellB4pt \n");
				}

/* Calculation of the local strain matrix */

                	   	check=matX(strain_el, B, U_el, sdim, 1, neqel );
                    	   	if(!check) printf( "Problems with matX \n");
#if 0
                    	   	for( i1 = 0; i1 < sdim; ++i1 )
                    	   	{
                          	     printf("%12.8f ",*(stress_el+i1));
                          	     /*printf("%12.2f ",*(stress_el+i1));
                          	     printf("%12.8f ",*(B+i1));*/
                    	   	}
                    	   	printf("\n");
#endif

/* Update of the global strain matrix */

				i5 = num_intb*i4+j;

		       	   	strain[k].pt[i5].xx = *(strain_el);
		     	   	strain[k].pt[i5].yy = *(strain_el+1);
		    	   	strain[k].pt[i5].xy = *(strain_el+2);
		    	   	strain[k].pt[i5].zx = *(strain_el+3);
		    	   	strain[k].pt[i5].yz = *(strain_el+4);

/* Calculate the principal straines */

#if 0
/* The code below assumed a plane state of stress which I now feel is incorrect.  So I
   calculate all 3 principal stresses and strains.
*/
			   	xxaddyy = .5*(strain[k].pt[i5].xx + strain[k].pt[i5].yy);
			   	xxsubyy = .5*(strain[k].pt[i5].xx - strain[k].pt[i5].yy);
			   	xysq = strain[k].pt[i5].xy*strain[k].pt[i5].xy;

		    	   	strain[k].pt[i5].I = xxaddyy + sqrt( xxsubyy*xxsubyy
					+ xysq);
		    	   	strain[k].pt[i5].II = xxaddyy - sqrt( xxsubyy*xxsubyy
					+ xysq);
           		   	/*printf("%14.6e %14.6e %14.6e\n",xxaddyy,xxsubyy,xysq);*/
#endif
#if 1
				memset(invariant,0,nsd*sof);
				xysq = strain[k].pt[i5].xy*strain[k].pt[i5].xy;
				zxsq = strain[k].pt[i5].zx*strain[k].pt[i5].zx;
				yzsq = strain[k].pt[i5].yz*strain[k].pt[i5].yz;
				xxyy = strain[k].pt[i5].xx*strain[k].pt[i5].yy;

				*(invariant) = - strain[k].pt[i5].xx -
				     strain[k].pt[i5].yy;
				*(invariant+1) = xxyy - yzsq - zxsq - xysq;
				*(invariant+2) = -
				     2*strain[k].pt[i5].yz*strain[k].pt[i5].zx*strain[k].pt[i5].xy +
				     yzsq*strain[k].pt[i5].xx + zxsq*strain[k].pt[i5].yy;
				     
				check = brcubic(invariant);

				strain[k].pt[i5].I = *(invariant);
				strain[k].pt[i5].II = *(invariant+1);
				strain[k].pt[i5].III = *(invariant+2);
#endif

/* Add all the strains for a particular node from all the elements which share that node */

				strain_node[node].xx += strain[k].pt[i5].xx;
				strain_node[node].yy += strain[k].pt[i5].yy;
				strain_node[node].xy += strain[k].pt[i5].xy;
				strain_node[node].zx += strain[k].pt[i5].zx;
				strain_node[node].yz += strain[k].pt[i5].yz;
				strain_node[node].I += strain[k].pt[i5].I;
				strain_node[node].II += strain[k].pt[i5].II;
				strain_node[node].III += strain[k].pt[i5].III;

/* Calculation of the local stress matrix */

                        	*(stress_el)= strain[k].pt[i5].xx*D11 +
					strain[k].pt[i5].yy*D12;
                        	*(stress_el+1)= strain[k].pt[i5].xx*D21 +
					strain[k].pt[i5].yy*D22;
                        	*(stress_el+2)= strain[k].pt[i5].xy*G1;
                            	*(stress_el+3)= strain[k].pt[i5].zx*G2;
                            	*(stress_el+4)= strain[k].pt[i5].yz*G3;

/* Update of the global stress matrix */

		       	   	stress[k].pt[i5].xx += *(stress_el);
		     	   	stress[k].pt[i5].yy += *(stress_el+1);
		    	   	stress[k].pt[i5].xy += *(stress_el+2);
		    	   	stress[k].pt[i5].zx += *(stress_el+3);
		    	   	stress[k].pt[i5].yz += *(stress_el+4);

/* Calculate the principal stresses */

#if 0
/* The code below assumed a plane state of stress which I now feel is incorrect.  So I
   calculate all 3 principal stresses and strains.
*/
			   	xxaddyy = .5*(stress[k].pt[i5].xx + stress[k].pt[i5].yy);
			   	xxsubyy = .5*(stress[k].pt[i5].xx - stress[k].pt[i5].yy);
			   	xysq = stress[k].pt[i5].xy*stress[k].pt[i5].xy;

		    	   	stress[k].pt[i5].I = xxaddyy + sqrt( xxsubyy*xxsubyy
					+ xysq);
		    	   	stress[k].pt[i5].II = xxaddyy - sqrt( xxsubyy*xxsubyy
					+ xysq);
#endif
#if 1
				memset(invariant,0,nsd*sof);
				xysq = stress[k].pt[i5].xy*stress[k].pt[i5].xy;
				zxsq = stress[k].pt[i5].zx*stress[k].pt[i5].zx;
				yzsq = stress[k].pt[i5].yz*stress[k].pt[i5].yz;
				xxyy = stress[k].pt[i5].xx*stress[k].pt[i5].yy;

				*(invariant) = - stress[k].pt[i5].xx -
				     stress[k].pt[i5].yy;
				*(invariant+1) = xxyy - yzsq - zxsq - xysq;
				*(invariant+2) = -
				     2*stress[k].pt[i5].yz*stress[k].pt[i5].zx*stress[k].pt[i5].xy +
				     yzsq*stress[k].pt[i5].xx + zxsq*stress[k].pt[i5].yy;
				     
				check = brcubic(invariant);

				stress[k].pt[i5].I = *(invariant);
				stress[k].pt[i5].II = *(invariant+1);
				stress[k].pt[i5].III = *(invariant+2);
#endif

/* Add all the stresses for a particular node from all the elements which share that node */

				stress_node[node].xx += stress[k].pt[i5].xx;
				stress_node[node].yy += stress[k].pt[i5].yy;
				stress_node[node].xy += stress[k].pt[i5].xy;
				stress_node[node].zx += stress[k].pt[i5].zx;
				stress_node[node].yz += stress[k].pt[i5].yz;
				stress_node[node].I += stress[k].pt[i5].I;
				stress_node[node].II += stress[k].pt[i5].II;
				stress_node[node].III += stress[k].pt[i5].III;

/*
           		   	printf("%14.6e ",stress[k].pt[i5].xx);
           		   	printf("%14.6e ",stress[k].pt[i5].yy);
           		   	printf("%14.6e ",stress[k].pt[i5].xy);
           		   	printf( "\n");
*/
                	    }
           		    /*printf( "\n");*/
                	}
		}
	}

	if(analysis_flag == 1)
	{

/* Contract the global force matrix using the id array only if linear
   algebra is used. */

	  if(lin_algebra_flag)
	  {
	     counter = 0;
	     for( i = 0; i < dof ; ++i )
	     {
		if( *(id + i ) > -1 )
		{
			*(force + counter ) = *(force + i );
			++counter;
		}
	     }
          }
        }
	if(analysis_flag == 2)
	{

/* Average all the stresses and strains at the nodes */

	  for( i = 0; i < 2*numnp ; ++i )
	  {
		   strain_node[i].xx /= *(node_counter + i);
		   strain_node[i].yy /= *(node_counter + i);
		   strain_node[i].xy /= *(node_counter + i);
		   strain_node[i].zx /= *(node_counter + i);
		   strain_node[i].yz /= *(node_counter + i);
		   strain_node[i].I /= *(node_counter + i);
		   strain_node[i].II /= *(node_counter + i);
		   strain_node[i].III /= *(node_counter + i);

		   stress_node[i].xx /= *(node_counter + i);
		   stress_node[i].yy /= *(node_counter + i);
		   stress_node[i].xy /= *(node_counter + i);
		   stress_node[i].zx /= *(node_counter + i);
		   stress_node[i].yz /= *(node_counter + i);
		   stress_node[i].I /= *(node_counter + i);
		   stress_node[i].II /= *(node_counter + i);
		   stress_node[i].III /= *(node_counter + i);
	  }
	}

	return 1;
}
