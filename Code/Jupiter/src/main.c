/***
 *
 *  JUPITER Eurorack Module Firmware
 *
 *  Pin connection:
 *
 *                                  ___
 *         MUX Bit 1 <---    P5.4 -|   |- P3.3    <--- SWITCH
 *                5V --->     VCC -|   |- P3.2    ---> DEBUG LED
 *         MUX Bit 2 <---    P5.5 -|   |- TX      ---> LED 2
 *               GND --->     GND -|___|- RX      ---> LED 1
 *
 */

#include "fw_hal.h"

#define LOWPASS_24_DB 0
#define LOWPASS_12_DB 1
#define BANDPASS_12_DB 2
#define HIGHPASS_24_DB 3

void switch_to_next()
{
    static uint8_t filter_type = HIGHPASS_24_DB;

    switch (filter_type)
    {
    case LOWPASS_24_DB:
        filter_type = LOWPASS_12_DB;

        // LEDS
        P30 = RESET;
        P31 = RESET;

        // Mux
        P54 = RESET;
        P55 = RESET;
        break;
    case LOWPASS_12_DB:
        filter_type = BANDPASS_12_DB;

        // LEDS
        P30 = SET;
        P31 = RESET;

        // Mux
        P54 = SET;
        P55 = RESET;
        break;
    case BANDPASS_12_DB:
        filter_type = HIGHPASS_24_DB;

        // LEDS
        P30 = SET;
        P31 = SET;

        // Mux
        P54 = RESET;
        P55 = SET;
        break;
    case HIGHPASS_24_DB:
        filter_type = LOWPASS_24_DB;

        // LEDS
        P30 = RESET;
        P31 = SET;

        // Mux
        P54 = SET;
        P55 = SET;
        break;

    default:
        // unreachable
        break;
    }
}

INTERRUPT(Int3_Routine, EXTI_VectInt3)
{
    // detect rising edge
    if (P33)
    {
        switch_to_next();
    }
}

void GPIO_Init(void)
{
    // LEDs
    GPIO_P3_SetMode(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2, GPIO_Mode_Output_PP);

    // Switch
    GPIO_P3_SetMode(GPIO_Pin_3, GPIO_Mode_Input_HIP);

    // Mux Switch Pins
    GPIO_P5_SetMode(GPIO_Pin_4 | GPIO_Pin_5, GPIO_Mode_Output_PP);
}

int main(void)
{
    GPIO_Init();

    // init LEDs and Mux
    switch_to_next();

    // Enable interrupts for Int3
    EXTI_Int3_SetIntState(HAL_State_ON);

    // Enable interrupts globally
    EXTI_Global_SetIntState(HAL_State_ON);

    // Turn Debug LED on
    P32 = SET;

    while (1)
    {
    }
}
