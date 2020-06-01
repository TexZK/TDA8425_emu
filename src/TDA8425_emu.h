/*
BSD 2-Clause License

Copyright (c) 2020, Andrea Zoppi
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _TDA8425_EMU_H_
#define _TDA8425_EMU_H_

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TDA8425_INLINE
#define TDA8425_INLINE static inline
#endif

#ifndef TDA8425_FLOAT
#define TDA8425_FLOAT double            //!< Floating point data type
#endif

// ============================================================================

typedef uint8_t       TDA8425_Address;   //!< Address data type
typedef uint8_t       TDA8425_Register;  //!< Register data type
typedef size_t        TDA8425_Index;     //!< Sample index data type
typedef TDA8425_FLOAT TDA8425_Float;     //!< Floating point data type

//! Registers enumerator
typedef enum TDA8425_Reg {
    TDA8425_Reg_VL = 0,
    TDA8425_Reg_VR = 1,
    TDA8425_Reg_BA = 2,
    TDA8425_Reg_TR = 3,
    TDA8425_Reg_SF = 8
} TDA8425_Reg;

//! Switch functions bit identifiers
typedef enum TDA8425_Reg_SF_Bit {
    TDA8425_Reg_SF_IS    = 0,
    TDA8425_Reg_SF_ML0   = 1,
    TDA8425_Reg_SF_ML1   = 2,
    TDA8425_Reg_SF_STL   = 3,
    TDA8425_Reg_SF_EFL   = 4,
    TDA8425_Reg_SF_MU    = 5,

    TDA8425_Reg_SF_Count = 6
} TDA8425_Reg_SF_Bit;

//! Stereo channels
typedef enum TDA8425_Stereo {
    TDA8425_Stereo_L     = 0,
    TDA8425_Stereo_R     = 1,

    TDA8425_Stereo_Count = 2
} TDA8425_Stereo;

//! Source channels
typedef enum TDA8425_Source {
    TDA8425_Source_1     = 0,
    TDA8425_Source_2     = 1,

    TDA8425_Source_Count = 2
} TDA8425_Source;

//! Source selectors as bit concatenation: ML1.ML0.IS
typedef enum TDA8425_Selector {
    TDA8425_Selector_Sound_A_1 = 2,
    TDA8425_Selector_Sound_A_2 = 3,
    TDA8425_Selector_Sound_B_1 = 4,
    TDA8425_Selector_Sound_B_2 = 5,
    TDA8425_Selector_Stereo_1  = 6,
    TDA8425_Selector_Stereo_2  = 7,

    TDA8425_Selector_Mask      = 7
} TDA8425_Selector;

//! Mode selectors as bit concatenation: STL.EFL
typedef enum TDA8425_Mode {
    TDA8425_Mode_ForcedMono    = 0,
    TDA8425_Mode_LinearStereo  = 1,
    TDA8425_Mode_PseudoStereo  = 2,
    TDA8425_Mode_SpatialStereo = 3,

    TDA8425_Mode_Count         = 4,
    TDA8425_Mode_Mask          = TDA8425_Mode_Count - 1
} TDA8425_Mode;

//! Auto-mute mode
typedef enum TDA8425_AutoMute {
    TDA8425_AutoMute_AfterPOR  = 0,
    TDA8425_AutoMute_NotActive = 1,

    TDA8425_AutoMute_Count     = 2
} TDA8425_AutoMute;

//! Pseudo-stereo presets
typedef enum TDA8425_Pseudo_Preset {
    TDA8425_Pseudo_Preset_1     = 0,
    TDA8425_Pseudo_Preset_2     = 1,
    TDA8425_Pseudo_Preset_3     = 2,

    TDA8425_Pseudo_Preset_Count = 3
} TDA8425_Pseudo_Preset;

//! Datasheet specifications
enum TDA8425_DatasheetSpecifications {
    TDA8425_Volume_Data_Bits  = 6,
    TDA8425_Volume_Data_Count = 1 << TDA8425_Volume_Data_Bits,
    TDA8425_Volume_Data_Mask  = TDA8425_Volume_Data_Count - 1,
    TDA8425_Volume_Data_Unity = 60,

    TDA8425_Tone_Data_Bits    = 4,
    TDA8425_Tone_Data_Count   = 1 << TDA8425_Tone_Data_Bits,
    TDA8425_Tone_Data_Mask    = TDA8425_Tone_Data_Count - 1,
    TDA8425_Tone_Data_Unity   = 6,

    TDA8425_Switch_Data_Bits  = TDA8425_Reg_SF_Count,
    TDA8425_Switch_Data_Mask  = (1 << TDA8425_Switch_Data_Bits) - 1,

    TDA8425_Bass_Frequency    =  300,  // [Hz]
    TDA8425_Treble_Frequency  = 4500,  // [Hz]

    TDA8425_Pseudo_R1         = 15000,  // [ohm]
    TDA8425_Pseudo_R2         = 15000,  // [ohm]

    TDA8425_Spatial_Crosstalk = 52,  // [%]
};

// ============================================================================

extern signed char const TDA8425_VolumeDecibel_Table[TDA8425_Volume_Data_Count];
extern signed char const TDA8425_BassDecibel_Table[TDA8425_Tone_Data_Count];
extern signed char const TDA8425_TrebleDecibel_Table[TDA8425_Tone_Data_Count];

extern TDA8425_Float const TDA8425_Pseudo_C1_Table[TDA8425_Pseudo_Preset_Count];
extern TDA8425_Float const TDA8425_Pseudo_C2_Table[TDA8425_Pseudo_Preset_Count];

// ----------------------------------------------------------------------------

TDA8425_Float TDA8425_RegisterToVolume(TDA8425_Register data);
TDA8425_Float TDA8425_RegisterToBass(TDA8425_Register data);
TDA8425_Float TDA8425_RegisterToTreble(TDA8425_Register data);

// ============================================================================

//! Bi-Quad model
typedef struct TDA8425_BiQuadModelFloat
{
    TDA8425_Float b0;
    TDA8425_Float b1;
    TDA8425_Float b2;

    TDA8425_Float a1;
    TDA8425_Float a2;
} TDA8425_BiQuadModelFloat;

//! Bi-Quad state variables
typedef struct TDA8425_BiQuadStateFloat
{
    TDA8425_Float x0;
    TDA8425_Float x1;
    TDA8425_Float x2;

    TDA8425_Float y0;
    TDA8425_Float y1;
    TDA8425_Float y2;
} TDA8425_BiQuadStateFloat;

// ----------------------------------------------------------------------------

void TDA8425_BiQuadModel_SetupPseudo(
    TDA8425_BiQuadModelFloat* model,
    TDA8425_Float sample_rate,
    TDA8425_Float pseudo_c1,
    TDA8425_Float pseudo_c2
);

void TDA8425_BiQuadModel_SetupBass(
    TDA8425_BiQuadModelFloat* model,
    TDA8425_Float sample_rate,
    TDA8425_Float bass_gain
);

void TDA8425_BiQuadModel_SetupTreble(
    TDA8425_BiQuadModelFloat* model,
    TDA8425_Float sample_rate,
    TDA8425_Float treble_gain
);

// ----------------------------------------------------------------------------

void TDA8425_BiQuadState_Clear(
    TDA8425_BiQuadStateFloat* state,
    TDA8425_Float output
);

// ----------------------------------------------------------------------------

TDA8425_Float TDA8425_BiQuad_Process(
    TDA8425_BiQuadModelFloat* model,
    TDA8425_BiQuadStateFloat* state,
    TDA8425_Float input
);

// ============================================================================

void TDA8425_ForcedMono_Process(
    TDA8425_Float stereo[TDA8425_Stereo_Count]
);

// ----------------------------------------------------------------------------

void TDA8425_PseudoStereo_Process(
    TDA8425_Float stereo[TDA8425_Stereo_Count],
    TDA8425_BiQuadModelFloat* model,
    TDA8425_BiQuadStateFloat* state
);

// ----------------------------------------------------------------------------

void TDA8425_SpatialStereo_Process(
    TDA8425_Float stereo[TDA8425_Stereo_Count]
);

// ============================================================================

typedef struct TDA8425_ChipFloat
{
    TDA8425_Register reg_vl_;
    TDA8425_Register reg_vr_;
    TDA8425_Register reg_ba_;
    TDA8425_Register reg_tr_;
    TDA8425_Register reg_sf_;

    TDA8425_Float sample_rate_;
    TDA8425_Selector selector_;
    TDA8425_Mode mode_;
    TDA8425_Float volume_[TDA8425_Stereo_Count];

    TDA8425_BiQuadModelFloat pseudo_model_;
    TDA8425_BiQuadStateFloat pseudo_state_;

    TDA8425_BiQuadModelFloat bass_model_;
    TDA8425_BiQuadStateFloat bass_state_[TDA8425_Stereo_Count];

    TDA8425_BiQuadModelFloat treble_model_;
    TDA8425_BiQuadStateFloat treble_state_[TDA8425_Stereo_Count];

} TDA8425_Chip;

typedef struct TDA8425_Chip_Process_Data
{
    TDA8425_Float inputs[TDA8425_Source_Count][TDA8425_Stereo_Count];
    TDA8425_Float outputs[TDA8425_Stereo_Count];
} TDA8425_Chip_Process_Data;

// ----------------------------------------------------------------------------

void TDA8425_Chip_Ctor(TDA8425_Chip* self);

void TDA8425_Chip_Dtor(TDA8425_Chip* self);

void TDA8425_Chip_Setup(
    TDA8425_Chip* self,
    TDA8425_Float sample_rate,
    TDA8425_Float pseudo_c1,
    TDA8425_Float pseudo_c2
);

void TDA8425_Chip_Reset(TDA8425_Chip* self);

void TDA8425_Chip_Start(TDA8425_Chip* self);

void TDA8425_Chip_Stop(TDA8425_Chip* self);

void TDA8425_Chip_Process(
    TDA8425_Chip* self,
    TDA8425_Chip_Process_Data* data
);

TDA8425_Register TDA8425_Chip_Read(
    TDA8425_Chip const* self,
    TDA8425_Address address
);

void TDA8425_Chip_Write(
    TDA8425_Chip* self,
    TDA8425_Address address,
    TDA8425_Register data
);

// ============================================================================

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // !_TDA8425_EMU_H_
