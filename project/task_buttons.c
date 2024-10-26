// /**
//  * @file task_buttons.c
//  * @author your name (you@domain.com)
//  * @brief 
//  * @version 0.1
//  * @date 2023-12-08
//  * 
//  * @copyright Copyright (c) 2023
//  * 
//  */

// #include "task_buttons.h"
// #include "imu.h"

// TaskHandle_t TaskHandle_SW1;
// TaskHandle_t TaskHandle_SW2;
// extern TaskHandle_t TaskHandle_send;
// extern TaskHandle_t TaskHandle_recieve;

// extern void task_uart_recieve();
// extern void task_uart_send();

// extern bool in_main_menu;
// extern bool in_color_select;
// extern bool in_game;
// extern bool in_ready;
// extern bool game_over;
// extern int board[];
// extern int high_score;
// extern int hammer_color;

// extern int x;
// extern int y;

// extern cyhal_uart_t cy_retarget_io_uart_obj;

// bool im_ready = false;

// void task_sw2(void *Parameters) {
//     /* Allocate any local variables used in this task */
//     bool state_button_prev = false;
//     bool state_button_curr = false;

//     while (1)
//     {
//         /* Sleep for 50mS -- DO NOT use any cyhal_ functions to delay */
//         vTaskDelay(50);

//         /* Check the current state of the button */
//         if ((PORT_BUTTONS->IN & SW2_MASK) == 0x00)
//         {
//             state_button_curr = true;
//         }
//         else
//         {
//             state_button_curr = false;
//         }

//         /* Detect if the button has been pressed */
//         if (state_button_curr && !state_button_prev)
//         {
//             printf("SW2 Pushed \n\r");
//             if (in_color_select) {
//                 lcd_clear_screen(LCD_COLOR_BLACK);
//                 lcd_draw_image(
//                     160,
//                     40,
//                     readyUpWidthPixels,
//                     readyUpHeightPixels,
//                     readyUpBitmaps,
//                     LCD_COLOR_BLUE,
//                     LCD_COLOR_BLACK
//                 );
//                 if(get_light() > 10) {
//                     hammer_color = LCD_COLOR_BLUE;
//                 } else {
//                     hammer_color = LCD_COLOR_GREEN;
//                 }

//                 in_color_select = false;
//                 in_ready = true;
//             }
//             if (game_over) {
//                 lcd_clear_screen(LCD_COLOR_BLACK);
//                 lcd_draw_image(
//                     160,
//                     40,
//                     readyUpWidthPixels,
//                     readyUpHeightPixels,
//                     readyUpBitmaps,
//                     LCD_COLOR_BLUE,
//                     LCD_COLOR_BLACK
//                 );

//                 in_color_select = false;
//                 in_ready = true;
//                 in_main_menu = false;
//                 game_over = false;
//                 im_ready = false;
//             }
//         }

//         /* Update the previous button state */
//         state_button_prev = state_button_curr;
//     }
// }

// void task_sw1(void *Parameters) {
//     /* Allocate any local variables used in this task */
//     bool state_button_prev = false;
//     bool state_button_curr = false;
//     uint8_t remote_rx_message[80];
//     memset(remote_rx_message,0,80);

//     while(1) {
//         /* Sleep for 50mS -- DO NOT use any cyhal_ functions to delay */
//         vTaskDelay(50);

//         /* Check the current state of the button */
//         if ((PORT_BUTTONS->IN & SW1_MASK) == 0x00)
//         {
//             state_button_curr = true;
//         }
//         else
//         {
//             state_button_curr = false;
//         }

//         /* Detect if the button has been pressed */
//         if (state_button_curr && !state_button_prev)
//         {
//             printf("SW1 Pushed \n\r");
//             if (x < 210 && x > 110 && y > 165 && y < 235 && in_main_menu) {
//                 lcd_clear_screen(LCD_COLOR_BLACK);
//                 lcd_draw_image(
//                     160,
//                     50,
//                     chooseColorWidthPixels,
//                     chooseColorHeightPixels,
//                     chooseColorBitmaps,
//                     LCD_COLOR_CYAN,
//                     LCD_COLOR_BLACK
//                 );
//                 lcd_draw_image(
//                     160,
//                     140,
//                     hammerWidthPixels,
//                     hammerHeightPixels,
//                     hammerBitmaps,
//                     LCD_COLOR_BROWN,
//                     LCD_COLOR_BLACK
//                 );
//                 lcd_draw_image(
//                     160,
//                     205,
//                     selectWidthPixels,
//                     selectHeightPixels,
//                     selectBitmaps,
//                     LCD_COLOR_CYAN,
//                     LCD_COLOR_BLACK
//                 );
//                 in_main_menu = false;
//                 in_color_select = true;
//             }
//             if (in_ready) {
//                 lcd_draw_image(
//                     160,
//                     100,
//                     readyWidthPixels,
//                     readyHeightPixels,
//                     readyBitmaps,
//                     LCD_COLOR_GREEN2,
//                     LCD_COLOR_BLACK
//                 );
//                 lcd_draw_image(
//                     160,
//                     150,
//                     waitingWidthPixels,
//                     waitingHeightPixels,
//                     waitingBitmaps,
//                     LCD_COLOR_GREEN2,
//                     LCD_COLOR_BLACK
//                 );
                
//                 im_ready = true;
//                 task_uart_send();
//                 in_ready = false;
//             }
//         }

//         /* Update the previous button state */
//         state_button_prev = state_button_curr;
//     }
// }

// void task_sw3(void *Parameters) {
//     /* Allocate any local variables used in this task */
//     bool state_button_prev = false;
//     bool state_button_curr = false;

//     while(1) {
//         /* Sleep for 50mS -- DO NOT use any cyhal_ functions to delay */
//         vTaskDelay(50);

//         /* Check the current state of the button */
//         if ((PORT_BUTTONS->IN & SW3_MASK) == 0x00)
//         {
//             state_button_curr = true;
//         }
//         else
//         {
//             state_button_curr = false;
//         }

//         /* Detect if the button has been pressed */
//         if (state_button_curr && !state_button_prev)
//         {
//             printf("SW3 Pushed \n\r");
//             if (in_ready) {
//                 lcd_clear_screen(LCD_COLOR_BLACK);
//                 // Draws high score at top
//                 lcd_draw_image(
//                     125,
//                     10,
//                     highScoreWidthPixels,
//                     highScoreHeightPixels,
//                     highScoreBitmaps,
//                     LCD_COLOR_BLUE,
//                     LCD_COLOR_BLACK
//                 );
//                 // Draws digit images for score
//                 switch (high_score % 10) {
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
//                         lcd_draw_image(
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
//                         lcd_draw_image(
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
//                         lcd_draw_image(
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
//                         lcd_draw_image(
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
//                         lcd_draw_image(
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
//                         lcd_draw_image(
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
//                         lcd_draw_image(
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
//                         lcd_draw_image(
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
//                         lcd_draw_image(
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
//                         lcd_draw_image(
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
//                         lcd_draw_image(
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
//                         lcd_draw_image(
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
//                         lcd_draw_image(
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
//                         lcd_draw_image(
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
//                 // ALL HOLE COORDINATES
//                 // TOP ROW
//                 lcd_draw_image(
//                     60,
//                     80,
//                     holeWidthPixels,
//                     holeHeightPixels,
//                     holeBitmaps,
//                     LCD_COLOR_BROWN,
//                     LCD_COLOR_BLACK
//                 );
//                 lcd_draw_image(
//                     160,
//                     80,
//                     holeWidthPixels,
//                     holeHeightPixels,
//                     holeBitmaps,
//                     LCD_COLOR_BROWN,
//                     LCD_COLOR_BLACK
//                 );
//                 lcd_draw_image(
//                     260,
//                     80,
//                     holeWidthPixels,
//                     holeHeightPixels,
//                     holeBitmaps,
//                     LCD_COLOR_BROWN,
//                     LCD_COLOR_BLACK
//                 );
//                 // BOTTOM ROW
//                 lcd_draw_image(
//                     60,
//                     185,
//                     holeWidthPixels,
//                     holeHeightPixels,
//                     holeBitmaps,
//                     LCD_COLOR_BROWN,
//                     LCD_COLOR_BLACK
//                 );
//                 lcd_draw_image(
//                     160,
//                     185,
//                     holeWidthPixels,
//                     holeHeightPixels,
//                     holeBitmaps,
//                     LCD_COLOR_BROWN,
//                     LCD_COLOR_BLACK
//                 );
//                 lcd_draw_image(
//                     260,
//                     185,
//                     holeWidthPixels,
//                     holeHeightPixels,
//                     holeBitmaps,
//                     LCD_COLOR_BROWN,
//                     LCD_COLOR_BLACK
//                 );
//                 //RANDOMIZES MOLE LOCATION
//                 int random = rand()/10000000;
//                 int next_mole = (random % 6);
//                 board[next_mole] = 1;
//                 if(next_mole == 0) {
//                     lcd_draw_image(
//                         60,
//                         55,
//                         moleWidthPixels,
//                         moleHeightPixels,
//                         moleBitmaps,
//                         LCD_COLOR_GRAY,
//                         LCD_COLOR_BLACK
//                     );
//                 } else if (next_mole == 1) {
//                     lcd_draw_image(
//                         160,
//                         55,
//                         moleWidthPixels,
//                         moleHeightPixels,
//                         moleBitmaps,
//                         LCD_COLOR_GRAY,
//                         LCD_COLOR_BLACK
//                     );
//                 } else if (next_mole == 2) {
//                     lcd_draw_image(
//                         260,
//                         55,
//                         moleWidthPixels,
//                         moleHeightPixels,
//                         moleBitmaps,
//                         LCD_COLOR_GRAY,
//                         LCD_COLOR_BLACK
//                     );
//                 }
//                 // BOTTOM ROW
//                 else if (next_mole == 3) {
//                     lcd_draw_image(
//                         60,
//                         160,
//                         moleWidthPixels,
//                         moleHeightPixels,
//                         moleBitmaps,
//                         LCD_COLOR_GRAY,
//                         LCD_COLOR_BLACK
//                     );
//                 } else if (next_mole == 4) {
//                     lcd_draw_image(
//                         160,
//                         160,
//                         moleWidthPixels,
//                         moleHeightPixels,
//                         moleBitmaps,
//                         LCD_COLOR_GRAY,
//                         LCD_COLOR_BLACK
//                     );
//                 } else if (next_mole == 5) {
//                     lcd_draw_image(
//                         260,
//                         160,
//                         moleWidthPixels,
//                         moleHeightPixels,
//                         moleBitmaps,
//                         LCD_COLOR_GRAY,
//                         LCD_COLOR_BLACK
//                     );
//                 }
//                 lcd_draw_image(
//                     135,
//                     115,
//                     scoreWidthPixels,
//                     scoreHeightPixels,
//                     scoreBitmaps,
//                     LCD_COLOR_GREEN2,
//                     LCD_COLOR_BLACK
//                 );
//                 lcd_draw_image(
//                     180,
//                     115,
//                     number0WidthPixels,
//                     number0HeightPixels,
//                     number0Bitmaps,
//                     LCD_COLOR_YELLOW,
//                     LCD_COLOR_BLACK
//                 );
//                 lcd_draw_image(
//                     195,
//                     115,
//                     number0WidthPixels,
//                     number0HeightPixels,
//                     number0Bitmaps,
//                     LCD_COLOR_YELLOW,
//                     LCD_COLOR_BLACK
//                 );
//                 in_color_select = false;
//                 in_game = true;
//             }
//         }

//         /* Update the previous button state */
//         state_button_prev = state_button_curr;
//     }
// }

// void task_buttons_init(void) {
//     /* Init the buttons*/
//     xTaskCreate(
//         task_sw1,
//         "Task SW1",
//         configMINIMAL_STACK_SIZE,
//         NULL,
//         3,
//         &TaskHandle_SW1
//     );

//     xTaskCreate(
//        task_sw2,
//        "Task SW2",
//        configMINIMAL_STACK_SIZE,
//        NULL,
//        3,
//        &TaskHandle_SW2
//     );

//     xTaskCreate(
//        task_sw3,
//        "Task SW3",
//        configMINIMAL_STACK_SIZE,
//        NULL,
//        3,
//        &TaskHandle_SW2
//     );
// }