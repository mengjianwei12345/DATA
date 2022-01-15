#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define UCRAdd 0
#define TBRAdd 1
#define DAA 2      
#define DAB 3
#define MAXSTEP 2200                     
#define MAXSTEP7500 150  
int TempA=0,TempB=0,f_go=0xaa,f_4byte=0xaa,f_6byte=0x55,i1=0,jj=0,AorB=0,BrakeTimes=0,state=0,FStop=0x55,FStopTemp=0x55,DaoWei7500Last=0x55,jA=2,jB=3,sA=0,sB=0,CmdLastByte=0,t20ms=0;
int DistanceStepsLast=0,i2=0,DistanceSteps=0,steps=2,steps1=0,steps2=0,steps3=0,StepsTemp=0,counter=0,Dir=0,Dir1=0,Dir2=0,Dir3=0,DirTemp=0;
int cmd[6]={0,0,0,0,0,0};
int XBYTE[10] = {0},IE = 0;

static struct task_struct *taskThread = 0;

void Go(void)
{
    	i2=0;
	if(1)         
	{

		if(Dir==0xaa)                           
		{
			i1++;
			if(i1>=8) {i1=0;}
			i2++;
		        DistanceSteps--;
		        if(DistanceSteps==0xffff) {DistanceSteps=0;}
	        }
	        else 
	        {	                         				
			i1--;
	                if(i1==0xff) {i1=7;}
			i2++;
			DistanceSteps++;			
	        }	            
	}
	f_go=0xaa;
	Dir = 0x11;
	f_4byte=0xaa;
}
int mainFun(void)
{
	char j=0;
	IE=0xae;
	Dir = 0xaa;

	if(f_go==0xaa)
	{
		Go();
	}
	if(f_6byte==0xaa)
	{
		f_6byte=0x55;                         
	}
	if(f_4byte==0xaa)
	{
		f_4byte=0x55;
		XBYTE[TBRAdd]=0x16;
		XBYTE[TBRAdd]=state;
		XBYTE[TBRAdd]=counter;
		XBYTE[TBRAdd]=counter>>8;
	}
	j++;
	if(j>=15)
	{
		j=0;
	}
   	return 0;
}

int stakeTask(void *data)
{

    while(!kthread_should_stop())
    {
        msleep(1000);
        mainFun();
    }
    return 0;
}

irqreturn_t xint1(int irq, void *dev_id, struct pt_regs *regs)
{
	DirTemp=0xaa;
	f_go=0xaa;
	counter++; 
	if(counter==0) {counter=1;}
	FStopTemp=0x55;
	cmd[3]=0x55;                          
	DistanceSteps=0;
	StepsTemp=MAXSTEP-DistanceSteps;
	f_go=0xaa;
	cmd[3]=0xaa;
	return (irqreturn_t) IRQ_HANDLED;
 }

irqreturn_t timer0(int irq, void *dev_id, struct pt_regs *regs)
{
        Dir=0xaa;       			
	if(Dir==0xaa)                           
	{
		i1++;
		if(i1>=8) {i1=0;}
		i2++;
		DistanceSteps--;
		if(DistanceSteps==0xffff) {DistanceSteps=0;}
	}
	else 
	{
		if(Dir==0x55)                     
		{
			i1--;
			if(i1==0xff) {i1=7;}
			i2++;
			DistanceSteps++;
		}

	}
	return (irqreturn_t) IRQ_HANDLED;
}


static int __init linebox_init(void)
{
	f_go=0xaa; 
	XBYTE[TBRAdd]=0x16;
	counter=1020;
	printk(KERN_ALERT"SV:i2:%p i1:%p DistanceSteps:%p counter:%p\n",&i2,&i1,&DistanceSteps,&counter);
	cmd[3]==0xaa;
	taskThread = kthread_run(stakeTask, NULL, "stakeTask");
	if (IS_ERR(taskThread)) {  
		printk(KERN_INFO "create kthread failed!\n");  
	} 
	request_irq(10,(irq_handler_t)xint1,IRQF_SHARED,"xint1",(void *)(xint1));
	request_irq(11,(irq_handler_t)timer0,IRQF_SHARED,"timer0",(void *)(timer0));
	return 0;
}


static void __exit linebox_exit(void)
{
	kthread_stop(taskThread);
}

module_init(linebox_init);
module_exit(linebox_exit);
MODULE_LICENSE("GPL");
