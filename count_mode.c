/*Base register adddress header file*/
#include "stm32l1xx.h"
/*Library related header files*/
#include "stm32l1xx_ll_gpio.h"
#include "stm32l1xx_ll_pwr.h"
#include "stm32l1xx_ll_rcc.h"
#include "stm32l1xx_ll_bus.h"
#include "stm32l1xx_ll_utils.h"
#include "stm32l1xx_ll_system.h"
#include "stm32l1xx_ll_tim.h"
#include "stm32l1xx_ll_exti.h"

#include "dwt_delay.h"

#define zero  0x7C04
#define one   0x0C00
#define two   0xB404
#define three 0x9C04
#define four  0xCC00
#define five  0xD804
#define six   0xF804
#define seven 0x0C04
#define eight 0xFC04
#define nine  0xDC04

uint32_t number[4] = {zero, four, seven, zero};
uint32_t test[10] = {zero, one, two, three, four, five, six, seven, eight, nine};

/*already implemented */
void SystemClock_Config(void);
void OW_WriteBit(uint8_t d);
uint8_t OW_ReadBit(void);
void DS1820_GPIO_Configure(void);
uint8_t DS1820_ResetPulse(void);

/*haven't been implemented yet!*/
void OW_Master(void);
void OW_Slave(void);
void OW_WriteByte(uint8_t data);
uint16_t OW_ReadByte(void);
void LED_ON();
void Test();

uint16_t temp1,temp2,temp;
uint16_t user_btn;
float temp_cal;
int count_bnt=0;
int main()
{
	SystemClock_Config();
	DWT_Init();
	DS1820_GPIO_Configure();
	
	LL_GPIO_InitTypeDef ltc4727_init, GPIO_Init;
	LL_EXTI_InitTypeDef EXTI_A0 ; //Create EXTI object name EXTI_A0
	
	
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	
	ltc4727_init.Mode = LL_GPIO_MODE_OUTPUT;
	ltc4727_init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	ltc4727_init.Pull = LL_GPIO_PULL_NO;
	ltc4727_init.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	ltc4727_init.Pin = LL_GPIO_PIN_2|LL_GPIO_PIN_10|LL_GPIO_PIN_11|LL_GPIO_PIN_12|LL_GPIO_PIN_13|LL_GPIO_PIN_14|LL_GPIO_PIN_15;
	LL_GPIO_Init(GPIOB, &ltc4727_init);
	
	ltc4727_init.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3;
	LL_GPIO_Init(GPIOC, &ltc4727_init);
	
	GPIO_Init.Mode = LL_GPIO_MODE_INPUT;
	GPIO_Init.Pin = LL_GPIO_PIN_0;
	GPIO_Init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_Init.Pull = LL_GPIO_PULL_NO;
	GPIO_Init.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	LL_GPIO_Init(GPIOA, &GPIO_Init);
	
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_0);//Enable interrupt at EXTI Line 0
	EXTI_A0.Line_0_31 = LL_EXTI_LINE_0 ;//Set EXTI line of object at EXTI Line 0
	EXTI_A0.Mode = LL_EXTI_MODE_IT ;//Set mode to interrupt 
	EXTI_A0.Trigger = LL_EXTI_TRIGGER_RISING ;//Set trigger at rising edge
	EXTI_A0.LineCommand = ENABLE ;//Enable interrupt API
	LL_EXTI_Init(&EXTI_A0);//Assign the EXTI object to hardware EXTI
	NVIC_EnableIRQ((IRQn_Type)6);
	
	while(1)
	{
		switch (count_bnt){
			case 1:
				LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3);
				LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2|LL_GPIO_PIN_10|LL_GPIO_PIN_11|LL_GPIO_PIN_12|LL_GPIO_PIN_13|LL_GPIO_PIN_14|LL_GPIO_PIN_15);

				LL_GPIO_SetOutputPin(GPIOB, number[1]);
				LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_0);
				break;
				
			case 2:
				LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3);
				LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2|LL_GPIO_PIN_10|LL_GPIO_PIN_11|LL_GPIO_PIN_12|LL_GPIO_PIN_13|LL_GPIO_PIN_14|LL_GPIO_PIN_15);

				LL_GPIO_SetOutputPin(GPIOB, number[2]);
				LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_1);
				break;
				
			default:
				LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3);
				LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2|LL_GPIO_PIN_10|LL_GPIO_PIN_11|LL_GPIO_PIN_12|LL_GPIO_PIN_13|LL_GPIO_PIN_14|LL_GPIO_PIN_15);

				LL_GPIO_SetOutputPin(GPIOB, number[0]);
				LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_3);
				break;
		}
	}
}
void EXTI0_IRQHandler(){
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_0)){
			count_bnt = (count_bnt+1)%3;
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_0);
    }
}

void Test(){
		DS1820_ResetPulse(); //Send reset pulse
		OW_WriteByte(0xCC); //Send 'Skip Rom (0xCC)' command
		OW_WriteByte(0x44); //Send 'Temp Convert (0x44)' command
		LL_mDelay(200); //Delay at least 200ms (typical conversion time)
		DS1820_ResetPulse(); //Send reset pulse
		OW_WriteByte(0xCC); //Send 'Skip Rom (0xCC)' command
		OW_WriteByte(0xBE);//Send 'Read Scractpad (0xBE)' command
		
		temp1 = OW_ReadByte();//Read byte 1 (Temperature data in LSB)
		temp2 = OW_ReadByte();//Read byte 2 (Temperature data in MSB)
		
		//Convert to readable floating point temperature
		temp = (temp2<<8)|temp1;
		temp_cal = (temp*1.0) / 16.0;
}

void LED_ON(){
	uint8_t count = 0;
	uint32_t digit[4] = {LL_GPIO_PIN_0,LL_GPIO_PIN_1,LL_GPIO_PIN_2,LL_GPIO_PIN_3};
	for(count=0;count<4;count++){
			LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_0|LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_3);
			LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2|LL_GPIO_PIN_10|LL_GPIO_PIN_11|LL_GPIO_PIN_12|LL_GPIO_PIN_13|LL_GPIO_PIN_14|LL_GPIO_PIN_15);

			LL_GPIO_SetOutputPin(GPIOB, number[count]);
			LL_GPIO_SetOutputPin(GPIOC, digit[count]);
		}
}

void OW_WriteBit(uint8_t d)
{
	if(d == 1) //Write 1
	{
		OW_Master(); //uC occupies wire bus
		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6);
		DWT_Delay(1);
		OW_Slave(); //uC releases wire bus
		DWT_Delay(60);
	}
	else //Write 0
	{
		OW_Master(); //uC occupies wire bus
		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6);
		DWT_Delay(60);
		OW_Slave(); //uC releases wire bus
	}
}

uint8_t OW_ReadBit(void)
{
	OW_Master();
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6);
	DWT_Delay(2);
	OW_Slave();
	
	return LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_6);	
}


void DS1820_GPIO_Configure(void)
{
	LL_GPIO_InitTypeDef ds1820_io;
	
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	
	ds1820_io.Mode = LL_GPIO_MODE_OUTPUT;
	ds1820_io.Pin = LL_GPIO_PIN_6;
	ds1820_io.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	ds1820_io.Pull = LL_GPIO_PULL_NO;
	ds1820_io.Speed = LL_GPIO_SPEED_FREQ_LOW;
	LL_GPIO_Init(GPIOB, &ds1820_io);
}

uint8_t DS1820_ResetPulse(void)
{	
	OW_Master();
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6);
	DWT_Delay(480);
	OW_Slave();
	DWT_Delay(80);
	
	if(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_6) == 0)
	{
		DWT_Delay(400);
		return 0;
	}
	else
	{
		DWT_Delay(400);
		return 1;
	}
}

void SystemClock_Config(void)
{
  /* Enable ACC64 access and set FLASH latency */ 
  LL_FLASH_Enable64bitAccess();; 
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

  /* Set Voltage scale1 as MCU will run at 32MHz */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  
  /* Poll VOSF bit of in PWR_CSR. Wait until it is reset to 0 */
  while (LL_PWR_IsActiveFlag_VOSF() != 0)
  {
  };
  
  /* Enable HSI if not already activated*/
  if (LL_RCC_HSI_IsReady() == 0)
  {
    /* HSI configuration and activation */
    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != 1)
    {
    };
  }
  
	
  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3);

  LL_RCC_PLL_Enable();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  };
  
  /* Sysclk activation on the main PLL */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  };
  
  /* Set APB1 & APB2 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  /* Set systick to 1ms in using frequency set to 32MHz                             */
  /* This frequency can be calculated through LL RCC macro                          */
  /* ex: __LL_RCC_CALC_PLLCLK_FREQ (HSI_VALUE, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3); */
  LL_Init1msTick(32000000);
  
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(32000000);
}

void OW_Master(void){
	LL_GPIO_InitTypeDef ds1820_io;
	
	ds1820_io.Mode = LL_GPIO_MODE_OUTPUT;
	ds1820_io.Pin = LL_GPIO_PIN_6;
	ds1820_io.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	ds1820_io.Pull = LL_GPIO_PULL_NO;
	ds1820_io.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	LL_GPIO_Init(GPIOB, &ds1820_io);
}

void OW_Slave(void){
	LL_GPIO_InitTypeDef ds1820_io;
	
	ds1820_io.Mode = LL_GPIO_MODE_INPUT;
	ds1820_io.Pin = LL_GPIO_PIN_6;
	ds1820_io.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	ds1820_io.Pull = LL_GPIO_PULL_NO;
	ds1820_io.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	LL_GPIO_Init(GPIOB, &ds1820_io);
}

void OW_WriteByte(uint8_t data){
	uint8_t i,temp;
	
	OW_Master();		//The data line is configured as output
	for(i = 0; i < 8; i++)
	{
	    if ((data & (1<<i))!=0){
				OW_WriteBit(1);
			}else{
				OW_WriteBit(0);
			}
	}
}

uint16_t OW_ReadByte(void){
	uint16_t value=0;
	
	for (int i=0;i<8;i++)
	{
		OW_ReadBit();
		if(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_6))  // if the pin is HIGH
		{
			value |= 1<<i;  // read = 1
		}
		DWT_Delay(60);  // wait for 60 us
	}
	return value;
}