// /**
//  * @file timer.c
//  * @author your name (you@domain.com)
//  * @brief 
//  * @version 0.1
//  * @date 2023-12-14
//  * 
//  * @copyright Copyright (c) 2023
//  * 
//  */

// #include "timer.h"

// uint8_t minutes = 0;
// uint8_t seconds = 10;
// uint16_t color = LCD_COLOR_CYAN;
// extern bool game_over;
// extern bool in_game;

// extern void task_uart_recieve();
// extern void task_uart_send();

// TaskHandle_t TaskHandle_timer;

// void project_timer_init(){
// /* Create the Cap Touch Task */
//     xTaskCreate(task_timer,
//                 "Timer task",
//                 configMINIMAL_STACK_SIZE,
//                 NULL,
//                 2,
//                 &TaskHandle_timer
//     );
// }

// void task_timer(void *pvParameters)
// {
//     ulTaskNotifyTake(true, portMAX_DELAY);
//     TickType_t xLastWakeTime;
//     const TickType_t xFrequency = 1000; // 1 second tick

//     xLastWakeTime = xTaskGetTickCount();
//     while (seconds > 0) {
//         vTaskDelayUntil(&xLastWakeTime, xFrequency);
//         seconds--;
//         draw_timer();
//     }
//     if (seconds == 0) {
//         game_over = true;
//         in_game = false;
//         task_uart_send();
//     }
//     vTaskDelete(NULL);
// }

// void draw_timer()
// {
//     uint8_t minutes_msb = minutes / 10;
//     uint8_t minutes_lsb = minutes % 10;
//     uint8_t seconds_msb = seconds / 10;
//     uint8_t seconds_lsb = seconds % 10;

//     uint8_t *minutes_msb_bitmap = (uint8_t *)&brussels_24ptBitmaps[brussels_24ptOffsets[minutes_msb]];
//     uint8_t *minutes_lsb_bitmap = (uint8_t *)&brussels_24ptBitmaps[brussels_24ptOffsets[minutes_lsb]];
//     uint8_t *colon_bitmap = (uint8_t *)&brussels_24ptBitmaps[brussels_24ptOffsets[10]];
//     uint8_t *seconds_msb_bitmap = (uint8_t *)&brussels_24ptBitmaps[brussels_24ptOffsets[seconds_msb]];
//     uint8_t *seconds_lsb_bitmap = (uint8_t *)&brussels_24ptBitmaps[brussels_24ptOffsets[seconds_lsb]];
    
//     lcd_draw_image(
//         COORD_ALARM_X_SEC_MSB,
//         COORD_ALARM_Y,
//         FONT_ALARM_WIDTH,
//         FONT_ALARM_HEIGHT,
//         seconds_msb_bitmap,
//         color,
//         LCD_COLOR_BLACK
//     );

//     lcd_draw_image(
//         COORD_ALARM_X_SEC_LSB,
//         COORD_ALARM_Y,
//         FONT_ALARM_WIDTH,
//         FONT_ALARM_HEIGHT,
//         seconds_lsb_bitmap,
//         color,
//         LCD_COLOR_BLACK
//     );
// }