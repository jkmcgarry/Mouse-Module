#include <linux/module.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/keyboard.h>
#define PROC_FILE_NAME "mousectl"

struct timer_list my_timer;
struct input_dev *jerry;
struct notifier_block nb;

struct file_operations proc_fops = {
	write: 0
};

int kb_notifier_fn(struct notifier_block *nb, unsigned long action, void* data){
	struct keyboard_notifier_param *kp = (struct keyboard_notifier_param*)data;
	int n = 20;

 	if(kp->value == 106 && kp->down){
		input_report_rel(jerry, REL_X, n);
	}
	else if (kp->value == 108 && kp->down){
		input_report_rel(jerry, REL_Y, n);
	}
	else if(kp->value == 103 && kp->down){
		input_report_rel(jerry, REL_Y, -n);
	}
	else if(kp->value == 105 && kp->down){
		input_report_rel(jerry, REL_X, -n);
	}
	else if(kp->value == 119 && kp->down){
		input_report_key(jerry, BTN_LEFT, 1);
		input_report_key(jerry, BTN_LEFT, 0);
	}
	else{
		return 0;
	}
	input_sync(jerry);
	return 0;
}

static int __init mm_init(void)
{
	jerry = input_allocate_device();

	jerry->name = "jerry";
	set_bit(EV_REL, jerry->evbit);
	set_bit(REL_X, jerry->relbit);
	set_bit(REL_Y, jerry->relbit);

	set_bit(EV_KEY, jerry->evbit);
	set_bit(BTN_LEFT, jerry->keybit);
	
	input_register_device(jerry);
	nb.notifier_call = kb_notifier_fn;
	register_keyboard_notifier(&nb);
	proc_create(PROC_FILE_NAME,0,NULL,&proc_fops);
	return 0;
}

static void __exit mm_remove(void)
{
	input_unregister_device(jerry);
	unregister_keyboard_notifier(&nb);
	remove_proc_entry(PROC_FILE_NAME,NULL);
}

MODULE_LICENSE("GPL"); 
module_init(mm_init);
module_exit(mm_remove);
