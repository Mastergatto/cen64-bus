/* ============================================================================
 *  Controller.c: Bus controller.
 *
 *  BUSSIM: Reality Co-Processor BUS SIMulator.
 *  Copyright (C) 2013, Tyler J. Stachecki.
 *  All rights reserved.
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE', which is part of this source code package.
 * ========================================================================= */
#include "Common.h"
#include "Controller.h"
#include "Externs.h"
#include "MemoryMap.h"

#ifdef __cplusplus
#include <cstddef>
#include <cstdlib>
#include <cstring>
#else
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#endif

static int InitBus(
  struct BusController *, struct AIFController *, struct PIFController *,
  struct RDRAMController *, struct ROMController *, struct VIFController *,
  struct RDP *, struct RSP *, struct VR4300 *);

/* ============================================================================
 *  BusClearRCPInterrupt: Clears an RCP interrupt flag.
 * ========================================================================= */
void
BusClearRCPInterrupt(struct BusController *bus, unsigned mask) {
  VR4300ClearRCPInterrupt(bus->vr4300, mask);
}

/* ============================================================================
 *  BusGetRDRAMPointer: Hack for video subsystem.
 * ========================================================================= */
const uint8_t *
BusGetRDRAMPointer(const struct BusController *bus) {
  return GetRDRAMMemoryPointer(bus->rdram);
}

/* ============================================================================
 *  BusRaiseRCPInterrupt: Sets an RCP interrupt flag.
 * ========================================================================= */
void
BusRaiseRCPInterrupt(struct BusController *bus, unsigned mask) {
  VR4300RaiseRCPInterrupt(bus->vr4300, mask);
}

/* ============================================================================
 *  CreateBus: Creates and initializes a Bus instance.
 * ========================================================================= */
struct BusController *
CreateBus(struct AIFController *aif, struct PIFController *pif,
  struct RDRAMController *rdram, struct ROMController *rom,
  struct VIFController *vif, struct RDP *rdp, struct RSP *rsp,
  struct VR4300 *vr4300) {

  struct BusController *controller;
  size_t allocSize = sizeof(*controller);

  if ((controller = (struct BusController*) malloc(allocSize)) == NULL) {
    debug("Failed to allocate memory.");
    return NULL;
  }

  if (InitBus(controller, aif, pif, rdram, rom, vif, rdp, rsp, vr4300)) {
    free(controller);
    return NULL;
  }

  return controller;
}

/* ============================================================================
 *  DestroyBus: Releases any resources allocated for a Bus instance.
 * ========================================================================= */
void
DestroyBus(struct BusController *controller) {
  unsigned i;

  for (i = 0; i < 5; i++)
    DestroyMemoryMap(controller->memoryMaps[i]);

  free(controller);
}

/* ============================================================================
 *  InitBus: Initializes the Bus controller.
 * ========================================================================= */
static int
InitBus(struct BusController *controller, struct AIFController *aif,
  struct PIFController *pif, struct RDRAMController *rdram,
  struct ROMController *rom, struct VIFController *vif,
  struct RDP *rdp, struct RSP *rsp, struct VR4300 *vr4300) {

  debug("Initializing Bus.");
  memset(controller, 0, sizeof(*controller));

  /* Round up all the byte-addressable read/write functions. */
  if ((controller->memoryMaps[0] = CreateMemoryMap(3)) == NULL)
    return 1;

  MapAddressRange(controller->memoryMaps[0],
    PIF_RAM_BASE_ADDRESS, PIF_RAM_ADDRESS_LEN,
    pif, PIFRAMReadByte, PIFRAMWriteByte);

  MapAddressRange(controller->memoryMaps[0],
    RDRAM_BASE_ADDRESS, RDRAM_ADDRESS_LEN,
    rdram, RDRAMReadByte, RDRAMWriteByte);

  MapAddressRange(controller->memoryMaps[0],
    RSP_IMEM_BASE_ADDRESS, RSP_IMEM_ADDRESS_LEN,
    rsp, RSPIMemReadByte, RSPIMemWriteByte);

  /* Round up all the halfword-addressable read/write functions. */
  if ((controller->memoryMaps[1] = CreateMemoryMap(2)) == NULL) {
    DestroyMemoryMap(controller->memoryMaps[0]);
    return 1;
  }

  MapAddressRange(controller->memoryMaps[1],
    PIF_RAM_BASE_ADDRESS, PIF_RAM_ADDRESS_LEN,
    pif, PIFRAMReadHWord, PIFRAMWriteHWord);

  MapAddressRange(controller->memoryMaps[1],
    RDRAM_BASE_ADDRESS, RDRAM_ADDRESS_LEN,
    rdram, RDRAMReadHWord, RDRAMWriteHWord);

  /* Round up all the word-addressable read/write functions. */
  if ((controller->memoryMaps[2] = CreateMemoryMap(16)) == NULL) {
    DestroyMemoryMap(controller->memoryMaps[0]);
    DestroyMemoryMap(controller->memoryMaps[1]);
    return 1;
  }

  MapAddressRange(controller->memoryMaps[2],
    AI_REGS_BASE_ADDRESS, AI_REGS_ADDRESS_LEN,
    aif, AIRegRead, AIRegWrite);

  MapAddressRange(controller->memoryMaps[2],
    DP_REGS_BASE_ADDRESS, DP_REGS_ADDRESS_LEN,
    rdp, DPRegRead, DPRegWrite);

  MapAddressRange(controller->memoryMaps[2],
    MI_REGS_BASE_ADDRESS, MI_REGS_ADDRESS_LEN,
    vr4300, MIRegRead, MIRegWrite);

  MapAddressRange(controller->memoryMaps[2],
    PIF_RAM_BASE_ADDRESS, PIF_RAM_ADDRESS_LEN,
    pif, PIFRAMReadWord, PIFRAMWriteWord);

  MapAddressRange(controller->memoryMaps[2],
    PIF_ROM_BASE_ADDRESS, PIF_ROM_ADDRESS_LEN,
    pif, PIFROMRead, PIFROMWrite);

  MapAddressRange(controller->memoryMaps[2],
    PI_REGS_BASE_ADDRESS, PI_REGS_ADDRESS_LEN,
    rom, PIRegRead, PIRegWrite);

  MapAddressRange(controller->memoryMaps[2],
    RDRAM_BASE_ADDRESS, RDRAM_ADDRESS_LEN,
    rdram, RDRAMReadWord, RDRAMWriteWord);

  MapAddressRange(controller->memoryMaps[2],
    RDRAM_REGS_BASE_ADDRESS, RDRAM_REGS_ADDRESS_LEN,
    rdram, RDRAMRegRead, RDRAMRegWrite);

  MapAddressRange(controller->memoryMaps[2],
    RI_REGS_BASE_ADDRESS, RI_REGS_ADDRESS_LEN,
    rdram, RIRegRead, RIRegWrite);

  MapAddressRange(controller->memoryMaps[2],
    ROM_CART_BASE_ADDRESS, ROM_CART_ADDRESS_LEN,
    rom, CartRead, CartWrite);

  MapAddressRange(controller->memoryMaps[2],
    SI_REGS_BASE_ADDRESS, SI_REGS_ADDRESS_LEN,
    pif, SIRegRead, SIRegWrite);

  MapAddressRange(controller->memoryMaps[2],
    SP_REGS_BASE_ADDRESS, SP_REGS_ADDRESS_LEN,
    rsp, SPRegRead, SPRegWrite);

  MapAddressRange(controller->memoryMaps[2],
    SP_REGS2_BASE_ADDRESS, SP_REGS2_ADDRESS_LEN,
    rsp, SPRegRead2, SPRegWrite2);

  MapAddressRange(controller->memoryMaps[2],
    RSP_DMEM_BASE_ADDRESS, RSP_DMEM_ADDRESS_LEN,
    rsp, RSPDMemReadWord, RSPDMemWriteWord);
  
  MapAddressRange(controller->memoryMaps[2],
    RSP_IMEM_BASE_ADDRESS, RSP_IMEM_ADDRESS_LEN,
    rsp, RSPIMemReadWord, RSPIMemWriteWord);

  MapAddressRange(controller->memoryMaps[2],
    VI_REGS_BASE_ADDRESS, VI_REGS_ADDRESS_LEN,
    vif, VIRegRead, VIRegWrite);

  if ((controller->memoryMaps[3] = CreateMemoryMap(1)) == NULL) {
    DestroyMemoryMap(controller->memoryMaps[0]);
    DestroyMemoryMap(controller->memoryMaps[1]);
    DestroyMemoryMap(controller->memoryMaps[2]);
    DestroyMemoryMap(controller->memoryMaps[4]);
    return 1;
  }

  MapAddressRange(controller->memoryMaps[3],
    RDRAM_BASE_ADDRESS, RDRAM_ADDRESS_LEN,
    rdram, NULL, RDRAMWriteWordUnaligned);

  /* Round up all the word-addressable read/write functions. */
  if ((controller->memoryMaps[4] = CreateMemoryMap(1)) == NULL) {
    DestroyMemoryMap(controller->memoryMaps[0]);
    DestroyMemoryMap(controller->memoryMaps[1]);
    DestroyMemoryMap(controller->memoryMaps[2]);
    DestroyMemoryMap(controller->memoryMaps[3]);
    DestroyMemoryMap(controller->memoryMaps[4]);
    return 1;
  }

  MapAddressRange(controller->memoryMaps[4],
    RDRAM_BASE_ADDRESS, RDRAM_ADDRESS_LEN,
    rdram, RDRAMReadDWord, RDRAMWriteDWord);

  controller->aif = aif;
  controller->pif = pif;
  controller->rdram = rdram;
  controller->rom = rom;
  controller->vif = vif;

  controller->rdp = rdp;
  controller->rsp = rsp;
  controller->vr4300 = vr4300;

  /* Hardware should be initialized now. */
  debug("== Hardware Initialized ==");

  debug("Connecting devices.");
  ConnectAIFToBus(aif, controller);
  ConnectPIFToBus(pif, controller);
  ConnectRDRAMToBus(rdram, controller);
  ConnectROMToBus(rom, controller);
  ConnectVIFToBus(vif, controller);

  ConnectRDPToBus(rdp, controller);
  ConnectRSPToBus(rsp, controller);
  ConnectRDPtoRSP(rsp, rdp);
  ConnectVR4300ToBus(vr4300, controller);

  return 0;
}

/* ============================================================================
 *  DMAFromDRAM : Performs a DMA from RDRAM to a dest.
 * ========================================================================= */
void DMAFromDRAM(struct BusController *bus,
  void *dest, uint32_t source, uint32_t size) {
  CopyFromDRAM(bus->rdram, dest, source, size);
}

/* ============================================================================
 *  DMAToDRAM : Performs a DMA from a source to RDRAM.
 * ========================================================================= */
void DMAToDRAM(struct BusController *bus,
  uint32_t dest, const void *source, size_t size) {
  CopyToDRAM(bus->rdram, dest, source, size);
}

/* ============================================================================
 *  BusRead: Reads a variable amount of data from the bus.
 * ========================================================================= */
MemoryFunction BusRead(const struct BusController *bus,
  unsigned type, uint32_t address, void **opaque) {
  const struct MemoryMap *memoryMap = bus->memoryMaps[type];
  const struct MemoryMapping *mapping;

  if ((mapping = ResolveMappedAddress(memoryMap, address)) == NULL) {
    debugarg("Read from unmapped address [0x%.8X].", address);
    return NULL;
  }

  memcpy(opaque, &mapping->instance, sizeof(mapping->instance));
  return mapping->onRead;
}

/* ============================================================================
 *  BusReadWord: Read a word from a device using the bus.
 * ========================================================================= */
uint32_t BusReadWord(const struct BusController *bus, uint32_t address) {
  const struct MemoryMapping *mapping;
  uint32_t word;

  if ((mapping = ResolveMappedAddress(bus->memoryMaps[2], address)) == NULL) {
    debugarg("Read WORD from unmapped address [0x%.8x].", address);
    return 0;
  }

  mapping->onRead(mapping->instance, address, &word);
  return word;
}

/* ============================================================================
 *  BusWrite: Writes a variable amount of data to the bus.
 * ========================================================================= */
MemoryFunction BusWrite(const struct BusController *bus,
  unsigned type, uint32_t address, void **opaque) {
  const struct MemoryMap *memoryMap = bus->memoryMaps[type];
  const struct MemoryMapping *mapping;

  if ((mapping = ResolveMappedAddress(memoryMap, address)) == NULL) {
    debugarg("Write to unmapped address [0x%.8X].", address);
    return NULL;
  }

  memcpy(opaque, &mapping->instance, sizeof(mapping->instance));
  return mapping->onWrite;
}

/* ============================================================================
 *  BusWriteWord: Write a word to a device using the bus.
 * ========================================================================= */
void BusWriteWord(const struct BusController *bus,
  uint32_t address, uint32_t word) {
  const struct MemoryMapping *mapping;

  if ((mapping = ResolveMappedAddress(bus->memoryMaps[2], address)) == NULL) {
    debugarg("Write WORD to unmapped address [0x%.8x].", address);
    return;
  }

  mapping->onWrite(mapping->instance, address, &word);
}

