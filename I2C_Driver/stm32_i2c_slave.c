/*
 * STM32F401RE Bare Metal I2C Slave
 * I2C1 configured as slave at address 0x30
 * Receives data from Raspberry Pi 4
 * 
 * Connections:
 * PB8 - I2C1_SCL
 * PB9 - I2C1_SDA
 * PA5 - LED (Built-in LED on Nucleo board)
 */

/* STM32F401RE Register Definitions */
#define RCC_BASE            0x40023800
#define RCC_AHB1ENR         (*(volatile unsigned int *)(RCC_BASE + 0x30))
#define RCC_APB1ENR         (*(volatile unsigned int *)(RCC_BASE + 0x40))

#define GPIOA_BASE          0x40020000
#define GPIOA_MODER         (*(volatile unsigned int *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR           (*(volatile unsigned int *)(GPIOA_BASE + 0x14))

#define GPIOB_BASE          0x40020400
#define GPIOB_MODER         (*(volatile unsigned int *)(GPIOB_BASE + 0x00))
#define GPIOB_OTYPER        (*(volatile unsigned int *)(GPIOB_BASE + 0x04))
#define GPIOB_PUPDR         (*(volatile unsigned int *)(GPIOB_BASE + 0x0C))
#define GPIOB_AFRL          (*(volatile unsigned int *)(GPIOB_BASE + 0x20))
#define GPIOB_AFRH          (*(volatile unsigned int *)(GPIOB_BASE + 0x24))

#define I2C1_BASE           0x40005400
#define I2C1_CR1            (*(volatile unsigned int *)(I2C1_BASE + 0x00))
#define I2C1_CR2            (*(volatile unsigned int *)(I2C1_BASE + 0x04))
#define I2C1_OAR1           (*(volatile unsigned int *)(I2C1_BASE + 0x08))
#define I2C1_OAR2           (*(volatile unsigned int *)(I2C1_BASE + 0x0C))
#define I2C1_DR             (*(volatile unsigned int *)(I2C1_BASE + 0x10))
#define I2C1_SR1            (*(volatile unsigned int *)(I2C1_BASE + 0x14))
#define I2C1_SR2            (*(volatile unsigned int *)(I2C1_BASE + 0x18))
#define I2C1_CCR            (*(volatile unsigned int *)(I2C1_BASE + 0x1C))
#define I2C1_TRISE          (*(volatile unsigned int *)(I2C1_BASE + 0x20))

/* RCC Enable Bits */
#define RCC_AHB1ENR_GPIOAEN (1 << 0)
#define RCC_AHB1ENR_GPIOBEN (1 << 1)
#define RCC_APB1ENR_I2C1EN  (1 << 21)

/* I2C CR1 Register Bits */
#define I2C_CR1_PE          (1 << 0)
#define I2C_CR1_ACK         (1 << 10)
#define I2C_CR1_SWRST       (1 << 15)

/* I2C SR1 Register Bits */
#define I2C_SR1_ADDR        (1 << 1)
#define I2C_SR1_RXNE        (1 << 6)
#define I2C_SR1_TXE         (1 << 7)
#define I2C_SR1_STOPF       (1 << 4)
#define I2C_SR1_BTF         (1 << 2)

/* I2C OAR1 Register Bits */
#define I2C_OAR1_ADD0       (1 << 0)
#define I2C_OAR1_ADDMODE    (1 << 15)

/* Global variables */
volatile unsigned char received_data;
volatile unsigned char data_ready = 0;

/* Function prototypes */
void SystemInit(void);
void delay_ms(unsigned int ms);
void gpio_init(void);
void i2c_init(void);
void led_on(void);
void led_off(void);
void led_toggle(void);

/* System initialization - empty for bare metal */
void SystemInit(void)
{
    /* Nothing needed for basic operation */
}

/* Simple delay function */
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 16000; j++) {
            __asm__("nop");
        }
    }
}

/* Initialize GPIO for LED */
void gpio_init(void)
{
    /* Enable GPIOA clock */
    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    
    /* Configure PA5 as output for LED */
    GPIOA_MODER &= ~(3 << (5 * 2));  /* Clear mode bits */
    GPIOA_MODER |= (1 << (5 * 2));   /* Set as output (01) */
}

/* Initialize I2C1 as slave */
void i2c_init(void)
{
    /* Enable GPIOB and I2C1 clocks */
    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC_APB1ENR |= RCC_APB1ENR_I2C1EN;
    
    /* Configure PB8 (SCL) and PB9 (SDA) */
    /* Set to alternate function mode (10) */
    GPIOB_MODER &= ~((3 << (8 * 2)) | (3 << (9 * 2)));
    GPIOB_MODER |= (2 << (8 * 2)) | (2 << (9 * 2));
    
    /* Set to open-drain */
    GPIOB_OTYPER |= (1 << 8) | (1 << 9);
    
    /* Set pull-up */
    GPIOB_PUPDR &= ~((3 << (8 * 2)) | (3 << (9 * 2)));
    GPIOB_PUPDR |= (1 << (8 * 2)) | (1 << (9 * 2));
    
    /* Configure alternate function AF4 for I2C1 */
    /* PB8 and PB9 use AFRH register */
    GPIOB_AFRH &= ~((15 << ((8 - 8) * 4)) | (15 << ((9 - 8) * 4)));
    GPIOB_AFRH |= (4 << ((8 - 8) * 4)) | (4 << ((9 - 8) * 4));
    
    /* Reset I2C1 */
    I2C1_CR1 |= I2C_CR1_SWRST;
    I2C1_CR1 &= ~I2C_CR1_SWRST;
    
    /* Configure I2C1 */
    /* APB1 clock = 16MHz */
    I2C1_CR2 = 16;  /* 16 MHz peripheral clock */
    
    /* Configure CCR for 100kHz */
    /* CCR = Trise / (2 * Tscl) = 16MHz / (2 * 100kHz) = 80 */
    I2C1_CCR = 80;
    
    /* Configure rise time */
    /* TRISE = (max_rise_time / Tpclk) + 1 = (1000ns / 62.5ns) + 1 = 17 */
    I2C1_TRISE = 17;
    
    /* Set own address to 0x30 (7-bit addressing) */
    I2C1_OAR1 = 0;
    I2C1_OAR1 = (0x30 << 1);  /* Address in bits [7:1] */
    I2C1_OAR1 |= (1 << 14);   /* Bit 14 should be kept at 1 by software */
    
    /* Enable acknowledge */
    I2C1_CR1 |= I2C_CR1_ACK;
    
    /* Enable I2C1 */
    I2C1_CR1 |= I2C_CR1_PE;
}

/* LED control functions */
void led_on(void)
{
    GPIOA_ODR |= (1 << 5);
}

void led_off(void)
{
    GPIOA_ODR &= ~(1 << 5);
}

void led_toggle(void)
{
    GPIOA_ODR ^= (1 << 5);
}

/* I2C event handler */
void i2c_event_handler(void)
{
    volatile unsigned int sr1, sr2;
    
    sr1 = I2C1_SR1;
    
    /* Address matched */
    if (sr1 & I2C_SR1_ADDR) {
        /* Clear ADDR flag by reading SR1 and SR2 */
        sr1 = I2C1_SR1;
        sr2 = I2C1_SR2;
        
        /* LED indication that address is matched */
        led_on();
    }
    
    /* Data received */
    if (sr1 & I2C_SR1_RXNE) {
        /* Read data from DR register */
        received_data = (unsigned char)(I2C1_DR & 0xFF);
        data_ready = 1;
        
        /* Check if received data is 0xAA */
        if (received_data == 0xAA) {
            led_toggle();
        }
    }
    
    /* Stop condition detected */
    if (sr1 & I2C_SR1_STOPF) {
        /* Clear STOPF by reading SR1 and writing CR1 */
        sr1 = I2C1_SR1;
        I2C1_CR1 |= I2C_CR1_PE;
        
        led_off();
    }
}

/* Main function */
int main(void)
{
    /* Initialize peripherals */
    gpio_init();
    i2c_init();
    
    /* Blink LED 3 times to indicate startup */
    led_on();
    delay_ms(200);
    led_off();
    delay_ms(200);
    led_on();
    delay_ms(200);
    led_off();
    delay_ms(200);
    led_on();
    delay_ms(200);
    led_off();
    
    /* Main loop */
    while (1) {
        /* Check for I2C events */
        i2c_event_handler();
        
        /* Process received data if available */
        if (data_ready) {
            /* Data processing happens here */
            /* For now, just clear the flag */
            data_ready = 0;
        }
        
        /* Small delay */
        delay_ms(10);
    }
    
    return 0;
}
