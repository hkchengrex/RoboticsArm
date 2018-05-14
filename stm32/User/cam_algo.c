#include "cam_algo.h"
#include "mymath.h"
#include "lcd.h"

#define START_X 30
#define START_Y 10
#define END_X 90
#define END_Y 90

Point double_recur_center(const Point p, u8 thre, u8* width, u8* height){
    //First recur on y line
    Point np;
    np.x = p.x;
    np.y = p.y;
    
    s16 tmp_max_y = np.y;
    while(tmp_max_y < CAM_HEIGHT && (CameraData[tmp_max_y][np.x] > thre)){
        tmp_max_y++;
    }
    while(np.y >= 0 && (CameraData[np.y][np.x] > thre)){
        np.y--;
    }
		*height = ABS(np.y - tmp_max_y);
    np.y = (np.y + tmp_max_y) / 2;

    s16 tmp_max_x = np.x;
    while(tmp_max_x < CAM_WIDTH && (CameraData[np.y][tmp_max_x] > thre)){
        tmp_max_x++;
    }
    while(np.x >= 0 && (CameraData[np.y][np.x] > thre)){
        np.x--;
    }
		*width = ABS(np.x - tmp_max_x);
    np.x = (tmp_max_x + np.x) / 2;

    return np;
}

s32 closeness_in_h(s32 cur, s32 tar){
	s32 dif = cur - tar;
	while(dif > 128) dif -= 128;
	while(dif < -128) dif += 128;
	return 128 - ABS(dif);
}

//void locate_rgb(s16 *rx, s16 *ry, s16 *gx, s16 *gy, s16 *bx, s16 *by, u16 size, u32 threshold){
//	u32 max_r = 0, max_g=0, max_b=0;
//	
//	u16 max_rx, max_ry;
//	u16 max_gx, max_gy;
//	u16 max_bx, max_by;
//	
//	for (u16 i=START_Y; i<END_Y; i++){
//		for (u16 j=START_X; j<END_X; j++){
//			u32 r_sum=0, g_sum=0, b_sum=0;
//			for (u8 sx=0; sx<size; sx++){
//				for (u8 sy=0; sy<size; sy++){
//					if (CameraData[i+sx][j+sy].s > 80 && CameraData[i+sx][j+sy].v > 80){
//						r_sum += closeness_in_h(CameraData[i+sx][j+sy].h, 0);
//						g_sum += closeness_in_h(CameraData[i+sx][j+sy].h, 85);
//						b_sum += closeness_in_h(CameraData[i+sx][j+sy].h, 171);
//					}
//				}
//			}
//			
//			if (r_sum > max_r){
//				max_r = r_sum;
//				max_rx = j+size/2;
//				max_ry = i+size/2;
//			}
//			
//			if (g_sum > max_g){
//				max_g = g_sum;
//				max_gx = j+size/2;
//				max_gy = i+size/2;
//			}
//			
//			if (b_sum > max_b){
//				max_b = b_sum;
//				max_bx = j+size/2;
//				max_by = i+size/2;
//			}
//		}
//	}
//	
//	if (max_r > threshold){
//		*rx = max_rx;
//		*ry = max_ry;
//	}else{
//		*rx = 0;
//		*ry = 0;
//	}
//	
//	if (max_g > threshold){
//		*gx = max_gx;
//		*gy = max_gy;
//	}else{
//		*gx = 0;
//		*gy = 0;
//	}
//	
//	if (max_b > threshold){
//		*bx = max_bx;
//		*by = max_by;
//	}else{
//		*bx = 0;
//		*by = 0;
//	}
//}

void locate_rgb(s16 *rx, s16 *ry, s16 *gx, s16 *gy, s16 *bx, s16 *by, u16 size, u32 threshold){
	u32 max_r = 0, max_g=0, max_b=0;
	
	u16 max_rx, max_ry;
	u16 max_gx, max_gy;
	u16 max_bx, max_by;
	
	for (u16 i=START_Y; i<END_Y; i++){
		for (u16 j=START_X; j<END_X; j++){
			u32 r_sum=0, g_sum=0, b_sum=0;
			for (u8 sx=0; sx<size; sx++){
				for (u8 sy=0; sy<size; sy++){
					r_sum += (CameraData[i+sx][j+sy] & 0x1F);
				}
			}
			
			if (r_sum > max_r){
				max_r = r_sum;
				max_rx = j+size/2;
				max_ry = i+size/2;
			}
			
			if (g_sum > max_g){
				max_g = g_sum;
				max_gx = j+size/2;
				max_gy = i+size/2;
			}
			
			if (b_sum > max_b){
				max_b = b_sum;
				max_bx = j+size/2;
				max_by = i+size/2;
			}
		}
	}
	
	if (max_r > threshold){
		*rx = max_rx;
		*ry = max_ry;
	}else{
		*rx = 0;
		*ry = 0;
	}
}

void draw_locator(u16 x, u16 y, u16 min_x, u16 min_y, u16 max_x, u16 max_y, u16 color){
	LCD_DrawLine(x, min_y, x, max_y, color);
	LCD_DrawLine(min_x, y, max_x, y, color);
}

//Point red_located[MAX_RED_TO_BE_FOUND] = {0};
//u8 cur_red_located_pt = 0;
//s32 rel_dist[MAX_RED_TO_BE_FOUND][MAX_RED_TO_BE_FOUND] = {-1};
//s32 found_data = 0;
//u8 data_array[25] = {0};
//Point point_c, point_1, point_2;

//void smooth_red(){
//	for (u8 y=1; y<CAM_HEIGHT-1; y++){
//		for (u8 x=1; x<CAM_WIDTH-1; x++){
//			u8 nei_ok = (CameraData[y-1][x-1] & 1) + (CameraData[y-1][x] & 1) + (CameraData[y-1][x+1] & 1)
//			+ (CameraData[y][x-1] & 1) + (CameraData[y][x] & 1) + (CameraData[y][x+1] & 1)
//			+ (CameraData[y+1][x-1] & 1) + (CameraData[y+1][x] & 1) + (CameraData[y+1][x+1] & 1);
//			
//			CameraData[y][x] &= ((nei_ok > 7) | (~(1)));
//		}
//	}
//}

//void find_pattern(){
//	cur_red_located_pt = 0;
//	for (u8 i=0; i<MAX_RED_TO_BE_FOUND; i++){
//		for (u8 j=0; j<MAX_RED_TO_BE_FOUND; j++){
//			rel_dist[i][j] = -1;
//		}
//	}
//	
//	for (u8 y=0; y<CAM_HEIGHT; y += SEARCH_RED_STEP){
//        for (u8 x=0; x<CAM_WIDTH; x += SEARCH_RED_STEP){
//            if (CameraData[y][x] & 1){
//                //Is red
//                if (cur_red_located_pt < MAX_RED_TO_BE_FOUND){
//                    //Check for closeness, reject too close
//                    u8 passed = 1;
//                    Point p;
//                    p.x = x;
//                    p.y = y;
//                    for (int i=0; i<cur_red_located_pt; i++){
//                        s32 dist = DIST_TIMES_16(p, red_located[i]);
//                        if (dist/16 < DIST_THRE){
//                            passed = 0;
//                            break;
//                        }else{
//                            //Temp variables, save computation later
//                            rel_dist[i][cur_red_located_pt] = dist;
//                            rel_dist[cur_red_located_pt][i] = dist;
//                        }
//                    }
//                    if (passed){
//                        red_located[cur_red_located_pt++] = p;
//                        //printf("Man passed %d %d\n", x, y);
//                    }else{
//                       // printf("Man not passed\n");
//                    }
//                }else{
//                    //printf("Red lCameraDatait reached\n");
//                }
//            }
//        }
//    }

//    //Looks for the corner one
//    for (u8 i=0; i<cur_red_located_pt; i++){
//        //Pick every point and check it against pairs of points, looking for same distance
//        //First update it's relative distance matrix
//        for (u8 j=0; j<cur_red_located_pt; j++){
//            if (i != j && rel_dist[i][j] == -1){
//                //Update value
//                rel_dist[i][j] = DIST_TIMES_16(red_located[i], red_located[j]);
//                rel_dist[j][i] = rel_dist[i][j];
//            }
//        }
//    }

//    //Check distance
//    u8 center_found = 0;
//    u8 pc=0, p1=0, p2=0;
//    for (u8 i=0; i<cur_red_located_pt; i++){
//        for (u8 j=0; j<cur_red_located_pt; j++){
//            if (i == j) continue;
//            for (u8 k=0; k<cur_red_located_pt; k++){
//                if (k==i || k==j) continue;
//                if (ABS(rel_dist[i][k] - rel_dist[i][j])/16 < CENTER_DIST_DIFF_THRE){
//                    center_found = 1;
//                    pc = i;
//                    if ((red_located[j].x - red_located[pc].x)*(red_located[k].x - red_located[pc].x) + 
//                    (red_located[j].y -  - red_located[pc].y)*(red_located[k].y -  - red_located[pc].y) < 0){
//                        p2 = j;
//                        p1 = k;
//                    }else{
//                        p1 = j;
//                        p2 = k;
//                    }
//                    break;
//                }
//            }
//        }
//    }

//    s32 data = 0;
//    s32 one_at_front = 1 << (NUM_OF_GRIDS*NUM_OF_GRIDS-3);
//    if (center_found){
//        //printf("Center found: %d %d %d\n", pc, p1, p2);
//        Point p1_min_pc;
//        p1_min_pc.x = red_located[p1].x - red_located[pc].x;
//        p1_min_pc.y = red_located[p1].y - red_located[pc].y;
//        Point p2_min_pc;
//        p2_min_pc.x = red_located[p2].x - red_located[pc].x;
//        p2_min_pc.y = red_located[p2].y - red_located[pc].y;

//				u8 i = 0;
//        //Step through to obtain data
//        for (u8 yIdx=0; yIdx<NUM_OF_GRIDS; yIdx++){
//            for (u8 xIdx=0; xIdx<NUM_OF_GRIDS; xIdx++){
//                if (yIdx == 0 && (xIdx == 0 || xIdx == NUM_OF_GRIDS - 1)) continue;
//                if (xIdx == NUM_OF_GRIDS - 1 && yIdx == NUM_OF_GRIDS - 1) continue;

//                Point look;
//                look.x = red_located[pc].x + p1_min_pc.x * xIdx / (NUM_OF_GRIDS-1) + p2_min_pc.x * yIdx / (NUM_OF_GRIDS-1);
//                look.y = red_located[pc].y + p1_min_pc.y * xIdx / (NUM_OF_GRIDS-1) + p2_min_pc.y * yIdx / (NUM_OF_GRIDS-1);
//                //Determine White/Black
//                //printf("Look: %d %d\n", look.x, look.y);
//                if (CameraData[look.y][look.x] & 2){
//                    data += one_at_front;
//										data_array[i] = 1;
//                    //printf("1");
//                }else{
//										data_array[i] = 0;
//                    //printf("0");
//                }
//                data >>= 1;
//								i++;
//            }
//        }
//				found_data = data;
//        //printf("\nData: %d", data);
//				
//				point_c = red_located[pc];
//				point_1 = red_located[p1];
//				point_2 = red_located[p2];
//    }else{
//        //printf("Center not found\n");
//    }
//}

