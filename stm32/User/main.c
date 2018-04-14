#include "stm32f10x.h"
#include "lcd.h"
#include "math.h"

#include "ticks.h"

int main(void)
{
  LCD_INIT();
	
	ticks_init();
	
  while (1) {
		u32 this_ticks = get_ticks();
		
		LCD_Clear();
		LCD_Printf(0, 0, "Hello %d", this_ticks);
		LCD_Update();
  }
}
