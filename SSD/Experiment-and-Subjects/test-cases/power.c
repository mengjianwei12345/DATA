#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>

int dog = 1,fgSRAMchk=1,fgRunNewCode=1,downHeatNumber=1,tmp=1;
int *pBuffer=&tmp;
struct _HEAT_PARA{
                   int Mode;	     
                   int Close;	     
                   int Open;	     
                   int State;      
                   int TH1;        
                   int TH2;        
                 };      
struct _HEAT_PARA heatPara[4]={{1,1,1,1,1,1}};  
static struct task_struct *taskThread = 0;
void DataSave(void)
{
	*pBuffer = downHeatNumber;    
	//pBuffer++;
	*pBuffer = heatPara[tmp-1].Mode;     
	//pBuffer++;
}
int mainFun(void)
{
	if(fgRunNewCode == 0)
	{

	}
	else
	{
		dog = !dog;

		if(fgSRAMchk == 0)	 
		{

		}
		else
		{
			DataSave();
		}
	}
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

irqreturn_t RsManageA(int irq, void *dev_id, struct pt_regs *regs)
{
	downHeatNumber++;    
	if(downHeatNumber > 4)
	{
		downHeatNumber = 1;
	}
}

irqreturn_t RsManageB(int irq, void *dev_id, struct pt_regs *regs)
{
	downHeatNumber++;    
	if(downHeatNumber > 4)
	{
		downHeatNumber = 1;
	}
}


static int __init power_init(void)
{
	printk(KERN_ALERT"SV:downHeatNumber:%p\n",&downHeatNumber);
	taskThread = kthread_run(stakeTask, NULL, "stakeTask");
	if (IS_ERR(taskThread)) {  
		printk(KERN_INFO "create kthread failed!\n");  
	} 
	request_irq(10,(irq_handler_t)RsManageA,IRQF_SHARED,"RsManageA",(void *)(RsManageA));
	request_irq(11,(irq_handler_t)RsManageB,IRQF_SHARED,"RsManageB",(void *)(RsManageB));
	return 0;
}


static void __exit power_exit(void)
{
	kthread_stop(taskThread);
}

module_init(power_init);
module_exit(power_exit);
MODULE_LICENSE("GPL");
