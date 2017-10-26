
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/bug.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <asm/barrier.h>

MODULE_AUTHOR("Sergii Kolisnyk <kolkmail@gmail.com>");
MODULE_DESCRIPTION("9th homework");
MODULE_LICENSE("Dual BSD/GPL");

static volatile unsigned long long count; // = 0; by insmod
static int f1, f2;
static volatile int a,b,c,d;

//module_param(count, uint, 0000);

static void print_count(void)
{
	pr_emerg("Count %llu\n", count);
}

static int thr(void *data)
{
	volatile unsigned long long i = 1000000;

	*(int *)data = 1;

	pr_emerg("Enter count %llu\n", count);

	do count++; while (--i);

	pr_emerg("Leave count %llu\n", count);
	return 0;
}

static int __init t9_init(void)
{
	struct task_struct *t91, *t92;

	t91 = kthread_create(thr, &f1, "t91");
	t92 = kthread_create(thr, &f2, "t92");

	wake_up_process(t91);
	wake_up_process(t92);

	do schedule(); while (!(f1 && f2));
	//mdelay(1000);

	kthread_stop(t91);
	kthread_stop(t92);

	print_count();
	return -EINVAL;
}

static void m_stub(void)
{
	a = b;
	c = d;
}

static void m_rmb(void)
{
	a = b;
	rmb();
	c = d;
}

static void m_wmb(void)
{
	a = b;
	wmb();
	c = d;
}

static void __exit t9_exit(void)
{
	m_stub();
	m_wmb();
	m_rmb();
}

module_init(t9_init);
module_exit(t9_exit);
