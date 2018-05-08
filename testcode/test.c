#include <stdio.h>

#include "test.h"

#define MAX_RED_TO_BE_FOUND 7
#define SEARCH_RED_STEP 2
#define DIST_THRE 30

#define CENTER_DIST_DIFF_THRE 20
#define NUM_OF_GRIDS 5

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
#define DIST_TIMES_16(p1, p2) (Sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y)) / 64)

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
s32 rel_dist[MAX_RED_TO_BE_FOUND][MAX_RED_TO_BE_FOUND] = {-1};

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
                        s32 dist = DIST_TIMES_16(p, red_located[i]);
                        if (dist/16 < DIST_THRE){
                            passed = 0;
                            break;
                        }else{
                            //Temp variables, save computation later
                            rel_dist[i][cur_red_located_pt] = dist;
                            rel_dist[cur_red_located_pt][i] = dist;
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
        //First update it's relative distance matrix
        for (u8 j=0; j<cur_red_located_pt; j++){
            if (i != j && rel_dist[i][j] == -1){
                //Update value
                rel_dist[i][j] = DIST_TIMES_16(red_located[i], red_located[j]);
                rel_dist[j][i] = rel_dist[i][j];
            }
        }
    }

    //Check distance
    u8 center_found = 0;
    u8 pc=0, p1=0, p2=0;
    for (u8 i=0; i<cur_red_located_pt; i++){
        for (u8 j=0; j<cur_red_located_pt; j++){
            if (i == j) continue;
            for (u8 k=0; k<cur_red_located_pt; k++){
                if (k==i || k==j) continue;
                if (ABS(rel_dist[i][k] - rel_dist[i][j])/16 < CENTER_DIST_DIFF_THRE){
                    center_found = 1;
                    pc = i;
                    p2 = j;
                    p1 = k;
                    break;
                }
            }
        }
    }

    s32 data = 0;
    s32 one_at_front = 1 << (NUM_OF_GRIDS*NUM_OF_GRIDS-3);
    if (center_found){
        printf("Center found: %d %d %d\n", pc, p1, p2);
        Point p1_min_pc;
        p1_min_pc.x = red_located[p1].x - red_located[pc].x;
        p1_min_pc.y = red_located[p1].y - red_located[pc].y;
        Point p2_min_pc;
        p2_min_pc.x = red_located[p2].x - red_located[pc].x;
        p2_min_pc.y = red_located[p2].y - red_located[pc].y;

        //Step through to obtain data
        for (u8 yIdx=0; yIdx<NUM_OF_GRIDS; yIdx++){
            for (u8 xIdx=0; xIdx<NUM_OF_GRIDS; xIdx++){
                if (yIdx == 0 && (xIdx == 0 || xIdx == NUM_OF_GRIDS - 1)) continue;
                if (xIdx == NUM_OF_GRIDS - 1 && yIdx == NUM_OF_GRIDS - 1) continue;

                Point look;
                look.x = red_located[pc].x + p1_min_pc.x * xIdx / (NUM_OF_GRIDS-1) + p2_min_pc.x * yIdx / (NUM_OF_GRIDS-1);
                look.y = red_located[pc].y + p1_min_pc.y * xIdx / (NUM_OF_GRIDS-1) + p2_min_pc.y * yIdx / (NUM_OF_GRIDS-1);
                //Determine White/Black
                printf("Look: %d %d\n", look.x, look.y);
                if (im[look.y][look.x] & 2){
                    data += one_at_front;
                    printf("1");
                }else{
                    printf("0");
                }
                data >>= 1;
            }
        }
        printf("\nData: %d", data);
    }else{
        printf("Center not found\n");
    }


    return 0;
}
