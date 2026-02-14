/*
 * Startup code for STM32F401RE
 * Bare metal - no HAL
 */

/* External declarations */
extern unsigned int _etext;
extern unsigned int _sdata;
extern unsigned int _edata;
extern unsigned int _sidata;
extern unsigned int _sbss;
extern unsigned int _ebss;

/* Stack top (end of RAM) */
extern unsigned int _estack;

/* Main function */
extern int main(void);

/* Function prototypes */
void Reset_Handler(void);
void Default_Handler(void);

/* Cortex-M4 core exceptions */
void NMI_Handler(void)                __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler(void)          __attribute__ ((weak, alias("Default_Handler")));
void MemManage_Handler(void)          __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler(void)           __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler(void)         __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler(void)                __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler(void)           __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler(void)             __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler(void)            __attribute__ ((weak, alias("Default_Handler")));

/* STM32F401RE specific interrupts */
void WWDG_IRQHandler(void)            __attribute__ ((weak, alias("Default_Handler")));
void PVD_IRQHandler(void)             __attribute__ ((weak, alias("Default_Handler")));
void TAMP_STAMP_IRQHandler(void)      __attribute__ ((weak, alias("Default_Handler")));
void RTC_WKUP_IRQHandler(void)        __attribute__ ((weak, alias("Default_Handler")));
void FLASH_IRQHandler(void)           __attribute__ ((weak, alias("Default_Handler")));
void RCC_IRQHandler(void)             __attribute__ ((weak, alias("Default_Handler")));
void EXTI0_IRQHandler(void)           __attribute__ ((weak, alias("Default_Handler")));
void EXTI1_IRQHandler(void)           __attribute__ ((weak, alias("Default_Handler")));
void EXTI2_IRQHandler(void)           __attribute__ ((weak, alias("Default_Handler")));
void EXTI3_IRQHandler(void)           __attribute__ ((weak, alias("Default_Handler")));
void EXTI4_IRQHandler(void)           __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Stream0_IRQHandler(void)    __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Stream1_IRQHandler(void)    __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Stream2_IRQHandler(void)    __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Stream3_IRQHandler(void)    __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Stream4_IRQHandler(void)    __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Stream5_IRQHandler(void)    __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Stream6_IRQHandler(void)    __attribute__ ((weak, alias("Default_Handler")));
void ADC_IRQHandler(void)             __attribute__ ((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler(void)         __attribute__ ((weak, alias("Default_Handler")));
void TIM1_BRK_TIM9_IRQHandler(void)   __attribute__ ((weak, alias("Default_Handler")));
void TIM1_UP_TIM10_IRQHandler(void)   __attribute__ ((weak, alias("Default_Handler")));
void TIM1_TRG_COM_TIM11_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler(void)         __attribute__ ((weak, alias("Default_Handler")));
void TIM2_IRQHandler(void)            __attribute__ ((weak, alias("Default_Handler")));
void TIM3_IRQHandler(void)            __attribute__ ((weak, alias("Default_Handler")));
void TIM4_IRQHandler(void)            __attribute__ ((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler(void)         __attribute__ ((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler(void)         __attribute__ ((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler(void)         __attribute__ ((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler(void)         __attribute__ ((weak, alias("Default_Handler")));
void SPI1_IRQHandler(void)            __attribute__ ((weak, alias("Default_Handler")));
void SPI2_IRQHandler(void)            __attribute__ ((weak, alias("Default_Handler")));
void USART1_IRQHandler(void)          __attribute__ ((weak, alias("Default_Handler")));
void USART2_IRQHandler(void)          __attribute__ ((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler(void)       __attribute__ ((weak, alias("Default_Handler")));
void RTC_Alarm_IRQHandler(void)       __attribute__ ((weak, alias("Default_Handler")));
void OTG_FS_WKUP_IRQHandler(void)     __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Stream7_IRQHandler(void)    __attribute__ ((weak, alias("Default_Handler")));
void SDIO_IRQHandler(void)            __attribute__ ((weak, alias("Default_Handler")));
void TIM5_IRQHandler(void)            __attribute__ ((weak, alias("Default_Handler")));
void SPI3_IRQHandler(void)            __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Stream0_IRQHandler(void)    __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Stream1_IRQHandler(void)    __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Stream2_IRQHandler(void)    __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Stream3_IRQHandler(void)    __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Stream4_IRQHandler(void)    __attribute__ ((weak, alias("Default_Handler")));
void OTG_FS_IRQHandler(void)          __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Stream5_IRQHandler(void)    __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Stream6_IRQHandler(void)    __attribute__ ((weak, alias("Default_Handler")));
void DMA2_Stream7_IRQHandler(void)    __attribute__ ((weak, alias("Default_Handler")));
void USART6_IRQHandler(void)          __attribute__ ((weak, alias("Default_Handler")));
void I2C3_EV_IRQHandler(void)         __attribute__ ((weak, alias("Default_Handler")));
void I2C3_ER_IRQHandler(void)         __attribute__ ((weak, alias("Default_Handler")));
void FPU_IRQHandler(void)             __attribute__ ((weak, alias("Default_Handler")));
void SPI4_IRQHandler(void)            __attribute__ ((weak, alias("Default_Handler")));

/* Vector table */
__attribute__ ((section(".isr_vector")))
unsigned int vectors[] = {
    (unsigned int) &_estack,          /* Stack pointer */
    (unsigned int) Reset_Handler,     /* Reset Handler */
    (unsigned int) NMI_Handler,       /* NMI Handler */
    (unsigned int) HardFault_Handler, /* Hard Fault Handler */
    (unsigned int) MemManage_Handler, /* MPU Fault Handler */
    (unsigned int) BusFault_Handler,  /* Bus Fault Handler */
    (unsigned int) UsageFault_Handler,/* Usage Fault Handler */
    0,                                /* Reserved */
    0,                                /* Reserved */
    0,                                /* Reserved */
    0,                                /* Reserved */
    (unsigned int) SVC_Handler,       /* SVCall Handler */
    (unsigned int) DebugMon_Handler,  /* Debug Monitor Handler */
    0,                                /* Reserved */
    (unsigned int) PendSV_Handler,    /* PendSV Handler */
    (unsigned int) SysTick_Handler,   /* SysTick Handler */
    
    /* External Interrupts */
    (unsigned int) WWDG_IRQHandler,
    (unsigned int) PVD_IRQHandler,
    (unsigned int) TAMP_STAMP_IRQHandler,
    (unsigned int) RTC_WKUP_IRQHandler,
    (unsigned int) FLASH_IRQHandler,
    (unsigned int) RCC_IRQHandler,
    (unsigned int) EXTI0_IRQHandler,
    (unsigned int) EXTI1_IRQHandler,
    (unsigned int) EXTI2_IRQHandler,
    (unsigned int) EXTI3_IRQHandler,
    (unsigned int) EXTI4_IRQHandler,
    (unsigned int) DMA1_Stream0_IRQHandler,
    (unsigned int) DMA1_Stream1_IRQHandler,
    (unsigned int) DMA1_Stream2_IRQHandler,
    (unsigned int) DMA1_Stream3_IRQHandler,
    (unsigned int) DMA1_Stream4_IRQHandler,
    (unsigned int) DMA1_Stream5_IRQHandler,
    (unsigned int) DMA1_Stream6_IRQHandler,
    (unsigned int) ADC_IRQHandler,
    0, 0, 0, 0,                       /* Reserved */
    (unsigned int) EXTI9_5_IRQHandler,
    (unsigned int) TIM1_BRK_TIM9_IRQHandler,
    (unsigned int) TIM1_UP_TIM10_IRQHandler,
    (unsigned int) TIM1_TRG_COM_TIM11_IRQHandler,
    (unsigned int) TIM1_CC_IRQHandler,
    (unsigned int) TIM2_IRQHandler,
    (unsigned int) TIM3_IRQHandler,
    (unsigned int) TIM4_IRQHandler,
    (unsigned int) I2C1_EV_IRQHandler,
    (unsigned int) I2C1_ER_IRQHandler,
    (unsigned int) I2C2_EV_IRQHandler,
    (unsigned int) I2C2_ER_IRQHandler,
    (unsigned int) SPI1_IRQHandler,
    (unsigned int) SPI2_IRQHandler,
    (unsigned int) USART1_IRQHandler,
    (unsigned int) USART2_IRQHandler,
    0,                                /* Reserved */
    (unsigned int) EXTI15_10_IRQHandler,
    (unsigned int) RTC_Alarm_IRQHandler,
    (unsigned int) OTG_FS_WKUP_IRQHandler,
    0, 0, 0, 0,                       /* Reserved */
    (unsigned int) DMA1_Stream7_IRQHandler,
    0,                                /* Reserved */
    (unsigned int) SDIO_IRQHandler,
    (unsigned int) TIM5_IRQHandler,
    (unsigned int) SPI3_IRQHandler,
    0, 0, 0, 0,                       /* Reserved */
    (unsigned int) DMA2_Stream0_IRQHandler,
    (unsigned int) DMA2_Stream1_IRQHandler,
    (unsigned int) DMA2_Stream2_IRQHandler,
    (unsigned int) DMA2_Stream3_IRQHandler,
    (unsigned int) DMA2_Stream4_IRQHandler,
    0, 0, 0, 0, 0, 0,                 /* Reserved */
    (unsigned int) OTG_FS_IRQHandler,
    (unsigned int) DMA2_Stream5_IRQHandler,
    (unsigned int) DMA2_Stream6_IRQHandler,
    (unsigned int) DMA2_Stream7_IRQHandler,
    (unsigned int) USART6_IRQHandler,
    (unsigned int) I2C3_EV_IRQHandler,
    (unsigned int) I2C3_ER_IRQHandler,
    0, 0, 0, 0, 0, 0, 0,              /* Reserved */
    (unsigned int) FPU_IRQHandler,
    0, 0,                             /* Reserved */
    (unsigned int) SPI4_IRQHandler,
};

/* Reset handler */
void Reset_Handler(void)
{
    unsigned int *src, *dst;
    
    /* Copy data section from flash to RAM */
    src = &_sidata;
    dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }
    
    /* Zero fill bss section */
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }
    
    /* Call main function */
    main();
    
    /* Infinite loop if main returns */
    while (1);
}

/* Default handler for unhandled interrupts */
void Default_Handler(void)
{
    while (1);
}
