#include "stm32f10x.h"
#include "common.h"


#ifdef BOOT_UART2
static void usart_init(void)
{
    // enable usart2 clock
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    // set gpioa[2,3] to 50 MHz, AFIO; 2 -> pp, 3 -> open drain
    GPIOA->CRL = 0x4444EB44;

    uint32_t tmpreg = USART2->CR2;
    tmpreg &= (uint16_t) 0xCFFF;
    USART2->CR2 = (uint16_t) tmpreg;

    tmpreg = USART2->CR1;
    // Clear M, PCE, PS, TE and RE bits
    tmpreg &= (uint16_t) 0xE9F3;
    USART2->CR1 = tmpreg | USART_Mode_Rx | USART_Mode_Tx; // configure usart: rx, tx

    tmpreg = USART2->CR3;
    tmpreg &= (uint16_t) 0xFCFF;
    USART2->CR3 = (uint16_t) tmpreg;

    #define APBCLOCK 36000000

    // Integer part computing in case Oversampling mode is 16 Samples
    uint32_t integerdivider = ((25 * APBCLOCK) / (4 * (DATABAUD)));

    tmpreg = (integerdivider / 100) << 4;

    // Determine the fractional part
    uint32_t fractionaldivider = integerdivider - (100 * (tmpreg >> 4));

    tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t) 0x0F);

    // Write to USART BRR
    USART2->BRR = (uint16_t) tmpreg;
    USART2->CR1 |= USART_Clock_Enable;
}

uint8_t uart_getc(void)
{
    while (!(USART2->SR & USART_SR_RXNE)); // FIXME
    return (uint16_t) (USART2->DR & (uint16_t) 0x01FF); // clear flag
}

void uart_putc(char c)
{
    while (!(USART2->SR & USART_SR_TXE)); // FIXME ?
    USART2->DR = (c & (uint16_t) 0x01FF); // clear flag
}

#endif // BOOT_UART2
