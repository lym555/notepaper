#include "ImageData.h"

const unsigned char gImage_disconnect[32] = { /* 0X01,0X01,0X10,0X00,0X10,0X00, */
0XFF,0XFF,0XF9,0XFF,0XF9,0XFF,0XF3,0XFF,0XF3,0X3F,0XE6,0X3F,0XFE,0X7F,0XC0,0X33,
0XC0,0X33,0XFF,0X7F,0XE6,0X3F,0XF3,0X3F,0XF3,0XFF,0XF9,0XFF,0XF9,0XFF,0XFF,0XFF,
};

const unsigned char gImage_connect[32] = { /* 0X01,0X01,0X10,0X00,0X10,0X00, */
0XFC,0XFF,0XF9,0XFF,0XF3,0XBF,0XE3,0X3F,0XE6,0X7F,0XE6,0X7F,0XCC,0XE7,0XCC,0XC7,
0XCC,0XC7,0XCC,0XE7,0XE6,0X7F,0XE6,0X7F,0XE3,0X3F,0XF3,0XBF,0XF9,0XFF,0XFC,0XFF,
};

const unsigned char gImage_battery_empty[32] = { /* 0X01,0X01,0X10,0X00,0X10,0X00, */
0XFF,0XFF,0XF0,0X0F,0XF7,0XEF,0XF7,0XEF,0XF7,0XEF,0XF7,0XEF,0XF7,0XEF,0XF7,0XEF,
0XF7,0XEF,0XF7,0XEF,0XF7,0XEF,0XF7,0XEF,0XF7,0XEF,0XF7,0XEF,0XF0,0X0F,0XFC,0X3F,
};

const unsigned char gImage_battery_half[32] = { /* 0X01,0X01,0X10,0X00,0X10,0X00, */
0XFF,0XFF,0XF0,0X0F,0XF0,0X0F,0XF0,0X0F,0XF0,0X0F,0XF0,0X0F,0XF0,0X0F,0XF0,0X0F,
0XF7,0XEF,0XF7,0XEF,0XF7,0XEF,0XF7,0XEF,0XF7,0XEF,0XF7,0XEF,0XF0,0X0F,0XFC,0X3F,
};


const unsigned char gImage_battery_full[32] = { /* 0X01,0X01,0X10,0X00,0X10,0X00, */
0XFF,0XFF,0XF0,0X0F,0XF0,0X0F,0XF0,0X0F,0XF0,0X0F,0XF0,0X0F,0XF0,0X0F,0XF0,0X0F,
0XF0,0X0F,0XF0,0X0F,0XF0,0X0F,0XF0,0X0F,0XF0,0X0F,0XF0,0X0F,0XF0,0X0F,0XFC,0X3F,
};


const unsigned char gImage_battery_charging[32] = { /* 0X81,0X01,0X10,0X00,0X10,0X00, */
0XFF,0XFF,0XF0,0X0F,0XF7,0XEF,0XF7,0XEF,0XF7,0XEF,0XF7,0X6F,0XF7,0X6F,0XF7,0X6F,
0XF4,0X2F,0XF6,0XEF,0XF6,0XEF,0XF7,0XEF,0XF7,0XEF,0XF7,0XEF,0XF0,0X0F,0XFC,0X3F,
};


const unsigned char gImage_clear[128] = { /* 0X81,0X01,0X20,0X00,0X20,0X00, */
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0X7F,0XFF,0XFF,0XFE,0X7F,0XFF,
0XFF,0XFE,0X7F,0XFF,0XFF,0XFE,0X7F,0XFF,0XFC,0XFF,0XFF,0X3F,0XFC,0X7F,0XFE,0X3F,
0XFE,0X70,0X0E,0X7F,0XFF,0XE3,0XC3,0XFF,0XFF,0XCF,0XF1,0XFF,0XFF,0X9F,0XF9,0XFF,
0XFF,0X3F,0XFC,0XFF,0XFF,0X3F,0XFC,0XFF,0XFF,0X7F,0XFE,0XFF,0XC3,0X7F,0XFE,0X43,
0XC3,0X7F,0XFE,0X43,0XFF,0X7F,0XFE,0X7F,0XFF,0X3F,0XFE,0XFF,0XFF,0X3F,0XFC,0XFF,
0XFF,0X9F,0XF9,0XFF,0XFF,0X8F,0XF1,0XFF,0XFF,0XC3,0XC3,0XFF,0XFE,0X70,0X0E,0X7F,
0XFC,0XFC,0X3E,0X3F,0XFC,0XFF,0XFF,0X3F,0XFF,0XFE,0X7F,0XFF,0XFF,0XFE,0X7F,0XFF,
0XFF,0XFE,0X7F,0XFF,0XFF,0XFE,0X7F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
};