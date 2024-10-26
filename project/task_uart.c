// /**
//  * @file task_uart.c
//  * @author your name (you@domain.com)
//  * @brief 
//  * @version 0.1
//  * @date 2023-12-12
//  * 
//  * @copyright Copyright (c) 2023
//  * 
//  */

// #include "task_uart.h"

// TaskHandle_t TaskHandle_recieve;
// TaskHandle_t TaskHandle_send;

// QueueHandle_t QueueHandle_ready;

// uint8_t remote_rx_message[80];
// extern cyhal_uart_t cy_retarget_io_uart_obj;
// extern TaskHandle_t TaskHandle_CapTouch;
// extern cap_touch_irq_enable();
// extern TaskHandle_t TaskHandle_timer;

// extern bool in_color_select;
// extern bool in_game;
// extern bool in_ready;
// extern bool in_main_menu;
// extern bool game_over;
// extern int board[];
// extern int high_score;
// extern int score;
// extern bool im_ready;
// extern int hammer_color;

// void task_uart_recieve() {
//     while(game_over) {
//         if (remote_uart_rx_string_polling(remote_rx_message)) {
//             if(atoi(remote_rx_message) > score) {
//                 eeprom_write_enable();
//                 eeprom_write_byte(0x0000, atoi(remote_rx_message));
//                 eeprom_wait_for_write();
//                 eeprom_write_disable();
//                 lcd_clear_screen(LCD_COLOR_BLACK);
//                 lcd_draw_image(
//                         320/2,
//                         240/2,
//                         youloseWidthPixels,
//                         youloseHeightPixels,
//                         youloseBitmaps,
//                         LCD_COLOR_BLUE,
//                         LCD_COLOR_BLACK
//                     );
//                 lcd_draw_image(
//                     320/2,
//                     215,
//                     resetWidthPixels,
//                     resetHeightPixels,
//                     resetBitmaps,
//                     LCD_COLOR_BLUE,
//                     LCD_COLOR_WHITE
//                 );
//             } else if (atoi(remote_rx_message) < score) {
//                 eeprom_write_enable();
//                 eeprom_write_byte(0x0000, score);
//                 eeprom_write_disable();
//                 lcd_clear_screen(LCD_COLOR_BLACK);
//                 lcd_draw_image(
//                         320/2,
//                         240/2,
//                         youwinWidthPixels,
//                         youwinHeightPixels,
//                         youwinBitmaps,
//                         LCD_COLOR_BLUE,
//                         LCD_COLOR_BLACK
//                     );
//                 lcd_draw_image(
//                     320/2,
//                     215,
//                     resetWidthPixels,
//                     resetHeightPixels,
//                     resetBitmaps,
//                     LCD_COLOR_BLUE,
//                     LCD_COLOR_WHITE
//                 );
//             }
//         }
//     }
//     while(in_ready || in_main_menu || in_color_select) {
//         if (remote_uart_rx_string_polling(remote_rx_message)) {
//             if (strcmp(remote_rx_message, "ready") == 0) {
//                 if (in_ready && im_ready) {
//                     xTaskNotifyGive(TaskHandle_timer);
//                     lcd_clear_screen(LCD_COLOR_BLACK);
//                     score = 0;
//                     // Draws high score at top
//                     lcd_draw_image(
//                         125,
//                         10,
//                         highScoreWidthPixels,
//                         highScoreHeightPixels,
//                         highScoreBitmaps,
//                         LCD_COLOR_BLUE,
//                         LCD_COLOR_BLACK
//                     );
//                     // Draws digit images for score
//                     high_score = eeprom_read_byte(0x0000);
//                     switch (high_score % 10) {
//                         case 0:
//                             lcd_draw_image(
//                                 210,
//                                 10,
//                                 number0WidthPixels,
//                                 number0HeightPixels,
//                                 number0Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 1:
//                             lcd_draw_image(
//                                 210,
//                                 10,
//                                 number1WidthPixels,
//                                 number1HeightPixels,
//                                 number1Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
                
//                         case 2:
//                             lcd_draw_image(
//                                 210,
//                                 10,
//                                 number2WidthPixels,
//                                 number2HeightPixels,
//                                 number2Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
                
//                         case 3:
//                             lcd_draw_image(
//                                 210,
//                                 10,
//                                 number3WidthPixels,
//                                 number3HeightPixels,
//                                 number3Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 4:
//                             lcd_draw_image(
//                                 210,
//                                 10,
//                                 number4WidthPixels,
//                                 number4HeightPixels,
//                                 number4Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 5:
//                             lcd_draw_image(
//                                 210,
//                                 10,
//                                 number5WidthPixels,
//                                 number5HeightPixels,
//                                 number5Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 6:
//                             lcd_draw_image(
//                                 210,
//                                 10,
//                                 number6WidthPixels,
//                                 number6HeightPixels,
//                                 number6Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 7:
//                             lcd_draw_image(
//                                 210,
//                                 10,
//                                 number7WidthPixels,
//                                 number7HeightPixels,
//                                 number7Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 8:
//                             lcd_draw_image(
//                                 210,
//                                 10,
//                                 number8WidthPixels,
//                                 number8HeightPixels,
//                                 number8Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 9:
//                             lcd_draw_image(
//                                 210,
//                                 10,
//                                 number9WidthPixels,
//                                 number9HeightPixels,
//                                 number9Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                     }
//                     /* Updates score display, Tens Place*/
//                     switch ((high_score- (high_score % 10))/10) {
//                         case 0:
//                             lcd_draw_image(
//                                 195,
//                                 10,
//                                 number0WidthPixels,
//                                 number0HeightPixels,
//                                 number0Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 1:
//                             lcd_draw_image(
//                                 195,
//                                 10,
//                                 number1WidthPixels,
//                                 number1HeightPixels,
//                                 number1Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
                
//                         case 2:
//                             lcd_draw_image(
//                                 195,
//                                 10,
//                                 number2WidthPixels,
//                                 number2HeightPixels,
//                                 number2Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
                
//                         case 3:
//                             lcd_draw_image(
//                                 195,
//                                 10,
//                                 number3WidthPixels,
//                                 number3HeightPixels,
//                                 number3Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 4:
//                             lcd_draw_image(
//                                 195,
//                                 10,
//                                 number4WidthPixels,
//                                 number4HeightPixels,
//                                 number4Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 5:
//                             lcd_draw_image(
//                                 195,
//                                 10,
//                                 number5WidthPixels,
//                                 number5HeightPixels,
//                                 number5Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 6:
//                             lcd_draw_image(
//                                 195,
//                                 10,
//                                 number6WidthPixels,
//                                 number6HeightPixels,
//                                 number6Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 7:
//                             lcd_draw_image(
//                                 195,
//                                 10,
//                                 number7WidthPixels,
//                                 number7HeightPixels,
//                                 number7Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 8:
//                             lcd_draw_image(
//                                 195,
//                                 10,
//                                 number8WidthPixels,
//                                 number8HeightPixels,
//                                 number8Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 9:
//                             lcd_draw_image(
//                                 195,
//                                 10,
//                                 number9WidthPixels,
//                                 number9HeightPixels,
//                                 number9Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                     }
//                     // ALL HOLE COORDINATES
//                     // TOP ROW
//                     lcd_draw_image(
//                         60,
//                         80,
//                         holeWidthPixels,
//                         holeHeightPixels,
//                         holeBitmaps,
//                         LCD_COLOR_BROWN,
//                         LCD_COLOR_BLACK
//                     );
//                     lcd_draw_image(
//                         160,
//                         80,
//                         holeWidthPixels,
//                         holeHeightPixels,
//                         holeBitmaps,
//                         LCD_COLOR_BROWN,
//                         LCD_COLOR_BLACK
//                     );
//                     lcd_draw_image(
//                         260,
//                         80,
//                         holeWidthPixels,
//                         holeHeightPixels,
//                         holeBitmaps,
//                         LCD_COLOR_BROWN,
//                         LCD_COLOR_BLACK
//                     );
//                     // BOTTOM ROW
//                     lcd_draw_image(
//                         60,
//                         185,
//                         holeWidthPixels,
//                         holeHeightPixels,
//                         holeBitmaps,
//                         LCD_COLOR_BROWN,
//                         LCD_COLOR_BLACK
//                     );
//                     lcd_draw_image(
//                         160,
//                         185,
//                         holeWidthPixels,
//                         holeHeightPixels,
//                         holeBitmaps,
//                         LCD_COLOR_BROWN,
//                         LCD_COLOR_BLACK
//                     );
//                     lcd_draw_image(
//                         260,
//                         185,
//                         holeWidthPixels,
//                         holeHeightPixels,
//                         holeBitmaps,
//                         LCD_COLOR_BROWN,
//                         LCD_COLOR_BLACK
//                     );
//                     //RANDOMIZES MOLE LOCATION
//                     int random = rand()/10000000;
//                     int next_mole = (random % 6);
//                     board[next_mole] = 1;
//                     if(next_mole == 0) {
//                         lcd_draw_image(
//                             60,
//                             55,
//                             moleWidthPixels,
//                             moleHeightPixels,
//                             moleBitmaps,
//                             LCD_COLOR_GRAY,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (next_mole == 1) {
//                         lcd_draw_image(
//                             160,
//                             55,
//                             moleWidthPixels,
//                             moleHeightPixels,
//                             moleBitmaps,
//                             LCD_COLOR_GRAY,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (next_mole == 2) {
//                         lcd_draw_image(
//                             260,
//                             55,
//                             moleWidthPixels,
//                             moleHeightPixels,
//                             moleBitmaps,
//                             LCD_COLOR_GRAY,
//                             LCD_COLOR_BLACK
//                         );
//                     }
//                     // BOTTOM ROW
//                     else if (next_mole == 3) {
//                         lcd_draw_image(
//                             60,
//                             160,
//                             moleWidthPixels,
//                             moleHeightPixels,
//                             moleBitmaps,
//                             LCD_COLOR_GRAY,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (next_mole == 4) {
//                         lcd_draw_image(
//                             160,
//                             160,
//                             moleWidthPixels,
//                             moleHeightPixels,
//                             moleBitmaps,
//                             LCD_COLOR_GRAY,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (next_mole == 5) {
//                         lcd_draw_image(
//                             260,
//                             160,
//                             moleWidthPixels,
//                             moleHeightPixels,
//                             moleBitmaps,
//                             LCD_COLOR_GRAY,
//                             LCD_COLOR_BLACK
//                         );
//                     }
//                     lcd_draw_image(
//                         135,
//                         115,
//                         scoreWidthPixels,
//                         scoreHeightPixels,
//                         scoreBitmaps,
//                         LCD_COLOR_GREEN2,
//                         LCD_COLOR_BLACK
//                     );
//                     lcd_draw_image(
//                         180,
//                         115,
//                         number0WidthPixels,
//                         number0HeightPixels,
//                         number0Bitmaps,
//                         LCD_COLOR_YELLOW,
//                         LCD_COLOR_BLACK
//                     );
//                     lcd_draw_image(
//                         195,
//                         115,
//                         number0WidthPixels,
//                         number0HeightPixels,
//                         number0Bitmaps,
//                         LCD_COLOR_YELLOW,
//                         LCD_COLOR_BLACK
//                     );
//                     in_color_select = false;
//                     in_game = true;
//                     in_main_menu = false;
//                     in_ready = false;
//                     im_ready = false;
//                 }
//             }
//         }
//     }
//     memset(remote_rx_message,0,80);
// }

// void task_uart_send() {
//     memset(remote_rx_message,0,80);
//     char *send;
//     char send_score[20];
//     if (!game_over) {
//         send = "ready\n";
//         remote_uart_tx_string_polling(send);
//     }
//     if (game_over) {
//         itoa(score, send_score, 10);
//         strcat(send_score, "\n");
//         remote_uart_tx_string_polling(send_score);
//     }
//     task_uart_recieve();
// }

// void task_uart_init(void) {
//     /* Init the uart task*/
//     //xTaskCreate(
//     //    task_uart_recieve,
//     //    "Task UART receive",
//     //    configMINIMAL_STACK_SIZE,
//     //    NULL,
//     //    3,
//     //    &TaskHandle_recieve
//     //);
// //
//     //xTaskCreate(
//     //    task_uart_send,
//     //    "Task UART send",
//     //    configMINIMAL_STACK_SIZE,
//     //    NULL,
//     //    3,
//     //    &TaskHandle_send
//     //);

//     memset(remote_rx_message,0,80);
//     cyhal_uart_putc(&cy_retarget_io_uart_obj, '\r');
// }