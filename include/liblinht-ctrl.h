#ifndef LIBLINHT_CTRL_H
#define LIBLINHT_CTRL_H

#include <stdint.h>
#include <stdbool.h>

// GPIO Chip Definitions
#define LINHT_CTRL_GPIO1_PATH    "/dev/gpiochip3"  // GPIO1 [47400000.gpio]
#define LINHT_CTRL_GPIO2_PATH    "/dev/gpiochip0"  // GPIO2 [43810000.gpio]
#define LINHT_CTRL_GPIO3_PATH    "/dev/gpiochip1"  // GPIO3 [43820000.gpio]
#define LINHT_CTRL_GPIO4_PATH    "/dev/gpiochip2"  // GPIO4 [43830000.gpio]

// GPIO Pin Definitions
#define LINHT_CTRL_FLASHLIGHT_PIN   18  // MX93_PAD_GPIO_IO18__GPIO2_IO18
#define LINHT_CTRL_GREEN_LED_PIN    4   // MX93_PAD_GPIO_IO05__GPIO2_IO04
#define LINHT_CTRL_RED_LED_PIN      5   // MX93_PAD_GPIO_IO05__GPIO2_IO05
#define LINHT_CTRL_TX_RX_PIN        13  // MX93_PAD_GPIO_IO13__GPIO2_IO13

// PWM Chip and Channel Definitions
#define LINHT_CTRL_PWM_CHIP_PATH    "/sys/class/pwm/pwmchip0"  // TPM3 PWM controller
#define LINHT_CTRL_LCD_PWM_CHANNEL  3   // TPM3_CH3
#define LINHT_CTRL_KBD_PWM_CHANNEL  2   // TPM3_CH2

// PWM Pin Definitions
#define LINHT_CTRL_LCD_PWM_PIN      24  // GPIO2_IO[24] - Pin 99 - LCD_PWM
#define LINHT_CTRL_KBD_PWM_PIN      12  // GPIO2_IO[12] - Pin 105 - KBD_PWM

// PWM Configuration
#define LINHT_CTRL_PWM_DEFAULT_PERIOD_NS  50000  // 20kHz default period
#define LINHT_CTRL_PWM_MAX_BRIGHTNESS     255    // Maximum brightness value

// GPIO Direction Options
typedef enum {
    LINHT_CTRL_GPIO_INPUT,
    LINHT_CTRL_GPIO_OUTPUT
} linht_ctrl_gpio_direction_t;

// GPIO Bias Options
typedef enum {
    LINHT_CTRL_GPIO_BIAS_DISABLED,
    LINHT_CTRL_GPIO_BIAS_PULL_UP,
    LINHT_CTRL_GPIO_BIAS_PULL_DOWN
} linht_ctrl_gpio_bias_t;

// GPIO Active Level Options
typedef enum {
    LINHT_CTRL_GPIO_ACTIVE_HIGH,  // Normal: HIGH=active, LOW=inactive
    LINHT_CTRL_GPIO_ACTIVE_LOW   // Inverted: LOW=active, HIGH=inactive
} linht_ctrl_gpio_active_t;

// GPIO Configuration Structure
typedef struct {
    linht_ctrl_gpio_direction_t direction;
    linht_ctrl_gpio_bias_t bias;
    linht_ctrl_gpio_active_t active_level;
} linht_ctrl_gpio_config_t;

// Specific GPIO controls (these handle the chip selection internally)
int linht_ctrl_flashlight_set(bool state);
int linht_ctrl_green_led_set(bool state);
int linht_ctrl_red_led_set(bool state);
int linht_ctrl_tx_rx_switch_set(bool tx_mode);  // true=TX, false=RX

// Generic GPIO control (requires explicit chip specification)
int linht_ctrl_gpio_set(const char* gpio_chip_path, uint16_t pin_offset, bool state);
int linht_ctrl_gpio_configure(const char* gpio_chip_path, uint16_t pin_offset,
                             const linht_ctrl_gpio_config_t* config);

// LCD backlight control
int linht_ctrl_lcd_backlight_set_brightness(uint8_t brightness);  // 0-255
int linht_ctrl_lcd_backlight_on(void);
int linht_ctrl_lcd_backlight_off(void);

// Keyboard backlight control
int linht_ctrl_kbd_backlight_set_brightness(uint8_t brightness);   // 0-255
int linht_ctrl_kbd_backlight_on(void);
int linht_ctrl_kbd_backlight_off(void);

// Generic PWM control functions
int linht_ctrl_pwm_enable(uint8_t channel);
int linht_ctrl_pwm_disable(uint8_t channel);
int linht_ctrl_pwm_configure(uint8_t channel, uint32_t period_ns, uint8_t duty_cycle);
int linht_ctrl_pwm_export(uint8_t channel);
int linht_ctrl_pwm_unexport(uint8_t channel);

#endif // LIBLINHT_CTRL_H