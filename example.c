#include "liblinht-ctrl.h"
#include <unistd.h>

#define TRANSITION_DELAY_US 10000  // Delay between brightness steps (10ms)

// Smooth brightness transition function
void smooth_transition(uint8_t start_brightness, uint8_t end_brightness)
{
    if (start_brightness > end_brightness) {
        // Fade down
        for (int brightness = start_brightness; brightness >= end_brightness; brightness--) {
            linht_ctrl_lcd_backlight_set_brightness(brightness);
            linht_ctrl_kbd_backlight_set_brightness(brightness);
            usleep(TRANSITION_DELAY_US);
        }
    } else {
        // Fade up
        for (int brightness = start_brightness; brightness <= end_brightness; brightness++) {
            linht_ctrl_lcd_backlight_set_brightness(brightness);
            linht_ctrl_kbd_backlight_set_brightness(brightness);
            usleep(TRANSITION_DELAY_US);
        }
    }
}

int main(void)
{
    // 1. Test green LED using specific function
    linht_ctrl_green_led_set(true);
    sleep(1);
    linht_ctrl_green_led_set(false);
    
    // 2. Test red LED using generic GPIO function
    linht_ctrl_gpio_set(LINHT_CTRL_GPIO2_PATH, LINHT_CTRL_RED_LED_PIN, true);
    sleep(1);
    linht_ctrl_gpio_set(LINHT_CTRL_GPIO2_PATH, LINHT_CTRL_RED_LED_PIN, false);
    
    // 3. Test backlight control - turn off both backlights
    linht_ctrl_lcd_backlight_off();
    linht_ctrl_kbd_backlight_off();
    sleep(1);
    
    // 4. Test backlight control - turn on both backlights
    linht_ctrl_lcd_backlight_on();
    linht_ctrl_kbd_backlight_on();
    sleep(2);
    
    // 5. Test smooth transition from 255 to 0
    smooth_transition(255, 0);
    sleep(1);
    
    // 6. Test smooth transition from 0 to 255
    smooth_transition(0, 255);
    sleep(1);
    
    // 7. Final state - turn everything off
    linht_ctrl_green_led_set(false);
    linht_ctrl_gpio_set(LINHT_CTRL_GPIO2_PATH, LINHT_CTRL_RED_LED_PIN, false);
    
    return 0;
}