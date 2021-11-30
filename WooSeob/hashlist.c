#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/hashtable.h>
#include <linux/slab.h>
#include <linux/list.h>

#define MY_HASH_BITS 8

void hash_list(void);

struct hashlist_node{
	u32 key;
	int value;
	struct hlist_node hnode;
	struct list_head lnode;
};

int __init hello_module_init(void)
{
	hash_list();
	printk("Hash List init\n");
	return 0;
}

void __exit hello_module_cleanup(void)
{
	printk("Hash List Exit\n");
}

module_init(hello_module_init);
module_exit(hello_module_cleanup);

struct list_head hl_list_head;
DEFINE_HASHTABLE(hl_hash, MY_HASH_BITS);

struct hashlist_head
{	
	struct hlist_head hash[1 << MY_HASH_BITS];
	struct list_head lhead;
};

void hashlist_init(struct hashlist_head *head)
{
	INIT_LIST_HEAD(&head->lhead);
	hash_init(head->hash);
}

void hashlist_add(struct hashlist_head *head, int key, int data)
{
	struct hashlist_node *new = kmalloc(sizeof(struct hashlist_node), GFP_KERNEL);
	new->value = data;
	new->key = key;
	list_add(&new->lnode, &head->lhead);
	hash_add(head->hash, &new->hnode, key);
}

struct hashlist_node *hashlist_search(struct hashlist_head *head, int key)
{
	struct hashlist_node *cur;
	hash_for_each_possible(head->hash, cur, hnode, key){
		return cur;
	}
	return cur;
}

void hashlist_del(struct hashlist_head *head, int key)
{
	struct hashlist_node *node = hashlist_search(head, key);
	list_del(&node->lnode);
	hash_del(&node->hnode);
	kfree(node);
}

void hash_list(void)
{
	int i;
	struct hashlist_head hh;
	hashlist_init(&hh);
	for(i = 0; i < 10; i++)
	{
		hashlist_add(&hh, i, i * 10);
	}

	for(i = 0; i < 10; i++)
	{
		struct hashlist_node *node;
		node = hashlist_search(&hh, i);
		printk("%d\n", node->value);
	}

	return;
}
