void* new_2d_array(int h, int w, int size);
#define NEW2DARR(H, W, TYPE) (TYPE **)new_2d_array(H, W, sizeof(TYPE))