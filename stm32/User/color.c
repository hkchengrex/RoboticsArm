#include "color.h"

inline ColorTuple rgb_to_hsv(u8 r, u8 g, u8 b){
	ColorTuple hsv;
	
	u8 cmin = r < g ? (r < b ? r : b) : (g < b ? g : b);
	u8 cmax = r > g ? (r > b ? r : b) : (g > b ? g : b);

	if (cmax == 0){
		hsv.v = 0;
		hsv.h = 0;
		hsv.s = 0;
		return hsv;
	}else{
		hsv.v = cmax;
	}

	hsv.s = (s32)255 * ((s32)cmax - (s32)cmin) / hsv.v;
	if (hsv.s == 0) {
		hsv.h = 0;
		return hsv;
	}

	if (cmax == r){
		hsv.h = 0 + 43 * (g - b) / (cmax - cmin);
	} else if (cmax == g){
		hsv.h = 85 + 43 * (b - r) / (cmax - cmin);
	}	else {
		hsv.h = 171 + 43 * (r - g) / (cmax - cmin);
	}

	return hsv;
}
