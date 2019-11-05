// *** Hardwarespecific functions ***

//#define FSMC_TFT
#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

typedef struct
{
	volatile uint16_t  REG;
	volatile uint16_t  RAM;
} LCD_IO_TypeDef;



#if defined(STM32GENERIC)||defined(STM32F107xC)

#if __has_include("bsp.h")
#  include "bsp.h"
#endif
#define TFT_LCD_BASE 0x40011800
#define   TFT_LCD             ((LCD_IO_TypeDef *) TFT_LCD_BASE)
	#if defined(STM32F107xC)
	//#define cbi(reg, bitmask) {*reg &= ~bitmask;}
	//#define cbi(reg, bitmask) HAL_GPIO_WritePin(reg,bitmask,GPIO_PIN_RESET)
	//#define sbi(reg, bitmask) HAL_GPIO_WritePin(reg,bitmask,GPIO_PIN_SET)
	//GPIOB->BSRR = (uint32_t)LCD_nWR_Pin << 16;
	#define sbi(reg, bitmask) *reg |= bitmask
	#define cbi(reg, bitmask) *reg |= bitmask
    //GPIOB->BSRR = LCD_nWR_Pin;
	#endif


void UTFT::_hw_special_init(void)
{
 // STM_FSMC_LCD_Init();
}

#elif defined(ARDUINO_ARCH_MAPLE_STM32F1)||defined(ARDUINO_ARCH_MAPLE_STM32F4)
// FSMC_NOR_PSRAM_REGION4                           ((void*)0x6C000000)
#define LCD_REG              (*((volatile unsigned short *) 0x6C000000))  /* RS = 0 */
#define LCD_RAM              (*((volatile unsigned short *) 0x6C000002))  /* RS = 1 */
void UTFT::_hw_special_init()
{
//    pinMode(PA1, OUTPUT); digitalWrite(PA1, HIGH); // Set the backlighe HIGH move to class init
    fsmc_nor_psram_reg_map *regs;
//    fsmc_sram_init_gpios();   //已在board.cpp 初始化norsram nand时初始化
//    rcc_clk_enable(RCC_FSMC); //已在board.cpp 打开时钟

	regs = FSMC_NOR_PSRAM4_BASE;  //nand blank4 PG12/FSMC_NE4
    regs->BCR = (FSMC_BCR_WREN  | FSMC_BCR_MWID_16BITS | FSMC_BCR_MTYP_SRAM |
                 FSMC_BCR_MBKEN);
    fsmc_nor_psram_set_addset(regs, 0);
    fsmc_nor_psram_set_datast(regs, 3);	
}

#else
	#error "arch err"
#endif


//VH==0 REG
void UTFT::LCD_Writ_Bus(int VH,int VL, byte mode) 
{   

#if defined(STM32F107xC)
	if (VH==0) 
	{
		//HAL_GPIO_WritePin(FILAMENT_DI_GPIO_Port,FILAMENT_DI_Pin, GPIO_PIN_RESET);
		GPIOE->ODR = (uint16_t)VL;
		HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_SET);
		//HAL_GPIO_WritePin(FILAMENT_DI_GPIO_Port,FILAMENT_DI_Pin, GPIO_PIN_SET);
	}
	else
	{
		//HAL_GPIO_WritePin(POWER_DI_GPIO_Port,POWER_DI_Pin, GPIO_PIN_SET);
		
		GPIOE->ODR=(uint16_t)VH;
		HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_SET);
		GPIOE->ODR=(uint16_t)VL;
		HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_SET);
		
		//HAL_GPIO_WritePin(POWER_DI_GPIO_Port,POWER_DI_Pin, GPIO_PIN_RESET);
	}
#else
	     if (VH == 0) TFT_LCD->REG = VL;
     else TFT_LCD->RAM = VL;
    HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_SET);
#endif
   	
   
}

void UTFT::_set_direction_registers(byte mode)
{

}

void UTFT::_fast_fill_16(int ch, int cl, long pix)
{ 
    volatile uint32_t i;
	for(i=0;i<pix;i++)
	{
		
		LCD_Write_DATA(ch,cl);		  
		
	}
}

void UTFT::_fast_fill_8(int ch, long pix)
{
	volatile uint32_t  i; 
	for(i=0;i<pix;i++)
	{
		
		LCD_Write_DATA(ch);
		//	LCD_Write_DATA(ch>>8);	  
		
	}

}

__inline void UTFT::Blip(int numb)
{
	return;
	for (int x=0;x<numb;x++)
	{
	HAL_GPIO_WritePin(POWER_DI_GPIO_Port,POWER_DI_Pin, GPIO_PIN_RESET);
	delay(5);
	HAL_GPIO_WritePin(POWER_DI_GPIO_Port,POWER_DI_Pin, GPIO_PIN_SET);
	}
	delay(50);
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : controller command
* Input          : - cmd: writes command.
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
__inline void UTFT::LCD_Write_COM(int com1)
{
	/* Write cmd */
	
	
	
	#if defined(STM32F107xC)
	
	HAL_GPIO_WritePin(LCD_nCS_GPIO_Port,LCD_nCS_Pin, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(LCD_nRD_GPIO_Port,LCD_nRD_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port,LCD_RS_Pin, GPIO_PIN_RESET);
	GPIOE->ODR = (uint16_t)com1;
	HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_SET);
	
#else
	TFT_LCD->REG = com1;
	HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_SET);
	#endif
}

/*******************************************************************************
* Function Name  : LCD_WriteRAM
* Description    : Writes to the LCD RAM.
* Input          : - data: the pixel color in RGB mode (5-6-5).
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
__inline void UTFT::LCD_Write_DATA(int VL)					 
{
	/* Write 8-bit data */
	//Blip(2);
	#if defined(STM32F107xC)
	HAL_GPIO_WritePin(LCD_nCS_GPIO_Port,LCD_nCS_Pin, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(LCD_nRD_GPIO_Port,LCD_nRD_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port,LCD_RS_Pin, GPIO_PIN_SET);
	
	GPIOE->ODR = (uint16_t)VL;
	HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_SET);
	#else
	TFT_LCD->RAM = VL;
	HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_SET);
		#endif
}

__inline void UTFT::LCD_Write_DATA(int VH,int VL)
{
	//Blip(4);
	/* Write 16-bit data */
	#if defined(STM32F107xC)
	
	HAL_GPIO_WritePin(LCD_nCS_GPIO_Port,LCD_nCS_Pin, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(LCD_nRD_GPIO_Port,LCD_nRD_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port,LCD_RS_Pin, GPIO_PIN_SET);
	
	GPIOE->ODR = (uint16_t)((VH<<8)+VL);
	HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_SET);
	
	GPIOE->ODR = (uint16_t)VL>>8;
	HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_SET);
	
	#else
	TFT_LCD->RAM = (uint16_t)((VH<<8)+VL);
	HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_SET);
		#endif
}

__inline void UTFT::LCD_Write_COM_DATA(int com1,int dat1)
{
	LCD_Write_COM(com1);
	LCD_Write_DATA(dat1);
	//Blip(6);
	return;
		#if defined(STM32F107xC)
		HAL_GPIO_WritePin(LCD_nCS_GPIO_Port,LCD_nCS_Pin, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(LCD_nRD_GPIO_Port,LCD_nRD_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port,LCD_RS_Pin, GPIO_PIN_RESET);
	GPIOE->ODR = (uint16_t)com1;
	HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port,LCD_RS_Pin, GPIO_PIN_SET);
	GPIOE->ODR = (uint16_t)dat1;
	HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_SET);
	#else
	
     TFT_LCD->REG = com1;
     
	 TFT_LCD->RAM = dat1;
	 HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LCD_nWR_GPIO_Port,LCD_nWR_Pin, GPIO_PIN_SET);
		#endif
}
/*******************************************************************************
* Function Name  : LCD_ReadRAM
* Description    : Reads the LCD RAM.
* Input          : None
* Output         : None
* Return         : LCD RAM Value.
* Attention		 : None
*******************************************************************************/
__inline uint16_t LCD_ReadData(void)
{  
	/* Read 16-bit data */
	return TFT_LCD->RAM;
}



#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif