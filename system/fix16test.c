#include "fix16.h"
#include <xinu.h>

void fix16test(){

    fix16_t a = fix16_div(fix16_from_int(1),fix16_from_int(60));
    fix16_t b = fix16_div(fix16_from_int(1),fix16_from_int(50));
    
    printf("\n a>b is %d \n", a>b);
    printf("\n a<b is %d \n", a<b);           

    fix16_t c = fix16_mul(a,fix16_pi);
    
    char output[128];
    fix16_to_str(fix16_pi,output,4);
    printf("\n pi is %s \n",output);

    fix16_to_str(a,output,4);
    printf("\n a is %s \n",output);

    fix16_to_str(c,output,4);
    printf("\n output of 1/60*pi is %s \n",output);

    return;

}
