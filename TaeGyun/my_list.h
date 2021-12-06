/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_MY_LIST_H
#define _LINUX_MY_LIST_H

#include <linux/types.h>
#include <linux/stddef.h>
#include <linux/poison.h>
#include <linux/const.h>
#include <linux/kernel.h>
#include <linux/hashtable.h>
#include <linux/hash.h>
#include <linux/rculist.h>

#define BUILD_BUG_ON_ZERO(e) (1)

 // hashtable
#define DECLARE_LINKED_HASHTABLE(name, bits) \
	struct hlist_head name[1 << (bits)]

 // hashtable 
#define my_list_hash_add(hashtable, node, key) \
	hlist_add_head(node, &hashtable[hash_min(key, HASH_BITS(hashtable))])
	
static inline void INIT_MY_LIST_HEAD(struct list_head *list)
{
	WRITE_ONCE(list->next, list);
	list->prev = list;
}
#ifdef CONFIG_DEBUG_LIST
extern bool __my_list_add_valid(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next);
			      
extern bool __my_list_del_entry_valid(struct list_head *entry);
#else
static inline bool __my_list_add_valid(struct list_head *new,
				struct list_head *prev,
				struct list_head *next)
{
	return true;
}
static inline bool __my_list_del_entry_valid(struct list_head *entry)
{
	return true;
}
#endif
static inline void __my_list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next)
{
	if (!__my_list_add_valid(new, prev, next))
		return;

	next->prev = new;
	new->next = next;
	new->prev = prev;
	WRITE_ONCE(prev->next, new);
}
	
static inline void my_list_add(struct list_head *new, struct list_head *head)
{
	__my_list_add(new, head, head->next);
}

static inline void MY_INIT_HLIST_NODE(struct hlist_node *h)
{
	h->next = NULL;
	h->pprev = NULL;
}


static inline void __my_list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	WRITE_ONCE(prev->next, next);
}

static inline void __my_list_del_entry(struct list_head *entry)
{
	if (!__my_list_del_entry_valid(entry))
		return;

	__my_list_del(entry->prev, entry->next);
}

static inline int my_hlist_unhashed(const struct hlist_node *h)
{
	return !h->pprev;
}
static inline void __my_hlist_del(struct hlist_node *n)
{
	struct hlist_node *next = n->next;
	struct hlist_node **pprev = n->pprev;

	WRITE_ONCE(*pprev, next);
	if (next)
		WRITE_ONCE(next->pprev, pprev);
}


static inline void my_hlist_del_init(struct hlist_node *n)
{
	if (!my_hlist_unhashed(n)) {
		__my_hlist_del(n);
		MY_INIT_HLIST_NODE(n);
	}
}

 // hashtable 
static inline void my_list_hash_del(struct hlist_node *node)
{
	my_hlist_del_init(node);
}


static inline void my_list_del(struct list_head *entry)
{
	__my_list_del_entry(entry);
	entry->next = LIST_POISON1;
	entry->prev = LIST_POISON2;
}

static inline void my_hlist_del(struct hlist_node *n)
{
	__my_hlist_del(n);
	n->next = LIST_POISON1;
	n->pprev = LIST_POISON2;
}



#define MY_INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)

#define my_list_entry(ptr, type, member) \
	container_of(ptr, type, member)

#define my_list_first_entry(ptr, type, member) \
	my_list_entry((ptr)->next, type, member)

#define my_list_next_entry(pos, member) \
	my_list_entry((pos)->member.next, typeof(*(pos)), member)

#define my_list_for_each_entry(pos, head, member)				\
	for (pos = my_list_first_entry(head, typeof(*pos), member);	\
	     !list_entry_is_head(pos, head, member);			\
	     pos = my_list_next_entry(pos, member))
	     
// hashtable 
#define my_list_search(name, obj, member, key) \
	my_hlist_for_each_entry(obj, &name[hash_min(key, HASH_BITS(name))], member)
	
	
#define my_hlist_for_each_entry(pos, head, member)				\
	for (pos = my_hlist_entry_safe((head)->first, typeof(*(pos)), member);\
	     pos;							\
	     pos = my_hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

#define my_hlist_entry_safe(ptr, type, member) \
	({ typeof(ptr) ____ptr = (ptr); \
	   ____ptr ? my_hlist_entry(____ptr, type, member) : NULL; \
	})
#define my_hlist_entry(ptr, type, member) container_of(ptr,type,member)

#endif
