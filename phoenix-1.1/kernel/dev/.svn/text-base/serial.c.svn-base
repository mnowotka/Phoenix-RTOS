/* 
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * UART 16450 driver
 *
 * Copyright 2001, 2005 Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * Phoenix-RTOS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Phoenix-RTOS kernel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Phoenix-RTOS kernel; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <hal/current/if.h>
#include <init/errors.h>
#include <init/std.h>
#include <task/if.h>
#include <dev/serial.h>


typedef struct {
	u16 active;
	u16 base;
	u32 irq;
	u8 rbuff[RBUFFSZ];
	unsigned int rb;
	unsigned int rp;
	u8 sbuff[SBUFFSZ];
	unsigned int sp;
	unsigned int se;
} serial_t;


serial_t serials[NPORTS];


void serial_isr(u32 irq)
{
	unsigned int k;
	serial_t *serial;
	u8 iir;
	
	for (k = 0; k < NPORTS; k++) {
		if (serials[k].irq == irq)
			break;
	}
	
	if (k == NPORTS) {
		std_printf("serial: Strange interrupt!\n");
		__intr_end(irq);
		return;
	}
	
	serial = &serials[k];
	
	for (;;) {
		if ((iir = bus_inb(serial->base + REG_IIR)) & IIR_IRQPEND)
			break;

		/* Receive */
		if ((iir & IIR_DR) == IIR_DR) {
			serial->rbuff[serial->rp] = bus_inb(serial->base + REG_RBR);
			serial->rp = (++serial->rp % RBUFFSZ);
			if (serial->rp == serial->rb) {
				serial->rb = (++serial->rb % RBUFFSZ);
			}
		}
			
		/* Transmit */
		if ((iir & IIR_THRE) == IIR_THRE) {
			serial->sp = (++serial->sp % SBUFFSZ);
			if (serial->sp != serial->se)
				bus_outb(serial->base + REG_THR, serial->sbuff[serial->sp]);
		}
	}
	
	__intr_end(irq);
	return;
}


int serial_read(unsigned int pn, u8 *buff, u16 len, u16 timeout)
{
	serial_t *serial;
	unsigned int l;
	unsigned int cnt;

	if (pn >= NPORTS)
		return ERR_ARG;
	serial = &serials[pn];
	if (!serial->active)
		return ERR_ARG;

	/* Wait for data */
	sleep_on_unintr(timeout, &serial->rp, serial->rb);
	if (serial->rp == serial->rb)
		return ERR_SERIAL_TIMEOUT;

	cli();

	if (serial->rp > serial->rb)
		l = min(serial->rp - serial->rb, len);
	else
		l = min(RBUFFSZ - serial->rb, len);

	memcpy(buff, &serial->rbuff[serial->rb], l);
	cnt = l;
	if ((len > l) && (serial->rp < serial->rb)) {
		memcpy(buff + l, &serial->rbuff[0], min(len - l, serial->rp));
		cnt += min(len - l, serial->rp);
	}
	serial->rb = ((serial->rb + cnt) % RBUFFSZ);

	sti();

	return cnt;
}


int serial_write(unsigned int pn, u8 *buff, u16 len)
{
	serial_t *serial;
	unsigned int l;
	unsigned int cnt;

	if (pn >= NPORTS)
		return ERR_ARG;
	serial = &serials[pn];
	if (!serial->active)
		return ERR_ARG;

	cli();

	if (serial->sp > serial->se)
		l = min(serial->sp - serial->se, len);
	else
		l = min(SBUFFSZ - serial->se, len);

	memcpy(&serial->sbuff[serial->se], buff, l);
	cnt = l;
	if ((len > l) && (serial->se >= serial->sp)) {
		memcpy(serial->sbuff, buff + l, min(len - l, serial->sp));
		cnt += min(len - l, serial->sp);
	}

	/* Initialize sending process */
	if (serial->se == serial->sp)
		bus_outb(serial->base, serial->sbuff[serial->sp]);
	
	serial->se = ((serial->se + cnt) % SBUFFSZ);
	sti();

	return cnt;
}


void serial_initone(unsigned int pn, u16 base, u16 irq, u16 speed, serial_t *serial)
{
	serial->base = base;
	serial->irq = irq;
	serial->rb = 0;
	serial->rp = 0;
	serial->sp = (u16)-1;
	serial->se = 0;

	if (bus_inb(serial->base + REG_IIR) == 0xff) {
		serial->active = 0;
		return;
	}
	serial->active = 1;

	cli();
	set_intr_handler(serial->irq, serial_isr);
	
	/* Set speed */
	bus_outb(serial->base + REG_LCR, LCR_DLAB);
	bus_outb(serial->base + REG_LSB, speed);
	bus_outb(serial->base + REG_MSB, 0);

	/* Set data format */
	bus_outb(serial->base + REG_LCR, LCR_D8N1);

	/* Enable hardware interrupts */
	bus_outb(serial->base + REG_MCR, MCR_OUT2);

	/* Set interrupt mask */
	bus_outb(serial->base + REG_IMR, IMR_THRE | IMR_DR);
	
	sti();
	return;
}


void serial_init(u16 speed)
{
	serial_initone(0, COM1_BASE, COM1_IRQ, speed, &serials[0]);
	serial_initone(1, COM2_BASE, COM2_IRQ, speed, &serials[1]);
	return;
}


void serial_done(void)
{
/*	unsigned int k;

	for (k = 0; k < NPORTS; k++) {
		low_cli();
		low_maskirq(serials[k].irq, 1);
		low_irquninst(serials[k].irq);
		low_sti();
		return;
	} */
}


