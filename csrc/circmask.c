# define square(name) ((name) * (name))
# define max(a, b) ((a) > (b) ? (a) : (b))
# define min(a, b) ((a) < (b) ? (a) : (b))

struct circmaskset {
	array2d *labels;
    array2d *circle;
	void *maskset;
    double *masksums;
    int maxlabel;
};


// BI stands for Bilinear Interpolation
double getRotatedMaskBI(array2d *mask_info, double alpha, double *outbuf) {
	double theta, px, py, p1, p2, p3, p4, ps, pt, PV, sum;
    int idx, idy;
	int radius = (mask_info->w - 1) / 2;
    int windows = mask_info->w;

	getarray2d(mask_info, double, circle, );
    castarray2d(outbuf, windows, double, outarr, );
	
    sum = 0;
    for (idy = 0; idy < windows; idy++) {
        for (idx = 0; idx < windows; idx++) {
            theta = atan2(idy-radius, idx-radius);
            
            px = radius * (1 + cos(theta - alpha));
            py = radius * (1 + sin(theta - alpha));
            p1 = circle[(int)floor(py)][(int)floor(px)]; 
            p2 = circle[(int)floor(py)][(int)ceil(px) ];  
            p3 = circle[(int)ceil(py) ][(int)floor(px)];   
            p4 = circle[(int)ceil(py) ][(int)ceil(px) ];
            ps = px - floor(px);
            pt = py - floor(py);

            if (p1 == 0 && p2 == 0 && p3 == 0 && p4 == 0) {
                PV = 0;
            } else {
                PV = (1-pt) * (1-ps) * p1
                   + (1-pt) *    ps  * p2
                   +    pt  * (1-ps) * p3
                   +    pt  *    ps  * p4;
            }

            outarr[idy][idx] = PV;
            sum += PV;
        }
    }

    return sum;
}

// defines the precision of SR AntiAlias for the circular mask
// of course, the larger the precision, the more precise the value for pixels at border
// NOTE: unit of the value will be 1/square(PRECISION)
//       for example, when PRECISION is 100, the smallest value will be 0.0001
# define PRECISION 100

void calcAllCircularMasks(point *points2calc, int nlabels, int radius, array2d *circmask_info, void *maskdata, double *masksums) {
    int windows = 2 * radius + 1; int idx, idy, i, j, vsum;

	double dis_inner, dis_outer, xbase, ybase;
	// circmask is full circle
	getarray2d(circmask_info, double, circmask, );
	for (idy = radius; idy < windows; idy++) {
        for (idx = radius; idx < windows; idx++) {
			dis_outer = hypot(idx-radius+0.5, idy-radius+0.5);
			dis_inner = hypot(idx-radius-0.5, idy-radius-0.5);
			       if (dis_outer <= radius) {
				circmask[idy][idx] = 1;
			} else if (dis_inner >= radius) {
				circmask[idy][idx] = 0;
			} else {
				// naive "SR AntiAlias" ?
                // It is slow for sure, but we only do this once for pixels at (quarter) border.
                vsum = 0;
                ybase = idy - 0.5 - radius;
                xbase = idx - 0.5 - radius;

                for (i = 1; i < PRECISION+1; i++) {
                    for (j = 1; j < PRECISION+1; j++) {
                        if (hypot(ybase+(double)(i)/PRECISION, xbase+(double)(j)/PRECISION) <= radius) { vsum ++; }
                    }
                }
                circmask[idy][idx] = (double)(vsum)/square(PRECISION);
			}
        }
    }
    // mirroring
	for (idy = 0; idy < radius; idy++) { for (idx = radius; idx < windows; idx++) { circmask[idy][idx] = circmask[2*radius-idy][idx]; } }
	for (idy = radius; idy < windows; idy++) { for (idx = 0; idx < radius; idx++) { circmask[idy][idx] = circmask[idy][2*radius-idx]; } }
	for (idy = 0; idy < radius; idy++) { for (idx = 0; idx < radius; idx++) { circmask[idy][idx] = circmask[2*radius-idy][2*radius-idx]; } }
	
	// circle is quarter circle
	array2d *circle_info = callocarray2d(windows, windows, sizeof(double));
	getarray2d(circle_info, double, circle, );
	for (idy = 0; idy < windows; idy++) {
        for (idx = 0; idx < windows; idx++) {
			if (circmask[idy][idx] > 0 && ((idy >= idx) && (idy + idx <= windows))) {
				circle[idy][idx] = circmask[idy][idx];
			}
        }
    }

    // uncomment following to see how the circular mask really look like ;)

    //for (idy = 0; idy < windows; idy++) {
    //    for (idx = 0; idx < windows; idx++) {
    //        if (circmask[idy][idx] == 0) {
    //            printf("%8d", 0);
    //        } else {
    //            printf("%4f", circmask[idy][idx]);
    //        }
    //        printf("  ");
    //    }
    //    printf("\n");
    //}

    //for (idy = 0; idy < windows; idy++) {
    //    for (idx = 0; idx < windows; idx++) {
    //        if (circle[idy][idx] == 0) {
    //            printf("%8d", 0);
    //        } else {
    //            printf("%4f", circle[idy][idx]);
    //        }
    //        printf("  ");
    //    }
    //    printf("\n");
    //}

    point p; double *maskbuf; double alpha;
    double (*maskset)[radius][radius] = (double (*)[radius][radius])maskdata;

    for (i = 0; i < nlabels; i++) {
        p = points2calc[i];
        maskbuf = (double *)malloc(square(windows) * sizeof(double));
        alpha = atan2(p.y, p.x);
        masksums[i] = getRotatedMaskBI(circle_info, alpha, (double *)(maskset[i]));
    }
}


# define SET(COMMAND) \
for (i = 0; i < goahead; i++) {\
	COMMAND;\
	if (masklabels[offset+idy][offset+idx] == 0) {\
		labelv++;\
        p.y = idy; p.x = idx; points2calc[labelv] = p;\
        j = 1;\
		while ((idx * j >= -offset) && (idx * j <= offset) \
            && (idy * j >= -offset) && (idy * j <= offset)) {\
			masklabels[offset+idy*j][offset+idx*j] = labelv;\
            j++;\
		}\
	}\
}
# define TURN direction = direction ^ 1;

struct circmaskset * getAllCircularMasks(int offset, int radius) {
	int sppsize = 2 * offset + 1;
	int circsize = 2 * radius + 1;
	
    array2d *masklabels_info = callocarray2d(sppsize, sppsize, sizeof(int));
	getarray2d(masklabels_info, int, masklabels, );
	
    int idx = 0; int idy = 0;
	int goahead, i, j; int labelv = 0;
	bool direction = 1;

    // don't know how much points in the list now, just allocating a size that is sure to be enough
    point *points2calc = (point *)malloc(square(sppsize) * sizeof(point));
    point p; p.y = 0; p.x = 0; points2calc[0] = p;
	
    // we lookup the 2D array spirally starting from center, sorting all points into different labels
    // one label value stands for a different atan2 value ;)
    // also, the first (nearest to center) point of a label value is put into corresponding position of points2calc
    for (goahead = 1; goahead < sppsize; goahead++) {
		if (direction) { SET(idx++) SET(idy++) }
                  else { SET(idx--) SET(idy--) }
		TURN
	} SET(idx++)

    // free unused extra space
    points2calc = (point *)realloc(points2calc, (labelv+1) * sizeof(point));

	array2d *circmask_info = mallocarray2d(circsize, circsize, sizeof(double));
    void *maskdata = malloc((labelv+1) * circsize * circsize * sizeof(double));
    double *masksums = (double *)malloc((labelv+1) * sizeof(double));
    calcAllCircularMasks(points2calc, labelv+1, radius, circmask_info, maskdata, masksums);

    struct circmaskset *ret = (struct circmaskset *)malloc(sizeof(struct circmaskset));
    ret->labels = masklabels_info;
    ret->circle = circmask_info;
    ret->maskset = maskdata;
    ret->masksums = masksums;
    ret->maxlabel = labelv;

    return ret;
}

# undef SET
# undef TURN