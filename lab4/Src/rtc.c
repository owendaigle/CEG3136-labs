// Real-time clock (RTC) driver
#include <stdio.h>
#include "stm32l5xx.h"
#include "rtc.h"

// Async/sync pre-scalers (recommended values)
#define PDA (128 - 1)
#define PDS (256 - 1)

// --------------------------------------------------------
// Initialization
// --------------------------------------------------------
void RTC_Enable (void) {
	// Enable clock to PWR block and disable backup domain write protection
	// Refer to MCU RM 9.8.19 and 8.6.1 and device header file
	RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;//1 << 28; // PWREN
	PWR->CR1 |= PWR_CR1_DBP;//1 << 8; //disable backup write prot

	// Enable LSE 32.768kHz, select LSE for RTC clock, and enable RTC clock
	// Refer to MCU RM 9.8.29 and device header file
	RCC->BDCR |= RCC_BDCR_LSEON; //1 << 7; //enable lse 32.768 MHz
	RCC->BDCR |= 0b01 << RCC_BDCR_RTCSEL_Pos;//0b01 << 8; //select lse oscillator clock
	RCC->BDCR |= RCC_BDCR_RTCEN;//1 << 15; //enable rtc clk

	// Enable APB clock to RTC peripheral
	// Refer to MCU RM 9.8.19 and device header file
	RCC->APB1ENR1 |= RCC_APB1ENR1_RTCAPBEN;//1 << 10;

	// Disable RTC write protection (2 writes)
	// Refer to MCU RM 41.6.10 and 41.3.11, 2nd subsection
	RTC->WPR |= 0xCA;
	RTC->WPR |= 0x53;

	// Asynchronous and synchronous pre-scalers
	// Refer to MCU RM 41.3.6 and 41.6.5. Use the PDA and PDS constants.
	RTC->PRER |= PDA << 16 | PDS;

}
// --------------------------------------------------------
// Get time/date from RTC
// --------------------------------------------------------
// Generate time string from RTC, formatted HH:MM:SS (24-hour clock)
// Refer to MCU RM 41.6.1
void RTC_GetTime (char *time) {
	uint32_t tr;
	char ht, hu, mnt, mnu, st, su;
	tr = RTC->TR;
	ht = '0' + ((tr & RTC_TR_HT_Msk ) >> RTC_TR_HT_Pos ); // Hour
	hu = '0' + ((tr & RTC_TR_HU_Msk ) >> RTC_TR_HU_Pos );
	mnt = '0' + ((tr & RTC_TR_MNT_Msk ) >> RTC_TR_MNT_Pos );
	mnu = '0' + ((tr & RTC_TR_MNU_Msk ) >> RTC_TR_MNU_Pos );
	st = '0' + ((tr & RTC_TR_ST_Msk ) >> RTC_TR_ST_Pos );
	su = '0' + ((tr & RTC_TR_SU_Msk ) >> RTC_TR_SU_Pos );
	snprintf(time, 9, "%c%c:%c%c:%c%c", ht, hu, mnt, mnu, st, su);
}
// Generate date string from RTC, formatted YYYY-MM-DD
// Refer to MCU RM 41.6.2
void RTC_GetDate (char *date) {
	uint32_t dr;
	int ym, yc, yt, yu, mt, mu, dt, du;
	dr = RTC->DR;
	ym = '2';
	yc = '0';
	yt = '0' + ((dr & RTC_DR_YT_Msk) >> RTC_DR_YT_Pos);
	yu = '0' + ((dr & RTC_DR_YU_Msk) >> RTC_DR_YU_Pos);
	mt = '0' + ((dr & RTC_DR_MT_Msk) >> RTC_DR_MT_Pos);
	mu = '0' + ((dr & RTC_DR_MU_Msk) >> RTC_DR_MU_Pos);
	dt = '0' + ((dr & RTC_DR_DT_Msk) >> RTC_DR_DT_Pos);
	du = '0' + ((dr & RTC_DR_DU_Msk) >> RTC_DR_DU_Pos);
	snprintf(date, 11, "%c%c%c%c-%c%c-%c%c",ym,yc,yt,yu,mt,mu,dt,du);
}
// --------------------------------------------------------
// Set RTC time/date
// --------------------------------------------------------
// Set RTC time from string, formatted HH:MM:SS
void RTC_SetTime (const char *time) {
	uint32_t tr;
	// Enter initialization mode
	// Refer to MCU RM 41.6.4 and 41.3.11, 3rd subsection, items 1-2
	RTC->ICSR |= RTC_ICSR_INIT; //set init bit
	while (!((RTC->ICSR & RTC_ICSR_INITF))) {} //wait for initf to be 1

	tr = 0;
	tr |= (time[2] - '0') << RTC_TR_HT_Pos & RTC_TR_HT_Msk; // Hour
	tr |= (time[3] - '0') << RTC_TR_HU_Pos & RTC_TR_HU_Msk;
	tr |= (time[4] - '0') << RTC_TR_MNT_Pos & RTC_TR_MNT_Msk;
	tr |= (time[5] - '0') << RTC_TR_MNU_Pos & RTC_TR_MNU_Msk;
	tr |= (time[6] - '0') << RTC_TR_ST_Pos & RTC_TR_ST_Msk;
	tr |= (time[7] - '0') << RTC_TR_SU_Pos & RTC_TR_SU_Msk;
	RTC->TR = tr;
	RTC->CR &= ~RTC_CR_FMT; // FMT=0 (24-hour)
	// Return to free-running mode
	// Refer to MCU RM 41.6.4 and 41.3.11, 3rd subsection, item 5
	RTC->ICSR &= !(1 << 6); //clear init bit which writes
}
// Set RTC date from string, formatted YYYY-MM-DD
void RTC_SetDate (const char *date) {
	uint32_t dr;
	// Enter initialization mode
	// Refer to MCU RM 41.6.4 and 41.3.11, 3rd subsection, items 1-2
	RTC->ICSR |= RTC_ICSR_INIT; //set init bit
	while (!((RTC->ICSR & RTC_ICSR_INITF))) {} //wait for initf to be 1
	dr = 0;
	dr |= (date[4] - '0') << RTC_DR_YT_Pos & RTC_DR_YT_Msk;
	dr |= (date[5] - '0') << RTC_DR_YU_Pos & RTC_DR_YU_Msk;
	dr |= (date[6] - '0') << RTC_DR_MT_Pos & RTC_DR_MT_Msk;
	dr |= (date[7] - '0') << RTC_DR_MU_Pos & RTC_DR_MU_Msk;
	dr |= (date[8] - '0') << RTC_DR_DT_Pos & RTC_DR_DT_Msk;
	dr |= (date[9] - '0') << RTC_DR_DU_Pos & RTC_DR_DU_Msk;
	RTC->DR = dr;
	// Return to free-running mode
	// Refer to MCU RM 41.6.4 and 41.3.11, 3rd subsection, item 5
	RTC->ICSR &= !(1 << 6); //clear init bit which writes
}
