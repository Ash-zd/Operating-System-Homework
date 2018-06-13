#include <linux/init.h>
#include <linux/sched/signal.h>
#include <linux/module.h>
#include <linux/kernel.h>
static int pid;
MODULE_LICENSE("GPL");
module_param(pid,int,0644);
static int __init myprocess_init(void) {
	struct task_struct *p = pid_task(find_get_pid(pid), PIDTYPE_PID);
    struct task_struct *parent = NULL;
    struct task_struct *p1 = NULL;

    if (p != NULL) {
        printk(KERN_ALERT"name:%s, pid:%d, state:%ld\n", p->comm, p->pid, p->state);
        parent = p->parent;
        p1 = p->parent;
        printk(KERN_ALERT"father_name:%s, pid:%d, state:%ld\n", parent->comm, parent->pid, parent->state);
        list_for_each_entry(p1, &p->children, sibling) {
            printk(KERN_ALERT"children name:%s, pid:%d, state:%ld\n", p1->comm, p1->pid, p1->state);
        }
        list_for_each_entry(p1, &p->real_parent->children, sibling) {
            printk(KERN_ALERT"sibling name:%s, pid:%d, state:%ld\n", p1->comm, p1->pid, p1->state);
        }
    }
    else {
        printk(KERN_ALERT"WRONG PID!\n");
    }

    return 0;
}
static void __exit myprocess_exit(void) {
    printk("Good bye~\n");
}
module_init(myprocess_init);
module_exit(myprocess_exit);