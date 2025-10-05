#include "liblinht-ctrl.h"

// Internal backlight type
typedef enum {
    LINHT_CTRL_BACKLIGHT_LCD,
    LINHT_CTRL_BACKLIGHT_KBD
} linht_ctrl_backlight_type_t;

static uint8_t backlight_get_channel(linht_ctrl_backlight_type_t type)
{
    return (type == LINHT_CTRL_BACKLIGHT_LCD) ? LINHT_CTRL_LCD_PWM_CHANNEL : LINHT_CTRL_KBD_PWM_CHANNEL;
}

static int backlight_set_brightness(linht_ctrl_backlight_type_t type, uint8_t brightness)
{
    // Map brightness (0-255) to duty cycle (0-100%)
    uint8_t duty_cycle = (brightness * 100) / LINHT_CTRL_PWM_MAX_BRIGHTNESS;
    uint8_t channel = backlight_get_channel(type);
    
    if (linht_ctrl_pwm_configure(channel, LINHT_CTRL_PWM_DEFAULT_PERIOD_NS, duty_cycle) != 0) {
        return -1;
    }
    
    return linht_ctrl_pwm_enable(channel);
}

static int backlight_on(linht_ctrl_backlight_type_t type)
{
    return backlight_set_brightness(type, LINHT_CTRL_PWM_MAX_BRIGHTNESS);
}

static int backlight_off(linht_ctrl_backlight_type_t type)
{
    return backlight_set_brightness(type, 0);
}

// LCD backlight control - Convenience wrappers
int linht_ctrl_lcd_backlight_set_brightness(uint8_t brightness)
{
    return backlight_set_brightness(LINHT_CTRL_BACKLIGHT_LCD, brightness);
}


int linht_ctrl_lcd_backlight_on(void)
{
    return backlight_on(LINHT_CTRL_BACKLIGHT_LCD);
}

int linht_ctrl_lcd_backlight_off(void)
{
    return backlight_off(LINHT_CTRL_BACKLIGHT_LCD);
}

// Keyboard backlight control - Convenience wrappers
int linht_ctrl_kbd_backlight_set_brightness(uint8_t brightness)
{
    return backlight_set_brightness(LINHT_CTRL_BACKLIGHT_KBD, brightness);
}

int linht_ctrl_kbd_backlight_on(void)
{
    return backlight_on(LINHT_CTRL_BACKLIGHT_KBD);
}

int linht_ctrl_kbd_backlight_off(void)
{
    return backlight_off(LINHT_CTRL_BACKLIGHT_KBD);
}