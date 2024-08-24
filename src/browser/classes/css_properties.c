#include <stdlib.h>
#include "css_properties.h"

FourDimentional *FourDimentional_create() {
    FourDimentional *fd;

    fd = malloc(sizeof(FourDimentional));
    fd->top = 0;
    fd->bot = 0;
    fd->left = 0;
    fd->right = 0;
}
