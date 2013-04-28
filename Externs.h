/* ============================================================================
 *  Eterns.h: Eternal definitions for the Bus plugin.
 *
 *  BusSIM: Reality Co-Processor Bus SIMulator.
 *  Copyright (C) 2013, Tyler J. Stachecki.
 *  All rights reserved.
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE', which is part of this source code package.
 * ========================================================================= */
#ifndef __BUS__EXTERNS_H__
#define __BUS__EXTERNS_H__
#include "Controller.h"

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

struct UnalignedData {
  uint32_t data;
  size_t size;
};

int AIRegRead(void *, uint32_t, void *);
int AIRegWrite(void *, uint32_t, void *);

int CartRead(void *, uint32_t, void *);
int CartWrite(void *, uint32_t, void *);

int DPRegRead(void *, uint32_t, void *);
int DPRegWrite(void *, uint32_t, void *);

int MIRegRead(void *, uint32_t, void *);
int MIRegWrite(void *, uint32_t, void *);

int PIFRAMRead(void *, uint32_t, void *);
int PIFRAMWrite(void *, uint32_t, void *);

int PIFROMRead(void *, uint32_t, void *);
int PIFROMWrite(void *, uint32_t, void *);

int PIRegRead(void *, uint32_t, void *);
int PIRegWrite(void *, uint32_t, void *);

int RDRAMReadByte(void *, uint32_t, void *);
int RDRAMWriteByte(void *, uint32_t, void *);
int RDRAMReadDWord(void *, uint32_t, void *);
int RDRAMWriteDWord(void *, uint32_t, void *);
int RDRAMReadHWord(void *, uint32_t, void *);
int RDRAMWriteHWord(void *, uint32_t, void *);
int RDRAMReadWord(void *, uint32_t, void *);
int RDRAMWriteWord(void *, uint32_t, void *);
int RDRAMReadWordUnaligned(void *, uint32_t, void *);
int RDRAMWriteWordUnaligned(void *, uint32_t, void *);

int RDRAMRegRead(void *, uint32_t, void *);
int RDRAMRegWrite(void *, uint32_t, void *);

int RIRegRead(void *, uint32_t, void *);
int RIRegWrite(void *, uint32_t, void *);

int RSPDMemReadWord(void *, uint32_t, void *);
int RSPDMemWriteWord(void *, uint32_t, void *);

int RSPIMemReadByte(void *, uint32_t, void *);
int RSPIMemWriteByte(void *, uint32_t, void *);
int RSPIMemReadWord(void *, uint32_t, void *);
int RSPIMemWriteWord(void *, uint32_t, void *);

int SIRegRead(void *, uint32_t, void *);
int SIRegWrite(void *, uint32_t, void *);

int SPRegRead(void *, uint32_t, void *);
int SPRegWrite(void *, uint32_t, void *);
int SPRegRead2(void *, uint32_t, void *);
int SPRegWrite2(void *, uint32_t, void *);

int VIRegRead(void *, uint32_t, void *);
int VIRegWrite(void *, uint32_t, void *);

void ConnectAIFToBus(struct AIFController *, struct BusController *);
void ConnectPIFToBus(struct PIFController *, struct BusController *);
void ConnectRDRAMToBus(struct RDRAMController *, struct BusController *);
void ConnectRDPToBus(struct RDP *, struct BusController *);
void ConnectROMToBus(struct ROMController *, struct BusController *);
void ConnectRSPToBus(struct RSP *, struct BusController *);
void ConnectVIFToBus(struct VIFController *, struct BusController *);
void ConnectVR4300ToBus(struct VR4300 *, struct BusController *);
void ConnectRDPtoRSP(struct RSP *, struct RDP *);

void CopyFromDRAM(struct RDRAMController *, void *, uint32_t, size_t);
void CopyToDRAM(struct RDRAMController *, uint32_t, const void *, size_t);

const uint8_t *GetRDRAMMemoryPointer(struct RDRAMController *);
void VR4300ClearRCPInterrupt(struct VR4300 *, unsigned);
void VR4300RaiseRCPInterrupt(struct VR4300 *, unsigned);

#endif

