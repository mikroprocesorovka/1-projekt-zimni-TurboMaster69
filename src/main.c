/*
Projekt 1 - Minutky
*/

#include "stm8s.h"
#include "milis.h"
#include "stdio.h"
#include "stm8_hd44780.h"

void init_timer(void);    // inicializace všech funkcí a promněnných
void pocty1 (void);
void pocty2 (void);
void pocty3 (void);
void init_enc(void);
void process_enc(void);
void mod (void);
void minutky (void);
void init_tim2 (void);
void sound_generator (void);
void nastav_cas1 (void);
void nastav_cas2 (void);
void display (void);
void start (void);

uint32_t time, cas, doba, trvani;
int32_t sekundy=0;
uint8_t help1=1, help2=0, help3=0;
volatile uint8_t led=0, alarm=0, odpocet=0;
int16_t minuty=0,vteriny=0;
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
start();

  }
}

void start (void){
if (help1==1){
	display();
	help1=0;
	}
}

void pocty1 (void){  //převádění času na minuty a sekundy
vteriny=sekundy-(60*help2);
if (vteriny>=60){
	minuty++;
	vteriny=vteriny-60;
	help2++;
}
}

void pocty2 (void){  //převádění času na minuty a sekundy
vteriny=sekundy-(60*help2);
if (vteriny<1 && minuty>0){
	minuty--;
	vteriny=vteriny+50;
	help2--;
}
}

void pocty3 (void){  //převádění času na minuty a sekundy
vteriny=sekundy-(60*help2);
if (vteriny<=1 && minuty>0){
	minuty--;
	vteriny=vteriny+58;
	help2--;
	sekundy=sekundy-1;
}
}

void display (void){  // výpis textu na displej
sprintf(text,"Minuty = %3u",minuty); 
lcd_gotoxy(0,0); 
lcd_puts(text);
sprintf(text,"Sekundy = %2u",vteriny); 
lcd_gotoxy(0,1); 
lcd_puts(text);
}

void nastav_cas1 (void){  //změna velikosti přidávaného času
pocty1();
display();
if (odpocet==0){
	if (sekundy>=180){
		sekundy=sekundy+60;
		}else{
		sekundy=sekundy+10;
	}
}
}

void nastav_cas2 (void){  //změna velikosti odebíraného času
pocty2();
display();
if (odpocet==0){
	if (sekundy>=180){
		sekundy=sekundy-60;
		}else{
		sekundy=sekundy-10;
	}
}

if (sekundy<=0){
	sekundy=0;
	}
}

void sound_generator (void){  //generování obdel. signálu pro zvukovou signalizaci
	static uint32_t last_time1=0;  
  if((milis() - last_time1 >= 500) && alarm==1 && odpocet==1){ // pravidelně měníme frekvenci/periodu
		last_time1 = milis();
		TIM2_Cmd(ENABLE);
		if(milis() - last_time1 >= 250){
			TIM2_Cmd(DISABLE);
		} 
  }
	if (alarm!=1){ 
		TIM2_Cmd(DISABLE);
	}
}

void mod (void){  //zde se pomocí tlačítka na enkodéru mění "módy" = nastavování času a odpočítávání(minutky)
static uint8_t zmena=0;
if(GPIO_ReadInputPin(GPIOE, GPIO_PIN_4) == RESET && zmena==0){
	 odpocet=1;
	 zmena=1;
	} else if(GPIO_ReadInputPin(GPIOE, GPIO_PIN_4) == RESET && zmena==1){
	 odpocet=0;
	 alarm=0;
	 led=0;
	 zmena=0;
	}
}

void minutky (void){  //zde se provádí funkce spojené s odpočítáváním již nastaveného času
if (odpocet==1 && sekundy>0){ //odpočítávání nastaveního času po sekundách
	
	if (help3==0 && sekundy>10){
		sekundy=sekundy-10;
		help3=1;
	}

	if(doba - cas >= 1000){
		cas = milis();
		sekundy--;
		pocty3();
		display();
	}
}

	if (sekundy<1 && odpocet==1){   //když dojde čas
		sekundy=0;
		alarm=1;	//spuštění zvukové signalizace
		led=1; //spuštění světelné signalizace
		vteriny=0;
		help3=0;
	}

if (led==1){	//světelná signalizace po skončení odpočítávání
	if(doba - trvani >= 200){
		trvani = milis();
		GPIO_WriteReverse(GPIOC,GPIO_PIN_5);
	}
	}else{
		GPIO_WriteLow(GPIOC,GPIO_PIN_5);
	}

}

@svlreg INTERRUPT_HANDLER (TIM3_UPD_OVF_BRK_IRQHandler,15){ // inicializace přerušení
	TIM3_ClearITPendingBit(TIM3_IT_UPDATE);
	process_enc();
	mod();
 }

void process_enc(void){ // nastavování času pomocí enkodéru
	static uint8_t last_time2=1;
	if (odpocet==0){
		if(GPIO_ReadInputPin(GPIOF,GPIO_PIN_7) == RESET && last_time2==1){
			last_time2 = 0;
			if(GPIO_ReadInputPin(GPIOF,GPIO_PIN_6) == RESET){
				nastav_cas1();
			}else{
				nastav_cas2();
			}
		}
		if(GPIO_ReadInputPin(GPIOF,GPIO_PIN_7) != RESET){
			last_time2 = 1;
		}
	}
}

void init_enc(void){ //inicializace enkodéru
GPIO_Init(GPIOF,GPIO_PIN_7,GPIO_MODE_IN_PU_NO_IT); 
GPIO_Init(GPIOF,GPIO_PIN_6,GPIO_MODE_IN_PU_NO_IT);
}

void init_timer(void){ // nastavení timeru
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
