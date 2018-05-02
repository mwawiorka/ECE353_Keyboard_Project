#include "mcp23017.h"

//*****************************************************************************
//*****************************************************************************
static i2c_status_t mcp23017_set_addr
( 
  uint32_t  i2c_base,
  uint8_t  address
)
{
  i2c_status_t status;
  
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};

  //==============================================================
  // ADD CODE
  // Set the I2C address 
  //==============================================================
  status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_WRITE);
		
    
    
  if ( status != I2C_OK )
  {
    return status;
  }
  
  //==============================================================
  // ADD CODE
  // Send the address
  //==============================================================
	status = i2cSendByte(i2c_base, address ,I2C_MCS_START | I2C_MCS_RUN |I2C_MCS_STOP);

  return status;
}

//*****************************************************************************
//*****************************************************************************
static i2c_status_t mcp23017_write_data
( 
  uint32_t i2c_base,
	uint8_t reg,
  uint8_t data
)
{
  i2c_status_t status;
  
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};

  status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID , I2C_WRITE);
    
  if ( status != I2C_OK )
  {
    return status;
  }

  status = i2cSendByte( i2c_base, reg, I2C_MCS_START | I2C_MCS_RUN);
	
	if ( status != I2C_OK )
  {
    return status;
  }
	
	status = i2cSendByte( i2c_base, data, I2C_MCS_RUN | I2C_MCS_STOP);


  return status;
}

//*****************************************************************************
//*****************************************************************************
static i2c_status_t mcp23017_read_data
( 
  uint32_t i2c_base,
	uint8_t reg,
  uint8_t *data
)
{
  i2c_status_t status;
  
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};
		
	status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID , I2C_WRITE);
	
  if ( status != I2C_OK )
  {
    return status;
  }

  status = i2cSendByte( i2c_base, reg, I2C_MCS_START | I2C_MCS_RUN);
	
	if ( status != I2C_OK )
  {
    return status;
  }	

  status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID , I2C_READ);
    
  if ( status != I2C_OK )
  {
    return status;
  }

  status = i2cGetByte( i2c_base, data , I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);

  return status;
}

//*****************************************************************************
//*****************************************************************************
void mcp23017_write_reg(uint8_t reg, uint8_t data){
	mcp23017_write_data(MCP23017_I2C_BASE, reg, data);
}

//*****************************************************************************
//*****************************************************************************
uint8_t mcp23017_read_reg(uint8_t reg){
	uint8_t data = 0x00;
	// Should read from intcap
	mcp23017_read_data(MCP23017_I2C_BASE, reg, &data);
	
	return data;
}

//*****************************************************************************
//*****************************************************************************
void mcp23017_config(void){
	// config button
	mcp23017_write_reg(MCP23017_GPINTENB_R, 0x0F);
	//mcp23017_write_reg(MCP23017_DEFVALB_R, 0xFF);
	//mcp23017_write_reg(MCP23017_INTCONB_R, 0x0F);
	mcp23017_write_reg(MCP23017_GPPUB_R, 0x0F);
	
	// config led
	mcp23017_write_reg(MCP23017_IODIRA_R, 0x00);
}

//*****************************************************************************
//*****************************************************************************
void mcp23017_gpio_irq_config(void){
	SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_R5;
  while (! (SYSCTL->PRGPIO & SYSCTL_PRGPIO_R5) ) {}
	
	gpio_enable_port(MCP23017_IRQ_GPIO_BASE);
	gpio_config_digital_enable(MCP23017_IRQ_GPIO_BASE,MCP23017_IRQ_PIN_NUM);
	gpio_config_enable_input(MCP23017_IRQ_GPIO_BASE,MCP23017_IRQ_PIN_NUM);
		
	GPIOF->IS &= ~MCP23017_IRQ_PIN_NUM;
	GPIOF->ICR |= MCP23017_IRQ_PIN_NUM;
	GPIOF->IBE &= ~MCP23017_IRQ_PIN_NUM;
	GPIOF->IEV &= ~MCP23017_IRQ_PIN_NUM;
	GPIOF->IM |= MCP23017_IRQ_PIN_NUM;
	NVIC_SetPriority(GPIOF_IRQn, 1);
	NVIC_EnableIRQ(GPIOF_IRQn);
}

//*****************************************************************************
//*****************************************************************************
bool mcp23017_init(void){
	//==============================
  // Configure I2C GPIO Pins
  //==============================  
  if(gpio_enable_port(MCP23017_GPIO_BASE) == false)
  {
    return false;
  }
  
  // Configure SCL 
  if(gpio_config_digital_enable(MCP23017_GPIO_BASE, MCP23017_I2C_SCL_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_alternate_function(MCP23017_GPIO_BASE, MCP23017_I2C_SCL_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_port_control(MCP23017_GPIO_BASE, MCP23017_I2C_SCL_PCTL_M, MCP23017_I2C_SCL_PIN_PCTL)== false)
  {
    return false;
  }
    

  
  // Configure SDA 
  if(gpio_config_digital_enable(MCP23017_GPIO_BASE, MCP23017_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_open_drain(MCP23017_GPIO_BASE, MCP23017_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_alternate_function(MCP23017_GPIO_BASE, MCP23017_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_port_control(MCP23017_GPIO_BASE, MCP23017_I2C_SDA_PCTL_M, MCP23017_I2C_SDA_PIN_PCTL)== false)
  {
    return false;
  }
  
  
  //  Initialize the I2C peripheral
  if( initializeI2CMaster(MCP23017_I2C_BASE)!= I2C_OK)
  {
    return false;
  }
  
	mcp23017_config();
	mcp23017_gpio_irq_config();
	
  return true;
}