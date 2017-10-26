
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/bug.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/rwsem.h>
#include <asm/barrier.h>

MODULE_AUTHOR("Sergii Kolisnyk <kolkmail@gmail.com>");
MODULE_DESCRIPTION("10th homework");
MODULE_LICENSE("Dual BSD/GPL");

static volatile unsigned long long count; // = 0; by insmod
static atomic_t index, logged;
static int mylock;
static struct task_struct *t10[6];

static DECLARE_RWSEM(sem);
//module_param(count, uint, 0000);

static unsigned int rv;

static inline void lock(int *i)
{
	unsigned int c = 0;

	do {
		if ((++c % 8192U) == 0)
			schedule();
	} while (xchg(i, 1));
}

static inline void unlock(int *i)
{
	xchg(i, 0);
}

static int mr16(void)
{
	unsigned int r;

	lock(&mylock);
	rmb();
	r = rv;
	r *= 1103515245U;
	r &= 0x7fffffff;
	r += 12345U;
	r &= 0x7fffffff;
	rv = r;
	wmb();
	unlock(&mylock);

	r >>= 15;
	return r;
}

static int rd(void *data)
{
	int mask = (int) data;

	do {

		down_read(&sem);

		atomic_or(mask, &index);
		msleep((mr16() & 3) + 1);
		atomic_and(~mask, &index);

		up_read(&sem);

		msleep((mr16() & 3) + 1);

	} while (!kthread_should_stop());

	return 0;
}

static int wr(void *data)
{
	int mask = (int) data;

	do {

		down_write(&sem);

		atomic_or(mask, &index);
		msleep((mr16() & 3) + 1);
		atomic_and(~mask, &index);

		up_write(&sem);

		msleep((mr16() & 3) + 1);

	} while (!kthread_should_stop());

	return 0;
}

static int mlog(void *data)
{
	int mask;

	do {
		mask = 1U << atomic_read(&index);
		atomic_or(mask, &logged);
		schedule();
	} while (!kthread_should_stop());

	return 0;
}

static int __init t10_init(void)
{

	t10[0] = kthread_create(rd, (void *) 1, "t101");
	t10[1] = kthread_create(rd, (void *) 2, "t102");
	t10[2] = kthread_create(rd, (void *) 4, "t103");
	t10[3] = kthread_create(rd, (void *) 8, "t104");
	t10[4] = kthread_create(wr, (void *) 16, "t105");
	t10[5] = kthread_create(mlog, (void *) 0, "t106");

	wake_up_process(t10[0]);
	wake_up_process(t10[1]);
	wake_up_process(t10[2]);
	wake_up_process(t10[3]);
	wake_up_process(t10[4]);
	wake_up_process(t10[5]);

	return 0;
}

static void __exit t10_exit(void)
{
	kthread_stop(t10[0]);
	kthread_stop(t10[1]);
	kthread_stop(t10[2]);
	kthread_stop(t10[3]);
	kthread_stop(t10[4]);
	kthread_stop(t10[5]);

	pr_emerg("Logged: %08X\n", atomic_read(&logged));
}

module_init(t10_init);
module_exit(t10_exit);
