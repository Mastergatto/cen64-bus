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
struct VIFController;

struct RDP;
struct RSP;
struct VR4300;

struct BusController {
  struct AIFController *aif;
  struct PIFController *pif;
  struct RDRAMController *rdram;
  struct ROMController *rom;
  struct RDP *rdp;
  struct RSP *rsp;
  struct VIFController *vif;
  struct VR4300 *vr4300;

  struct MemoryMap *memoryMaps[5];
};

struct BusController *CreateBus(
  struct AIFController *, struct PIFController *,
  struct RDRAMController *, struct ROMController *,
  struct VIFController *, struct RDP *, struct RSP *,
  struct VR4300 *);

#endif

