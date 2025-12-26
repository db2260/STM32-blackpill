#include "stm32f411xe.h"

#define ADC_BUF_LEN 16
volatile uint16_t adc_buffer[ADC_BUF_LEN];

/* ================= CLOCK ================= */
void clock_init(void)
{
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));
}

/* ================= UART1 ================= */
void uart1_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /* PA9 → USART1_TX */
    GPIOA->MODER &= ~(3U << (9 * 2));
    GPIOA->MODER |=  (2U << (9 * 2));

    GPIOA->AFR[1] &= ~(0xFU << (4 * (9 - 8))); // AFR[1], pin 9
    GPIOA->AFR[1] |=  (7U   << (4 * (9 - 8)));

    USART1->BRR = 16000000U / 115200U;
    USART1->CR1 |= USART_CR1_TE | USART_CR1_UE;
}

void uart_send_uint(uint16_t val)
{
    char buf[6];
    int i = 0;

    if (val == 0) {
        buf[i++] = '0';
    } else {
        while (val) {
            buf[i++] = (val % 10) + '0';
            val /= 10;
        }
    }

    while (i--) {
        while (!(USART1->SR & USART_SR_TXE));
        USART1->DR = buf[i];
    }

    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = '\n';
}

/* ================= TIM3 ================= */
void tim3_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    TIM3->PSC = 16000 - 1;   // 16 MHz / 16000 = 1 kHz
    TIM3->ARR = 10 - 1;      // 1 kHz / 10 = 100 Hz

    TIM3->CR2 |= TIM_CR2_MMS_1; // TRGO on update
    TIM3->CR1 |= TIM_CR1_CEN;
}

/* ================= DMA ================= */
void dma_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    DMA2_Stream0->CR = 0;
    DMA2_Stream0->PAR  = (uint32_t)&ADC1->DR;
    DMA2_Stream0->M0AR = (uint32_t)adc_buffer;
    DMA2_Stream0->NDTR = ADC_BUF_LEN;

    DMA2_Stream0->CR |=
        DMA_SxCR_MINC |
        DMA_SxCR_CIRC |
        DMA_SxCR_TCIE |
        DMA_SxCR_PSIZE_0 |
        DMA_SxCR_MSIZE_0;

    DMA2_Stream0->CR |= DMA_SxCR_EN;
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}

/* ================= ADC ================= */
void adc_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /* PA0 → ADC1_IN0 */
    GPIOA->MODER |= (3U << (0 * 2));

    ADC1->SQR3 = 0;
    ADC1->SMPR2 |= ADC_SMPR2_SMP0;

    ADC1->CR2 |=
        ADC_CR2_DMA |
        ADC_CR2_DDS |
        ADC_CR2_EXTEN_0 |
        (0b100U << ADC_CR2_EXTSEL_Pos) | // TIM3_TRGO
        ADC_CR2_ADON;
}

/* ================= DMA IRQ ================= */
void DMA2_Stream0_IRQHandler(void)
{
    if (DMA2->LISR & DMA_LISR_TCIF0) {
        DMA2->LIFCR |= DMA_LIFCR_CTCIF0;

        for (volatile int i = 0; i < ADC_BUF_LEN; i++) {
            uart_send_uint(adc_buffer[i]);
        }
    }
}

/* ================= MAIN ================= */
int main(void)
{
    clock_init();
    uart1_init();
    dma_init();
    adc_init();
    tim3_init();

    while (1) {
        /* everything runs via interrupts */
    }
}
