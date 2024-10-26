// /**
//  * @file project.c
//  * @author your name (you@domain.com)
//  * @brief 
//  * @version 0.1
//  * @date 2023-12-08
//  * 
//  * @copyright Copyright (c) 2023
//  * 
//  */

// #include "project.h"
// #include "task_lcd.h"
// #include "task_buttons.h"
// #include "task_uart.h"

// #if defined(project)

// char ICE_DESCRIPTION[] = "Final project - Whack a mole";

// /*****************************************************************************/
// /* Global Variables                                                          */
// /*****************************************************************************/
// int hammer_color = LCD_COLOR_WHITE;
// extern int score;
// int high_score = 00;

// /**
//  * @brief
//  * This function will initialize all of the hardware resources for
//  * the ICE
//  */
// void peripheral_init(void)
// {
//     /* Initialize the i2C bus */
//     i2c_init();

//     ece353_enable_lcd();

//     cap_touch_irq_enable();

//     push_buttons_init();

//     task_buttons_init();

//     leds_init();

//     task_uart_init();

//     remote_uart_init();

//     spi_init();

//     project_timer_init();

//     eeprom_cs_init();
//     pwm_buzzer_init();
//     io_expander_set_configuration(0x80); 	// Set bit 7 as input, bits 6-0 as outputs 
// 	io_expander_set_output_port(0x00); 		// Turn OFF all LEDs
// }

// /*****************************************************************************/
// /* Application Code                                                          */
// /*****************************************************************************/
// /**
//  * @brief
//  * This function implements the behavioral requirements for the project
//  */
// void main_app(void)
// {
//     lcd_init();

//     // Main menu 
//     lcd_draw_image(
//         320/2,
//         200/2,
//         Whack_a_mole_intro_width,
//         Whack_a_mole_intro_height,
//         Whack_a_mole_intro_bitmaps,
//         LCD_COLOR_BLUE,
//         LCD_COLOR_BLACK
//     );

//     // Start button
//     lcd_draw_image(
//         320/2,
//         400/2,
//         startWidthPixels,
//         startHeightPixels,
//         start_button_bitmaps,
//         LCD_COLOR_WHITE,
//         LCD_COLOR_BLACK
//     );


//     /* Start the Scheduler */
//     vTaskStartScheduler();

//     while (1)
//     {
//     };
// }


// //*****************************************************************************
// //
// //! \brief Application defined malloc failed hook
// //!
// //! \param  none
// //!
// //! \return none
// //!
// //*****************************************************************************
// void vApplicationMallocFailedHook()
// {
//     /* Handle Memory Allocation Errors */
//     while (1)
//     {
//     }
// }

// //*****************************************************************************
// //
// //! \brief Application defined stack overflow hook
// //!
// //! \param  none
// //!
// //! \return none
// //!
// //*****************************************************************************
// void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
// {
//     // Handle FreeRTOS Stack Overflow
//     while (1)
//     {
//     }
// }

// #endif