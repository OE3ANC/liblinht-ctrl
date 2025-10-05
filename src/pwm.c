#include "liblinht-ctrl.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// Internal helper to write a value to a sysfs file
static int pwm_write_sysfs(const char* path, const char* value)
{
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        return -1;
    }
    
    ssize_t len = strlen(value);
    ssize_t written = write(fd, value, len);
    close(fd);
    
    if (written != len) {
        return -1;
    }
    return 0;
}

// Export a PWM channel for use
int linht_ctrl_pwm_export(uint8_t channel)
{
    char path[256];
    snprintf(path, sizeof(path), "%s/pwm%d", LINHT_CTRL_PWM_CHIP_PATH, channel);
    
    if (access(path, F_OK) == 0) {
        return 0;
    }
    
    char value[16];
    snprintf(path, sizeof(path), "%s/export", LINHT_CTRL_PWM_CHIP_PATH);
    snprintf(value, sizeof(value), "%d", channel);
    return pwm_write_sysfs(path, value);
}

// Unexport a PWM channel
int linht_ctrl_pwm_unexport(uint8_t channel)
{
    char path[256];
    snprintf(path, sizeof(path), "%s/pwm%d", LINHT_CTRL_PWM_CHIP_PATH, channel);
    
    if (access(path, F_OK) != 0) {
        return 0;
    }
    
    char value[16];
    snprintf(path, sizeof(path), "%s/unexport", LINHT_CTRL_PWM_CHIP_PATH);
    snprintf(value, sizeof(value), "%d", channel);
    return pwm_write_sysfs(path, value);
}

// Enable PWM output on a channel
int linht_ctrl_pwm_enable(uint8_t channel)
{
    if (linht_ctrl_pwm_export(channel) != 0) {
        return -1;
    }
    
    char path[256];
    snprintf(path, sizeof(path), "%s/pwm%d/enable", LINHT_CTRL_PWM_CHIP_PATH, channel);
    return pwm_write_sysfs(path, "1");
}

// Disable PWM output on a channel
int linht_ctrl_pwm_disable(uint8_t channel)
{
    char path[256];
    snprintf(path, sizeof(path), "%s/pwm%d", LINHT_CTRL_PWM_CHIP_PATH, channel);
    
    if (access(path, F_OK) != 0) {
        return 0;
    }
    
    snprintf(path, sizeof(path), "%s/pwm%d/enable", LINHT_CTRL_PWM_CHIP_PATH, channel);
    return pwm_write_sysfs(path, "0");
}

// Configure PWM period and duty cycle
int linht_ctrl_pwm_configure(uint8_t channel, uint32_t period_ns, uint8_t duty_cycle)
{
    if (duty_cycle > 100) {
        return -1;
    }
    
    if (linht_ctrl_pwm_export(channel) != 0) {
        return -1;
    }
    
    char path[256];
    char value[32];
    
    snprintf(path, sizeof(path), "%s/pwm%d/period", LINHT_CTRL_PWM_CHIP_PATH, channel);
    snprintf(value, sizeof(value), "%u", period_ns);
    if (pwm_write_sysfs(path, value) != 0) {
        return -1;
    }
    
    snprintf(path, sizeof(path), "%s/pwm%d/duty_cycle", LINHT_CTRL_PWM_CHIP_PATH, channel);
    uint32_t duty_ns = (period_ns * duty_cycle) / 100;
    snprintf(value, sizeof(value), "%u", duty_ns);
    return pwm_write_sysfs(path, value);
}
