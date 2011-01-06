/* Services list handler implementation.
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

#include "services.h"
#include "slist.h"

static SListOpts slist_defopts = { 0, NULL, NULL, NULL };

/*************************************************************************/

/**
 * Adds a pointer to the list. Returns the index of the new item.
 * Returns -2 if there are too many items in the list, -3 if the
 * item already exists when the flags of the list contain SLISTF_NODUP.
 * @param slist Slist Struct
 * @param item Item being added
 * @return int
 */
int slist_add(SList * slist, void *item)
{
    if (slist->limit != 0 && slist->count >= slist->limit)
        return -2;
    if (slist->opts && (slist->opts->flags & SLISTF_NODUP)
        && slist_indexof(slist, item) != -1)
        return -3;
    if (slist->capacity == slist->count)
        slist_setcapacity(slist, slist->capacity + 1);

    if (slist->opts && (slist->opts->flags & SLISTF_SORT)
        && slist->opts->compareitem) {
        int i;

        for (i = 0; i < slist->count; i++) {
            if (slist->opts->compareitem(slist, item, slist->list[i]) <= 0) {
                memmove(&slist->list[i + 1], &slist->list[i],
                        sizeof(void *) * (slist->count - i));
                slist->list[i] = item;
                break;
            }
        }

        if (i == slist->count)
            slist->list[slist->count] = item;
    } else {
        slist->list[slist->count] = item;
    }

    return slist->count++;
}

/*************************************************************************/

/**
 * Clears the list. If free is 1, the freeitem function will be called
 * for each item before clearing. 
 * @param slist Slist Struct
 * @param mustfree What is being freed
 * @return void
 */
void slist_clear(SList * slist, int mustfree)
{
    if (mustfree && slist->opts && slist->opts->freeitem && slist->count) {
        int i;

        for (i = 0; i < slist->count; i++)
            if (slist->list[i])
                slist->opts->freeitem(slist, slist->list[i]);
    }

    if (slist->list) {
        free(slist->list);
        slist->list = NULL;
    }
    slist->capacity = 0;
    slist->count = 0;
}

/*************************************************************************/

/**
 * Deletes an item from the list, by index. Returns 1 if successful, 
 * 0 otherwise.
 * @param slist Slist Struct
 * @param index beign deleted
 * @return int Returns 1 if successful, 0 otherwise.
 */
int slist_delete(SList * slist, int index)
{
    /* Range check */
    if (index >= slist->count)
        return 0;

    if (slist->list[index] && slist->opts && slist->opts->freeitem)
        slist->opts->freeitem(slist, slist->list[index]);

    slist->list[index] = NULL;
    slist->count--;

    if (index < slist->count)
        memmove(&slist->list[index], &slist->list[index + 1],
                sizeof(void *) * (slist->count - index));

    slist_setcapacity(slist, slist->capacity - 1);

    return 1;
}

/*************************************************************************/

/**
 * Deletes a range of entries. Return -1 if the permission was denied,
 * 0 if no records were deleted, or the number of records deleted
 * @param slist Slist Struct
 * @param range Range to delete
 * @param cb Call back function
 * @param ... various args
 * @return int
 */
int slist_delete_range(SList * slist, char *range, slist_delcheckcb_t cb,
                       ...)
{
    int count = 0, i, n1, n2;
    va_list args, preserve;

    va_start(args, cb);

    for (;;) {
        n1 = n2 = strtol(range, (char **) &range, 10);
        range += strcspn(range, "0123456789,-");

        if (*range == '-') {
            range++;
            range += strcspn(range, "0123456789,");
            if (isdigit(*range)) {
                n2 = strtol(range, (char **) &range, 10);
                range += strcspn(range, "0123456789,-");
            }
        }

        for (i = n1; i <= n2 && i > 0 && i <= slist->count; i++) {

            if (!slist->list[i - 1])
                continue;

            /* copy this off the stack for safety's sake --nenolod */
            VA_COPY(preserve, args);

            if (cb && !cb(slist, slist->list[i - 1], preserve)) {
                va_end(preserve);
                return -1;
            }

            /* if it's to be freed, lets free it */
            if (slist->opts && slist->opts->freeitem)
                slist->opts->freeitem(slist, slist->list[i - 1]);
            slist->list[i - 1] = NULL;

            /* and release the copied list */
            va_end(preserve);

            count++;
        }

        range += strcspn(range, ",");
        if (*range)
            range++;
        else
            break;
    }

    /* We only really delete the items from the list after having processed
     * everything because it would change the position of the items in the
     * list otherwise.
     */
    slist_pack(slist);

    va_end(args);
    return count;
}

/*************************************************************************/

/**
 * Enumerates all entries of the list. If range is not NULL, will only
 * enumerate entries that are in the range. Returns the total number
 * of entries enumerated.
 * @param slit Slist struct
 * @param range Range to enum
 * @param cb Call back function
 * @param ... various args
 * @return int
 */
int slist_enum(SList * slist, char *range, slist_enumcb_t cb, ...)
{
    int count = 0, i, res;
    va_list args, preserve;

    va_start(args, cb);

    if (!range) {
        for (i = 0; i < slist->count; i++) {
            if (!slist->list[i]) {
                alog("SList: warning: NULL pointer in the list (?)");
                continue;
            }

            /* copy off stack for safety */
            VA_COPY(preserve, args);

            res = cb(slist, i + 1, slist->list[i], preserve);
            if (res < 0) {
                va_end(preserve);
                break;
            }

            /* and release our copy */
            va_end(preserve);

            count += res;
        }
    } else {
        int n1, n2;

        for (;;) {
            res = 0;
            n1 = n2 = strtol(range, (char **) &range, 10);
            range += strcspn(range, "0123456789,-");
            if (*range == '-') {
                range++;
                range += strcspn(range, "0123456789,");
                if (isdigit(*range)) {
                    n2 = strtol(range, (char **) &range, 10);
                    range += strcspn(range, "0123456789,-");
                }
            }
            for (i = n1; i <= n2 && i > 0 && i <= slist->count; i++) {
                if (!slist->list[i - 1]) {
                    alog("SList: warning: NULL pointer in the list (?)");
                    continue;
                }

                /* copy off stack for safety */
                VA_COPY(preserve, args);

                res = cb(slist, i, slist->list[i - 1], preserve);
                if (res < 0) {
                    va_end(preserve);
                    break;
                }
                count += res;

                /* and release our copy */
                va_end(preserve);
            }
            if (res < -1)
                break;
            range += strcspn(range, ",");
            if (*range)
                range++;
            else
                break;
        }
    }

    va_end(args);

    return count;
}

/*************************************************************************/

/**
 * Determines whether the list is full.
 * @param slist Slist Struct
 * @return int
 */
int slist_full(SList * slist)
{
    if (slist->limit != 0 && slist->count >= slist->limit)
        return 1;
    else
        return 0;
}

/*************************************************************************/

/**
 * Initialization of the list.
 * @param slist Slist Struct
 * @return int
 */
void slist_init(SList * slist)
{
    memset(slist, 0, sizeof(SList));
    slist->limit = SLIST_DEFAULT_LIMIT;
    slist->opts = &slist_defopts;
}

/*************************************************************************/

/**
 * Returns the index of an item in the list, -1 if inexistant.
 * @param slist Slist Struct
 * @param item Item index
 * @return int
 */
int slist_indexof(SList * slist, void *item)
{
    int16 i;
    void *entry;

    if (slist->count == 0)
        return -1;

    for (i = 0, entry = slist->list[0]; i < slist->count;
         i++, entry = slist->list[i]) {
        if ((slist->opts
             && slist->opts->isequal) ? (slist->opts->isequal(slist, item,
                                                              entry))
            : (item == entry))
            return i;
    }

    return -1;
}

/*************************************************************************/

/**
 * Removes all NULL pointers from the list. 
 * @param slist Slist Struct
 * @return void
 */
void slist_pack(SList * slist)
{
    int i;

    for (i = slist->count - 1; i >= 0; i--)
        if (!slist->list[i])
            slist_delete(slist, i);
}

/*************************************************************************/

/**
 * Removes a specific item from the list. Returns the old index of the
 * deleted item, or -1 if the item was not found.
 * @param slist Slist Struct
 * @param item to remove
 * @return int
 */
int slist_remove(SList * slist, void *item)
{
    int index = slist_indexof(slist, item);
    if (index == -1)
        return -1;
    slist_delete(slist, index);
    return index;
}

/*************************************************************************/

/**
 * Sets the maximum capacity of the list
 * @param slist Slist Struct
 * @param capacity How large the list can be
 * @return int
 */
int slist_setcapacity(SList * slist, int16 capacity)
{
    if (slist->capacity == capacity)
        return 1;
    slist->capacity = capacity;
    if (slist->capacity)
        slist->list =
            srealloc(slist->list, sizeof(void *) * slist->capacity);
    else {
        free(slist->list);
        slist->list = NULL;
    }
    if (slist->capacity < slist->count)
        slist->count = slist->capacity;
    return 1;
}
