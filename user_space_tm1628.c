#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

// Paths for sysfs GPIO
#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO_UNEXPORT "/sys/class/gpio/unexport"

// Global GPIO numbers (using GPIO2 base offset: 512)
#define GPIO_STB  530  // GPIO2_IO18 = 512 + 18
#define GPIO_DIO  531  // GPIO2_IO19 = 512 + 19
#define GPIO_CLK  533  // GPIO2_IO21 = 512 + 21

// Helper function to write a string to a file
int write_to_file(const char *path, const char *value) {
    int fd = open(path, O_WRONLY);
    if(fd < 0) {
        perror("Unable to open file");
        return -1;
    }
    if(write(fd, value, strlen(value)) < 0) {
        perror("Error writing to file");
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

// Export a GPIO pin
void gpio_export(int pin) {
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%d", pin);
    write_to_file(GPIO_EXPORT, buffer);
    usleep(100000);
}

// Set GPIO direction ("in" or "out")
void gpio_set_direction(int pin, const char *direction) {
    char path[50];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    write_to_file(path, direction);
}

// Write a value (0 or 1) to a GPIO
void gpio_write(int pin, int value) {
    char path[50];
    char valStr[2];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    snprintf(valStr, sizeof(valStr), "%d", value);
    write_to_file(path, valStr);
}

// Simple delay function in microseconds
void delay_us(int us) {
    usleep(us);
}

// Send one byte to TM1628 via bit-banging (LSB first)
void tm1628_send_byte(unsigned char data) {
    for (int i = 0; i < 8; i++) {
        gpio_write(GPIO_CLK, 0);
        delay_us(5);
        gpio_write(GPIO_DIO, (data >> i) & 0x01);
        delay_us(5);
        gpio_write(GPIO_CLK, 1);
        delay_us(5);
    }
}

// Send a command to TM1628
void tm1628_send_command(unsigned char command) {
    gpio_write(GPIO_STB, 0);
    delay_us(5);
    tm1628_send_byte(command);
    delay_us(5);
    gpio_write(GPIO_STB, 1);
    delay_us(5);
}

// Set brightness by sending display control command
// Brightness level: 0 (off) to 15 (max). Command = 0x80 | (level & 0x0F)
 void tm1628_set_brightness(unsigned char level)
 {
     unsigned char cmd = 0x80 | (level & 0x0F);
         tm1628_send_command(cmd);
 }

// TM1628 initialization sequence
void tm1628_init() {
    // Export and configure GPIO pins
    gpio_export(GPIO_STB);
    gpio_export(GPIO_CLK);
    gpio_export(GPIO_DIO);
    
    gpio_set_direction(GPIO_STB, "out");
    gpio_set_direction(GPIO_CLK, "out");
    gpio_set_direction(GPIO_DIO, "out");
    
    // 1. Set display mode (6 grids x 11 segments; command 0x02 as per datasheet)
    tm1628_send_command(0x02);
    
    // 2. Data command: auto-increment mode (0x40)
    tm1628_send_command(0x40);
    
    // 3. Display control: turn on display with maximum brightness (0x8F)
    //tm1628_send_command(0x8F);
    tm1628_set_brightness(10); // Set brightness (15 = maximum). Adjust as needed.

}

// Segment map for digits 0-9 (common cathode)
// Bits 0-6: segments a,b,c,d,e,f,g; Bit 7 (0x80) controls the decimal point (dp)
const unsigned char digit_map[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

// Grid addresses for 6 grids
const unsigned char grid_addresses[6] = {0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA};

// Display a pattern on all 6 grids; pattern is an array of 6 bytes (one per grid)
void tm1628_display_pattern(const unsigned char pattern[6]) {
    gpio_write(GPIO_STB, 0);
    delay_us(5);
    for (int i = 0; i < 6; i++) {
        tm1628_send_byte(grid_addresses[i]);
        tm1628_send_byte(pattern[i]);
    }
    gpio_write(GPIO_STB, 1);
    delay_us(5);
}

// Display a repeated digit on all grids with dp turned on (e.g. 1.1.1.1.1.1)
void display_repeated_dp(unsigned char digit) {
    unsigned char pattern[6];
    unsigned char dp_mask = 0x80;  // always on for this function
    for (int i = 0; i < 6; i++) {
        pattern[i] = digit_map[digit] | dp_mask;
    }
    tm1628_display_pattern(pattern);
}

// Display the current time in HHMMSS format with dp on grid1 and grid3 to form "HH.MM.SS"
void display_time() {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    int hour = tm_info->tm_hour;
    int min  = tm_info->tm_min;
    int sec  = tm_info->tm_sec;
    
    unsigned char pattern[6];
    // Grid0: hour tens (no dp)
    pattern[0] = digit_map[hour / 10];
    // Grid1: hour ones (dp ON)
    pattern[1] = digit_map[hour % 10] | 0x80;
    // Grid2: minute tens (no dp)
    pattern[2] = digit_map[min / 10];
    // Grid3: minute ones (dp ON)
    pattern[3] = digit_map[min % 10] | 0x80;
    // Grid4: second tens (no dp)
    pattern[4] = digit_map[sec / 10];
    // Grid5: second ones (no dp)
    pattern[5] = digit_map[sec % 10];
    
    tm1628_display_pattern(pattern);
}

int main() {
    printf("Initializing TM1628...\n");
    tm1628_init();
    delay_us(1000000);
    
    // Display repeated patterns for digits 0-9 with dp (only one display per digit)
    for (unsigned char d = 0; d <= 9; d++) {
        printf("Displaying '%d.%d.%d.%d.%d.%d' on the LED grid (with dp)...\n", d, d, d, d, d, d);
        display_repeated_dp(d);
        sleep(1);
    }
    
    // Continuously display the current time in HHMMSS format with dp on grid1 and grid3
    printf("Displaying current time (HH.MM.SS) on the LED grid...\n");
    while (1) {
        display_time();
        sleep(1);
    }
    
    return 0;
}

