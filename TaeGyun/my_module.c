#include <linux/hashtable.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/sched.h>

static struct list_head my_list;

struct my_node{
	struct list_head entry;
	int key;
	struct hlist_node node;
};


DECLARE_HASHTABLE(linked_hashtable, 3);

void insert(int value){
	struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
	new->key = value;
	list_add(&new->entry, &my_list);
	hash_add(linked_hashtable, &new->node, value);
}

struct my_node* search(int value){
	struct my_node *cur;
	hash_for_each_possible(linked_hashtable, cur, node, value){
		return cur;
	}
	return cur;
}

void delete(int value){
	struct my_node *del = search(value);
	list_del(&del->entry);
	hash_del(&del->node);
	kfree(del);
}


static int my_module_init(void)
{
	unsigned i;
	struct my_node *cur;
	
	printk("my module init");
	
	INIT_LIST_HEAD(&my_list);
	hash_init(linked_hashtable);
	
	insert(1);
	insert(2);
	insert(4);
	insert(6);
	insert(3);
	insert(8);
	insert(9);
	insert(8);	
	insert(8);
	
	printk("-----linked hashtable-----\n");
	hash_for_each(linked_hashtable, i, cur, node) {
		printk("key= %d\n", cur->key);
	}
	
	printk("---------------------------\n");
	struct my_node* search_node = search(4);
	printk("search key= %d", search_node->key);
	
	delete(4);
	printk("delete 4 on linked hashtable");
	
	printk("-----linked hashtable-----\n");
	hash_for_each(linked_hashtable, i, cur, node) {
		printk("key= %d\n", cur->key);
	}
	
	printk("---------------------------\n");
	
	return 0;
}

static void my_module_cleanup(void)
{
	printk("\n");
}



module_init(my_module_init);
module_exit(my_module_cleanup);
MODULE_LICENSE("GPL");
