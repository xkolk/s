#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/slab.h>

MODULE_AUTHOR("Sergii Kolisnyk <kolkmail@gmail.com>");
MODULE_DESCRIPTION("11th homework");
MODULE_LICENSE("Dual BSD/GPL");

typedef struct kmem_cache kmem_cache_t;

struct myst { int a, b, c; } *mp[13];

kmem_cache_t *cp;

void mycr(void *mx)
{
	struct myst *ms = mx;
	static int num = 0;

	ms->a = ++num;
	ms->b = num + 1;
	ms->c = num + 2;
}

static int __init t11_init(void)
{
	int i;

	cp = kmem_cache_create("myst", sizeof(struct myst) * 13, 0, 0, &mycr);
	if (cp)
		for (i = 0; i < 13; i++)
			mp[i] = kmem_cache_alloc(cp, 0);

	return 0;
}

static void __exit t11_exit(void)
{
	int i;

	for (i = 0; i < 13; i++)
		if (mp[i]) {
			pr_emerg("%u: %u, %u, %u\n",
				i, mp[i]->a, mp[i]->b, mp[i]->c);
			if (cp)
				kmem_cache_free(cp, mp[i]);
		}

	kmem_cache_destroy(cp);
}

module_init(t11_init);
module_exit(t11_exit);
