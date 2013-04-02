/* ===========================================================================
 *  MemoryMap.h: Memory Mapper.
 *
 *  BusSIM: Reality Co-Processor Bus SIMulator.
 *  Copyright (C) 2013, Tyler J. Stachecki.
 *  All rights reserved.
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE', which is part of this source code package.
 * ========================================================================= */
#ifndef __BUS__MEMORYMAP_H__
#define __BUS__MEMORYMAP_H__
#include "Common.h"
#include <stddef.h>

/* Callback functions to handle reads/writes. */
typedef int (*MemoryFunction)(void *, uint32_t, void *);

enum MemoryMapColor {
  MEMORYMAP_BLACK,
  MEMORYMAP_RED
};

struct MemoryMapping {
  void *instance;

  MemoryFunction onRead;
  MemoryFunction onWrite;

  uint32_t length;
  uint32_t start;
  uint32_t end;
};

struct MemoryMapNode {
  struct MemoryMapNode *left;
  struct MemoryMapNode *parent;
  struct MemoryMapNode *right;

	struct MemoryMapping mapping;
  enum MemoryMapColor color;
};

struct MemoryMap {
  struct MemoryMapNode *mappings;
  struct MemoryMapNode *nil;
  struct MemoryMapNode *root;

  unsigned nextMapIndex;
  unsigned numMappings;
};

struct MemoryMap* CreateMemoryMap(unsigned);
void DestroyMemoryMap(struct MemoryMap *);

void MapAddressRange(struct MemoryMap *, uint32_t,
	uint32_t, void *, MemoryFunction, MemoryFunction);

const struct MemoryMapping* ResolveMappedAddress(
  const struct MemoryMap *, uint32_t);

#endif

