typedef struct _TSSP_result_t {
    int nlabels;
    int *labels;
} TSSP_result_t;

TSSP_result_t *TSSPmain(int numsuper, size_t h, size_t w,
                        unsigned char *img, double *img_lab, double *img_gray,
                        double *magnitude, double *direction,
                        int radius, int gamma);