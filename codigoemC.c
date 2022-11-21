#include <reg51.h>

#define onda					100
#define cem 					100
#define setentaEcinco 75
#define cinquenta 		50
#define vinteEcinco 	25
#define zero 					0

sbit led = P1^0;
sbit pino1 = P1^1;

unsigned char pino2 = 0;
unsigned char intensidade = zero;
unsigned char palmas = 0;

unsigned int conta = 0; //nº vezes que ocorre 0.2ms
unsigned int contaPalmas = 0;

bit onoff = 0;
bit palmaDetetada = 0;

	void Init(void){
		led = 0;
		pino1 = 1;
		pino2 = 0;
		onoff = 1;
	
		EA = 1;  //Ativa interrupcoes externas
		ET0 = 1; //Ativa interrupcao do timer
		
		EX0 = 1;	//Ativa interrupcao externa 0 - Luminosidade
		EX1 = 1;  //Ativa interrupcao externa 1	- Palmas
		
		TMOD &= 0xF0; //ver teorica????
		TMOD |= 0x02; //    ''
		
		TH0 = 0x38;	//Temporizador high bits
		TL0 = 0x38;	//Temporizador low bits
		
		TR0 = 1;	//Interrupcao do timer
		IT0 = 1;	//Ativar falling_edge EX0
		IT1 = 1;	//Ativar falling_edge EX1
		
	}
	
	
	void main(void){
		//Inicializadores
		Init();
		
		for(;;){
			
//---------------------------------------LigaDesliga--------------------------------------
			if (pino1 == 0){							//Pressionar botão
				if(onoff == 0){
					onoff = 1;
					intensidade = cem;
				}else{
					onoff = 0;
				}
				while(pino1 == 0){}				//Mantém o valor enquanto nao voltar a premir o botao
			}
			
			//LigarPalmas
			if(contaPalmas >= (10000 * (palmas + 1))){ //se n detetou palmas no quad seguinte
				if(palmas >= 2){ //se nº palmas -> liga
					onoff = !onoff;
				}
				palmas = 0; //reset
				contaPalmas = 0; //reset
			}
			if(palmaDetetada == 1){
				if(contaPalmas >= (10000 * palmas) && contaPalmas < (10000 * (palmas + 1))){
					palmas++; //deteta 1 palma dps verifica se 2ª palma encontra.se no intervalo de 2s
				}
				palmaDetetada = 0;
			}
			
			//MudaIntensidade
			
			if(onoff == 1){	
				if(conta >= onda){
					conta = 0; //reinicia a contagem
					led = 1; //impulso positivo até atingir o valor de referencia
				}
				if(conta >= intensidade && intensidade != cem){
					led = 0; //coloca a saída a 0 até atingir os 20ms
				}
				if(pino2 == 1){
					switch(intensidade){
						case zero:										
							intensidade = vinteEcinco;
						break;
						case vinteEcinco:
							intensidade = cinquenta;
						break;
						case cinquenta:
							intensidade = setentaEcinco;
						break;
						case setentaEcinco:
							intensidade = cem;
						break;
						case cem:
							intensidade = zero;
						break;
					}
					pino2 = 0;
				}
			}
			else{
				led=0;
			}
		}	
	}

	void External0_ISR(void) interrupt 0{ //interrupcao qnd
		pino2 = 1;										//user toca botao
	}

	void External1_ISR(void) interrupt 2{ //interrupcao qnd
		palmaDetetada = 1;									//bate uma palma
	}
	
	void Timer0_ISR(void) interrupt 1{ //qnd timer=200us
		if(onoff==1){
			conta++;											//incrementa 1
		}		
		if (palmas >= 1){			//se nº palma >=1
			contaPalmas++;			//incrementa 1
		}
	}

