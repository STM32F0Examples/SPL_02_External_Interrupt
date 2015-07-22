#include "stm32f0xx.h"                  // Device header

/**
 * Set System Clock Speed to 48Mhz
 */
void setToMaxSpeed(void);

/**
 * Configures pin xxx as input with pull-ups and enables both edge interrupt
 */
void button_init(void);
/**
 * Configure the pin xxx as output to follow the button state
 */
void led_init(void);

int EXTI0_1_counter=0;

int main(void){
	setToMaxSpeed();
	led_init();
	button_init();
	while(1){
		
	}
}

void button_init(void){
	//Configure PB0 as input with pull-ups
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	GPIO_InitTypeDef myGPIO;
	GPIO_StructInit(&myGPIO);
	myGPIO.GPIO_Pin=GPIO_Pin_0;
	myGPIO.GPIO_Mode=GPIO_Mode_IN;
	myGPIO.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&myGPIO);
	
	//Configure PB0 both edges interrupt
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_SYSCFGEN,ENABLE);//Turn on SYSCFG since it's disabled by default
	EXTI_InitTypeDef myEXTI;
	EXTI_StructInit(&myEXTI);
	myEXTI.EXTI_Line=EXTI_Line0;
	myEXTI.EXTI_LineCmd=ENABLE;
	myEXTI.EXTI_Mode=EXTI_Mode_Interrupt;
	myEXTI.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
	EXTI_Init(&myEXTI);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource0);//Reamp Line 0 to Port B
	
	EXTI_ClearITPendingBit(EXTI_Line0);//Clear pening interrupts
	NVIC_EnableIRQ(EXTI0_1_IRQn);//Enable EXTI0  interrupts
	EXTI_GenerateSWInterrupt(EXTI_Line0);//Genrate SW interrupt to update initial led state
}

void led_init(void){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	GPIO_InitTypeDef myGPIO;
	GPIO_StructInit(&myGPIO);
	myGPIO.GPIO_Pin=GPIO_Pin_1;
	myGPIO.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_Init(GPIOB,&myGPIO);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
}

void setToMaxSpeed(void){
	int internalClockCounter;
	RCC_PLLCmd(DISABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
	RCC_HSEConfig(RCC_HSE_OFF);
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_12);
	RCC_PREDIV1Config(RCC_PREDIV1_Div1);
	RCC_PLLCmd(ENABLE);
	while(!RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	for(internalClockCounter=0;internalClockCounter<1024;internalClockCounter++){
		if(RCC_GetSYSCLKSource()==RCC_SYSCLKSource_PLLCLK){
			SystemCoreClockUpdate();
			break;
		}
	}
}

void EXTI0_1_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line0)){
		EXTI_ClearITPendingBit(EXTI_Line0);
		//TODO
		GPIO_WriteBit(GPIOB,GPIO_Pin_1,!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0));
	}
	EXTI0_1_counter++;
}
