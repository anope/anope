/* Header for Services list handler.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 *
 */
 
#ifndef SLIST_H
#define SLIST_H
 
typedef struct slist_ SList;
typedef struct slistopts_ SListOpts;

struct slist_ {
	void **list;
	
	int16 count;		/* Total entries of the list */
	int16 capacity; 	/* Capacity of the list */
	int16 limit;		/* Maximum possible entries on the list */
	
	SListOpts *opts;
};

struct slistopts_ {
	int32 flags;		/* Flags for the list. See below. */
	
	int  (*compareitem)	(SList *slist, void *item1, void *item2); 	/* Called to compare two items */
	int  (*isequal)     (SList *slist, void *item1, void *item2); 	/* Called by slist_indexof. item1 can be an arbitrary pointer. */
	void (*freeitem) 	(SList *slist, void *item);					/* Called when an item is removed */
};

#define SLIST_DEFAULT_LIMIT 32767

#define SLISTF_NODUP	0x00000001		/* No duplicates in the list. */
#define SLISTF_SORT 	0x00000002		/* Automatically sort the list. Used with compareitem member. */

/* Note that number is the index in the array + 1 */
typedef int (*slist_enumcb_t) (SList *slist, int number, void *item, va_list args);
/* Callback to know whether we can delete the entry. */
typedef int (*slist_delcheckcb_t) (SList *slist, void *item, va_list args);

#endif /* SLIST_H */

