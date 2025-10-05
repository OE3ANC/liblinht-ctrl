# liblinht-ctrl

A modular C library for controlling GPIO pins and PWM channels on LinHT

## Building


```bash
# Build both static and shared libraries
make

# Build example program
make example

# Clean build artifacts
make clean

# Install to system (requires sudo)
sudo make install

# Uninstall from system
sudo make uninstall

# Show help
make help
```

### Build Outputs

- `liblinht-ctrl.a` - Static library
- `liblinht-ctrl.so` - Shared library
- `example` - Example program (when built)

### Usage

```c
#include <liblinht-ctrl.h>

int main(void)
{
    // Control flashlight
    linht_ctrl_flashlight_set(true);   // Turn on
    linht_ctrl_flashlight_set(false);  // Turn off
    
    // Control LEDs
    linht_ctrl_green_led_set(true);
    linht_ctrl_red_led_set(true);
    
    // Set LCD backlight to 50% brightness
    linht_ctrl_lcd_backlight_set_brightness(128);
    
    // Set keyboard backlight to full brightness
    linht_ctrl_kbd_backlight_on();
    
    return 0;
}
```


## API Reference

### GPIO Functions

#### Specific GPIO Controls

Pre-configured functions for common hardware controls. These handle chip selection internally.

```c
int linht_ctrl_flashlight_set(bool state);
```
Control the device flashlight (GPIO2 pin 18).
- **Parameters:** `state` - true to turn on, false to turn off
- **Returns:** 0 on success, -1 on error

```c
int linht_ctrl_green_led_set(bool state);
int linht_ctrl_red_led_set(bool state);
```
Control the status LEDs (GPIO2 pins 4 and 5).
- **Parameters:** `state` - true to turn on, false to turn off
- **Returns:** 0 on success, -1 on error

```c
int linht_ctrl_tx_rx_switch_set(bool tx_mode);
```
Control the TX/RX RF switch (GPIO2 pin 13).
- **Parameters:** `tx_mode` - true for TX mode, false for RX mode
- **Returns:** 0 on success, -1 on error

#### Generic GPIO Control

Direct control of any GPIO pin on any chip with full configuration options.

```c
int linht_ctrl_gpio_set(const char* gpio_chip_path, uint16_t pin_offset, bool state);
```
Set the state of a GPIO pin.
- **Parameters:**
  - `gpio_chip_path` - Path to GPIO chip (e.g., `LINHT_CTRL_GPIO2_PATH`)
  - `pin_offset` - Pin number on the chip
  - `state` - true for active, false for inactive
- **Returns:** 0 on success, -1 on error

```c
int linht_ctrl_gpio_configure(const char* gpio_chip_path, uint16_t pin_offset,
                              const linht_ctrl_gpio_config_t* config);
```
Configure a GPIO pin with custom settings.
- **Parameters:**
  - `gpio_chip_path` - Path to GPIO chip
  - `pin_offset` - Pin number on the chip
  - `config` - Configuration structure with direction, bias, and active level
- **Returns:** 0 on success, -1 on error

**GPIO Configuration Structure:**
```c
typedef struct {
    linht_ctrl_gpio_direction_t direction;  // INPUT or OUTPUT
    linht_ctrl_gpio_bias_t bias;            // DISABLED, PULL_UP, or PULL_DOWN
    linht_ctrl_gpio_active_t active_level;  // ACTIVE_HIGH or ACTIVE_LOW
} linht_ctrl_gpio_config_t;
```

### PWM Functions

#### Backlight Controls

Convenient functions for LCD and keyboard backlight management.

```c
int linht_ctrl_lcd_backlight_set_brightness(uint8_t brightness);
int linht_ctrl_kbd_backlight_set_brightness(uint8_t brightness);
```
Set backlight brightness.
- **Parameters:** `brightness` - 0 (off) to 255 (maximum)
- **Returns:** 0 on success, -1 on error

```c
int linht_ctrl_lcd_backlight_on(void);
int linht_ctrl_kbd_backlight_on(void);
```
Turn backlight on at full brightness (255).
- **Returns:** 0 on success, -1 on error

```c
int linht_ctrl_lcd_backlight_off(void);
int linht_ctrl_kbd_backlight_off(void);
```
Turn backlight off (brightness 0).
- **Returns:** 0 on success, -1 on error

#### Generic PWM Control

Direct control of PWM channels for custom applications.

```c
int linht_ctrl_pwm_export(uint8_t channel);
```
Export a PWM channel for use.
- **Parameters:** `channel` - PWM channel number
- **Returns:** 0 on success, -1 on error

```c
int linht_ctrl_pwm_unexport(uint8_t channel);
```
Unexport a PWM channel.
- **Parameters:** `channel` - PWM channel number
- **Returns:** 0 on success, -1 on error

```c
int linht_ctrl_pwm_enable(uint8_t channel);
int linht_ctrl_pwm_disable(uint8_t channel);
```
Enable or disable PWM output.
- **Parameters:** `channel` - PWM channel number
- **Returns:** 0 on success, -1 on error

```c
int linht_ctrl_pwm_configure(uint8_t channel, uint32_t period_ns, uint8_t duty_cycle);
```
Configure PWM period and duty cycle.
- **Parameters:**
  - `channel` - PWM channel number
  - `period_ns` - Period in nanoseconds (default: 50000 for 20kHz)
  - `duty_cycle` - Duty cycle percentage (0-100)
- **Returns:** 0 on success, -1 on error

