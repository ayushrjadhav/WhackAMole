// /**
//  * @file task_lcd.c
//  * @author your name (you@domain.com)
//  * @brief 
//  * @version 0.1
//  * @date 2023-12-09
//  * 
//  * @copyright Copyright (c) 2023
//  * 
//  */

// #include "task_lcd.h"

// /*****************************************************************************/
// /*  FreeRTOS Handles
// /*****************************************************************************/
// /* Create two TaskHandle_T variables for the Cap Touch and LCD tasks */
// TaskHandle_t TaskHandle_CapTouch;
// TaskHandle_t TaskHandle_LCD;
// TaskHandle_t TaskHandle_UpdateBoard;

// /* Create a QueueHandle_t for the CapTouch position queue */
// QueueHandle_t QueueHandle_position;

// /*****************************************************************************/
// /* Function Declarations                                                     */
// /*****************************************************************************/
// void task_cap_touch(void *pvParameters);
// void task_lcd(void *pvParameters);
// void lcd_init(void);

// /*****************************************************************************/
// /* Global Variables                                                          */
// /*****************************************************************************/
// cyhal_gpio_callback_data_t cap_touch_callback_data;

// int score = 0;

// int board[] = {
//     0, 0, 0,
//     0, 0, 0
// };

// typedef enum {
//     UPPER_LEFT,
//     UPPER_MIDDLE,
//     UPPER_RIGHT,
//     LOWER_LEFT,
//     LOWER_MIDDLE,
//     LOWER_RIGHT,
//     NONE
// } mole_coordinates;

// bool mole_clicked = false;
// bool in_main_menu = true;
// bool in_game = false;
// bool in_color_select = false;
// bool in_ready = false;
// bool game_over = false;

// mole_coordinates prev_mole_coordinates = NONE;

// typedef struct LCD_Coordinates_t{
//     uint16_t x;
//     uint16_t y;
// } LCD_Coordinates_t;

// LCD_Coordinates_t coordinates;

// bool upper_left_clicked = false;
// bool upper_middle_clicked = false;
// bool upper_right_clicked = false;
// bool lower_left_clicked = false;
// bool lower_middle_clicked = false;
// bool lower_right_clicked = false;

// extern int hammer_color;

// void lcd_init(void) {
//     /* Initialize the LCD queue that will be used to send the coordinates of where
//     * the user is touching the LCD screen
//     */
//     QueueHandle_position = xQueueCreate(1, sizeof(LCD_Coordinates_t));

//     /* Create the LCD Task */
//     xTaskCreate(task_lcd,
//                 "LCD Task",
//                 configMINIMAL_STACK_SIZE,
//                 NULL,
//                 2,
//                 &TaskHandle_LCD
//     );

//     /* Create the Cap Touch Task */
//     xTaskCreate(task_cap_touch,
//                 "Cap Touch Task",
//                 configMINIMAL_STACK_SIZE,
//                 NULL,
//                 2,
//                 &TaskHandle_CapTouch
//     );
// }

// /*****************************************************************************/
// /* Interrupt Handlers                                                        */
// /*****************************************************************************/
// /* Interrupt handler callback function */
// void cap_touch_handler(void *handler_arg, cyhal_gpio_event_t event)
// {
//     BaseType_t xHigherPriorityTaskWoken;

//     /* The xHigherPriorityTaskWoken parameter must be initialized to pdFALSE as
//      it will get set to pdTRUE inside the interrupt safe API function if a
//      context switch is required. */
//     xHigherPriorityTaskWoken = pdFALSE;

//     /* Send a notification directly to the task to which interrupt processing
//      * is being deferred.
//      */
//     vTaskNotifyGiveFromISR(TaskHandle_CapTouch, &xHigherPriorityTaskWoken);

//     /* Call the function that will force the task that was running prior to the
//      * interrupt to yield*/
//     portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
// }

// /*****************************************************************************/
// /* Peripheral Initialization                                                 */
// /*****************************************************************************/
// void cap_touch_irq_enable(void)
// {
//     cyhal_gpio_init(PIN_IO_CAP_TOUCH_INT,CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, 0);
//     cap_touch_callback_data.callback = cap_touch_handler;
//     cyhal_gpio_register_callback(PIN_IO_CAP_TOUCH_INT, &cap_touch_callback_data);
//     cyhal_gpio_enable_event(PIN_IO_CAP_TOUCH_INT, CYHAL_GPIO_IRQ_FALL, 3, true);
// }

// /**
//  * @brief 
//  * This task is a bottom half task for the IO pin that generates an interrupt when the user
//  * touches the LCD screen.  This task will wait indefinitely for a task notification from the 
//  * IO pins handler.  Once the task notification is received, the task will determine the 
//  * coordinates where the user is making contact with the LCD using the I2C bus. You will need
//  * to make use of the functions defined in  ft6x06.c
//  * @param pvParameters 
//  */
// void task_cap_touch(void *pvParameters)
// {
//     uint8_t td_status;
//     LCD_Coordinates_t coordinates;
//     while (1)
//     {
//         /* Wait for the Cap Touch IO Interrupt Handler to send
//          * a task notification
//          */
//         ulTaskNotifyTake(true, portMAX_DELAY);
//         uint8_t active_led = 0x01;
//         uint8_t io_sw = 0xFF;

//         /* Check to see if the TD status is equal to 1 or 2*/
//         td_status = ft6x06_read_td_status();

//         if(td_status == 1 || td_status == 2)
//         {
//             /* Read the X and Y coordinates */
//             coordinates.x = ft6x06_read_x();
//             coordinates.y = ft6x06_read_y();

//             printf("X: %d Y: %d \n\r",coordinates.x, coordinates.y);

//             /* Send the message to the LCD Task */
//             xTaskNotifyGive(TaskHandle_LCD);
//             xQueueSend(QueueHandle_position, &coordinates, portMAX_DELAY);
//         }
//     }
// }

// /**
//  * @brief 
//  * This Task will wait indefinitely for a message to be received on a Queue. The data in 
//  * the queue will contain the coordinates of where the user is making contact with the 
//  * LCD screen.  Using this information, the task will then draw a rectangle on the LCD screen 
//  * @param pvParameters 
//  */
// void task_lcd(void *pvParameters)
// {
//     /* Allocate any local variables used in this task */
//     uint16_t cursor_color = LCD_COLOR_GREEN;
//     LCD_Coordinates_t coordinates;
//     float temp;

//     while (1)
//     {
//         /* Wait to receive a message with the coordinates to draw the cursor*/
//         ulTaskNotifyTake(true, portMAX_DELAY);
//         xQueueReceive(QueueHandle_position, &coordinates, portMAX_DELAY); 	
//         if (in_main_menu) {
//             lsm6dsm_orientation();
//         }
        

//         /* Start Button Click*/
//         if (coordinates.x < 210 && coordinates.x > 110 && coordinates.y > 165 && coordinates.y < 235 && in_main_menu) {
//             lcd_clear_screen(LCD_COLOR_BLACK);
//             lcd_draw_image(
//                 160,
//                 50,
//                 chooseColorWidthPixels,
//                 chooseColorHeightPixels,
//                 chooseColorBitmaps,
//                 LCD_COLOR_CYAN,
//                 LCD_COLOR_BLACK
//             );
//             lcd_draw_image(
//                 160,
//                 140,
//                 hammerWidthPixels,
//                 hammerHeightPixels,
//                 hammerBitmaps,
//                 hammer_color,
//                 LCD_COLOR_BLACK
//             );
//             lcd_draw_image(
//                 160,
//                 205,
//                 selectWidthPixels,
//                 selectHeightPixels,
//                 selectBitmaps,
//                 LCD_COLOR_CYAN,
//                 LCD_COLOR_BLACK
//             );
//             in_main_menu = false;
//             in_color_select = true;
//         }
        
//         /* Clear Mole Image */
//         if (in_game) {
//             if (coordinates.x < 90 && coordinates.x > 50 && coordinates.y > 50 && coordinates.y < 90 && !upper_left_clicked) {
//                 if (board[0] == 1) {
//                     // upper left
//                     score++;
//                     PORT_RGB_GRN->OUT_SET = MASK_RGB_GRN;
//                     io_expander_set_output_port(0x08);
//                     cyhal_system_delay_ms(250);
//                     PORT_RGB_GRN->OUT_CLR = MASK_RGB_GRN;
//                     io_expander_set_output_port(0x00);
                   
            
//                     lcd_draw_rectangle(165, 40, 105, 25, LCD_COLOR_BLACK);
//                     /* Updates score display, Ones Place*/
//                     switch (score % 10) {
//                         case 0:
//                             lcd_draw_image(
//                                 195,
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
//                                 number9WidthPixels,
//                                 number9HeightPixels,
//                                 number9Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                     }

//                     /* Updates score display, Tens Place*/
//                     switch ((score - (score%10))/10) {
//                         case 0:
//                             lcd_draw_image(
//                                 180,
//                                 115,
//                                 number0WidthPixels,
//                                 number0HeightPixels,
//                                 number0Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 1:
//                             lcd_draw_image(
//                                 180,
//                                 115,
//                                 number1WidthPixels,
//                                 number1HeightPixels,
//                                 number1Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
                
//                         case 2:
//                             lcd_draw_image(
//                                 180,
//                                 115,
//                                 number2WidthPixels,
//                                 number2HeightPixels,
//                                 number2Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
                
//                         case 3:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number3WidthPixels,
//                                 number3HeightPixels,
//                                 number3Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 4:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number4WidthPixels,
//                                 number4HeightPixels,
//                                 number4Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 5:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number5WidthPixels,
//                                 number5HeightPixels,
//                                 number5Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 6:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number6WidthPixels,
//                                 number6HeightPixels,
//                                 number6Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 7:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number7WidthPixels,
//                                 number7HeightPixels,
//                                 number7Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 8:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number8WidthPixels,
//                                 number8HeightPixels,
//                                 number8Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 9:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number9WidthPixels,
//                                 number9HeightPixels,
//                                 number9Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                     }

//                     upper_left_clicked = true;
//                     upper_middle_clicked = false;
//                     upper_right_clicked = false;
//                     lower_left_clicked = false;
//                     lower_middle_clicked = false;
//                     lower_right_clicked = false;

//                     lcd_draw_rectangle_centered(60, 55, 40, 40, LCD_COLOR_BLACK);
//                     lcd_draw_image(
//                         75,
//                         50,
//                         hammerWidthPixels,
//                         hammerHeightPixels,
//                         hammerBitmaps,
//                         hammer_color,
//                         LCD_COLOR_BLACK
//                     );

//                     // Clears previous based on previous location
//                     if (prev_mole_coordinates == UPPER_RIGHT) {
//                         // Clear Hammer
//                         lcd_draw_rectangle(225, 100, 0, 120, LCD_COLOR_BLACK);
//                         // Redraw Hole
//                         lcd_draw_image(
//                             260,
//                             80,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == UPPER_MIDDLE) {
//                         lcd_draw_rectangle(125, 100, 25, 55, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             160,
//                             80,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == LOWER_RIGHT) {
//                         lcd_draw_rectangle(225, 100, 120, 70, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             260,
//                             185,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == LOWER_LEFT) {
//                         lcd_draw_rectangle(0, 120, 130, 70, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             60,
//                             185,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == LOWER_MIDDLE) {
//                         lcd_draw_rectangle(125, 100, 130, 70, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             160,
//                             185,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     }
//                     int random = rand()/10000000;
//                     int next_mole = (random % 5);
//                     board[0] = 0;
//                     if (next_mole == 0) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 160,
//                                 y+50,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y!=1) {
//                                 lcd_draw_image(
//                                     160,
//                                     80,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[1] = 1;
//                     } else if (next_mole == 1) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 260,
//                                 y+50,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y!=1) {
//                                 lcd_draw_image(
//                                     160,
//                                     80,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[2] = 1;
//                     }
//                     // BOTTOM ROW
//                     else if (next_mole == 2) {
//                         int y = 20;
//                         while(y>0) {
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 60,
//                                 y+155,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y!=1) {
//                                 lcd_draw_image(
//                                     60,
//                                     185,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }       
//                             y--;
//                         }
//                         board[3] = 1;
//                     } else if (next_mole == 3) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 160,
//                                 y+155,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y!=1) {
//                                 lcd_draw_image(
//                                     160,
//                                     185,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[4] = 1;
//                     } else if (next_mole == 4) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 260,
//                                 y+155,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y!=1) {
//                                 lcd_draw_image(
//                                     260,
//                                     185,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[5] = 1;
//                     }
//                     prev_mole_coordinates = UPPER_LEFT;
//                 } else {
//                     PORT_RGB_RED->OUT_SET = MASK_RGB_RED;
//                     pwm_buzzer_start();
//                     cyhal_system_delay_ms(250);
//                     pwm_buzzer_stop();
//                     PORT_RGB_RED->OUT_CLR = MASK_RGB_RED;
//                 }
//             } else if (coordinates.x < 190 && coordinates.x > 150 && coordinates.y > 50 && coordinates.y < 90 & !upper_middle_clicked) {
//                 if (board[1] == 1) {
//                     // upper middle
//                     score++;
//                     PORT_RGB_GRN->OUT_SET = MASK_RGB_GRN;
//                     io_expander_set_output_port(0x08);
//                     cyhal_system_delay_ms(250);
//                     PORT_RGB_GRN->OUT_CLR = MASK_RGB_GRN;
//                     io_expander_set_output_port(0x00);
//                     lcd_draw_rectangle(165, 40, 105, 25, LCD_COLOR_BLACK);
//                     /* Updates score display, Ones Place*/
//                     switch (score % 10) {
//                         case 0:
//                             lcd_draw_image(
//                                 195,
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
//                                 number9WidthPixels,
//                                 number9HeightPixels,
//                                 number9Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                     }

//                     /* Updates score display, Tens Place*/
//                     switch ((score - (score%10))/10) {
//                         case 0:
//                             lcd_draw_image(
//                                 180,
//                                 115,
//                                 number0WidthPixels,
//                                 number0HeightPixels,
//                                 number0Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 1:
//                             lcd_draw_image(
//                                 180,
//                                 115,
//                                 number1WidthPixels,
//                                 number1HeightPixels,
//                                 number1Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
                
//                         case 2:
//                             lcd_draw_image(
//                                 180,
//                                 115,
//                                 number2WidthPixels,
//                                 number2HeightPixels,
//                                 number2Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
                
//                         case 3:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number3WidthPixels,
//                                 number3HeightPixels,
//                                 number3Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 4:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number4WidthPixels,
//                                 number4HeightPixels,
//                                 number4Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 5:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number5WidthPixels,
//                                 number5HeightPixels,
//                                 number5Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 6:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number6WidthPixels,
//                                 number6HeightPixels,
//                                 number6Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 7:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number7WidthPixels,
//                                 number7HeightPixels,
//                                 number7Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 8:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number8WidthPixels,
//                                 number8HeightPixels,
//                                 number8Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 9:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number9WidthPixels,
//                                 number9HeightPixels,
//                                 number9Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                     }

//                     upper_left_clicked = false;
//                     upper_middle_clicked = true;
//                     upper_right_clicked = false;
//                     lower_left_clicked = false;
//                     lower_middle_clicked = false;
//                     lower_right_clicked = false;
        
//                     lcd_draw_rectangle_centered(160, 55, 40, 40, LCD_COLOR_BLACK);
//                     lcd_draw_image(
//                         175,
//                         50,
//                         hammerWidthPixels,
//                         hammerHeightPixels,
//                         hammerBitmaps,
//                         hammer_color,
//                         LCD_COLOR_BLACK
//                     );
//                     // Clears previous based on previous location
//                     if (prev_mole_coordinates == UPPER_RIGHT) {
//                         // Clear Hammer
//                         lcd_draw_rectangle(225, 100, 0, 120, LCD_COLOR_BLACK);
//                         // Redraw Hole
//                         lcd_draw_image(
//                             260,
//                             80,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == UPPER_LEFT) {
//                         lcd_draw_rectangle(0, 120, 20, 55, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             60,
//                             80,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == LOWER_RIGHT) {
//                         lcd_draw_rectangle(225, 100, 120, 70, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             260,
//                             185,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == LOWER_LEFT) {
//                         lcd_draw_rectangle(0, 120, 130, 70, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             60,
//                             185,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == LOWER_MIDDLE) {
//                         lcd_draw_rectangle(125, 100, 130, 70, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             160,
//                             185,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     }
//                     int random = rand()/10000000;
//                     int next_mole = (random % 5);
//                     board[1] = 0;
//                     if(next_mole == 0) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 60,
//                                 y+55,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y!=1) {
//                                 lcd_draw_image(
//                                     60,
//                                     80,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[0] = 1;
//                     } else if (next_mole == 1) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 260,
//                                 y+55,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y != 1){
//                                 lcd_draw_image(
//                                     260,
//                                     80,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[2] = 1;
//                     }
//                     // BOTTOM ROW
//                     else if (next_mole == 2) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 60,
//                                 y+160,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y != 1){
//                                 lcd_draw_image(
//                                     60,
//                                     185,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[3] = 1;
//                     } else if (next_mole == 3) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 160,
//                                 y+160,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y != 1){
//                                 lcd_draw_image(
//                                     160,
//                                     185,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[4] = 1;
//                     } else if (next_mole == 4) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 260,
//                                 y+160,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y != 1){
//                                 lcd_draw_image(
//                                     260,
//                                     185,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[5] = 1;
//                     }
//                     prev_mole_coordinates = UPPER_MIDDLE;
//                 } else {
//                     PORT_RGB_RED->OUT_SET = MASK_RGB_RED;
//                     pwm_buzzer_start();
//                     cyhal_system_delay_ms(250);
//                     pwm_buzzer_stop();
//                     PORT_RGB_RED->OUT_CLR = MASK_RGB_RED;
//                 }
//             } else if (coordinates.x < 290 && coordinates.x > 250 && coordinates.y > 50 && coordinates.y < 90 & !upper_right_clicked) {
//                 if (board[2] == 1) {
//                     // upper right
//                     score++;
//                     PORT_RGB_GRN->OUT_SET = MASK_RGB_GRN;
//                     io_expander_set_output_port(0x08);
//                     cyhal_system_delay_ms(250);
//                     PORT_RGB_GRN->OUT_CLR = MASK_RGB_GRN;
//                     io_expander_set_output_port(0x00);
//                     lcd_draw_rectangle(165, 40, 105, 25, LCD_COLOR_BLACK);
//                     /* Updates score display, Ones Place*/
//                     switch (score % 10) {
//                         case 0:
//                             lcd_draw_image(
//                                 195,
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
//                                 number9WidthPixels,
//                                 number9HeightPixels,
//                                 number9Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                     }

//                     /* Updates score display, Tens Place*/
//                     switch ((score - (score%10))/10) {
//                         case 0:
//                             lcd_draw_image(
//                                 180,
//                                 115,
//                                 number0WidthPixels,
//                                 number0HeightPixels,
//                                 number0Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 1:
//                             lcd_draw_image(
//                                 180,
//                                 115,
//                                 number1WidthPixels,
//                                 number1HeightPixels,
//                                 number1Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
                
//                         case 2:
//                             lcd_draw_image(
//                                 180,
//                                 115,
//                                 number2WidthPixels,
//                                 number2HeightPixels,
//                                 number2Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
                
//                         case 3:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number3WidthPixels,
//                                 number3HeightPixels,
//                                 number3Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 4:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number4WidthPixels,
//                                 number4HeightPixels,
//                                 number4Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 5:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number5WidthPixels,
//                                 number5HeightPixels,
//                                 number5Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 6:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number6WidthPixels,
//                                 number6HeightPixels,
//                                 number6Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 7:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number7WidthPixels,
//                                 number7HeightPixels,
//                                 number7Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 8:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number8WidthPixels,
//                                 number8HeightPixels,
//                                 number8Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 9:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number9WidthPixels,
//                                 number9HeightPixels,
//                                 number9Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                     }

//                     upper_left_clicked = false;
//                     upper_middle_clicked = false;
//                     upper_right_clicked = true;
//                     lower_left_clicked = false;
//                     lower_middle_clicked = false;
//                     lower_right_clicked = false;
                    
//                     lcd_draw_rectangle_centered(260, 55, 40, 40, LCD_COLOR_BLACK);
//                     lcd_draw_image(
//                         275,
//                         50,
//                         hammerWidthPixels,
//                         hammerHeightPixels,
//                         hammerBitmaps,
//                         hammer_color,
//                         LCD_COLOR_BLACK
//                     );
//                     // Clears previous based on previous location
//                     if (prev_mole_coordinates == UPPER_MIDDLE) {
//                         lcd_draw_rectangle(125, 100, 25, 55, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             160,
//                             80,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == UPPER_LEFT) {
//                         lcd_draw_rectangle(0, 120, 20, 55, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             60,
//                             80,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == LOWER_RIGHT) {
//                         lcd_draw_rectangle(225, 100, 120, 70, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             260,
//                             185,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == LOWER_LEFT) {
//                         lcd_draw_rectangle(0, 120, 130, 70, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             60,
//                             185,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == LOWER_MIDDLE) {
//                         lcd_draw_rectangle(125, 100, 130, 70, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             160,
//                             185,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     }
//                     int random = rand()/10000000;
//                     int next_mole = (random % 5);
//                     board[2] = 0;
//                     if(next_mole == 0) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 60,
//                                 y+55,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y != 1) {
//                                 lcd_draw_image(
//                                     60,
//                                     80,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[0] = 1;
//                     } else if (next_mole == 1) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 160,
//                                 y+55,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y != 1){
//                                 lcd_draw_image(
//                                     260,
//                                     80,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[1] = 1;
//                     } else if (next_mole == 2) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 60,
//                                 y+160,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y != 1){
//                                 lcd_draw_image(
//                                     60,
//                                     185,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[3] = 1;
//                     } else if (next_mole == 3) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 160,
//                                 y+160,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y != 1){
//                                 lcd_draw_image(
//                                     160,
//                                     185,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[4] = 1;
//                     } else if (next_mole == 4) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 260,
//                                 y+160,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y != 1){
//                                 lcd_draw_image(
//                                     260,
//                                     185,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[5] = 1;
//                     }
//                     prev_mole_coordinates = UPPER_RIGHT;
//                 } else {
//                     PORT_RGB_RED->OUT_SET = MASK_RGB_RED;
//                     pwm_buzzer_start();
//                     cyhal_system_delay_ms(250);
//                     pwm_buzzer_stop();
//                     PORT_RGB_RED->OUT_CLR = MASK_RGB_RED;
//                 }
//             } else if (coordinates.x < 90 && coordinates.x > 50 && coordinates.y > 155 && coordinates.y < 195 & !lower_left_clicked) {
//                 if (board[3] == 1) {
//                     // lower left
//                     score++;
//                     PORT_RGB_GRN->OUT_SET = MASK_RGB_GRN;
//                     io_expander_set_output_port(0x08);
//                     cyhal_system_delay_ms(250);
//                     PORT_RGB_GRN->OUT_CLR = MASK_RGB_GRN;
//                     io_expander_set_output_port(0x00);
//                     lcd_draw_rectangle(165, 40, 105, 25, LCD_COLOR_BLACK);
//                     /* Updates score display, Ones Place*/
//                     switch (score % 10) {
//                         case 0:
//                             lcd_draw_image(
//                                 195,
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
//                                 number9WidthPixels,
//                                 number9HeightPixels,
//                                 number9Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                     }

//                     /* Updates score display, Tens Place*/
//                     switch ((score - (score%10))/10) {
//                         case 0:
//                             lcd_draw_image(
//                                 180,
//                                 115,
//                                 number0WidthPixels,
//                                 number0HeightPixels,
//                                 number0Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 1:
//                             lcd_draw_image(
//                                 180,
//                                 115,
//                                 number1WidthPixels,
//                                 number1HeightPixels,
//                                 number1Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
                
//                         case 2:
//                             lcd_draw_image(
//                                 180,
//                                 115,
//                                 number2WidthPixels,
//                                 number2HeightPixels,
//                                 number2Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
                
//                         case 3:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number3WidthPixels,
//                                 number3HeightPixels,
//                                 number3Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 4:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number4WidthPixels,
//                                 number4HeightPixels,
//                                 number4Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 5:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number5WidthPixels,
//                                 number5HeightPixels,
//                                 number5Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 6:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number6WidthPixels,
//                                 number6HeightPixels,
//                                 number6Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 7:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number7WidthPixels,
//                                 number7HeightPixels,
//                                 number7Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 8:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number8WidthPixels,
//                                 number8HeightPixels,
//                                 number8Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 9:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number9WidthPixels,
//                                 number9HeightPixels,
//                                 number9Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                     }

//                     upper_left_clicked = false;
//                     upper_middle_clicked = false;
//                     upper_right_clicked = false;
//                     lower_left_clicked = true;
//                     lower_middle_clicked = false;
//                     lower_right_clicked = false;

//                     lcd_draw_rectangle_centered(60, 160, 40, 40, LCD_COLOR_BLACK);
//                     lcd_draw_image(
//                         75,
//                         155,
//                         hammerWidthPixels,
//                         hammerHeightPixels,
//                         hammerBitmaps,
//                         hammer_color,
//                         LCD_COLOR_BLACK
//                     );
//                     // Clears previous based on previous location
//                     if (prev_mole_coordinates == UPPER_RIGHT) {
//                         // Clear Hammer
//                         lcd_draw_rectangle(225, 100, 0, 120, LCD_COLOR_BLACK);
//                         // Redraw Hole
//                         lcd_draw_image(
//                             260,
//                             80,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == UPPER_LEFT) {
//                         lcd_draw_rectangle(0, 120, 20, 55, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             60,
//                             80,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == LOWER_RIGHT) {
//                         lcd_draw_rectangle(225, 100, 120, 70, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             260,
//                             185,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == UPPER_MIDDLE) {
//                         lcd_draw_rectangle(125, 100, 25, 55, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             160,
//                             80,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == LOWER_MIDDLE) {
//                         lcd_draw_rectangle(125, 100, 130, 70, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             160,
//                             185,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     }
//                     int random = rand()/10000000;
//                     int next_mole = (random % 5);
//                     board[3] = 0;
//                     if(next_mole == 0) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 60,
//                                 y+55,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if ( y != 1) {
//                                 lcd_draw_image(
//                                     60,
//                                     80,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[0] = 1;
//                     } else if (next_mole == 1) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 160,
//                                 y+55,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y != 1){
//                                 lcd_draw_image(
//                                     160,
//                                     80,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[1] = 1;
//                     } else if (next_mole == 2) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 260,
//                                 y+55,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y != 1){
//                                 lcd_draw_image(
//                                     260,
//                                     80,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[2] = 1;
//                     }
//                     // BOTTOM ROW
//                     else if (next_mole == 3) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 160,
//                                 y+160,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y != 1){
//                                 lcd_draw_image(
//                                     160,
//                                     185,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[4] = 1;
//                     } else if (next_mole == 4) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 260,
//                                 y+160,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y != 1){
//                                 lcd_draw_image(
//                                     260,
//                                     185,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[5] = 1;
//                     }
//                     prev_mole_coordinates = LOWER_LEFT;
//                 } else {
//                     PORT_RGB_RED->OUT_SET = MASK_RGB_RED;
//                     pwm_buzzer_start();
//                     cyhal_system_delay_ms(250);
//                     pwm_buzzer_stop();
//                     PORT_RGB_RED->OUT_CLR = MASK_RGB_RED;
//                 }
//             } else if (coordinates.x < 190 && coordinates.x > 150 && coordinates.y > 155 && coordinates.y < 195 & !lower_middle_clicked) {
//                 if (board[4] == 1) {
//                     // lower middle
//                     score++;
//                     PORT_RGB_GRN->OUT_SET = MASK_RGB_GRN;
//                     io_expander_set_output_port(0x08);
//                     cyhal_system_delay_ms(250);
//                     PORT_RGB_GRN->OUT_CLR = MASK_RGB_GRN;
//                     io_expander_set_output_port(0x00);
//                     lcd_draw_rectangle(165, 40, 105, 25, LCD_COLOR_BLACK);
//                     /* Updates score display, Ones Place*/
//                     switch (score % 10) {
//                         case 0:
//                             lcd_draw_image(
//                                 195,
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
//                                 number9WidthPixels,
//                                 number9HeightPixels,
//                                 number9Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                     }

//                     /* Updates score display, Tens Place*/
//                     switch ((score - (score%10))/10) {
//                         case 0:
//                             lcd_draw_image(
//                                 180,
//                                 115,
//                                 number0WidthPixels,
//                                 number0HeightPixels,
//                                 number0Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 1:
//                             lcd_draw_image(
//                                 180,
//                                 115,
//                                 number1WidthPixels,
//                                 number1HeightPixels,
//                                 number1Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
                
//                         case 2:
//                             lcd_draw_image(
//                                 180,
//                                 115,
//                                 number2WidthPixels,
//                                 number2HeightPixels,
//                                 number2Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
                
//                         case 3:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number3WidthPixels,
//                                 number3HeightPixels,
//                                 number3Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 4:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number4WidthPixels,
//                                 number4HeightPixels,
//                                 number4Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 5:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number5WidthPixels,
//                                 number5HeightPixels,
//                                 number5Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 6:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number6WidthPixels,
//                                 number6HeightPixels,
//                                 number6Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 7:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number7WidthPixels,
//                                 number7HeightPixels,
//                                 number7Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 8:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number8WidthPixels,
//                                 number8HeightPixels,
//                                 number8Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 9:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number9WidthPixels,
//                                 number9HeightPixels,
//                                 number9Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                     }

//                     upper_left_clicked = false;
//                     upper_middle_clicked = false;
//                     upper_right_clicked = false;
//                     lower_left_clicked = false;
//                     lower_middle_clicked = true;
//                     lower_right_clicked = false;

//                     lcd_draw_rectangle_centered(160, 160, 40, 40, LCD_COLOR_BLACK);
//                     lcd_draw_image(
//                         175,
//                         155,
//                         hammerWidthPixels,
//                         hammerHeightPixels,
//                         hammerBitmaps,
//                         hammer_color,
//                         LCD_COLOR_BLACK
//                     );
//                     // Clears previous based on previous location
//                     if (prev_mole_coordinates == UPPER_RIGHT) {
//                         // Clear Hammer
//                         lcd_draw_rectangle(225, 100, 0, 120, LCD_COLOR_BLACK);
//                         // Redraw Hole
//                         lcd_draw_image(
//                             260,
//                             80,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == UPPER_LEFT) {
//                         lcd_draw_rectangle(0, 120, 20, 55, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             60,
//                             80,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == LOWER_RIGHT) {
//                         lcd_draw_rectangle(225, 100, 120, 70, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             260,
//                             185,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == LOWER_LEFT) {
//                         lcd_draw_rectangle(0, 120, 130, 70, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             60,
//                             185,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == UPPER_MIDDLE) {
//                         lcd_draw_rectangle(125, 100, 25, 55, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             160,
//                             80,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     }

//                     int random = rand()/10000000;
//                     int next_mole = (random % 5);
//                     board[4] = 0;
//                     if(next_mole == 0) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 60,
//                                 y+55,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y != 1){
//                                 lcd_draw_image(
//                                     160,
//                                     80,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[0] = 1;
//                     } else if (next_mole == 1) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 160,
//                                 y+55,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y != 1){
//                                 lcd_draw_image(
//                                     160,
//                                     80,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[1] = 1;
//                     } else if (next_mole == 2) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 260,
//                                 y+55,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y != 1) {
//                                 lcd_draw_image(
//                                     260,
//                                     80,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[2] = 1;
//                     }
//                     // BOTTOM ROW
//                     else if (next_mole == 3) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 60,
//                                 y+160,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y != 1){
//                                 lcd_draw_image(
//                                     60,
//                                     185,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[3] = 1;
//                     } else if (next_mole == 4) {
//                         int y = 20;
//                         while(y>0){
//                             cyhal_system_delay_ms(10);
//                             lcd_draw_image(
//                                 260,
//                                 y+160,
//                                 moleWidthPixels,
//                                 moleHeightPixels,
//                                 moleBitmaps,
//                                 LCD_COLOR_GRAY,
//                                 LCD_COLOR_BLACK
//                             );
//                             if (y != 1) {
//                                 lcd_draw_image(
//                                     260,
//                                     185,
//                                     holeWidthPixels,
//                                     holeHeightPixels,
//                                     holeBitmaps,
//                                     LCD_COLOR_BROWN,
//                                     LCD_COLOR_BLACK
//                                 );
//                             }
//                             y--;
//                         }
//                         board[5] = 1;
//                     }
//                     prev_mole_coordinates = LOWER_MIDDLE;
//                 } else {
//                     PORT_RGB_RED->OUT_SET = MASK_RGB_RED;
//                     pwm_buzzer_start();
//                     cyhal_system_delay_ms(250);
//                     pwm_buzzer_stop();
//                     PORT_RGB_RED->OUT_CLR = MASK_RGB_RED;
//                 }
//             } else if (coordinates.x < 290 && coordinates.x > 250 && coordinates.y > 155 && coordinates.y < 195 & !lower_right_clicked) {
//                 if(board[5] == 1) {
//                     // lower right
//                     score++;
//                     PORT_RGB_GRN->OUT_SET = MASK_RGB_GRN;
//                     io_expander_set_output_port(0x08);
                    
//                     cyhal_system_delay_ms(250);
//                     PORT_RGB_GRN->OUT_CLR = MASK_RGB_GRN;
//                     io_expander_set_output_port(0x00);
                   
//                     lcd_draw_rectangle(165, 40, 105, 25, LCD_COLOR_BLACK);
//                     /* Updates score display, Ones Place*/
//                     switch (score % 10) {
//                         case 0:
//                             lcd_draw_image(
//                                 195,
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
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
//                                 115,
//                                 number9WidthPixels,
//                                 number9HeightPixels,
//                                 number9Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                     }

//                     /* Updates score display, Tens Place*/
//                     switch ((score - (score%10))/10) {
//                         case 0:
//                             lcd_draw_image(
//                                 180,
//                                 115,
//                                 number0WidthPixels,
//                                 number0HeightPixels,
//                                 number0Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                         case 1:
//                             lcd_draw_image(
//                                 180,
//                                 115,
//                                 number1WidthPixels,
//                                 number1HeightPixels,
//                                 number1Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
                
//                         case 2:
//                             lcd_draw_image(
//                                 180,
//                                 115,
//                                 number2WidthPixels,
//                                 number2HeightPixels,
//                                 number2Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
                
//                         case 3:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number3WidthPixels,
//                                 number3HeightPixels,
//                                 number3Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 4:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number4WidthPixels,
//                                 number4HeightPixels,
//                                 number4Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 5:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number5WidthPixels,
//                                 number5HeightPixels,
//                                 number5Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 6:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number6WidthPixels,
//                                 number6HeightPixels,
//                                 number6Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 7:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number7WidthPixels,
//                                 number7HeightPixels,
//                                 number7Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 8:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number8WidthPixels,
//                                 number8HeightPixels,
//                                 number8Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;

//                         case 9:
//                         lcd_draw_image(
//                                 180,
//                                 115,
//                                 number9WidthPixels,
//                                 number9HeightPixels,
//                                 number9Bitmaps,
//                                 LCD_COLOR_YELLOW,
//                                 LCD_COLOR_BLACK
//                             );
//                             break;
//                     }

//                     upper_left_clicked = false;
//                     upper_middle_clicked = false;
//                     upper_right_clicked = false;
//                     lower_left_clicked = false;
//                     lower_middle_clicked = false;
//                     lower_right_clicked = true;

//                     lcd_draw_rectangle_centered(260, 160, 40, 40, LCD_COLOR_BLACK);
//                     lcd_draw_image(
//                         275,
//                         155,
//                         hammerWidthPixels,
//                         hammerHeightPixels,
//                         hammerBitmaps,
//                         hammer_color,
//                         LCD_COLOR_BLACK
//                     );
//                     // Clears previous based on previous location
//                     if (prev_mole_coordinates == UPPER_RIGHT) {
//                         // Clear Hammer
//                         lcd_draw_rectangle(225, 100, 0, 120, LCD_COLOR_BLACK);
//                         // Redraw Hole
//                         lcd_draw_image(
//                             260,
//                             80,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == UPPER_LEFT) {
//                         lcd_draw_rectangle(0, 120, 20, 55, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             60,
//                             80,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == UPPER_MIDDLE) {
//                         lcd_draw_rectangle(125, 100, 25, 55, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             160,
//                             80,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == LOWER_LEFT) {
//                         lcd_draw_rectangle(0, 120, 130, 70, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             60,
//                             185,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     } else if (prev_mole_coordinates == LOWER_MIDDLE) {
//                         lcd_draw_rectangle(125, 100, 130, 70, LCD_COLOR_BLACK);
//                         lcd_draw_image(
//                             160,
//                             185,
//                             holeWidthPixels,
//                             holeHeightPixels,
//                             holeBitmaps,
//                             LCD_COLOR_BROWN,
//                             LCD_COLOR_BLACK
//                         );
//                     }
//                     int random = rand()/10000000;
//                     int next_mole = (random % 5);
//                     board[5] = 0;
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
//                         board[0] = 1;
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
//                         board[1] = 1;
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
//                         board[2] = 1;
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
//                         board[3] = 1;
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
//                         board[4] = 1;
//                     }
//                     prev_mole_coordinates = LOWER_RIGHT;
//                 }  else {
//                     PORT_RGB_RED->OUT_SET = MASK_RGB_RED;
//                     pwm_buzzer_start();
//                     cyhal_system_delay_ms(250);
//                     pwm_buzzer_stop();
//                     PORT_RGB_RED->OUT_CLR = MASK_RGB_RED;
//                 }
//             }
//         }
//     }
// }