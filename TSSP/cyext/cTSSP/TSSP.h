#include "fakemex.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

typedef struct _TSSP_result_t {
    int nlabels;
    int *labels;
} TSSP_result_t;

TSSP_result_t *TSSPmain(int numsuper, mwSize h, mwSize w,
                        unsigned char *img, double *img_lab, double *img_gray,
                        double *magnitude, double *direction,
                        int radius, int gamma);