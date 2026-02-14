/*
 * Test application for I2C communication
 * Sends 0xAA to STM32F401RE slave
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_PATH "/dev/i2c_stm32"

int main(int argc, char *argv[])
{
    int fd;
    unsigned char data = 0xAA;
    ssize_t ret;
    
    printf("I2C Test Application\n");
    printf("====================\n\n");
    
    /* Open device */
    printf("Opening device %s...\n", DEVICE_PATH);
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        printf("\nMake sure the driver is loaded:\n");
        printf("  sudo insmod i2c_char_driver.ko\n");
        return -1;
    }
    printf("Device opened successfully!\n\n");
    
    /* Write data */
    printf("Sending data 0x%02X to STM32...\n", data);
    ret = write(fd, &data, 1);
    if (ret < 0) {
        perror("Write failed");
        close(fd);
        return -1;
    }
    printf("Data sent successfully! (%zd bytes)\n\n", ret);
    
    /* Optional: Read response */
    printf("Reading response from STM32...\n");
    unsigned char read_data[10];
    ret = read(fd, read_data, sizeof(read_data));
    if (ret > 0) {
        printf("Received %zd bytes: ", ret);
        for (int i = 0; i < ret; i++) {
            printf("0x%02X ", read_data[i]);
        }
        printf("\n");
    } else {
        printf("No data received (this is normal for write-only slave)\n");
    }
    
    /* Close device */
    close(fd);
    printf("\nDevice closed.\n");
    printf("Check STM32 LED - it should toggle when 0xAA is received!\n");
    
    return 0;
}
