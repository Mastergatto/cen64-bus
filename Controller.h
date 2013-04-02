/* ============================================================================
 *  Controller.h: Bus controller.
 *
 *  BusSIM: Reality Co-Processor Bus SIMulator.
 *  Copyright (C) 2013, Tyler J. Stachecki.
 *  All rights reserved.
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE', which is part of this source code package.
 * ========================================================================= */
#ifndef __BUS__CONTROLLER_H__
#define __BUS__CONTROLLER_H__
#include "Address.h"
#include "Common.h"
#include "MemoryMap.h"

struct AIFController;
struct PIFController;
struct RDRRAMController;
struct ROMController;
struct RSP;
struct VIFController;
struct VR4300;

struct BusController {
  struct AIFController *aif;
  struct PIFController *pif;
  struct RDRAMController *rdram;
  struct ROMController *rom;
  struct RSP *rsp;
  struct VIFController *vif;
  struct VR4300 *vr4300;

  struct MemoryMap *memoryMap1;
  struct MemoryMap *memoryMap2;
  struct MemoryMap *memoryMap4;
  struct MemoryMap *memoryMap4U;
  struct MemoryMap *memoryMap8;
};

#endif
