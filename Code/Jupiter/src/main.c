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

static uint8_t pin_changed = 0;
static uint8_t pin_state = 0;
static uint8_t wait_for_next = 0;

INTERRUPT(Timer0_Routine, EXTI_VectTimer0)
{
    // any edge detection
    if (P33 != pin_state)
    {
        pin_state = P33; // update pin state
        pin_changed = 1; // trigger pin change
    }

    // in case a valid pin change has been registered
    if (pin_changed)
    {
        pin_changed = 0;
        switch_to_next();
    }
}

void GPIO_Init(void)
{
    // LEDs
    GPIO_P3_SetMode(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2, GPIO_Mode_Output_PP);

    // Switch
    GPIO_P3_SetMode(GPIO_Pin_3, GPIO_Mode_InOut_QBD);

    // Mux Switch Pins
    GPIO_P5_SetMode(GPIO_Pin_4 | GPIO_Pin_5, GPIO_Mode_Output_PP);
}

int main(void)
{
    // Enable system clock
    SYS_SetClock();

    // Set debug LED before the system starts
    P32 = SET;

    // Setup GPIOs
    GPIO_Init();

    // Set system clock. Remove this line if system clock is already set by STC-ISP
    SYS_SetClock();

    // Setup Timer to run every 10ms
    TIM_Timer0_Config(HAL_State_ON, TIM_TimerMode_16BitAuto, 10000);

    // Enable Interrupts
    EXTI_Timer0_SetIntState(HAL_State_ON);
    EXTI_Timer0_SetIntPriority(EXTI_IntPriority_High);
    EXTI_Global_SetIntState(HAL_State_ON);

    // Start Timer
    TIM_Timer0_SetRunState(HAL_State_ON);

    // Turn off debug LED after system startup
    P32 = RESET;

    while (1)
    {
    }
}
