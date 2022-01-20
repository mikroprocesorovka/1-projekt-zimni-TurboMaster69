/*
Projekt 1 - Minutky
*/

#include "stm8s.h"
#include "milis.h"
#include "stdio.h"
#include "stm8_hd44780.h"
#include "spse_stm8.c"

void init_timer(void);    // inicializace všech funkcí a promněnných
void pocty (void);
void init_enc(void);
void process_enc(void);
void mod (void);
void minutky (void);
void init_tim2 (void);
void sound_generator (void);
void nastav_cas1 (void);
void nastav_cas2 (void);
void display (void);

uint16_t time, cas, doba, trvani;
int16_t sekundy=0;
volatile int16_t minuty=0,vteriny=0, led=0, alarm=0, odpocet=0;
char text[24];

void main(void){
CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // 16MHz z interního RC oscilátoru
GPIO_Init(GPIOC,GPIO_PIN_5,GPIO_MODE_OUT_PP_LOW_SLOW);
GPIO_Init(GPIOE,GPIO_PIN_4,GPIO_MODE_IN_FL_NO_IT);
init_milis(); 
init_timer();
init_tim2();
init_enc();
lcd_init();

  while (1){

doba=milis();   // provedení funkcí
minutky();
sound_generator();
display();

  }
}


void display (void){  // výpis textu na displej
pocty();
sprintf(text,"%5u"/*,minuty*/,sekundy); 
lcd_gotoxy(0,0); 
lcd_puts(text);
}

void pocty (void){  //převádění času na minuty a sekundy
vteriny=sekundy;
minuty=0;
if (vteriny>=60){
	minuty++;
	vteriny=vteriny-60;
	}
}

void nastav_cas1 (void){  //změna velikosti přidávaného času
if (sekundy>=180){
	sekundy=sekundy+60;
	}else{
	sekundy=sekundy+10;
	}
}

void nastav_cas2 (void){  //změna velikosti odebíraného času
if (sekundy>=180){
	sekundy=sekundy-60;
	}else{
	sekundy=sekundy-10;
	}
}

void sound_generator (void){  //generování obdel. signálu pro zvukovou signalizaci
	static uint16_t last_time=0;  
  if((milis() - last_time >= 500) && alarm==1 && odpocet==1){ // pravidelně měníme frekvenci/periodu
		last_time = milis();
		TIM2_Cmd(ENABLE);
		if(milis() - last_time >= 250){
			TIM2_Cmd(DISABLE);
		} 
  }
	if (alarm!=1){ 
		TIM2_Cmd(DISABLE);
	}
}

void mod (void){  //zde se pomocí tlačítka na enkodéru mění "módy" = nastavování času a odpočítávání(minutky)
static uint8_t y=0;
if(GPIO_ReadInputPin(GPIOE, GPIO_PIN_4) == RESET && y==0){
	 y=1;
	 odpocet=1;
	} else if(GPIO_ReadInputPin(GPIOE, GPIO_PIN_4) == RESET && y==1){
	 y=0;
	 odpocet=0;
	 alarm=0;
	 led=0;
	 
	}
}

void minutky (void){  //zde se provádí funkce spojené s odpočítáváním již nastaveného času
if (led==1){	//světelná signalizace po skončení odpočítávání
	if(doba - trvani >= 200){
		trvani = milis();
		GPIO_WriteReverse(GPIOC,GPIO_PIN_5);
	}
	}else{
		GPIO_WriteLow(GPIOC,GPIO_PIN_5);
	}

if (odpocet==1 && sekundy>0){ //odpočítávání nastaveního času po sekundách
/*static uint16_t korekce=0;
	if (cas > 1000){
		korekce=cas-1000;
		if ((doba - cas) >= (1000-korekce)){
			cas = milis();
			sekundy--;
			//display();
		}
	} else if (doba - cas >= 1000){
		cas = milis();
		sekundy--;
		//display();
	} */
	if(doba - cas >= 1000){
		cas = milis();
		sekundy--;
		}
}

	if (sekundy<1 && odpocet==1){   //když dojde čas
		sekundy=0;
		alarm=1;	//spuštění zvukové signalizace
		led=1; //spuštění světelné signalizace
	}
}

@svlreg INTERRUPT_HANDLER (TIM3_UPD_OVF_BRK_IRQHandler,15){ // inicializace přerušení
	TIM3_ClearITPendingBit(TIM3_IT_UPDATE);
	process_enc();
	mod();
 }

void process_enc(void){ // nastavování času pomocí enkodéru
	static last_time=1;
	if (odpocet==0){
		if(GPIO_ReadInputPin(GPIOF,GPIO_PIN_7) == RESET && last_time==1){
			last_time = 0;
			if(GPIO_ReadInputPin(GPIOF,GPIO_PIN_6) == RESET){
				nastav_cas1();
			}else{
				nastav_cas2();
			}
		}
		if(GPIO_ReadInputPin(GPIOF,GPIO_PIN_7) != RESET){
			last_time = 1;
		}
	}
}

void init_enc(void){ 
GPIO_Init(GPIOF,GPIO_PIN_7,GPIO_MODE_IN_PU_NO_IT); 
GPIO_Init(GPIOF,GPIO_PIN_6,GPIO_MODE_IN_PU_NO_IT);
}

void init_timer(void){ // nastavní timeru
TIM3_TimeBaseInit(TIM3_PRESCALER_16,1999);
TIM3_ITConfig(TIM3_IT_UPDATE,ENABLE);
TIM3_Cmd(ENABLE);
}

void init_tim2(void){ //nastavení generování obdelníku pro zvukovou signalizaci
GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_SLOW);
TIM2_TimeBaseInit(TIM2_PRESCALER_16,300-1);
TIM2_OC1Init(TIM2_OCMODE_TOGGLE,TIM2_OUTPUTSTATE_ENABLE,1,TIM2_OCPOLARITY_LOW);
TIM2_ARRPreloadConfig(ENABLE);
}


#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
