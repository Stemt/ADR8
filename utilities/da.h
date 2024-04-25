
#ifndef DA_H
#define DA_H

#include <assert.h>
#include <stdlib.h>

#ifndef DA_ASSERT
#define DA_ASSERT assert
#endif

#ifndef DA_FREE
#define DA_FREE free
#endif

#ifndef DA_REALLOC
#define DA_REALLOC realloc
#endif

#define DA_DA_INIT_CAP 4

#define DA_def(type)                                                           \
  typedef struct {                                                             \
    type *items;                                                               \
    size_t capacity;                                                           \
    size_t count;                                                              \
  } DA_##type;

#define DA_len(da) ((da)->count)

#define DA_at(da, index)                                                       \
  (DA_ASSERT(index >= 0), DA_ASSERT(index < DA_len(da)), (da)->items[index])

#define DA_set(da, index, value)                                               \
  (DA_ASSERT(index >= 0), DA_ASSERT(index < DA_len(da)),                       \
   (da)->items[index] = (value))

#define DA_grow(da)                                                            \
  (da)->capacity = (da)->capacity == 0 ? DA_DA_INIT_CAP : (da)->capacity * 2;  \
  (da)->items =                                                                \
      DA_REALLOC((da)->items, (da)->capacity * sizeof(*(da)->items));          \
  DA_ASSERT((da)->items != NULL && "Buy more RAM lol")

// Append an item to a dynamic array
#define DA_append(da, item)                                                    \
  do {                                                                         \
    if ((da)->count >= (da)->capacity) {                                       \
      DA_grow((da));                                                           \
    }                                                                          \
                                                                               \
    (da)->items[(da)->count++] = (item);                                       \
  } while (0)

// Remove an item from a dynamic array
#define DA_remove(da, index)                                                   \
  do {                                                                         \
    memcpy((da)->items + index, (da)->items + index + 1,                       \
           sizeof(*(da)->items) * ((da)->count - index));                      \
                                                                               \
    (da)->count--;                                                             \
  } while (0)

#define DA_free(da) DA_FREE((da).items)

// Append several items to a dynamic array
#define DA_append_many(da, new_items, new_items_count)                         \
  do {                                                                         \
    if ((da)->count + new_items_count > (da)->capacity) {                      \
      if ((da)->capacity == 0) {                                               \
        (da)->capacity = DA_DA_INIT_CAP;                                       \
      }                                                                        \
      while ((da)->count + new_items_count > (da)->capacity) {                 \
        (da)->capacity *= 2;                                                   \
      }                                                                        \
      (da)->items =                                                            \
          DA_REALLOC((da)->items, (da)->capacity * sizeof(*(da)->items));      \
      DA_ASSERT((da)->items != NULL && "Buy more RAM lol");                    \
    }                                                                          \
    memcpy((da)->items + (da)->count, new_items,                               \
           new_items_count * sizeof(*(da)->items));                            \
    (da)->count += new_items_count;                                            \
  } while (0)

#define DA_insert(da, item, position)                                          \
  do {                                                                         \
    DA_ASSERT((da)->count > position &&                                        \
              "Cannot insert in position that doesnt exist");                  \
    if ((da)->count + 1 >= (da)->capacity) {                                   \
      DA_grow(da);                                                             \
    }                                                                          \
    memmove((da)->items + position + 1, (da)->items + position,                \
            sizeof(*(da)->items) * ((da)->count - position));                  \
    (da)->items[position] = item;                                              \
    (da)->count++;                                                             \
  } while (0)

#define DA_foreach(array, type, item)                                          \
  for (type(item) = (array)->items; (item) != (array)->items + (array)->count; \
       (item)++)

#endif // DA_H
