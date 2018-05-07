#include <stdio.h>

#include "test.h"

#define MAX_RED_TO_BE_FOUND 7
#define SEARCH_RED_STEP 2
#define DIST_THRE 30

#define s8 int8_t
#define s16 int16_t
#define s32 int32_t
#define s64 int64_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define ABS(x) ((x)<0?(-(x)):(x))
#define SQR(x) ((x)*(x))
#define SIGN(x) ((x)<0?(-1):(1))
#define CAP(a, b, c) ((a)<(b)?(b):((a)>(c)?(c):(a)))

#define MAN_DIST(p1, p2) (ABS(p1.x-p2.x) + ABS(p1.y-p2.y))
#define DIST(p1, p2) (Sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y)) / 1024)

/**
  * @brief  Rapid sqrt approximation with maximum 0.297944% deviation at sqrt(2) and average 0.0184811% deviation
  * @param  v:	Input limited to 1125899906840000 (2^50)
  * @retval Scaled value of 1024*sqrt(v) limiting input to ~2^48
  */
u32 Sqrt(s64 v){
	union
	{
		u32 tmp;
		float f;
	} u;

	v = v < 0 ? -v : v;
	u.f = v;
	u.tmp = (u32)(0x233b4000 + (u.tmp >> 1));
	u.tmp = (u32)u.f;
	u.tmp = ((u64)u.tmp + v*16384/u.tmp + 1)/2;
	//Second iteration to reduce error to within 0.000000559339% but halves speed
	//u.tmp = ((u64)u.tmp + v*16384/u.tmp + 1)/2;	
	return u.tmp * 8;
}


typedef struct{
    s16 x;
    s16 y;
} Point;

Point red_located[MAX_RED_TO_BE_FOUND] = {0};
u8 cur_red_located_pt = 0;

int main(int argc, char* argv[]){
    printf("Hello!\n");

    for (u8 y=0; y<IMAGE_HEIGHT; y += SEARCH_RED_STEP){
        for (u8 x=0; x<IMAGE_WIDTH; x += SEARCH_RED_STEP){
            if (im[y][x] & 1){
                //Is red
                if (cur_red_located_pt < MAX_RED_TO_BE_FOUND){
                    //Check for closeness, reject too close
                    u8 passed = 1;
                    Point p;
                    p.x = x;
                    p.y = y;
                    for (int i=0; i<cur_red_located_pt; i++){
                        if (DIST(p, red_located[i]) < DIST_THRE){
                            passed = 0;
                            break;
                        }
                    }
                    if (passed){
                        red_located[cur_red_located_pt++] = p;
                        printf("Man passed %d %d\n", x, y);
                    }else{
                       // printf("Man not passed\n");
                    }
                }else{
                    //printf("Red limit reached\n");
                }
            }
        }
    }

    //Looks for the corner one
    for (u8 i=0; i<cur_red_located_pt; i++){
        //Pick every point and check it against pairs of points, looking for same distance
    }


    return 0;
}
