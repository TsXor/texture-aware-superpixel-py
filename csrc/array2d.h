// naming:
// name the array2d struct xxx_info
// name the real casted 2d array xxx

# include <stdio.h>
# include <stdlib.h>

# define getarray2d(obj, type, dst, moredim) type (* dst)[obj->w]moredim = (type (*)[obj->w]moredim)(obj->data)
# define castarray2d(bufptr, w, type, dst, moredim) type (* dst)[w]moredim = (type (*)[w]moredim)(bufptr)
# define declarray2d(w, type, dst, moredim) type (* dst)[w]moredim
# define castarray2d_nodecl(bufptr, w, type, dst, moredim) dst = (type (*)[w]moredim)(bufptr)

typedef struct _array2d {
	void *data;
	size_t h;
	size_t w;
} array2d;

array2d * mallocarray2d(size_t h, size_t w, size_t esize) {
	array2d *arr = (array2d *)malloc(sizeof(array2d));
	void *data = malloc(h * w * esize);
	arr->data = data;
	arr->h = h; arr->w = w;
	return arr;
}

array2d * callocarray2d(size_t h, size_t w, size_t esize) {
	array2d *arr = (array2d *)malloc(sizeof(array2d));
	void *data = calloc(h * w, esize);
	arr->data = data;
	arr->h = h; arr->w = w;
	return arr;
}

array2d * packarray2d(void *data, size_t h, size_t w) {
	array2d *arr = (array2d *)malloc(sizeof(array2d));
	arr->data = data;
	arr->h = h; arr->w = w;
	return arr;
}

void freearray2d(array2d *arr) {
	free(arr->data); free(arr);
}

//int main() {
//	array2d *arr = mallocarray2d(3, 4, sizeof(int));
//	printarray2d(arr);
//	freearray2d(arr);
//}