#include "liblinht-ctrl.h"
#include <gpiod.h>

// Helper function to configure bias settings
static void configure_bias(struct gpiod_line_settings *settings, linht_ctrl_gpio_bias_t bias)
{
    switch (bias) {
        case LINHT_CTRL_GPIO_BIAS_PULL_UP:
            gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_UP);
            break;
        case LINHT_CTRL_GPIO_BIAS_PULL_DOWN:
            gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_DOWN);
            break;
        default:
            gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_DISABLED);
            break;
    }
}

// Internal helper to cleanup a GPIO line
static void gpio_cleanup_line(struct gpiod_line_request **line_request)
{
    if (!line_request) {
        return;
    }

    if (*line_request) {
        gpiod_line_request_release(*line_request);
        *line_request = NULL;
    }
}

// Simplified GPIO initialization function
static int gpio_init_line(struct gpiod_chip *chip, uint16_t pin_offset,
                          struct gpiod_line_request **line_request,
                          linht_ctrl_gpio_direction_t direction,
                          linht_ctrl_gpio_bias_t bias,
                          linht_ctrl_gpio_active_t active_level)
{
    // Validate inputs
    if (!chip || !line_request) {
        return -1;
    }
    
    // Validate pin offset
    struct gpiod_chip_info *info = gpiod_chip_get_info(chip);
    if (!info) {
        return -1;
    }
    
    unsigned int num_lines = gpiod_chip_info_get_num_lines(info);
    gpiod_chip_info_free(info);
    
    if (pin_offset >= num_lines) {
        return -1;
    }

    // Clean up existing line request
    gpio_cleanup_line(line_request);

    // Create and configure line settings
    struct gpiod_line_settings *settings = gpiod_line_settings_new();
    if (!settings) {
        return -1;
    }

    // Configure direction
    gpiod_line_settings_set_direction(settings,
        (direction == LINHT_CTRL_GPIO_OUTPUT) ? GPIOD_LINE_DIRECTION_OUTPUT : GPIOD_LINE_DIRECTION_INPUT);
    
    if (direction == LINHT_CTRL_GPIO_OUTPUT) {
        gpiod_line_settings_set_output_value(settings, GPIOD_LINE_VALUE_INACTIVE);
    } else {
        configure_bias(settings, bias);
    }

    // Configure active level
    gpiod_line_settings_set_active_low(settings, (active_level == LINHT_CTRL_GPIO_ACTIVE_LOW));

    // Create line config
    struct gpiod_line_config *line_cfg = gpiod_line_config_new();
    if (!line_cfg) {
        gpiod_line_settings_free(settings);
        return -1;
    }

    // Add line settings to config
    unsigned int offsets[] = {pin_offset};
    if (gpiod_line_config_add_line_settings(line_cfg, offsets, 1, settings) != 0) {
        gpiod_line_config_free(line_cfg);
        gpiod_line_settings_free(settings);
        return -1;
    }

    // Create request config
    struct gpiod_request_config *req_cfg = gpiod_request_config_new();
    if (!req_cfg) {
        gpiod_line_config_free(line_cfg);
        gpiod_line_settings_free(settings);
        return -1;
    }
    gpiod_request_config_set_consumer(req_cfg, "liblinht-ctrl");

    // Request the line
    *line_request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
    
    // Clean up all config objects
    gpiod_request_config_free(req_cfg);
    gpiod_line_config_free(line_cfg);
    gpiod_line_settings_free(settings);

    if (!*line_request) {
        return -1;
    }

    return 0;
}

// Specific GPIO controls
int linht_ctrl_flashlight_set(bool state)
{
    return linht_ctrl_gpio_set(LINHT_CTRL_GPIO2_PATH, LINHT_CTRL_FLASHLIGHT_PIN, state);
}

int linht_ctrl_green_led_set(bool state)
{
    return linht_ctrl_gpio_set(LINHT_CTRL_GPIO2_PATH, LINHT_CTRL_GREEN_LED_PIN, state);
}

int linht_ctrl_red_led_set(bool state)
{
    return linht_ctrl_gpio_set(LINHT_CTRL_GPIO2_PATH, LINHT_CTRL_RED_LED_PIN, state);
}

int linht_ctrl_tx_rx_switch_set(bool tx_mode)
{
    return linht_ctrl_gpio_set(LINHT_CTRL_GPIO2_PATH, LINHT_CTRL_TX_RX_PIN, tx_mode);
}

// Generic GPIO control
int linht_ctrl_gpio_set(const char* gpio_chip_path, uint16_t pin_offset, bool state)
{
    if (!gpio_chip_path) {
        return -1;
    }

    // Open the GPIO chip
    struct gpiod_chip *chip = gpiod_chip_open(gpio_chip_path);
    if (!chip) {
        return -1;
    }

    // Initialize line as output (active-low for LEDs and other hardware)
    struct gpiod_line_request *line_request = NULL;
    if (gpio_init_line(chip, pin_offset, &line_request, LINHT_CTRL_GPIO_OUTPUT,
                      LINHT_CTRL_GPIO_BIAS_DISABLED, LINHT_CTRL_GPIO_ACTIVE_LOW) != 0) {
        gpiod_chip_close(chip);
        return -1;
    }

    // Set value
    enum gpiod_line_value values[] = {state ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE};
    int result = 0;
    if (gpiod_line_request_set_values(line_request, values) < 0) {
        result = -1;
    }

    // Cleanup
    gpio_cleanup_line(&line_request);
    gpiod_chip_close(chip);

    return result;
}

int linht_ctrl_gpio_configure(const char* gpio_chip_path, uint16_t pin_offset,
                             const linht_ctrl_gpio_config_t* config)
{
    if (!gpio_chip_path || !config) {
        return -1;
    }

    // Open the GPIO chip
    struct gpiod_chip *chip = gpiod_chip_open(gpio_chip_path);
    if (!chip) {
        return -1;
    }

    // Initialize line with specified direction, bias, and active level
    struct gpiod_line_request *line_request = NULL;
    int result = gpio_init_line(chip, pin_offset, &line_request,
                               config->direction, config->bias, config->active_level);

    // Cleanup
    gpio_cleanup_line(&line_request);
    gpiod_chip_close(chip);

    return result;
}