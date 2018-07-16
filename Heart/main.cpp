#include "mbed.h"
#include "rtos.h"
 
DigitalOut pace(LED1);
DigitalOut sense(LED3);
//DigitalOut heart(LED3);
int random = 1;
Thread paceH(osPriorityNormal);
Thread senseH(osPriorityNormal);
Semaphore Mut=1;
Semaphore Heart=0;
 
void pace_thread() {
    Mut.wait();
    while (true) {
        pace = !pace;
        Thread::wait(100*random);
        printf("\r\nS\r\n");
        Mut.release();
    }
    //Heart.release();
    
}
void sense_thread() {
    //Heart.wait();
    Mut.wait();
    while (true) {
        sense = !sense;
        Thread::wait(100*random);
        printf("\r\nP\r\n");
        Mut.release();
    }
}
 
int main() {
    random = rand()%6;
    senseH.start(sense_thread);
    //Thread::wait(100*random);
    paceH.start(pace_thread);
    //Thread::wait(100*random);
    
}

/*#define heart_ready 1
#define waitRI 2
#define waitVRP 3
int heartMode;
Mutex mode_mutex;
void tHeart(void const*arg)
{
    int mode_copy;
    while(1)
    {
        mode_mutex.lock();
        mode_copy=heartMode;//read
        mode_mutex.unlock();
        heart(mode_copy);//compute
        //no output for inner communication
        }
        }
void tUserControl(void const*arg)
{
    int mode_copy;
    while(1)
    {
        //no input for inner communication
        mode_copy= userControl();//compute
        mode_mutex.lock();
        heartMode= mode_copy;
        mode_mutex.unlock();
        }
        }

int main() {
    
    Thread tHeart(heart);
    Thread tUserControl(userControl);
    }*/