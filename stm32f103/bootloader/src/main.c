#include "stm32f10x.h"
#include "version.h"

#define APP_START 0x08001000 // app start in flash
#define APP_END   0x08008000 // flash end
#define APP_RANGE_VALID(a, s) (!(((a) | (s)) & 3) && (a) >= APP_START && ((a) + (s)) <= APP_END)
#define VERSION_INFO_OFFSET 0x10c // from hv.map ".version_info" symbol

#define BOOT_UART2

extern uint32_t _start_bootloader; // .data starts at 0x20000000
static volatile const struct version_info *app_info = (void *) (APP_START + VERSION_INFO_OFFSET);

static int app_ok(void)
{
    if (!APP_RANGE_VALID(APP_START, app_info->image_size)) {
        return 0;
    }
    CRC_ResetDR(); // init crc unit to 0xffffffff start value

    if (CRC_CalcBlockCRC((uint32_t *) APP_START, app_info->image_size / 4) != 0) { // crc is is forged to be 0
        return 0;
    }

    return 1;
}
void SystemInit(void)
{
    // Reset the RCC clock configuration to the default reset state(for debug purpose)
    // Set HSION bit
    RCC->CR |= (uint32_t)0x00000001;
    // Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits
    RCC->CFGR &= (uint32_t)0xF0FF0000;
    // Reset HSEON, CSSON and PLLON bits
    RCC->CR &= (uint32_t)0xFEF6FFFF;

    // Reset HSEBYP bit
    RCC->CR &= (uint32_t)0xFFFBFFFF;

    // Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits
    RCC->CFGR &= (uint32_t)0xFF80FFFF;

    // Disable all interrupts and clear pending bits
    RCC->CIR = 0x009F0000;

    SCB->VTOR = FLASH_BASE | 0x0; // Vector Table Relocation in Internal FLASH
}

static void rcc_config(void)
{
    // Enable external oscillator
    RCC->CR |= ((uint32_t) RCC_CR_HSEON);

    // Wait for locked external oscillator
    while ((RCC->CR & RCC_CR_HSERDY) != RCC_CR_HSERDY);

    // Enable Prefetch Buffer
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    // Flash 2 wait state
    FLASH->ACR &= (uint32_t) ((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t) FLASH_ACR_LATENCY_2;

    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
                                        RCC_CFGR_PLLMULL));

    // PLLCLK = HSE 8MHz * 9 = 72 MHz, HCLK = SYSCLK, PCLK2 = HCLK, PCLK1 = HCLK
    RCC->CFGR |= (uint32_t) (RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9 | RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_ADCPRE_DIV6);

    // Enable PLL
    RCC->CR |= RCC_CR_PLLON;

    // Wait till PLL is ready
    while((RCC->CR & RCC_CR_PLLRDY) == 0);

    // Select PLL as system clock source
    RCC->CFGR &= (uint32_t) ((uint32_t) ~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t) RCC_CFGR_SW_PLL;

    // Wait till PLL is used as system clock source
    while ((RCC->CFGR & (uint32_t) RCC_CFGR_SWS) != (uint32_t) 0x08);

    // enable GPIOA-C and AFIO clock
    RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;
    // enabe CRC unit clock
    RCC->AHBENR |= RCC_AHBENR_CRCEN;
}

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
    #define INTEGERDIVIDER ((25 * APBCLOCK) / (4 * (DATABAUD)))

    tmpreg = (INTEGERDIVIDER / 100) << 4;

    // Determine the fractional part
    uint32_t fractionaldivider = INTEGERDIVIDER - (100 * (tmpreg >> 4));

    tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t) 0x0F);

    // Write to USART BRR
    USART2->BRR = (uint16_t) tmpreg;
    USART2->CR1 |= USART_Clock_Enable;
}

void uart_getc(void)
{
	if (USART2->SR & USART_SR_RXNE) {
		return USART2->DR;
	}
}

void uart_putc(char c)
{
	if (USART2->SR & USART_SR_TXE) {
		USART2->DR = c;
	}
}

#endif // BOOT_UART2

int main(void)
{
    rcc_config();
    // init gpio leds
    GPIOC->CRL = 0x44444222; // set gpioc[2-0] to pp, 2 MHz, output

    GPIOC->BSRR = 0x01; // red led on
    if ((_start_bootloader == 0xDEADBEEF) || !app_ok()) {
        _start_bootloader = 0xCAFEFEED; //Reset bootloader trigger
        GPIOC->BSRR = 0x10002; //gelb
#ifdef BOOT_UART2
        usart_init();
        //bootloader();
#endif // BOOT_UART2
        while (1);
    } else {
        GPIOC->BSRR = 0x00004; // rot, grün

        uint32_t  stack = ((const uint32_t *) APP_START)[0];
        uint32_t  entry = ((const uint32_t *) APP_START)[1];
        asm volatile(
           "msr    msp, %0        \n\t"
           "bx     %1             \n\t"
           : : "r" (stack), "r" (entry)
        );
        while (1);
    }
}

