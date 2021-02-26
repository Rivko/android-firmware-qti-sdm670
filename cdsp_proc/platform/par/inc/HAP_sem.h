#ifndef HAP_SEM_H
#define HAP_SEM_H
int HAP_par_sem_init(void **t);

int HAP_par_sem_close(void **t);

int HAP_par_sem_down(void *s);

int HAP_par_sem_up(void *s); 

#endif  // #ifndef HAP_SEM_H
