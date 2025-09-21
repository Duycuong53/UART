#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include <string.h>
#include <ctype.h>

#define LED GPIO_Pin_0

// ================= GPIO ==================
void GPIO_Config(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef g;
    // LED t?i PA0
    g.GPIO_Pin = LED;
    g.GPIO_Mode = GPIO_Mode_Out_PP;
    g.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &g);

    // PA9 = TX
    g.GPIO_Pin = GPIO_Pin_9;
    g.GPIO_Mode = GPIO_Mode_AF_PP;
    g.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &g);

    // PA10 = RX
    g.GPIO_Pin = GPIO_Pin_10;
    g.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &g);
}

// ================= UART ==================
void UART_Config(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    USART_InitTypeDef usart;
    USART_StructInit(&usart);

    usart.USART_BaudRate = 9600;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &usart);
    USART_Cmd(USART1, ENABLE);
}

void UART_SendChar(char c)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, c);
}

void UART_SendString(const char *s)
{
    while(*s) UART_SendChar(*s++);
}

char UART_ReceiveChar(void)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    return (char)USART_ReceiveData(USART1);
}

// ================= MAIN ==================
int main(void)
{
    GPIO_Config();
    UART_Config();

    // In chào 1 l?n duy nh?t
    UART_SendString("Hello STM32!\r\n");
    UART_SendString("Type ON to turn LED ON, OFF to turn LED OFF\r\n");

    char buffer[16];
    int idx = 0;

    while(1)
    {
        char c = UART_ReceiveChar();

        if(c == '\r' || c == '\n') // khi nh?n Enter
        {
            UART_SendString("\r\n");  // xu?ng dòng trên terminal
            buffer[idx] = '\0';       // k?t thúc chu?i

            // Chuy?n thành ch? hoa
            for(int i=0; i<idx; i++) buffer[i] = toupper(buffer[i]);

            // So sánh l?nh
            if(strcmp(buffer, "ON") == 0)
            {
                GPIO_SetBits(GPIOA, LED);
                UART_SendString("LED ON\r\n");
            }
            else if(strcmp(buffer, "OFF") == 0)
            {
                GPIO_ResetBits(GPIOA, LED);
                UART_SendString("LED OFF\r\n");
            }
            else if(idx > 0)
            {
                UART_SendString("Invalid command\r\n");
            }

            // Reset buffer
            idx = 0;
        }
        else
        {
            if(idx < sizeof(buffer)-1)
            {
                buffer[idx++] = c;
                UART_SendChar(c); // echo l?i ký t? d? th?y mình gõ gì
            }
        }
		}
}