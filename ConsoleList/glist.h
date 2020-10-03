/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/. 
 */

#ifndef __G_LIST_H__
#define __G_LIST_H__

//#include "uglib.h"

#include <memory>

#undef	MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#undef	MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b)




/* memory allocation functions */
#define g_malloc(sz)  malloc(sz)

static inline void * g_malloc0(int sz)
{
	void *p=malloc(sz);
	memset(p,0,sz);
	return p;
}

#define g_alloca(s)    alloca(s)
#define g_new(type,req_count)   (type *)g_malloc(sizeof(type)*(req_count))
#define g_new0(type, req_count)	(type *)g_malloc0(sizeof(type)*(req_count))
#define g_realloc(p,sz) realloc((p),(sz))
#define g_free(p) free(p)

/*misc*/
#define g_return_if_fail(expr) if (!(expr)) {printf("%s:%i- assertion" #expr "failed\n",__FILE__,__LINE__); return;}
#define g_return_val_if_fail(expr,ret) if (!(expr)) {printf("%s:%i- assertion #expr failed\n",__FILE__,__LINE__); return (ret);}
#define g_assert(expr) if (!(expr)) printf("%s:%i- assertion #expr failed\n",__FILE__,__LINE__)
#define g_assert_not_reached() printf("%s:%i- assert not reached failed\n",__FILE__,__LINE__)


typedef struct _GList		GList;

struct _GList
{
  void * data;
  GList *next;
  GList *prev;
};

typedef  int gint;
typedef  unsigned int guint;
typedef  void* gpointer;
typedef int gboolean;
typedef const void *gconstpointer;

#ifndef False
#define False	0
#endif
#ifndef True
#define True	1
#endif



typedef gint            (*GCompareFunc)         (gconstpointer  a,
                                                 gconstpointer  b);
typedef gint            (*GCompareDataFunc)     (gconstpointer  a,
                                                 gconstpointer  b,
						 gpointer       user_data);
typedef void            (*GFunc)                (gpointer       data,
                                                 gpointer       user_data);


#ifdef __cplusplus
extern "C" {
#endif
                                                 
/* Doubly linked lists
 */
/*
  void     g_list_push_allocator (GAllocator       *allocator);
  void     g_list_pop_allocator  (void);
*/
GList*   g_list_alloc          (void);
void     g_list_free           (GList            *list);
void     g_list_free_1         (GList            *list);
GList*   g_list_append         (GList            *list,
				void *          data);
GList*   g_list_prepend        (GList            *list,
				void *          data);
GList*   g_list_insert         (GList            *list,
				void *          data,
				gint              position);
GList*   g_list_insert_sorted  (GList            *list,
				void *          data,
				GCompareFunc      func);
GList*   g_list_insert_before  (GList            *list,
				GList            *sibling,
				void *          data);
GList*   g_list_concat         (GList            *list1,
				GList            *list2);
GList*   g_list_remove         (GList            *list,
				gconstpointer     data);
GList*   g_list_remove_all     (GList            *list,
				gconstpointer     data);
GList*   g_list_remove_link    (GList            *list,
				GList            *llink);
GList*   g_list_delete_link    (GList            *list,
				GList            *link_);
GList*   g_list_reverse        (GList            *list);
GList*   g_list_copy           (GList            *list);
GList*   g_list_nth            (GList            *list,
				guint             n);
GList*   g_list_nth_prev       (GList            *list,
				guint             n);
GList*   g_list_find           (GList            *list,
				gconstpointer     data);
GList*   g_list_find_custom    (GList            *list,
				gconstpointer     data,
				GCompareFunc      func);
gint     g_list_position       (GList            *list,
				GList            *llink);
gint     g_list_index          (GList            *list,
				gconstpointer     data);
GList*   g_list_last           (GList            *list);
GList*   g_list_first          (GList            *list);
guint    g_list_length         (GList            *list);
void     g_list_foreach        (GList            *list,
				GFunc             func,
				void *          user_data);
GList*   g_list_sort           (GList            *list,
				GCompareFunc      compare_func);
GList*   g_list_sort_with_data (GList            *list,
				GCompareDataFunc  compare_func,
				void *          user_data);
void    *g_list_nth_data       (GList            *list,
				guint             n);


#ifdef __cplusplus
}
#endif

#define g_list_previous(list)	((list) ? (((GList *)(list))->prev) : NULL)
#define g_list_next(list)	((list) ? (((GList *)(list))->next) : NULL)


typedef GList GSList;
#define g_slist_prepend g_list_prepend
#define g_slist_free g_list_free
#define g_slist_free_1  g_list_free_1
#define g_slist_reverse g_list_reverse
#define g_slist_next g_list_next
#define g_slist_foreach g_list_foreach
#define g_slist_length g_list_length

/* to be continued...*/

#endif /* __G_LIST_H__ */
