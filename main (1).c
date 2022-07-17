#include "derivative.h" /* include peripheral declarations */
#include "math.h"
//Código final para la silla de ruedas

#define SLAVE_ADDR 0x1D     /* 001 1101. */
#define ERR_NONE 0
#define ERR_NO_ACK 0x01
#define ERR_ARB_LOST 0x02
#define ERR_BUS_BUSY 0x03

unsigned char dum_d=0;

unsigned short voltaje_DAC=1800;		//mV
 short ref_vieja=0;

//variables I2C
unsigned char data2;
int rv;
int r=0;
unsigned char k;

unsigned char datos[2];
int count;


unsigned short t_viejo=0; 
unsigned short frecuencia_referencia=0;			//velocidad deseada			

unsigned char dummy_frecuencia;
unsigned char dummy_v;

//Variables PIT sensores//////////////////////////////////////////////////////////////////////////////////////////
#define valor_minimo_bateria 120			//0 a 255
#define valor_maximo_temp 150			//0 a 255

//Variable de modo automático
unsigned char modo=0;

//variable de frecuencia para modo automático
unsigned char frecuencia_tpm=0; //variable para saber la frecuancia que lee el TPM

//LEDS
#define valor_LED_V 200			//0 a 255
#define valor_LED_A 120			//0 a 255

//variable para grados medidos por acelerómetro
short grados_medidos=0;

//dummy
unsigned char dummy_buzzer;
unsigned char dummy_T;
unsigned char probar;



unsigned char canal=0;

unsigned char ADC_channels[]={(1<<6)+8,(1<<6)+9, (1<<6)+12 };   //valores para canal 8 (PTB0)) y canal 9 (PTB1)


unsigned short dum;
unsigned short dum_t;




int I2C0_byteWrite(unsigned char slaveAddr, unsigned char memAddr, unsigned char data) {

/*Solo para asegurar bus en estado idle	
	int retry = 1000;
    
    while (I2C0_S & (1<<5)) 		{    // wait until bus is available
        if (--retry <= 0)
            return ERR_BUS_BUSY;
        delayUs(100);
    }
*/
	
    /* send start */
    I2C0_C1 |= (1<<4);           // Tx on 
    I2C0_C1 |= (1<<5);           // become master, start

    /* send slave address and write flag */
    I2C0_D = slaveAddr << 1;	// slaveADDR-0-  0 indica write
    while(!(I2C0_S & 0x02));   // wait for transfer complete
    //while ((I2C0_S & 0x02)==0);	
    
    
    I2C0_S |= 0x02;            // clear IF 
    /*
    if (I2C0_S & 0x10) {       // arbitration lost
        I2C0_S |= 0x10;        // clear IF 
        return ERR_ARB_LOST;
    }
    */
    
    if (I2C0_S & 0x01)         /* got NACK from slave */
        return ERR_NO_ACK;

    /* send memory address */
    I2C0_D = memAddr;
    while(!(I2C0_S & 0x02));   /* wait for transfer complete */
    I2C0_S |= 0x02;            /* clear IF */
    if (I2C0_S & 0x01)         /* got NACK from slave */
        return ERR_NO_ACK;

    /* send data */
    I2C0_D = data;
    while(!(I2C0_S & 0x02));   /* wait for transfer complete */
    I2C0_S |= 0x02;            /* clear IF */
    if (I2C0_S & 0x01)         /* got NACK from slave */
        return ERR_NO_ACK;

    /* stop */
    I2C0_C1 &= ~0x30;			//~0x30 = 0xCF 1100 1111  Apaga bit 4 y bit 5

    return ERR_NONE;
}

void delayUs(int n)
{
    int i; int j;
    for(i = 0 ; i < n; i++) {
        for(j = 0; j < 7; j++) ;
    }
}

int I2C0_burstRead(unsigned char slaveAddr, unsigned char memAddr, int byteCount, unsigned char* data, int* cnt)
{
unsigned char dummy;

	/* send start */
	I2C0_C1 |= (1<<4);           // Tx on 
    I2C0_C1 |= (1<<5);           // become master, start

    /* send slave address and write flag */
    I2C0_D = slaveAddr << 1;	// slaveADDR-0-  0 indica write
    while(!(I2C0_S & 0x02));   // wait for transfer complete
    I2C0_S |= 0x02;            // clear IF    

    if (I2C0_S & 0x01)         /* got NACK from slave */
        return ERR_NO_ACK;

    /* send memory address */
    I2C0_D = memAddr;
    while(!(I2C0_S & 0x02));   /* wait for transfer complete */
    I2C0_S |= 0x02;            /* clear IF */
    if (I2C0_S & 0x01)         /* got NACK from slave */
        return ERR_NO_ACK;
    
    /* restart */
    I2C0_C1 |= 0x04;           /* send Restart */

    /* send slave address and read flag */
    I2C0_D = (slaveAddr << 1) | 1;
    while(!(I2C0_S & 0x02));   /* wait for transfer complete */
    I2C0_S |= 0x02;            /* clear IF */
    if (I2C0_S & 0x01)         /* got NACK from slave */
        return ERR_NO_ACK;

    /* change bus direction to read */
    I2C0_C1 &= ~0x18;          /* Tx off, prepare to give ACK */
    if (byteCount == 1)
        I2C0_C1 |= 0x08;       /* prepare to give NACK */
    dummy = I2C0_D;            /* dummy read to initiate bus read */

    /* read data */
    while (byteCount > 0) {
        if (byteCount == 1)
            I2C0_C1 |= 0x08;       /* prepare to give NACK for last byte */
        while(!(I2C0_S & 0x02));   /* wait for transfer complete */
        I2C0_S |= 0x02;            /* clear IF */
        if (byteCount == 1) {
            I2C0_C1 &= ~0x20;      /* stop the bus before reading last byte */
        }
        //*data = I2C0_D;          /* read received data */
       k=I2C0_D;
       
      unsigned char contador2=8; //para el siguiente while, variable local
      while (contador2 != 0 ){
    	  
    	//do{}while((UART0_S1&(1<<7))==0); //esperar para mandar siguiente dato
       
       	if ((k & (1<<(contador2-1)))!=0){
       	//UART0_D= '1';//mandar info	
       	}
       	else{
       		//UART0_D= '0';//mandar info	
       	}
       	
       contador2--;
      }
    
      
      
        byteCount--;
        (*cnt)++;
        
    }

    return k;
}

int unir_mag(unsigned short a, unsigned short b){
	unsigned char contador=14;
	long magnitud=0;
	
	a=(a<<8)|(b<<0);
	
	a = a>>2;
	
	while (contador!=0){
		if ((contador==14) && ((a & 1<<(13))!=0)){
			magnitud = magnitud + ((pow(2,(contador-1))));
			magnitud = magnitud*(-1);
		}
	    else if ((a & 1<<(contador-1))!=0){
	    	magnitud = magnitud + (pow(2,(contador-1)));
		}
		
		contador = contador-1;
	}
	return magnitud;
}

int obtener_grados(a,b){
	short grados;

	
	if((a>0) && (b<0)){ //primer cuadrante 0-90
		b=b*(-1);
		grados = 180 - ((atan2(a,b))*180000000/3141592);
	}
	else if((a<0) && (b<0)){ //cuadrante 2 90-180
		b=b*(-1);
		a=a*(-1);
		grados = 270 - ((atan2(b,a))*180000000/3141592);
	}
	else if((a<0) && (b>0)){ //tercer cuadrante 180-270
		a=a*(-1);
		grados = 360 - ((atan2(a,b))*180000000/3141592);
	}
	else if((a>0) && (b>0)){ //cuarto cuadrante 270-360
		grados = 90 - ((atan2(b,a))*180000000/3141592);
	}
	
	
	return grados;
}



////////////////////////////////////////////////////////////////////////////////

void FTM1_IRQHandler()
{
unsigned short periodo;
unsigned long frecuencia;
unsigned char n=0;

static unsigned short i=0;


static unsigned short rpm=0;
static unsigned short vel_cm_por_segundo=0;


	if ((TPM1_SC & (1<<6))!=0)			//overflow del timer, "dio la vuelta"
	{
		TPM1_SC|=(1<<7);				//apagar bandera
		n++;
	}

	if ((TPM1_C0SC & (1<<7))!=0)
	{
		TPM1_C0SC|=(1<<7);				//Apagar bandera de C0F
		
		if (TPM1_C0SC>t_viejo)			//T2>T1
		{	
			periodo=TPM1_C0V-t_viejo + (n*65536);
		
		}
		else							//T1>T2
		{
			periodo=TPM1_C0V-t_viejo + ((n-1)*65536);
		}
		
		//frecuencia=1/periodo;
		frecuencia=1000000/periodo;
		frecuencia_tpm = frecuencia;
		vel_cm_por_segundo = frecuencia * (31416/10000) * (165000/10000);
		
		
		
		
//Control **********
// Entrada 1: velocidad de referencia
// Entrada 2: velocidad real
// Salida: valor para el DAC para igualar las velocidades
		
		if (frecuencia>frecuencia_referencia)   // 4.4 > 2.1
		{
			//disminuir el voltaje
			if (voltaje_DAC>=10){
			voltaje_DAC=voltaje_DAC-5;				//Paso de 10 mV

			}
		}
		
		else if (frecuencia<frecuencia_referencia)
		{
			//aumentar el voltaje
			if (voltaje_DAC<=3290){ //
			voltaje_DAC=voltaje_DAC+5;				//Paso de 10 mV
	
			}
		}

		dummy_v=voltaje_DAC;
		i=(voltaje_DAC*4095)/3300;
		DAC0_DAT0L=i&0xFF;
		DAC0_DAT0H=(i>>8);
		
//************		
		t_viejo=TPM1_C0V;
		n=0;
	}

}



void PORTD_IRQHandler()
{
	static char espejo=0; //variable para hacer un Toggle por software en el modo aut y manual
	unsigned char i;
	if ((PORTD_PCR0 & (1<<24))!=0)
	{
		PORTD_PCR0|=(1<<24);
		
		frecuencia_referencia=0;
		ref_vieja=0;
		
		PIT_LDVAL0= 4000000/1;		// Clk PIT 4 MHz (250 ns) 5 Hz (200 ms) 
		PIT_TCTRL0=1;				//  b0=1 hab conteo
		
		do{
			i=(0*4095)/3300;
			DAC0_DAT0L=i&0xFF;
			DAC0_DAT0H=(i>>8);
		}while ((PIT_TFLG0&(1<<0))==0);
			
		PIT_TCTRL0=0;				//Detener timer
		
		
		
	}
	if ((PORTD_PCR5 & (1<<24))!=0)
	{
		PORTD_PCR5|=(1<<24);
		if (espejo==0){
					
			modo=1; //el 1 es para modo automático
			espejo=1;
		}
		else{
					
			modo=0; //el 0 es para modo manual
			espejo=0;
		}
	}
	

}



void ADC0_IRQHandler() //ISR ADC0
{
static unsigned char alarma_temp=0;
static unsigned char alarma_bat=0;
static unsigned short ayuda;
static unsigned char valor_ADC;
valor_ADC=ADC0_RA; //leer valor, apagar COCO


	if (canal==0)			//PTB0   bateria
	{
		if (valor_ADC<valor_minimo_bateria)		
		{
			alarma_bat=1;	
		}
		else
		{
			alarma_bat=0;
		}
		
		if(valor_LED_V<=valor_ADC){
			GPIOC_PSOR= (1<<11); 
			GPIOC_PCOR= (1<<10);
			GPIOC_PCOR= (1<<9);
		}
		else if((valor_LED_A<=valor_ADC) && (valor_ADC<valor_LED_V)){
			GPIOC_PCOR= (1<<11);
			GPIOC_PSOR= (1<<10);
			GPIOC_PCOR= (1<<9);
		}
		else if(valor_LED_A>=valor_ADC){
			GPIOC_PCOR= (1<<11);// verde
			GPIOC_PCOR= (1<<10);// amarillo
			GPIOC_PSOR= (1<<9);// rojo
		}
		
		
		canal=1;
	}
	
	else if(canal==1)			//PTB1  sensor temperatura
	{
		dum_t=valor_ADC;
	 	 if (valor_ADC>valor_maximo_temp)
	 	 {
	 		    alarma_temp=1; 
	 		   GPIOC_PSOR= (1<<12);// led temperatura
	 	 }
		else
		{			
				alarma_temp=0;	
				GPIOC_PCOR= (1<<12);// led temperatura
		}
	 	 
		canal=2;
		
		if(modo==1){
			canal=0;
		}
	}
	
	else if (canal==2 && modo==0){ //canal para establecer ref por potenciometro 
		
		frecuencia_referencia=(valor_ADC*140/255);
		dum=frecuencia_referencia;
		
		canal=0;
	}
	
	if(modo==1 && canal==1){
		
		unsigned short apoyo; //variable para compensar por inclinación
		
		if ( ((frecuencia_tpm-ref_vieja)>10)    ){
			
			frecuencia_referencia=frecuencia_tpm;
			ref_vieja = frecuencia_tpm;
			
		}
		
		if ((grados_medidos>=1) && (grados_medidos<=40)){
			apoyo=(grados_medidos/5)*3;
			frecuencia_referencia=frecuencia_referencia + apoyo;
			dummy_frecuencia=frecuencia_referencia;
		}
		else if (((grados_medidos>=270) && (grados_medidos<=345))){
			frecuencia_referencia=0;
		}
		
		
	}
	if (frecuencia_referencia<=20){
		ayuda=1;
	}
	
	if(ayuda==1 && frecuencia_referencia>20 && ((alarma_temp==0) && (alarma_bat==0))){
		voltaje_DAC=1500;
		frecuencia_referencia=25;
		ayuda=0;
	}
	
	if ((alarma_temp==1) | (alarma_bat==1) | (((grados_medidos>=270) && (grados_medidos<=345))) | (((GPIOC_PDIR & (1<<5))==0)) | (((GPIOC_PDIR & (1<<6)))==0) ){
		
		frecuencia_referencia=0;
		voltaje_DAC=0;
		ref_vieja=0;
	    
	}
	
}






void LPTimer_IRQHandler() //ISR para LPTMR0
{
	//"siempre" hay que apagar la bandera al entrar a la ISR respectiva
	LPTMR0_CSR|=(1<<7);    //b7=1 apagar la bandera, b6=1siga intr hab, b0=1 siga hab intr
	ADC0_SC1A= ADC_channels[canal];	//inicia una nueva conversiÃ³n ADC
	
	//-------------------------------------------------------
	//Sección I2C
	
	//variable para x
	unsigned short x_alta;
	unsigned char x_baja;
	//variable para z
	unsigned short y_alta;
	unsigned char y_baja;

	//variables para magnitudes
	long y_mag;
	long x_mag;
	
	
	//recolección de datos
    y_alta = I2C0_burstRead(SLAVE_ADDR, 0x03, 1, datos, &count); //dirección del esclavo, registro que quiero leer del esclavo, cuantos datos quiero leer, arregki de datos, cuantos caracteres pudo leer
    y_baja = I2C0_burstRead(SLAVE_ADDR, 0x04, 1, datos, &count);
    x_alta = I2C0_burstRead(SLAVE_ADDR, 0x01, 1, datos, &count); //dirección del esclavo, registro que quiero leer del esclavo, cuantos datos quiero leer, arregki de datos, cuantos caracteres pudo leer
    x_baja = I2C0_burstRead(SLAVE_ADDR, 0x02, 1, datos, &count);
    //unir datos
	x_mag = unir_mag(x_alta,x_baja);
	y_mag = unir_mag(y_alta,y_baja);

    //obtener grados
	grados_medidos=obtener_grados(y_mag,x_mag);
	
//------------------------------------------------------------------	
}


void portD_init()
{
	SIM_SCGC5|=(1<<12);			//Clk PORTD
	PORTD_PCR0=(1<<8)+(10<<16)+3;   //GPIO, Intr falling edge, pullup 
	PORTD_PCR5=(1<<8)+(10<<16)+3;   //GPIO, Intr falling edge, pullup 

	NVIC_ISER=(1<<31);
	
	NVIC_IPR7|=(3<<47); //PRIORIDAD
	
}

void clk_init (void)
{
// FIRC = 4 MHz. BusClk = 4 MHz
// UART0: FIRC. UART1: BusClk. UART2: BusClk. TPM: FIRC. IIC: BusClk. PIT:
MCG_C1|=(1<<6) + (1<<1);	//MCGOUTCLK : IRCLK. CG: Clock gate, MCGIRCLK enable pag 116
MCG_C2|=1;					//Mux IRCLK : FIRC (4 MHz) pag 116
MCG_SC=0;					//Preescaler FIRC 1:1 pag 116

SIM_CLKDIV1=0;				//OUTDIV4=OUTDIV1= 1:1 pag 116. Busclk 4 MHz
SIM_SOPT2|=15<<24;			//Seleccion MCGIRCLK tanto para UART0 como para TPM
}

void DAC0_init(void) {
	SIM_SCGC6 |= (1<<31);   		// clock to DAC module SIM_SCG6=SIMSCG6|(1<<31)
	DAC0_C0 = (1<<7)+ (1<<5);     	// enable DAC and use software trigger
}


void TPM1_CH0_init (void)
{
	//PTA12 TPM1_CH0 
	SIM_SCGC5|=(1<<9);				//hab reloj interfaz PORTA manteniendo el resto de la configuracion
	PORTA_PCR12=(3<<8);				//PTA12 toma funcion TPM1_CH0
	
	//Configurar TPM1
	SIM_SCGC6|=(1<<25);				//hab reloj interfaz TPM1 manteniendo el resto de la configuracion
	TPM1_SC=(1<<6)+(1<<3)+2;					//hab intr overflow del timer,reloj interno (4 MHz), 2: preesclaer de 4. Queda reloj de 1 MHz (1 us)
	TPM1_C0SC=(1<<2)+(1<<6);			//Input capture on rising edge, hab intr

	NVIC_ISER=(1<<18);				//Intr de TPM1 desde NVIC
}


void PORT_C_init(){
	SIM_SCGC5 |= (1<<11); //reloj de puerto c
	
	PORTC_PCR12=(1<<8);   //GPIO, temperatura
	PORTC_PCR9=(1<<8);   //GPIO, rojo
	PORTC_PCR10=(1<<8);   //GPIO, amarillo
	PORTC_PCR11=(1<<8);   //GPIO, verde
	
	PORTC_PCR5=(1<<8);   //GPIO, sensor inductivo 1 
	PORTC_PCR6=(1<<8);   //GPIO, sensor inductivo 2
	
	
	GPIOC_PDDR= (1<<12)+(1<<9) + (1<<10) + (1<<11); //pines 9, 10 y 11 como salidas del puerto C
	
	
	
}



void ADC_init(){
	SIM_SCGC6|=(1<<27);		//Reloj interfaz para ADC0
	//no es necesario configurar el registro PCR a PTB0 porque el default es entrada analÃ³gica
	
	//ADC0_SC1A= (1<<6)+8;	//hab intr ADC0, canal 8 (PTB0), al escribir en este reg
							//se inicia la conversion del ADC
	NVIC_ISER=(1<<15);		//hab interrupcion ADC0
}

void LPTMER_init(){
	SIM_SCGC5|=(1<<0);		//LPTMR

	LPTMR0_PSR=5;					//101 : bypass del preescaler, seleccion de LPO (1 KHz)
	LPTMR0_CSR=(1<<6)+1;			//Hab Intr LPtmr (bit 6), Hab timer (bit 0 = 1)
	LPTMR0_CMR=500-1;
	
	NVIC_ISER=(1<<28);				//Hab intr LPTMR0 desde NVIC (pag 53)
}

void i2c_init(){
	
	SIM_SCGC5|=(1<<13)+(1<<9);    //PORTE y PORTA
	PORTA_PCR14=(1<<8);			//faltar[ia definir el tipo de interrupci[on
	PORTA_PCR15=(1<<8);			//faltar[ia definir el tipo de interrupci[on
	
	SIM_SCGC4|=(1<<6);
	PORTE_PCR24=(5<<8);				//IIC - SCL
	PORTE_PCR25=(5<<8);				//IIC - SDA
	
    I2C0_F =0x1c; // (1<<6) + 21;  //0x1C;             /* set clock to 97.09KHz @13.981MHz bus clock */ //OJO registro muy importante
    I2C0_C1 = 0x80;               /* enable I2C0 */
}


void PIT_init ()
{
	SIM_SCGC6|=(1<<23);
	PIT_MCR=0;						//Habilitar modulo PIT
	//NVIC_ISER=(1<<22);
}

int main(void)
{
	
	unsigned char IIC_error_value;
	
	clk_init();
	i2c_init();
	//escribir para prender el acelerometro en modo constante
	IIC_error_value = I2C0_byteWrite(SLAVE_ADDR, 0x2A, 0x01);
	
	TPM1_CH0_init();
	portD_init();
	DAC0_init();
	PORT_C_init();
	
	PIT_init(); //idea nueva
	
	ADC_init();
	LPTMER_init();
	while (1);
	
	return 0;
}
