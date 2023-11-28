/*
 * LpdcLogic.h
 *
 *  Created on: 04-Feb-2023
 *      Author: MKS
 */

#ifndef SRCCPP_BUSSINESSLOGIC_APPLICATIONSW_H_
#define SRCCPP_BUSSINESSLOGIC_APPLICATIONSW_H_


#include "main.h"

class Powermeter {
public:
	Powermeter();
	virtual ~Powermeter();
	void run();
	GPIO_PinState readStartbutton;
	GPIO_PinState readEndbutton;


private:
	void m_selectframedata();
};

#endif /* SRCCPP_BUSSINESSLOGIC_APPLICATIONSW_H_ */
