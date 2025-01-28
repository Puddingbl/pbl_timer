#include "pbl_timer.h"
#include "pbl_linklist.h"
#include "pbl_mem.h"
#include "pbl_tick.h"

static void pbl_timer_exec(struct pbl_timer *timer);

#if PBL_TIMER_DYNA == 1	
static pbl_ll_t *pbl_timer_ll;
#elif PBL_TIMER_DYNA == 0
static pbl_sll_node_t node_buff[PBL_TIMER_NUMBER+2];
static pbl_sll_t pbl_timer_sll;
#endif	

void pbl_timer_handler(void) {
    struct pbl_timer *timer;

#if PBL_TIMER_DYNA == 1	
    pbl_ll_node_t *node = pbl_ll_get_head(pbl_timer_ll); 
	while (node) {
        timer = (struct pbl_timer *)(node->data);
		pbl_timer_exec(timer);
 		node = pbl_ll_get_next(node);		
	}
#elif PBL_TIMER_DYNA == 0
    pbl_sll_node_t *node = pbl_sll_get_head(&pbl_timer_sll); 
	while (node->cur) {
        node = pbl_sll_get_next(&pbl_timer_sll, node);
        if (node->data == NULL) {
            continue;
        }        
        timer = (struct pbl_timer *)node->data;
        pbl_timer_exec(timer);       
    }
#endif	
}

static void pbl_timer_exec(struct pbl_timer *timer) {
	uint32_t tick_elaps;
	
	tick_elaps = pbl_tick_elaps(timer->start_tick);       
	if (timer->start_flag == 0 || tick_elaps < timer->delay_ms) {
		return;
	}
	// tick_elaps - timer->delay_ms ÑÓÊ±²¹³¥
	timer->start_tick = pbl_tick_get() - (tick_elaps - timer->delay_ms);
	if (timer->cb) {           
		timer->cb(timer);               
	}       
	if (timer->repeats_num != -1) {
		timer->repeats_num--;
		if (timer->repeats_num == 0) {
			timer->start_flag = 0;
		}        
	}  
}

void pbl_timer_init(void) {
#if PBL_TIMER_DYNA == 1	
	pbl_timer_ll = pbl_ll_create();
#elif PBL_TIMER_DYNA == 0
	pbl_sll_init(&pbl_timer_sll, node_buff, PBL_TIMER_NUMBER+2);
#endif	
}

#if PBL_TIMER_DYNA == 1	
struct pbl_timer * pbl_timer_create_dyna(pbl_timer_cb cb, void *usr_data) {	
	struct pbl_timer *timer = pbl_mem_alloc(sizeof(struct pbl_timer));
	
    if (timer == NULL) {
		LOG_E("create err, timer null");
        return NULL;
    }	
    timer->cb = cb;
    timer->usr_data = usr_data;
    timer->start_flag = 0; 	
	
	pbl_ll_ins_head(pbl_timer_ll, timer);
	
	return timer;
}
#elif PBL_TIMER_DYNA == 0	
void pbl_timer_create_static(struct pbl_timer *timer, pbl_timer_cb cb, void *usr_data) {		

    if (timer == NULL) {
		LOG_E("create err, timer null");
        return;
    }
	
    timer->cb = cb;
    timer->usr_data = usr_data;
    timer->start_flag = 0; 	
	
    uint16_t node = pbl_sll_get_head_index(&pbl_timer_sll);
    uint16_t last_node = 0;
	while (node) {
        last_node = node;
		node =  pbl_sll_get_next_index(&pbl_timer_sll, node);
    }
	pbl_sll_insert(&pbl_timer_sll, last_node + 1, timer);
}
#endif

void pbl_timer_delete(struct pbl_timer *timer) {
    if (timer == NULL) {
        return;
    }
    timer->cb = NULL;
    timer->delay_ms = 0;
    timer->repeats_num = 0;
    timer->start_flag = 0;
    timer->start_tick = 0;
    timer->usr_data = NULL;
#if PBL_TIMER_DYNA == 1	
	for(pbl_ll_node_t *i = pbl_ll_get_head(pbl_timer_ll); i != NULL; i = pbl_ll_get_next(i)) {
		if (i->data  == timer) {
			pbl_ll_remove(pbl_timer_ll, i);
			pbl_mem_free(i);
			pbl_mem_free(timer);
			break;
		}
	}
#elif PBL_TIMER_DYNA == 0	 
    pbl_sll_node_t *node = pbl_sll_get_head(&pbl_timer_sll); 
	while (node->cur) {
        node = pbl_sll_get_next(&pbl_timer_sll, node);
        if (node->data == NULL) {
            continue;
        }  
		if (timer == node->data) {
			pbl_sll_delete_node(&pbl_timer_sll, node);
			break;
		}
	}
#endif         
}

void pbl_timer_start(struct pbl_timer *timer, uint16_t delay_ms, int repeats_num) {
    if (timer == NULL) {
		LOG_E("start err, timer null");
        return;
    }
    
    timer->start_flag = 1;
    timer->delay_ms = delay_ms;
    timer->repeats_num = repeats_num;
    timer->start_tick = pbl_tick_get();
}

void pbl_timer_stop(struct pbl_timer *timer) {
    if (timer == NULL) {
        return;
    }    
    timer->start_flag = 0;
    timer->repeats_num = 0;
}

//void pbl_timer_resume(struct pbl_timer *timer) {
//    if (timer == NULL) {
//        return;
//    }    
//    if (timer->repeats_num != 0) {
//        timer->start_tick = pbl_tick_get();
//        timer->start_flag = 1;        
//    }
//}

//void pbl_timer_pause(struct pbl_timer *timer) {
//    if (timer == NULL) {
//        return;
//    }    
//    timer->start_flag = 0;
//}
