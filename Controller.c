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
#include <cstdlib>
#include <cstring>
#else
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
void BusClearRCPInterrupt(struct BusController *bus, unsigned mask) {
  VR4300ClearRCPInterrupt(bus->vr4300, mask);
}

/* ============================================================================
 *  BusRaiseRCPInterrupt: Sets an RCP interrupt flag.
 * ========================================================================= */
void BusRaiseRCPInterrupt(struct BusController *bus, unsigned mask) {
  VR4300RaiseRCPInterrupt(bus->vr4300, mask);
}

/* ============================================================================
 *  BusGetRDRAMPointer: Hack for video subsystem.
 * ========================================================================= */
const uint8_t *
BusGetRDRAMPointer(struct BusController *bus) {
  return GetRDRAMMemoryPointer(bus->rdram);
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
  DestroyMemoryMap(controller->memoryMap8);
  DestroyMemoryMap(controller->memoryMap4U);
  DestroyMemoryMap(controller->memoryMap4);
  DestroyMemoryMap(controller->memoryMap2);
  DestroyMemoryMap(controller->memoryMap1);
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
  if ((controller->memoryMap1 = CreateMemoryMap(2)) == NULL)
    return 1;

  MapAddressRange(controller->memoryMap1,
    RDRAM_BASE_ADDRESS, RDRAM_ADDRESS_LEN,
    rdram, RDRAMReadByte, RDRAMWriteByte);

  MapAddressRange(controller->memoryMap1,
    RSP_IMEM_BASE_ADDRESS, RSP_IMEM_ADDRESS_LEN,
    rsp, RSPIMemReadByte, RSPIMemWriteByte);

  /* Round up all the halfword-addressable read/write functions. */
  if ((controller->memoryMap2 = CreateMemoryMap(1)) == NULL) {
    DestroyMemoryMap(controller->memoryMap1);
    return 1;
  }

  MapAddressRange(controller->memoryMap2,
    RDRAM_BASE_ADDRESS, RDRAM_ADDRESS_LEN,
    rdram, RDRAMReadHWord, RDRAMWriteHWord);

  /* Round up all the word-addressable read/write functions. */
  if ((controller->memoryMap4 = CreateMemoryMap(16)) == NULL) {
    DestroyMemoryMap(controller->memoryMap1);
    DestroyMemoryMap(controller->memoryMap2);
    return 1;
  }

  MapAddressRange(controller->memoryMap4,
    AI_REGS_BASE_ADDRESS, AI_REGS_ADDRESS_LEN,
    aif, AIRegRead, AIRegWrite);

  MapAddressRange(controller->memoryMap4,
    DP_REGS_BASE_ADDRESS, DP_REGS_ADDRESS_LEN,
    rdp, DPRegRead, DPRegWrite);

  MapAddressRange(controller->memoryMap4,
    MI_REGS_BASE_ADDRESS, MI_REGS_ADDRESS_LEN,
    vr4300, MIRegRead, MIRegWrite);

  MapAddressRange(controller->memoryMap4,
    PIF_RAM_BASE_ADDRESS, PIF_RAM_ADDRESS_LEN,
    pif, PIFRAMRead, PIFRAMWrite);

  MapAddressRange(controller->memoryMap4,
    PIF_ROM_BASE_ADDRESS, PIF_ROM_ADDRESS_LEN,
    pif, PIFROMRead, PIFROMWrite);

  MapAddressRange(controller->memoryMap4,
    PI_REGS_BASE_ADDRESS, PI_REGS_ADDRESS_LEN,
    rom, PIRegRead, PIRegWrite);

  MapAddressRange(controller->memoryMap4,
    RDRAM_BASE_ADDRESS, RDRAM_ADDRESS_LEN,
    rdram, RDRAMReadWord, RDRAMWriteWord);

  MapAddressRange(controller->memoryMap4,
    RDRAM_REGS_BASE_ADDRESS, RDRAM_REGS_ADDRESS_LEN,
    rdram, RDRAMRegRead, RDRAMRegWrite);

  MapAddressRange(controller->memoryMap4,
    RI_REGS_BASE_ADDRESS, RI_REGS_ADDRESS_LEN,
    rdram, RIRegRead, RIRegWrite);

  MapAddressRange(controller->memoryMap4,
    ROM_CART_BASE_ADDRESS, ROM_CART_ADDRESS_LEN,
    rom, CartRead, CartWrite);

  MapAddressRange(controller->memoryMap4,
    SI_REGS_BASE_ADDRESS, SI_REGS_ADDRESS_LEN,
    pif, SIRegRead, SIRegWrite);

  MapAddressRange(controller->memoryMap4,
    SP_REGS_BASE_ADDRESS, SP_REGS_ADDRESS_LEN,
    rsp, SPRegRead, SPRegWrite);

  MapAddressRange(controller->memoryMap4,
    SP_REGS2_BASE_ADDRESS, SP_REGS2_ADDRESS_LEN,
    rsp, SPRegRead2, SPRegWrite2);

  MapAddressRange(controller->memoryMap4,
    RSP_DMEM_BASE_ADDRESS, RSP_DMEM_ADDRESS_LEN,
    rsp, RSPDMemReadWord, RSPDMemWriteWord);
  
  MapAddressRange(controller->memoryMap4,
    RSP_IMEM_BASE_ADDRESS, RSP_IMEM_ADDRESS_LEN,
    rsp, RSPIMemReadWord, RSPIMemWriteWord);

  MapAddressRange(controller->memoryMap4,
    VI_REGS_BASE_ADDRESS, VI_REGS_ADDRESS_LEN,
    vif, VIRegRead, VIRegWrite);

  if ((controller->memoryMap4U = CreateMemoryMap(1)) == NULL) {
    DestroyMemoryMap(controller->memoryMap1);
    DestroyMemoryMap(controller->memoryMap2);
    DestroyMemoryMap(controller->memoryMap4);
    DestroyMemoryMap(controller->memoryMap8);
    return 1;
  }

  MapAddressRange(controller->memoryMap4U,
    RDRAM_BASE_ADDRESS, RDRAM_ADDRESS_LEN,
    rdram, RDRAMReadWordUnaligned, RDRAMWriteWordUnaligned);


  /* Round up all the word-addressable read/write functions. */
  if ((controller->memoryMap8 = CreateMemoryMap(1)) == NULL) {
    DestroyMemoryMap(controller->memoryMap1);
    DestroyMemoryMap(controller->memoryMap2);
    DestroyMemoryMap(controller->memoryMap4);
    DestroyMemoryMap(controller->memoryMap4U);
    DestroyMemoryMap(controller->memoryMap8);
    return 1;
  }

  MapAddressRange(controller->memoryMap8,
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

  debug("[HACK] Copying payload from DRAM.");
  CopyFromDRAM(bus->rdram, dest, source, size);
}

/* ============================================================================
 *  DMAToDRAM : Performs a DMA from a source to RDRAM.
 * ========================================================================= */
void DMAToDRAM(struct BusController *bus,
  uint32_t dest, const void *source, size_t size) {

  debug("[HACK] Copying payload to DRAM.");
  CopyToDRAM(bus->rdram, dest, source, size);
}

/* ============================================================================
 *  BusReadByte: Read a byte from a device using the bus.
 * ========================================================================= */
uint8_t BusReadByte(const struct BusController *bus, uint32_t address) {
  const struct MemoryMapping *mapping;
  uint8_t byte;

  if ((mapping = ResolveMappedAddress(bus->memoryMap1, address)) == NULL) {
    debugarg("Read BYTE to unmapped address [0x%.8x].", address);
    return 0;
  }

  mapping->onRead(mapping->instance, address, &byte);
  return byte;
}

/* ============================================================================
 *  BusReadDWord: Read a doubleword from a device using the bus.
 * ========================================================================= */
uint64_t BusReadDWord(const struct BusController *bus, uint32_t address) {
  const struct MemoryMapping *mapping;
  uint64_t dword;

  if ((mapping = ResolveMappedAddress(bus->memoryMap8, address)) == NULL) {
    debugarg("Read DWORD from unmapped address [0x%.8x].", address);
    return 0;
  }

  mapping->onRead(mapping->instance, address, &dword);
  return dword;
}

/* ============================================================================
 *  BusReadHWord: Read a halfword from a device using the bus.
 * ========================================================================= */
uint32_t BusReadHWord(const struct BusController *bus, uint32_t address) {
  const struct MemoryMapping *mapping;
  uint16_t hword;

  if ((mapping = ResolveMappedAddress(bus->memoryMap2, address)) == NULL) {
    debugarg("Read HWORD from unmapped address [0x%.8x].", address);
    return 0;
  }

  mapping->onRead(mapping->instance, address, &hword);
  return hword;
}

/* ============================================================================
 *  BusReadWord: Read a word from a device using the bus.
 * ========================================================================= */
uint32_t BusReadWord(const struct BusController *bus, uint32_t address) {
  const struct MemoryMapping *mapping;
  uint32_t word;

  if ((mapping = ResolveMappedAddress(bus->memoryMap4, address)) == NULL) {
    debugarg("Read WORD from unmapped address [0x%.8x].", address);
    return 0;
  }

  mapping->onRead(mapping->instance, address, &word);
  return word;
}

/* ============================================================================
 *  BusReadWordUnaligned: Read a word to a device using the bus.
 * ========================================================================= */
uint32_t BusReadWordUnaligned(const struct BusController *bus,
  uint32_t address, size_t size) {
  const struct MemoryMapping *mapping;
  struct UnalignedData data;

  data.size = size;

  if ((mapping = ResolveMappedAddress(bus->memoryMap4U, address)) == NULL) {
    debugarg("Read UNALIGNED WORD to unmapped address [0x%.8x].", address);
    return 0;
  }

  mapping->onRead(mapping->instance, address, &data);
  return data.data;
}

/* ============================================================================
 *  BusWriteByte: Write a byte to a device using the bus.
 * ========================================================================= */
void BusWriteByte(const struct BusController *bus,
  uint32_t address, uint8_t byte) {
  const struct MemoryMapping *mapping;

  if ((mapping = ResolveMappedAddress(bus->memoryMap1, address)) == NULL) {
    debugarg("Write BYTE to unmapped address [0x%.8x].", address);
    return;
  }

  mapping->onWrite(mapping->instance, address, &byte);
}

/* ============================================================================
 *  BusWriteDWord: Write a doubleword to a device using the bus.
 * ========================================================================= */
void BusWriteDWord(const struct BusController *bus,
  uint32_t address, uint64_t dword) {
  const struct MemoryMapping *mapping;

  if ((mapping = ResolveMappedAddress(bus->memoryMap8, address)) == NULL) {
    debugarg("Write DWORD to unmapped address [0x%.8x].", address);
    return;
  }

  mapping->onWrite(mapping->instance, address, &dword);
}

/* ============================================================================
 *  BusWriteHWord: Write a halfword to a device using the bus.
 * ========================================================================= */
void BusWriteHWord(const struct BusController *bus,
  uint32_t address, uint16_t hword) {
  const struct MemoryMapping *mapping;

  if ((mapping = ResolveMappedAddress(bus->memoryMap2, address)) == NULL) {
    debugarg("Write HWORD to unmapped address [0x%.8x].", address);
    return;
  }

  mapping->onWrite(mapping->instance, address, &hword);
}

/* ============================================================================
 *  BusWriteWord: Write a word to a device using the bus.
 * ========================================================================= */
void BusWriteWord(const struct BusController *bus,
  uint32_t address, uint32_t word) {
  const struct MemoryMapping *mapping;

  if ((mapping = ResolveMappedAddress(bus->memoryMap4, address)) == NULL) {
    debugarg("Write WORD to unmapped address [0x%.8x].", address);
    return;
  }

  mapping->onWrite(mapping->instance, address, &word);
}

/* ============================================================================
 *  BusWriteWordUnaligned: Write a word to a device using the bus.
 * ========================================================================= */
void BusWriteWordUnaligned(const struct BusController *bus,
  uint32_t address, uint32_t word, size_t size) {
  const struct MemoryMapping *mapping;
  struct UnalignedData data;

  data.data = word;
  data.size = size;

  if ((mapping = ResolveMappedAddress(bus->memoryMap4U, address)) == NULL) {
    debugarg("Write UNALIGNED WORD to unmapped address [0x%.8x].", address);
    return;
  }

  mapping->onWrite(mapping->instance, address, &data);
}

