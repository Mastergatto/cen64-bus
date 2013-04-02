/* ============================================================================
 *  MemoryMap.c: Memory Mapper.
 *
 *  BusSIM: Reality Co-Processor Bus SIMulator.
 *  Copyright (C) 2013, Tyler J. Stachecki.
 *  All rights reserved.
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE', which is part of this source code package.
 * ========================================================================= */
#include "MemoryMap.h"

#ifdef __cplusplus
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#else
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#endif

/* Internal functions used to maintain the state of the tree. */
static void MemoryMapFixup(struct MemoryMap *, struct MemoryMapNode *);
static void RotateLeft(struct MemoryMap *, struct MemoryMapNode *);
static void RotateRight(struct MemoryMap *, struct MemoryMapNode *);

/* ============================================================================
 *  CreateMemoryMap: Creates a new MemoryMap.
 * ========================================================================= */
struct MemoryMap*
CreateMemoryMap(unsigned numMaps) {
  struct MemoryMapNode *mappings;
	struct MemoryMap *map;

	size_t allocSize = sizeof(*map) + sizeof(*mappings) * (numMaps + 1);
	if ((map = (struct MemoryMap*) malloc(allocSize)) == NULL)
    return NULL;

	mappings = (struct MemoryMapNode*) (map + 1);

	/* Initialize the allocation. */
	memset(map, 0, allocSize);
	map->mappings = mappings;

  /* Initialize the tree. */
  map->numMappings = numMaps;
	map->nil = &mappings[numMaps];
  map->root = map->nil;

  return map;
}

/* ============================================================================
 *  DestroyMemoryMap: Deallocates memory reserved for a MemoryMap.
 * ========================================================================= */
void
DestroyMemoryMap(struct MemoryMap *memoryMap) {
  free(memoryMap);
}

/* ============================================================================
 *  MemoryMapFixup: Rebalances the tree after `node` is inserted.
 * ========================================================================= */
static void
MemoryMapFixup(struct MemoryMap *map, struct MemoryMapNode *node) {
	struct MemoryMapNode *cur;

  /* Rebalance the whole tree as needed. */
  while (node->parent->color == MEMORYMAP_RED) {
    if (node->parent == node->parent->parent->left) {
      cur = node->parent->parent->right;

      /* Case 1: We only need to update colors. */
      if (cur->color == MEMORYMAP_RED) {
        node->parent->color = MEMORYMAP_BLACK;
        cur->color = MEMORYMAP_BLACK;
        node->parent->parent->color = MEMORYMAP_RED;
        node = node->parent->parent;
      }

      else {

        /* Case 2: We need to perform a left rotation. */
        if (node == node->parent->right) {
          node = node->parent;
          RotateLeft(map, node);
        }

        /* Case 3: We need to perform a right rotation. */
        node->parent->color = MEMORYMAP_BLACK;
        node->parent->parent->color = MEMORYMAP_RED;
        RotateRight(map, node->parent->parent);
      }
    }

    else {
      cur = node->parent->parent->left;

      /* Case 1: We only need to update colors. */
      if (cur->color == MEMORYMAP_RED) {
        node->parent->color = MEMORYMAP_BLACK;
        cur->color = MEMORYMAP_BLACK;
        node->parent->parent->color = MEMORYMAP_RED;
        node = node->parent->parent;
      }

      else {

        /* Case 2: We need to perform a right rotation. */
        if (node == node->parent->left) {
          node = node->parent;
          RotateRight(map, node);
        }

        /* Case 3: We need to perform a left rotation. */
        node->parent->color = MEMORYMAP_BLACK;
        node->parent->parent->color = MEMORYMAP_RED;
        RotateLeft(map, node->parent->parent);
      }
    }
  }

  /* When we rebalanced the tree, we might have accidentally colored */
  /* the root red, so unconditionally color if back after rebalancing. */
  map->root->color = MEMORYMAP_BLACK;
}

/* ============================================================================
 *  MapAddressRange: Inserts a mapping into the tree.
 * ========================================================================= */
void
MapAddressRange(struct MemoryMap *map, uint32_t start, uint32_t length,
  void *instance, MemoryFunction onRead, MemoryFunction onWrite) {
  struct MemoryMapNode *check = map->root;
  struct MemoryMapNode *cur = map->nil;
	uint32_t end = start + length - 1;

  struct MemoryMapNode *newNode;
	struct MemoryMapping mapping;

  /* Make sure we have enough space in the map. */
  assert(map->nextMapIndex < map->numMappings &&
    "Tried to insert into a MemoryMap with no free mappings.");

  newNode =  &map->mappings[map->nextMapIndex++];

  /* Walk down the tree. */
  while (check != map->nil) {
    cur = check;

    check = (start < cur->mapping.start)
      ? check->left : check->right;
  }

	/* Insert the entry. */
  if (cur == map->nil)
    map->root = newNode;

  else if (start < cur->mapping.start)
    cur->left = newNode;
  else
    cur->right = newNode;

  newNode->left = map->nil;
  newNode->right = map->nil;
  newNode->parent = cur;

	/* Initialize the entry. */
	mapping.instance = instance;
	mapping.onRead = onRead;
	mapping.onWrite = onWrite;

	mapping.end = end;
	mapping.length = length;
	mapping.start = start;

	newNode->mapping = mapping;

	/* Rebalance the tree. */
  newNode->color = MEMORYMAP_RED;
	MemoryMapFixup(map, newNode);
}

/* ============================================================================
 *  ResolveMappedAddress: Returns a pointer to mapped memory (or NULL).
 * ========================================================================= */
const struct MemoryMapping*
ResolveMappedAddress(const struct MemoryMap *map, uint32_t address) {
  const struct MemoryMapNode *cur = map->root;

  while (cur != map->nil) {
    if (address < cur->mapping.start)
      cur = cur->left;
    else if (address > cur->mapping.end)
      cur = cur->right;

    else
      return &cur->mapping;
  }

  return NULL;
}

/* ============================================================================
 *  RotateLeft: Perform a left rotation (centered at n).
 * ========================================================================= */
static void
RotateLeft(struct MemoryMap *map, struct MemoryMapNode *n) {
  struct MemoryMapNode *y = n->right;

  /* Turn y's left subtree into n's right subtree. */
  n->right = y->left;

  if (y->left != map->nil)
    y->left->parent = n;

  /* Link n's parent to y. */
  y->parent = n->parent;

  if (n->parent == map->nil)
    map->root = y;
  else if (n == n->parent->left)
    n->parent->left = y;
  else
    n->parent->right = y;

  /* Put n on y's left. */
  y->left = n;
  n->parent = y;
}

/* ============================================================================
 *  RotateRight: Perform a right rotation (centered at n).
 * ========================================================================= */
static void
RotateRight(struct MemoryMap *map, struct MemoryMapNode *n) {
  struct MemoryMapNode *y = n->left;

  /* Turn y's right subtree into n's left subtree. */
  n->left = y->right;

  if (y->right != map->nil)
    y->right->parent = n;

  /* Link n's parent to y. */
  y->parent = n->parent;

  if (n->parent == map->nil)
    map->root = y;
  else if (n == n->parent->left)
    n->parent->left = y;
  else
    n->parent->right = y;

  /* Put n on y's right. */
  y->right = n;
  n->parent = y;
}

