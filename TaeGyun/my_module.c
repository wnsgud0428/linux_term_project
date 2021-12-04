#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/my_list.h>
#include <linux/sched.h>

static struct list_head my_list;

DECLARE_LINKED_HASHTABLE(linked_hashtable, 3);

struct my_node{
	struct list_head entry;
	int key;
	struct hlist_node node;
};


void insert(int value){
	struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
	new->key = value;
	my_list_add(&new->entry, &my_list, linked_hashtable, &new->node, value);
}

struct my_node* search(int value){
	struct my_node *cur;
	my_list_search(linked_hashtable, cur, node, value){
		return cur;
	}
	return cur;
}

void delete(int value){
	struct my_node *del = search(value);
	my_list_del(&del->entry, &del->node);
	kfree(del);
}


static int my_module_init(void)
{
	struct my_node *cur;
	
	printk("my module init");
	
	INIT_MY_LIST_HEAD(&my_list);
	
	insert(3);
	insert(4);
	insert(5);
	insert(5);
	insert(5);
	insert(6);
	
	printk("------insert------");
	
	my_list_for_each_entry(cur, &my_list, entry){
		printk("%d",cur->key);
	}

	struct my_node *tmp;
	printk("------search : 5------");
	my_list_search(linked_hashtable, tmp, node, 5){
		printk("%d", tmp->key);
	}
	
	printk("------delete : 5------");
	
	my_list_del(&cur->entry, &cur->node);
	
	my_list_for_each_entry(cur, &my_list, entry){
		printk("%d",cur->key);
	}
	
	
	return 0;
}

static void my_module_cleanup(void)
{
	printk("\n");
}



module_init(my_module_init);
module_exit(my_module_cleanup);
MODULE_LICENSE("GPL");
