#include <linux/hashtable.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>

DECLARE_HASHTABLE(lht, 3);

struct my_node{
	struct list_head entry;
	int key;
};


static int my_module_init(void)
{
	printk("my module init");
	hash_init(lht);
	
	return 0;
}


static void my_module_cleanup(void)
{
	printk("\n");
}



module_init(my_module_init);
module_exit(my_module_cleanup);
MODULE_DESCRIPTION("Linked Hashtable");
MODULE_LICENSE("GPL");

