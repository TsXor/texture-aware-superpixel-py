// Can partially simulate the functions in mex.h of matlab.

# ifndef __fakemex__
# define __fakemex__

# include <stdlib.h>
# include <stdbool.h>

# define mxMalloc  malloc
# define mxCalloc  calloc
# define mxRealloc realloc
# define mxFree    free

// Things on complex are not implemented...

typedef size_t mwSize;

typedef enum {
    mxDOUBLE_CLASS =  (1 << 4) + sizeof(double),
    mxSINGLE_CLASS =  (2 << 4) + sizeof(float),
    mxINT8_CLASS   =  (3 << 4) + sizeof(char),
    mxUINT8_CLASS  =  (4 << 4) + sizeof(unsigned char),
    mxINT16_CLASS  =  (5 << 4) + sizeof(short),
    mxUINT16_CLASS =  (6 << 4) + sizeof(unsigned short),
    mxINT32_CLASS  =  (7 << 4) + sizeof(long),
    mxUINT32_CLASS =  (8 << 4) + sizeof(unsigned long),
    mxINT64_CLASS  =  (9 << 4) + sizeof(long long),
    mxUINT64_CLASS = (10 << 4) + sizeof(unsigned long long)
} mxClassID;

typedef enum {mxREAL=0, mxCOMPLEX} mxComplexity;

typedef struct _mxArray {
    mxClassID classid;
    mxComplexity isreal;
    mwSize ndims;
    mwSize *shape;
    void *data;
} mxArray;

# define RET_FIRST(TYPE) return (double)((( TYPE *)mxObject->data)[0]);

static double mxGetScalar(const mxArray *mxObject){
    switch(mxObject->classid){
        case mxDOUBLE_CLASS: RET_FIRST(double            )
        case mxSINGLE_CLASS: RET_FIRST(float             )
        case mxINT8_CLASS  : RET_FIRST(char              )
        case mxUINT8_CLASS : RET_FIRST(unsigned char     )
        case mxINT16_CLASS : RET_FIRST(short             )
        case mxUINT16_CLASS: RET_FIRST(unsigned short    )
        case mxINT32_CLASS : RET_FIRST(long              )
        case mxUINT32_CLASS: RET_FIRST(unsigned long     )
        case mxINT64_CLASS : RET_FIRST(long long         )
        case mxUINT64_CLASS: RET_FIRST(unsigned long long)
        default: return 0;
    };
}

# undef RET_FIRST

static mwSize mxGetNumberOfDimensions(const mxArray *mxObject){
    return mxObject->ndims;
}

static mwSize * mxGetDimensions(const mxArray *mxObject){
    return mxObject->shape;
}

static void * mxGetData(const mxArray *mxObject){
    return mxObject->data;
}

static mxArray *mxCreateNumericMatrix (mwSize h, mwSize w, mxClassID type, mxComplexity reality){
    mxArray *mxObject = (mxArray *)mxMalloc(sizeof(mxArray));
    void *matrixData = mxCalloc(h * w, (type & 0b1111));
    mwSize *mshape = (mwSize *)mxMalloc(2 * sizeof(mwSize));
    mshape[0] = h; mshape[1] = w;
    mxObject->classid = type;
    mxObject->isreal = reality;
    mxObject->ndims = 2;
    mxObject->shape = mshape;
    mxObject->data = matrixData;
    return mxObject;
}

# endif