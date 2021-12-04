#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/my_list.h>
#include <linux/sched.h>

DECLARE_LINKED_HASHTABLE(linked_hashtable, 3);
unsigned long long calclock(struct timespec64 *spclock, unsigned long long *total_time);

struct my_node
{
	struct list_head entry;
	int key;
	struct hlist_node node;
};

struct list_node
{
	struct list_head entry;
	int key;
};


void my_insert(struct list_head *my_list, int value){
	struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
	new->key = value;
	my_list_add(&new->entry, my_list);
	my_list_hash_add(linked_hashtable, &new->node, new->key);
}

struct my_node* my_search(int value){
	struct my_node *cur = NULL;
	my_list_search(linked_hashtable, cur, node, value){
		return cur;
	}
	return cur;
}

void my_delete(struct my_node *node){
	my_list_del(&node->entry);
	my_list_hash_del(&node->node);
	kfree(node);
}

void test_list(int test_num)
{
	int i;
	unsigned long long total_time;
	struct timespec64 spclock[2];
	
	struct list_head my_list;
	INIT_LIST_HEAD(&my_list);
	
	printk("Start Test for Original List %d times\n", test_num);
	
	total_time = 0;
	for(i = 0; i < test_num; i++)
	{		
		ktime_get_real_ts64(&spclock[0]);
		
		struct list_node *new = kmalloc(sizeof(struct list_node), GFP_KERNEL);
		new->key = i;
		list_add(&new->entry, &my_list);
		
		ktime_get_real_ts64(&spclock[1]);
		calclock(spclock, &total_time);
	}
	printk("Insertion %d times : %llu ns\n", test_num, total_time);
	
	total_time = 0;
	for(i = 0; i < test_num; i++)
	{
		struct list_node *current_node = NULL;
		ktime_get_real_ts64(&spclock[0]);
		
		list_for_each_entry(current_node, &my_list, entry)
		{
			if(current_node->key == i)
				break;
		}
		
		ktime_get_real_ts64(&spclock[1]);
		calclock(spclock, &total_time);
	}
	printk("Searching %d times : %llu ns\n", test_num, total_time);
	
	total_time = 0;
	for(i = 0; i < test_num; i++)
	{
		struct list_node *current_node = NULL;
		struct list_node *temp;
		
		ktime_get_real_ts64(&spclock[0]);
		list_for_each_entry_safe(current_node, temp, &my_list, entry)
		{
			if(current_node->key == i)
			{
				list_del(&current_node->entry);
				kfree(current_node);
				break;
			}
		}
		
		ktime_get_real_ts64(&spclock[1]);
		calclock(spclock, &total_time);

	}
	printk("Deletion %d times : %llu n\n", test_num, total_time);
	
	printk("Test Finished\n");
	return;
}

void test_my_list(int test_num)
{
	int i;
	unsigned long long total_time;
	struct timespec64 spclock[2];
	
	struct list_head my_list;
	INIT_MY_LIST_HEAD(&my_list);
	
	printk("Start Test for My Custom List %d times\n", test_num);
	
	total_time = 0;
	for(i = 0; i < test_num; i++)
	{
		ktime_get_real_ts64(&spclock[0]);
		
		my_insert(&my_list, i);
		
		ktime_get_real_ts64(&spclock[1]);
		calclock(spclock, &total_time);
	}
	printk("Insertion %d times : %llu ns\n", test_num, total_time);
	
	total_time = 0;
	for(i = 0; i < test_num; i++)
	{
		ktime_get_real_ts64(&spclock[0]);
		
		if(!my_search(i))
		{
			printk("Failed to search\n");
		}
		else
		{
			ktime_get_real_ts64(&spclock[1]);
			calclock(spclock, &total_time);
		}
	}
	printk("Searching %d times : %llu ns\n", test_num, total_time);
	
	total_time = 0;
	for(i = 0; i < test_num; i++)
	{
		ktime_get_real_ts64(&spclock[0]);
		
		struct my_node *current_node = my_search(i);
		my_delete(current_node);
		
		ktime_get_real_ts64(&spclock[1]);
		calclock(spclock, &total_time);
	}
	printk("Deletion %d times : %llu n\n", test_num, total_time);
	
	printk("Test Finished\n");
	return;
}


static int my_module_init(void)
{
	printk("my module init");
	test_list(100000);
	printk("-----------------\n");
	test_my_list(100000);	
	
	
	return 0;
}



static void my_module_cleanup(void)
{
	printk("\n");
}

unsigned long long calclock(struct timespec64 *spclock, unsigned long long *total_time){
	long long temp, temp_n;
	unsigned long long timedelay = 0;
	if(spclock[1].tv_nsec >= spclock[0].tv_nsec){
		temp = spclock[1].tv_sec - spclock[0].tv_sec;
		temp_n = spclock[1].tv_nsec - spclock[0].tv_nsec;
		timedelay = 1000000000 * temp + temp_n;
	}else{
		temp = spclock[1].tv_sec - spclock[0].tv_sec - 1;
		temp_n = 1000000000 + spclock[1].tv_nsec - spclock[0].tv_nsec;
		timedelay = 1000000000 * temp + temp_n;
	}
	
	__sync_fetch_and_add(total_time, timedelay);

	return timedelay;
}

module_init(my_module_init);
module_exit(my_module_cleanup);
MODULE_LICENSE("GPL");
