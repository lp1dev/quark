#ifndef QUARK_CSS_PROPERTIES_H_
#define QUARK_CSS_PROPERTIES_H_

// typedef struct  CSS_Properties CSS_Properties;

// struct CSS_Properties {
//     char *backgroundColor;
//     char *color;
// };

typedef struct FourDimentional FourDimentional;

struct FourDimentional {
    int top;
    int bot;
    int left;
    int right;
};

#endif