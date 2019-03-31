/*
 * Accelerometer.cpp
 *
 *  Created on: 7 de mar de 2019
 *      Author: robertcabral
 */

#include "I2C.h"
#include <MKL25Z4.h>
#include <math.h>
#include "MMA8451Q.h"

MMA8451Q::MMA8451Q(mma8451q_frequency_t freq, mma8451q_range_t range, mma8451q_power_t power) : i2c(I2C0, MMA8451_I2C_ADDRESS) {

	// INITIALIZING VARIABLES
	this->freq = freq;
	this->range = range;
	this->power = power;

	// ENABLE MMA8451Q AND FREQUENCY RATE
	this->i2c.writeRegister(CTRL_REG1, CTRL_REG1_ACTIVE_MASK | (freq << CTRL_REG1_ASPL_RATE_SHIFT));

	// CONFIG RANGE
	this->i2c.writeRegister(XYZ_DATA_CFG_REG, range << XYZ_DATA_CFG_RANGE_SHIFT);

	// CONFIG POWER MODE
	this->i2c.writeRegister(CTRL_REG2, power << CTRL_REG2_MODS_SHIFT);

}

void MMA8451Q::setFilter(){
	uint8_t aux = this->i2c.readRegister(XYZ_DATA_CFG_REG);
	aux |= XYZ_DATA_CFG_HPF_OUT_MASK;
	this->i2c.writeRegister(XYZ_DATA_CFG_REG, aux);
}

uint16_t MMA8451Q::getX(){
	return (this->i2c.readRegister(OUT_X_MSB) << 8) | (this->i2c.readRegister(OUT_X_LSB) >> 2);
}

uint16_t MMA8451Q::getY(){
	return (this->i2c.readRegister(OUT_Y_MSB) << 8) | (this->i2c.readRegister(OUT_Y_LSB) >> 2);
}

uint16_t MMA8451Q::getZ(){
	return (this->i2c.readRegister(OUT_Z_MSB) << 8) | (this->i2c.readRegister(OUT_Z_LSB) >> 2);
}

float MMA8451Q::getRoll(){

	Xout_14_bit = this->getX();
	Yout_14_bit = this->getY();
	Zout_14_bit = this->getZ();

	Xout_g = ((float) Xout_14_bit) / SENSITIVITY_2G;		// Compute X-axis output value in g's
	Yout_g = ((float) Yout_14_bit) / SENSITIVITY_2G;		// Compute Y-axis output value in g's
	Zout_g = ((float) Zout_14_bit) / SENSITIVITY_2G;		// Compute Z-axis output value in g's

	if (Zout_g > 0)																	// Equation 38 in the AN3461
		Roll = atan2 (Yout_g, sqrt (0.01*Xout_g*Xout_g + Zout_g*Zout_g)) * 180 / PI;
	else
		Roll = atan2 (Yout_g, - sqrt (0.01*Xout_g*Xout_g + Zout_g*Zout_g)) * 180 / PI;

	return Roll;
}

float MMA8451Q::getPitch(){

	Xout_14_bit = this->getX();
	Yout_14_bit = this->getY();
	Zout_14_bit = this->getZ();

	Xout_g = ((float) Xout_14_bit) / SENSITIVITY_2G;		// Compute X-axis output value in g's
	Yout_g = ((float) Yout_14_bit) / SENSITIVITY_2G;		// Compute Y-axis output value in g's
	Zout_g = ((float) Zout_14_bit) / SENSITIVITY_2G;		// Compute Z-axis output value in g's

	Pitch = atan2 (-Xout_g, sqrt (Yout_g*Yout_g + Zout_g*Zout_g)) * 180 / PI;

	return Pitch;

}

void MMA8451Q::calibrate(){

	uint16_t x_offset = (this->getX() / (8 *(-1)));
	uint16_t y_offset = (this->getY() / (8 *(-1)));
	uint16_t z_offset = (this->getZ() - MMA8451Q_SENSITIVITY_2G) / (8 *(-1));

	this->i2c.writeRegister(CTRL_REG1, CTRL_REG1_RESET);
	this->i2c.writeRegister(OFF_X, x_offset);
	this->i2c.writeRegister(OFF_Y, y_offset);
	this->i2c.writeRegister(OFF_Z, z_offset);
	this->i2c.writeRegister(CTRL_REG3, CTRL_REG3_PP_OD_MASK);
	this->i2c.writeRegister(CTRL_REG4, CTRL_REG4_INT_EN_DRDY_MASK);
	this->i2c.writeRegister(CTRL_REG5, CTRL_REG5_INT_CFG_DRDY_MASK);
	this->i2c.writeRegister(CTRL_REG1, CTRL_REG1_ACTIVE_MASK | (this->freq << CTRL_REG1_ASPL_RATE_SHIFT));

}
