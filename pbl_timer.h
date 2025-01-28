/* define to prevent recursive inclusion -------------------------------------*/
#ifndef __CHERRY_TIMER_H
#define __CHERRY_TIMER_H



/* includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "at32f425.h"
#include "staticLinkList.h"
#include "rtdbg.h"

#ifdef __cplusplus
extern "C" {
#endif

/* define---------------------------------------------------------------------*/
#define PBL_TIMER_DYNA  1

#if PBL_TIMER_DYNA == 0	
#define PBL_TIMER_NUMBER    	  6    // 静态定时器最大创建个数
#endif

struct pbl_timer;
typedef void (*pbl_timer_cb)(struct pbl_timer *timer) ;

struct pbl_timer {
    uint32_t start_tick;
    uint16_t delay_ms;
    int repeats_num;
    uint8_t start_flag : 1;    

    pbl_timer_cb cb;
    void *usr_data;
};



/* exported functions ------------------------------------------------------- */
void pbl_timer_init(void) ;
void pbl_timer_create(struct pbl_timer *timer, pbl_timer_cb cb, void *usr_data);

#if PBL_TIMER_DYNA == 1	
struct pbl_timer * pbl_timer_create_dyna(pbl_timer_cb cb, void *usr_data);
#elif PBL_TIMER_DYNA == 0	
void pbl_timer_create_static(struct pbl_timer *timer, pbl_timer_cb cb, void *usr_data);
#endif

void pbl_timer_delete(struct pbl_timer *timer);
void pbl_timer_start(struct pbl_timer *timer, uint16_t delay_ms, int repeats_num);
void pbl_timer_stop(struct pbl_timer *timer);
//void pbl_timer_pause(struct pbl_timer *timer);
//void pbl_timer_resume(struct pbl_timer *timer);
void pbl_timer_handler(void); 
void pbl_timer_cb_hd(void);


/* extern variables-----------------------------------------------------------*/	





#ifdef __cplusplus
}
#endif

#endif

