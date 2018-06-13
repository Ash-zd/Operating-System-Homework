#include <linux/init.h>
#include <linux/sched/signal.h>
#include <linux/module.h>
#include <linux/kernel.h>
//#define for_each_process(p) for(p=&init_task;(p=next_task(p)) != &init_task;)

static int __init myprocess_info_init(void) {
    struct task_struct *p = NULL;
    for_each_process(p) {
        printk(KERN_ALERT"name:%s, pid:%d, state:%ld, pri:%d, father_pid:%d\n",
         p->comm, p->pid, p->state, p->prio, p->parent->pid);
    }
    return 0;
}

static void __exit myprocess_info_exit(void) {
    printk("Good bye~\n");
}
module_init(myprocess_info_init);
module_exit(myprocess_info_exit);
MODULE_LICENSE("GPL");