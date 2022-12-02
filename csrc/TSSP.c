// 注释参照：https://github.com/SamXiaosheng/SLIC-opencv/blob/master/SLICcv.py

#include <stdio.h>
#include <math.h>
#include <float.h>
#include "TSSP.h"
#include "array2d.h"
#include <string.h>
#include <stdbool.h>

// constants here
# define SIGMA_C 40
# define SIGMA_R 0.5
# define COMPACTNESS 20

# define square(name) ((name) * (name))
# define max(a, b) ((a) > (b) ? (a) : (b))
# define min(a, b) ((a) < (b) ? (a) : (b))
# define hypot3(a, b, c) hypot(hypot(a, b), c)

typedef struct _rgbu8 {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} rgbu8;

typedef struct _labf64 {
	double l;
	double a;
	double b;
} labf64;

typedef struct _labxyf64 {
	double l;
	double a;
	double b;
	double x;
	double y;
} labxyf64;

typedef struct _point {
	int y;
	int x;
} point;

const double epsilon = 0.008856;
const double kappa   = 903.3   ;
const double Xr      = 0.950456;
const double Yr      = 1.0     ;
const double Zr      = 1.088754;


void rgbtolab(rgbu8 *imgbytes, int sz, labf64 *imglab) {
	rgbu8 rgbc; labf64 labc;
    double R, G, B, r, g, b;
    double X, Y, Z, xr, yr, zr;
    double fx, fy, fz, lval, aval, bval;
    
	int i;
    for (i = 0; i < sz; i++) {
		rgbc = imgbytes[i];
		
		R = rgbc.r / 255.0;
		r = (R <= 0.04045) ? R / 12.92 : pow((R+0.055) / 1.055, 2.4);
		G = rgbc.g / 255.0;
		g = (G <= 0.04045) ? G / 12.92 : pow((G+0.055) / 1.055, 2.4);
		B = rgbc.b / 255.0;
		b = (B <= 0.04045) ? B / 12.92 : pow((B+0.055) / 1.055, 2.4);
        
        X = r*0.4124564 + g*0.3575761 + b*0.1804375; xr = X / Xr;
        Y = r*0.2126729 + g*0.7151522 + b*0.0721750; yr = Y / Yr;
        Z = r*0.0193339 + g*0.1191920 + b*0.9503041; zr = Z / Zr;
        
		fx = (xr > epsilon) ? pow(xr, 1.0/3.0) : (kappa*xr + 16.0) / 116.0;
		fy = (yr > epsilon) ? pow(yr, 1.0/3.0) : (kappa*yr + 16.0) / 116.0;
		fx = (zr > epsilon) ? pow(zr, 1.0/3.0) : (kappa*zr + 16.0) / 116.0;
        
        lval = 116.0 * fy - 16.0; labc.l = lval;
        aval = 500.0 * (fx-fy);   labc.a = aval;
        bval = 200.0 * (fy-fz);   labc.b = bval;
		
		imglab[i] = labc;
    }
}


void getLABXYSeeds(int STEP, int width, int height, point* seedIndices, int* numseeds) {
	int x, xe, xstrips, xerr, xoff;
	int y, ye, ystrips, yerr, yoff;
    double xerrperstrip, yerrperstrip;

	xstrips = 0.5 + (double)width  / (double)STEP;
	ystrips = 0.5 + (double)height / (double)STEP;
    
    xerr = width  - STEP * xstrips; if (xerr < 0) {xstrips--; xerr += STEP;}
    yerr = height - STEP * ystrips; if (yerr < 0) {ystrips--; yerr += STEP;}
    
	xerrperstrip = (double)xerr / (double)xstrips;
	yerrperstrip = (double)yerr / (double)ystrips;
    
	xoff = STEP/2;
	yoff = STEP/2;
    
    int n = 0; point seedp;
	for (y = 0; y < ystrips; y++) { ye = y*yerrperstrip;
		for (x = 0; x < xstrips; x++) { xe = x*xerrperstrip;
			seedp.x = x * STEP + xoff + xe;
			seedp.y = y * STEP + yoff + ye;
			seedIndices[n] = seedp;
			n++;
		}
	}
    *numseeds = n;
}


void EnforceSuperpixelConnectivity(int *labels, int width, int height, int numSuperpixels, int *nlabels, int *finalNumberOfLabels) {
	int i,j,k;
	int n,c,count;
	int x,y;
	int ind;
	int oindex, adjlabel;
	int label;
	const int dx4[4] = {-1,  0,  1,  0};
	const int dy4[4] = { 0, -1,  0,  1};
	const int sz = width*height;
	const int SUPSZ = sz/numSuperpixels;
	int* xvec = malloc(sizeof(int)*SUPSZ*10);
	int* yvec = malloc(sizeof(int)*SUPSZ*10);

     adjlabel = 0; label = 0;
	for ( oindex = 0, j = 0; j < height; j++ ) {
		for ( k = 0; k < width; k++, oindex++ ) {
			if ( 0 > nlabels[oindex] ) {
				nlabels[oindex] = label;
				
				xvec[0] = k;
				yvec[0] = j;
				
				for ( n = 0; n < 4; n++ ) {
					int x = xvec[0] + dx4[n];
					int y = yvec[0] + dy4[n];
					if ( (x >= 0 && x < width) && (y >= 0 && y < height) ) {
						int nindex = y*width + x;
						if(nlabels[nindex] >= 0) adjlabel = nlabels[nindex];
					}
				}
                
				count = 1;
				for ( c = 0; c < count; c++ ) {
					for ( n = 0; n < 4; n++ ) {
						x = xvec[c] + dx4[n];
						y = yvec[c] + dy4[n];
                        
						if ( (x >= 0 && x < width) && (y >= 0 && y < height) ) {
							int nindex = y*width + x;
                            
							if ( 0 > nlabels[nindex] && labels[oindex] == labels[nindex] ) {
								xvec[count] = x;
								yvec[count] = y;
								nlabels[nindex] = label;
								count++;
							}
						}
                        
					}
				}
				
				if(count <= SUPSZ >> 2) {
					for( c = 0; c < count; c++ ) {
						ind = yvec[c]*width+xvec[c];
						nlabels[ind] = adjlabel;
					} label--;
				} label++;
			}
		}
	}
	*finalNumberOfLabels = label;
    
	free(xvec);
	free(yvec);
}

#include "circmask.c"

// 这么写是为了方便opencl实现
void SPPcalcPoint(int pid, labxyf64 ks, int offset, int radius, int gamma, int width, int height, double NC, double NS,
				  struct circmaskset *maskset, array2d *distSmap_info, double *distvec, int *klabels,
				  array2d *MapLAB_info, array2d *Amptitude_info, array2d *Xita_info, array2d *Gray_info) {
	
	printf("M1");
	array2d *masklabels_info = maskset->labels;
    getarray2d(masklabels_info, int, masklabels, );
	getarray2d(distSmap_info, double, distSmap, );
	getarray2d(MapLAB_info, labf64, MapLAB, );
	getarray2d(Amptitude_info, double, Amptitude, );
	int windows = 2 * radius + 1;
	getarray2d(Xita_info, double, Xita, );
	getarray2d(Gray_info, double, Gray, );

	double *maskarrset = (double *)maskset->maskset;
	declarray2d(windows, double, circle, );
	double *masksums = maskset->masksums;
	int x1, y1, x2, y2, x, y, idx, idy;
	x1 = ks.x - offset;     y1 = ks.y - offset;
	x2 = ks.x + offset + 1; y2 = ks.y + offset + 1; 
	
	labf64 q, pQ; double alpha;
	double pGray, pXita, qGray, qXita;
	double WP, WPSum, cdis, WQ, WQSum, distP, distQ, distF, distG, distS, dist;
	int dm, mm, i; double dx, dy, xx, yy, igmax; int xxi, yyi;

	for (y = max(y1, 0); y < min(y2, height); y++) {
		for (x = max(x1, 0); x < min(x2, width); x++) {
			distP = distQ = 0;
			pQ.l = 0; pQ.a = 0; pQ.b = 0;
			pGray = Gray[y+radius][x+radius];
			pXita = Xita[y+radius][x+radius];
			WPSum = 0; WQSum = masksums[masklabels[y-y1][x-x1]];
			castarray2d_nodecl(maskarrset + masklabels[y-y1][x-x1] * windows * windows, windows, double, circle, );
			for (idy = 0; idy < windows; idy++) {
				for (idx = 0; idx < windows; idx++) {
					
					qGray = Gray[y+idy][x+idx];
					qXita = Xita[y+idy][x+idx];

					WQ = circle[idy][idx];
					if (WQ == 0) {continue;}
					
					// 这块可以提前算，空间换时间
					// 但是空间代价太大，寄！
					WP = exp(-square(qGray-pGray) / (2*square(SIGMA_C))
					         -square(qXita-pXita) / (2*square(SIGMA_R)));

					// eq.11
					WP = WP * WQ;
					cdis = hypot3(q.l-ks.l, q.a-ks.a, q.b-ks.b);
					WPSum += WP;
					distP += WP * cdis;

					q = MapLAB[y+idy][x+idx];
					// eq.13
					pQ.l += q.l * WQ;
					pQ.a += q.a * WQ;
					pQ.b += q.b * WQ;
				}	
			}
			distP = distP / WPSum;
			// eq.13
			distQ = hypot3(pQ.l/WQSum-ks.l, pQ.a/WQSum-ks.a, pQ.b/WQSum-ks.b);
			// eq.14
			distF = min(distP, distQ);

			// Bresenham’s line drawing algorithm
			dm = max(abs(x-(int)(ks.x)), abs(y-(int)(ks.y)));
			dx = (ks.x-x) * 1.0 / dm; dy = (ks.y-y) * 1.0 / dm;
			// eq.15
			for (igmax=0, xx=x, yy=y, mm=0; mm<dm; mm++) {
				xxi = (int)round(xx); yyi = (int)round(yy);
				if (xxi<0 || yyi<0 || xxi>width-1 || yyi>height-1) break;
				if (igmax < Amptitude[yyi][xxi]) igmax = Amptitude[yyi][xxi];
				xx+=dx; yy+=dy;
			}
			distG = 1 + gamma * igmax;
			
			distS = distSmap[y-y1][x-x1];

			// eq.16
			dist = distG * (square(distF/NC) + square(distS/NS));

			i = y*width + x;
			if (dist < distvec[i]) {
				distvec[i] = dist;
				klabels[i] = pid;
			}
		}
	}
}


void PerformSuperpixelSLIC(labf64 *imglab, labxyf64 *kseeds,
						   int width, int height, int numseeds, int *klabels, int STEP, double compactness,
						   array2d *MapLAB_info, array2d *Amptitude_info, array2d *Xita_info, array2d *Gray_info,
						   int radius, int gamma) {
    int itr, n, x, y, ind, r, c, i, j, k;
    int sz = width*height;
	int offset = STEP;
	int sppsize = 2 * offset + 1;
	int windows = 2 * radius + 1;
	double NS = square((double)STEP/compactness);
    double* clustersize = malloc(sizeof(double)*numseeds);
    labxyf64 *sigma     = (labxyf64 *)calloc(numseeds, sizeof(labxyf64));
    double *distvec     = malloc(sizeof(double)*sz);

	labxyf64 s; labf64 labc; double vclsz; labxyf64 ks;
	
	// pre-cauculate circular masks
	struct circmaskset *maskset = getAllCircularMasks(offset, radius);
	// pre-cauculate spacial distances
	array2d * distSmap_info = mallocarray2d(sppsize, sppsize, sizeof(double));
	getarray2d(distSmap_info, double, distSmap, );
	for (j = 0; j < sppsize; j++) {
		for (i = 0; i < sppsize; i++) {
			distSmap[j][i] = hypot(i-offset, j-offset);
		}
	}

	for (i = 0; i < sz; i++) {distvec[i] = DBL_MAX;}

	for (itr = 0; itr < 10; itr++) {
		for (n = 0; n < numseeds; n++) {
			SPPcalcPoint(n, kseeds[n], offset, radius, gamma, width, height, compactness, NS,
						 maskset, distSmap_info, distvec, klabels,
						 MapLAB_info, Amptitude_info, Xita_info, Gray_info);
		}

		// clear sigma and clustersize
		memset(sigma, 0, numseeds * sizeof(labxyf64));
		memset(clustersize, 0, numseeds * sizeof(double));
		ind = 0;
        for (r = 0; r < height; r++) {
            for (c = 0; c < width; c++) {
				labc = imglab[ind];
				s = sigma[klabels[ind]];
				s.l += labc.l; s.a += labc.a; s.b += labc.b;
				s.x += c; s.y += r;
				sigma[klabels[ind]] = s;
				clustersize[klabels[ind]] += 1.0;
                ind++;
            }
        }
		
		for (k = 0; k < numseeds; k++) {
			s = sigma[k]; vclsz = clustersize[k];
			ks = kseeds[k];
			ks.l = s.l / vclsz;
			ks.a = s.a / vclsz;
			ks.b = s.b / vclsz;
			ks.x = s.x / vclsz;
			ks.y = s.y / vclsz;
			kseeds[k] = ks;
		}
	}

    free(sigma);
    free(clustersize);
    free(distvec);
	// free all mask info
	freearray2d(maskset->labels);
    free(maskset->masksums);
	free(maskset->maskset);
	free(maskset);
}


TSSP_result_t *TSSPmain(int numSuperpixels, size_t height, size_t width,
                        unsigned char *img, double *imglab_buf, double *imggray_buf,
                        double *magnitude, double *direction,
                        int radius, int gamma) {
	
	int i, j, k, x, y, ii;
	
	double compactness = COMPACTNESS;
	
	int sz = width*height;
	// "p" means pad
	int widthp = width+2*radius; int heightp = height+2*radius;
	
	rgbu8 *imgbytes = (rgbu8 *)img;
	labf64 *imglab = (labf64 *)malloc(sizeof(labf64) * sz);
	rgbtolab(imgbytes, sz, imglab);
	
	int *klabels = calloc( sz,  sizeof(int) ); // initialize to 0
	int *clabels = malloc( sz * sizeof(int) ); 
	memset(clabels, 0xff, sz * sizeof(int)); // initialize to -1
	
	labf64* MapLAB_data = (labf64 *)imglab_buf;
	array2d *MapLAB_info = packarray2d(MapLAB_data, heightp, widthp);
	
	double* Gray_data = (double *)imggray_buf;
	array2d *Gray_info = packarray2d(Gray_data, heightp, widthp);
	
	double* Amptitude_data = (double *)magnitude;
	array2d *Amptitude_info = packarray2d(Amptitude_data, height, width);
	
	double* Xita_data = (double *)direction;
	array2d *Xita_info = packarray2d(Xita_data, heightp, widthp);
	
    int numseeds; int finalNumberOfLabels;
    int step = sqrt((double)sz / (double)numSuperpixels) + 0.5;
	point *seedIndices = malloc(sizeof(point) * sz);
    getLABXYSeeds(step, width, height, seedIndices, &numseeds);
	
	labxyf64 *kseeds = malloc(height * width * sizeof(labxyf64));
    labxyf64 ks; point seedp; labf64 labc;
	for(k = 0; k < numseeds; k++) {
		seedp = seedIndices[k];
		labc = imglab[seedp.y*width+seedp.x];
        ks.x = seedp.x;
        ks.y = seedp.y;
        ks.l = labc.l;
        ks.a = labc.a;
        ks.b = labc.b;
		kseeds[k] = ks;
    }
    PerformSuperpixelSLIC(imglab, kseeds,
	                      width, height, numseeds, klabels, step, compactness,
						  MapLAB_info, Amptitude_info, Xita_info, Gray_info,
						  radius, gamma);
	EnforceSuperpixelConnectivity(klabels, width, height, numSuperpixels, clabels, &finalNumberOfLabels);

    free(imglab);
    free(klabels);
    free(seedIndices);
    free(kseeds); 
    free(MapLAB_info);
	free(Gray_info);
	free(Amptitude_info);
	free(Xita_info);

    TSSP_result_t *result = (TSSP_result_t *)malloc(sizeof(TSSP_result_t));
    result->nlabels = finalNumberOfLabels;
    result->labels = clabels;
    return result;
}