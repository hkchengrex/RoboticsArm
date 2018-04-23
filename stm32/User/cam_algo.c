#include "cam_algo.h"
#include "mymath.h"
#include "lcd.h"

void locate_red(u16 *x, u16 *y, u16 size, u32 threshold){
	u32 max_r = 0;
	u16 max_x, max_y;
	for (u16 i=0; i<CAM_WIDTH-size; i++){
		for (u16 j=0; j<CAM_HEIGHT-size; j++){
			u32 r_sum = 0;
			for (u8 sx=0; sx<size; sx++){
				for (u8 sy=0; sy<size; sy++){
					r_sum += CameraData[i+sx][j+sy];
				}
			}
			
			if (r_sum > max_r){
				max_r = r_sum;
				max_x = j+size/2;
				max_y = i+size/2;
			}
		}
	}
	
	if (max_r > threshold){
		*x = max_x;
		*y = max_y;
	}else{
		*x = 0;
		*y = 0;
	}
}

void draw_locator(u16 x, u16 y, u16 min_x, u16 min_y, u16 max_x, u16 max_y){
	LCD_DrawLine(x, min_y, x, max_y, RED);
	LCD_DrawLine(min_x, y, max_x, y, RED);
}
