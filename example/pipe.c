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

#include "TDA8425_emu.h"

#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "endian.h"


#if __BYTE_ORDER == __LITTLE_ENDIAN
    #define READ_LE( dst_, size_)  (fread ((dst_), (size_), 1, stdin ) == 1)
    #define WRITE_LE(src_, size_)  (fwrite((src_), (size_), 1, stdout) == 1)
    #define READ_BE( dst_, size_)  (fread ((dst_), 1, (size_), stdin ) == (size_))
    #define WRITE_BE(src_, size_)  (fwrite((src_), 1, (size_), stdout) == (size_))
#elif __BYTE_ORDER == __BIG_ENDIAN
    #define READ_LE( dst_, size_)  (fread ((dst_), 1, (size_), stdin ) == (size_))
    #define WRITE_LE(src_, size_)  (fwrite((src_), 1, (size_), stdout) == (size_))
    #define READ_BE( dst_, size_)  (fread ((dst_), (size_), 1, stdin ) == 1)
    #define WRITE_BE(src_, size_)  (fwrite((src_), (size_), 1, stdout) == 1)
#else
    #error "Unsupported __BYTE_ORDER"
#endif


long const MAX_INPUTS = (long)TDA8425_Source_Count * (long)TDA8425_Stereo_Count;
long const MAX_OUTPUTS = (long)TDA8425_Stereo_Count;


int ReadU8(TDA8425_Float* dst) {
    int8_t src;
    if (READ_LE(&src, sizeof(src))) {
        src += INT8_MIN;
        *dst = (TDA8425_Float)src / -(TDA8425_Float)INT8_MIN;
        return 1;
    }
    return 0;
}

int ReadS8(TDA8425_Float* dst) {
    int8_t src;
    if (READ_LE(&src, sizeof(src))) {
        *dst = (TDA8425_Float)src / -(TDA8425_Float)INT8_MIN;
        return 1;
    }
    return 0;
}

int ReadU16L(TDA8425_Float* dst) {
    int16_t src;
    if (READ_LE(&src, sizeof(src))) {
        src += INT16_MIN;
        *dst = (TDA8425_Float)src / -(TDA8425_Float)INT16_MIN;
        return 1;
    }
    return 0;
}

int ReadU16B(TDA8425_Float* dst) {
    int16_t src;
    if (READ_BE(&src, sizeof(src))) {
        src += INT16_MIN;
        *dst = (TDA8425_Float)src / -(TDA8425_Float)INT16_MIN;
        return 1;
    }
    return 0;
}

int ReadS16L(TDA8425_Float* dst) {
    int16_t src;
    if (READ_LE(&src, sizeof(src))) {
        *dst = (TDA8425_Float)src / -(TDA8425_Float)INT16_MIN;
        return 1;
    }
    return 0;
}

int ReadS16B(TDA8425_Float* dst) {
    int16_t src;
    if (READ_BE(&src, sizeof(src))) {
        *dst = (TDA8425_Float)src / -(TDA8425_Float)INT16_MIN;
        return 1;
    }
    return 0;
}

int ReadU32L(TDA8425_Float* dst) {
    int32_t src;
    if (READ_LE(&src, sizeof(src))) {
        src += INT32_MIN;
        *dst = (TDA8425_Float)src / -(TDA8425_Float)INT32_MIN;
        return 1;
    }
    return 0;
}

int ReadU32B(TDA8425_Float* dst) {
    int32_t src;
    if (READ_BE(&src, sizeof(src))) {
        src += INT32_MIN;
        *dst = (TDA8425_Float)src / -(TDA8425_Float)INT32_MIN;
        return 1;
    }
    return 0;
}

int ReadS32L(TDA8425_Float* dst) {
    int32_t src;
    if (READ_LE(&src, sizeof(src))) {
        *dst = (TDA8425_Float)src / -(TDA8425_Float)INT32_MIN;
        return 1;
    }
    return 0;
}

int ReadS32B(TDA8425_Float* dst) {
    int32_t src;
    if (READ_BE(&src, sizeof(src))) {
        *dst = (TDA8425_Float)src / -(TDA8425_Float)INT32_MIN;
        return 1;
    }
    return 0;
}

int ReadU64L(TDA8425_Float* dst) {
    int64_t src;
    if (READ_LE(&src, sizeof(src))) {
        src += INT64_MIN;
        *dst = (TDA8425_Float)src / -(TDA8425_Float)INT64_MIN;
        return 1;
    }
    return 0;
}

int ReadU64B(TDA8425_Float* dst) {
    int64_t src;
    if (READ_BE(&src, sizeof(src))) {
        src += INT64_MIN;
        *dst = (TDA8425_Float)src / -(TDA8425_Float)INT64_MIN;
        return 1;
    }
    return 0;
}

int ReadS64L(TDA8425_Float* dst) {
    int64_t src;
    if (READ_LE(&src, sizeof(src))) {
        *dst = (TDA8425_Float)src / -(TDA8425_Float)INT64_MIN;
        return 1;
    }
    return 0;
}

int ReadS64B(TDA8425_Float* dst) {
    int64_t src;
    if (READ_BE(&src, sizeof(src))) {
        *dst = (TDA8425_Float)src / -(TDA8425_Float)INT64_MIN;
        return 1;
    }
    return 0;
}

int ReadF32L(TDA8425_Float* dst) {
    float src;
    if (READ_LE(&src, sizeof(src))) {
        *dst = (TDA8425_Float)src;
        return 1;
    }
    return 0;
}

int ReadF32B(TDA8425_Float* dst) {
    float src;
    if (READ_BE(&src, sizeof(src))) {
        *dst = (TDA8425_Float)src;
        return 1;
    }
    return 0;
}

int ReadF64L(TDA8425_Float* dst) {
    double src;
    if (READ_LE(&src, sizeof(src))) {
        *dst = (TDA8425_Float)src;
        return 1;
    }
    return 0;
}

int ReadF64B(TDA8425_Float* dst) {
    double src;
    if (READ_BE(&src, sizeof(src))) {
        *dst = (TDA8425_Float)src;
        return 1;
    }
    return 0;
}


int WriteU8(TDA8425_Float src) {
    int8_t dst = (int8_t)fmin(fmax(src * -(double)INT8_MIN, INT8_MIN), INT8_MAX) - INT8_MIN;
    return WRITE_LE(&dst, sizeof(dst));
}

int WriteS8(TDA8425_Float src) {
    int8_t dst = (int8_t)fmin(fmax(src * -(double)INT8_MIN, INT8_MIN), INT8_MAX);
    return WRITE_LE(&dst, sizeof(dst));
}

int WriteU16L(TDA8425_Float src) {
    int16_t dst = (int16_t)fmin(fmax(src * -(double)INT16_MIN, INT16_MIN), INT16_MAX) - INT16_MIN;
    return WRITE_LE(&dst, sizeof(dst));
}

int WriteU16B(TDA8425_Float src) {
    int16_t dst = (int16_t)fmin(fmax(src * -(double)INT16_MIN, INT16_MIN), INT16_MAX) - INT16_MIN;
    return WRITE_BE(&dst, sizeof(dst));
}

int WriteS16L(TDA8425_Float src) {
    int16_t dst = (int16_t)fmin(fmax(src * -(double)INT16_MIN, INT16_MIN), INT16_MAX);
    return WRITE_LE(&dst, sizeof(dst));
}

int WriteS16B(TDA8425_Float src) {
    int16_t dst = (int16_t)fmin(fmax(src * -(double)INT16_MIN, INT16_MIN), INT16_MAX);
    return WRITE_BE(&dst, sizeof(dst));
}

int WriteU32L(TDA8425_Float src) {
    int32_t dst = (int32_t)fmin(fmax(src * -(double)INT32_MIN, INT32_MIN), INT32_MAX) - INT32_MIN;
    return WRITE_LE(&dst, sizeof(dst));
}

int WriteU32B(TDA8425_Float src) {
    int32_t dst = (int32_t)fmin(fmax(src * -(double)INT32_MIN, INT32_MIN), INT32_MAX) - INT32_MIN;
    return WRITE_BE(&dst, sizeof(dst));
}

int WriteS32L(TDA8425_Float src) {
    int32_t dst = (int32_t)fmin(fmax(src * -(double)INT32_MIN, INT32_MIN), INT32_MAX);
    return WRITE_LE(&dst, sizeof(dst));
}

int WriteS32B(TDA8425_Float src) {
    int32_t dst = (int32_t)fmin(fmax(src * -(double)INT32_MIN, INT32_MIN), INT32_MAX);
    return WRITE_BE(&dst, sizeof(dst));
}

int WriteU64L(TDA8425_Float src) {
    int64_t dst = (int64_t)fmin(fmax(src * -(double)INT64_MIN, (double)INT64_MIN), (double)INT64_MAX) - INT64_MIN;
    return WRITE_LE(&dst, sizeof(dst));
}

int WriteU64B(TDA8425_Float src) {
    int64_t dst = (int64_t)fmin(fmax(src * -(double)INT64_MIN, (double)INT64_MIN), (double)INT64_MAX) - INT64_MIN;
    return WRITE_BE(&dst, sizeof(dst));
}

int WriteS64L(TDA8425_Float src) {
    int64_t dst = (int64_t)fmin(fmax(src * -(double)INT64_MIN, (double)INT64_MIN), (double)INT64_MAX);
    return WRITE_LE(&dst, sizeof(dst));
}

int WriteS64B(TDA8425_Float src) {
    int64_t dst = (int64_t)fmin(fmax(src * -(double)INT64_MIN, (double)INT64_MIN), (double)INT64_MAX);
    return WRITE_BE(&dst, sizeof(dst));
}

int WriteF32L(TDA8425_Float src) {
    float dst = (float)src;
    return WRITE_LE(&dst, sizeof(dst));
}

int WriteF32B(TDA8425_Float src) {
    float dst = (float)src;
    return WRITE_BE(&dst, sizeof(dst));
}

int WriteF64L(TDA8425_Float src) {
    double dst = (double)src;
    return WRITE_LE(&dst, sizeof(dst));
}

int WriteF64B(TDA8425_Float src) {
    double dst = (double)src;
    return WRITE_BE(&dst, sizeof(dst));
}


typedef int (*STREAM_READER)(TDA8425_Float* dst);
typedef int (*STREAM_WRITER)(TDA8425_Float src);

struct FormatTable {
    char const* label;
    STREAM_READER reader;
    STREAM_WRITER writer;
} const FORMAT_TABLE[] =
{
    { "U8",         ReadU8,   WriteU8   },
    { "S8",         ReadS8,   WriteS8   },
    { "U16_LE",     ReadU16L, WriteU16L },
    { "U16_BE",     ReadU16B, WriteU16B },
    { "S16_LE",     ReadS16L, WriteS16L },
    { "S16_BE",     ReadS16B, WriteS16B },
    { "U32_LE",     ReadU32L, WriteU32L },
    { "U32_BE",     ReadU32B, WriteU32B },
    { "S32_LE",     ReadS32L, WriteS32L },
    { "S32_BE",     ReadS32B, WriteS32B },
    { "FLOAT_LE",   ReadF32L, WriteF32L },
    { "FLOAT_BE",   ReadF32B, WriteF32B },
    { "FLOAT64_LE", ReadF64L, WriteF64L },
    { "FLOAT64_BE", ReadF64B, WriteF64B },
    { NULL,         NULL,     NULL      }
};


struct SelectorTable {
    char const* label;
    TDA8425_Selector value;
} const SELECTOR_TABLE[] =
{
    { "A1", TDA8425_Selector_Sound_A_1 },
    { "A2", TDA8425_Selector_Sound_A_2 },
    { "B1", TDA8425_Selector_Sound_B_1 },
    { "B2", TDA8425_Selector_Sound_B_2 },
    { "S1", TDA8425_Selector_Stereo_1  },
    { "S2", TDA8425_Selector_Stereo_2  },
    { NULL, (TDA8425_Selector)0        }
};


struct ModeTable {
    char const* label;
    TDA8425_Mode value;
} const MODE_TABLE[] =
{
    { "mono",    TDA8425_Mode_ForcedMono    },
    { "linear",  TDA8425_Mode_LinearStereo  },
    { "pseudo",  TDA8425_Mode_PseudoStereo  },
    { "spatial", TDA8425_Mode_SpatialStereo },
    { NULL,      (TDA8425_Mode)0            }
};


static char const* USAGE = ("\
TDA8425_emu / pipe (c) 2020, Andrea Zoppi. All rights reserved.\n\
\n\
This program emulates a TDA8425 Hi - fi stereo audio processor, made by\n\
Philips Semiconductors.\n\
It reads a sample stream from standard input, processes data, and writes\n\
to the standard output.\n\
In case of multiple input channels, samples are interleaved. The format is\n\
as specified by the --format option.\n\
The output is always stereo, with the same sample format as per the input.\n\
\n\
\n\
USAGE:\n\
  pipe [OPTION]...\n\
\n\
\n\
OPTION (evaluated as per command line argument order):\n\
\n\
-b, --bass DECIBEL_BASS\n\
    Bass gain [dB]; default: 0.\n\
    Must belong to the possible bass gains, see DECIBEL_BASS.\n\
\n\
-c, --channels COUNT\n\
    Number of input channels; default: 1, max: 32.\n\
\n\
-f, --format FORMAT\n\
    Sample format name; default: U8.\n\
    See FORMAT table.\n\
\n\
-m, --mode MODE\n\
    Stereo mode; default: linear.\n\
    See MODE table.\n\
\n\
-r, --rate RATE\n\
    Sample rate [Hz]; default: 44100.\n\
\n\
-h, --help\n\
    Prints this help message and quits.\n\
\n\
--pseudo-c1 FARAD\n\
    Capacitance of pseudo C1 [F]; default: 15e-9.\n\
\n\
--pseudo-c2 FARAD\n\
    Capacitance of pseudo C2 [F]; default: 15e-9.\n\
\n\
--pseudo-preset PSEUDO_PRESET\n\
    Pseudo-stereo capacitance preset; default 1.\n\
    See PSEUDO_PRESET table.\n\
\n\
--reg_ba [0x]HEX\n\
    Value of BA (bass) register; hexadecimal string.\n\
\n\
--reg_sf [0x]HEX\n\
    Value of SF (switch function) register; hexadecimal string.\n\
\n\
--reg_tr [0x]HEX\n\
    Value of TR (treble) register; hexadecimal string.\n\
\n\
--reg_vl [0x]HEX\n\
    Value of VL (volume left) register; hexadecimal string.\n\
\n\
--reg_vr [0x]HEX\n\
    Value of VR (volume right) register; hexadecimal string.\n\
\n\
-s, --selector SELECTOR\n\
    Input source selector; default: S1.\n\
    See SELECTOR table.\n\
\n\
-t, --treble DECIBEL_TREBLE\n\
    Treble gain [dB]; default: 0.\n\
    Must belong to the possible treble gains, see DECIBEL_TREBLE.\n\
\n\
-v, --volume DECIBEL_VOLUME\n\
    Volume gain [dB]; default: 0.\n\
    Must belong to the possible volume gains, see DECIBEL_VOLUME.\n\
\n\
--volume-left DECIBEL_VOLUME\n\
    Left channel volume gain [dB]; default: 0.\n\
    Must belong to the possible volume gains, see DECIBEL_VOLUME.\n\
\n\
--volume-right DECIBEL_VOLUME\n\
    Right channel volume gain [dB]; default: 0.\n\
    Must belong to the possible volume gains, see DECIBEL_VOLUME.\n\
\n\
\n\
FORMAT:\n\
\n\
| Name       | Bits | Sign | Endian |\n\
|------------|------|------|--------|\n\
| U8         |    8 | no   | same   |\n\
| S8         |    8 | yes  | same   |\n\
| U16_LE     |   16 | no   | little |\n\
| U16_BE     |   16 | no   | big    |\n\
| S16_LE     |   16 | yes  | little |\n\
| S16_BE     |   16 | yes  | big    |\n\
| U32_LE     |   32 | no   | little |\n\
| U32_BE     |   32 | no   | big    |\n\
| S32_LE     |   32 | yes  | little |\n\
| S32_BE     |   32 | yes  | big    |\n\
| FLOAT_LE   |   32 | yes  | little |\n\
| FLOAT_BE   |   32 | yes  | big    |\n\
| FLOAT64_LE |   64 | yes  | little |\n\
| FLOAT64_BE |   64 | yes  | big    |\n\
\n\
\n\
MODE:\n\
\n\
- linear:  linear stereo (default).\n\
- mono:    forces mono (left).\n\
- pseudo:  pseudo stereo.\n\
- spatial: spatial stereo.\n\
\n\
\n\
PSEUDO_PRESET:\n\
\n\
| # | C1 [nF] | C2 [nF] |\n\
|---|---------|---------|\n\
| 1 |      15 |      15 |\n\
| 2 |     5.6 |      47 |\n\
| 3 |     5.6 |      68 |\n\
\n\
\n\
SELECTOR:\n\
\n\
- S1: Source 1, stereo (default).\n\
- A1: Source 1, channel A (left).\n\
- B1: Source 1, channel B (right).\n\
- S2: Source 2, stereo.\n\
- A2: Source 2, channel A (left).\n\
- B2: Source 2, channel B (right).\n\
\n\
\n\
DECIBEL_BASS:\n\
\n\
- minimum: -12\n\
- maximum: +15\n\
- step:      3\n\
\n\
\n\
DECIBEL_TREBLE:\n\
\n\
- minimum: -12\n\
- maximum: +12\n\
- step:      3\n\
\n\
\n\
DECIBEL_VOLUME:\n\
\n\
- minimum: -64\n\
- maximum:  +6\n\
- step:      2\n\
- mute:   -128\n\
\n\
\n\
LICENSE:\n\
\n\
BSD 2-Clause License\n\
\n\
Copyright (c) 2020, Andrea Zoppi\n\
All rights reserved.\n\
\n\
Redistribution and use in source and binary forms, with or without\n\
modification, are permitted provided that the following conditions are met:\n\
\n\
1. Redistributions of source code must retain the above copyright notice, this\n\
   list of conditions and the following disclaimer.\n\
\n\
2. Redistributions in binary form must reproduce the above copyright notice,\n\
   this list of conditions and the following disclaimer in the documentation\n\
   and/or other materials provided with the distribution.\n\
\n\
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"\n\
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n\
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n\
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE\n\
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\n\
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR\n\
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n\
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,\n\
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n\
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n\
");


int main(int argc, char const* argv[])
{
    long channels = 1;
    STREAM_READER stream_reader = ReadU8;
    STREAM_WRITER stream_writer = WriteU8;
    TDA8425_Float rate = 44100;
    TDA8425_Float pseudo_c1 = TDA8425_Pseudo_C1_Table[0];
    TDA8425_Float pseudo_c2 = TDA8425_Pseudo_C2_Table[0];
    TDA8425_Register reg_vl = (TDA8425_Register)TDA8425_Volume_Data_Unity;
    TDA8425_Register reg_vr = (TDA8425_Register)TDA8425_Volume_Data_Unity;
    TDA8425_Register reg_ba = (TDA8425_Register)TDA8425_Tone_Data_Unity;
    TDA8425_Register reg_tr = (TDA8425_Register)TDA8425_Tone_Data_Unity;
    TDA8425_Register reg_sf = (
        (TDA8425_Register)TDA8425_Selector_Stereo_1 |
        ((TDA8425_Register)TDA8425_Mode_LinearStereo << TDA8425_Reg_SF_STL)
    );

    for (int i = 1; i < argc; ++i) {
        // Unary arguments
        if (!strcmp(argv[i], "-h") ||
            !strcmp(argv[i], "--help")) {
            fprintf(stderr, USAGE);
            return 0;
        }

        // Binary arguments
        if (i >= argc - 1) {
            fprintf(stderr, "Expecting binary argument: %s", argv[i]);
            return 1;
        }
        else if (!strcmp(argv[i], "-b") ||
                 !strcmp(argv[i], "--bass")) {
            long db = strtol(argv[++i], NULL, 10);
            int j;
            for (j = 0; j < TDA8425_Tone_Data_Count; ++j) {
                if (TDA8425_BassDecibel_Table[j] == db) {
                    reg_ba = (TDA8425_Register)j;
                    break;
                }
            }
            if (j >= TDA8425_Tone_Data_Count) {
                fprintf(stderr, "Unsupported bass decibel gain: %s", argv[i]);
                return 1;
            }
        }
        else if (!strcmp(argv[i], "-c") ||
                 !strcmp(argv[i], "--channels")) {
            channels = strtol(argv[++i], NULL, 10);
            if (channels < 1) {
                fprintf(stderr, "Invalid channels: %s", argv[i]);
                return 1;
            }
            else if (channels > MAX_INPUTS) {
                channels = MAX_INPUTS;
            }
        }
        else if (!strcmp(argv[i], "-f") ||
                 !strcmp(argv[i], "--format")) {
            char const* label = argv[++i];
            int j;
            for (j = 0; FORMAT_TABLE[j].label; ++j) {
                if (!strcmp(label, FORMAT_TABLE[j].label)) {
                    stream_reader = FORMAT_TABLE[j].reader;
                    stream_writer = FORMAT_TABLE[j].writer;
                    break;
                }
            }
            if (!FORMAT_TABLE[j].label) {
                fprintf(stderr, "Unknown format: %s", label);
                return 1;
            }
        }
        else if (!strcmp(argv[i], "-m") ||
                 !strcmp(argv[i], "--mode")) {
            char const* label = argv[++i];
            int j;
            for (j = 0; MODE_TABLE[j].label; ++j) {
                if (!strcmp(label, MODE_TABLE[j].label)) {
                    reg_sf &= (TDA8425_Register)~(TDA8425_Mode_Mask << TDA8425_Reg_SF_STL);
                    reg_sf |= (TDA8425_Register)MODE_TABLE[j].value << TDA8425_Reg_SF_STL;
                    break;
                }
            }
            if (!MODE_TABLE[j].label) {
                fprintf(stderr, "Unknown mode: %s", label);
                return 1;
            }
        }
        else if (!strcmp(argv[i], "--pseudo-c1")) {
            double value = atof(argv[++i]);
            if (value <= 0) {
                fprintf(stderr, "Invalid capacitance: %s", argv[i]);
                return 1;
            }
            pseudo_c1 = (TDA8425_Float)value;
        }
        else if (!strcmp(argv[i], "--pseudo-c2")) {
            double value = atof(argv[++i]);
            if (value <= 0) {
                fprintf(stderr, "Invalid capacitance: %s", argv[i]);
                return 1;
            }
            pseudo_c2 = (TDA8425_Float)value;
        }
        else if (!strcmp(argv[i], "--pseudo-preset")) {
            long preset = strtol(argv[++i], NULL, 10);
            if (preset < 1 || preset >(long)TDA8425_Pseudo_Preset_Count) {
                fprintf(stderr, "Invalid pseudo selection: %s", argv[i]);
                return 1;
            }
            --preset;
            pseudo_c1 = TDA8425_Pseudo_C1_Table[preset];
            pseudo_c2 = TDA8425_Pseudo_C2_Table[preset];
        }
        else if (!strcmp(argv[i], "-r") ||
                 !strcmp(argv[i], "--rate")) {
            rate = (TDA8425_Float)atof(argv[++i]);
            if (rate < 1) {
                fprintf(stderr, "Invalid rate: %s", argv[i]);
                return 1;
            }
        }
        else if (!strcmp(argv[i], "--reg_ba")) {
            reg_ba = (TDA8425_Register)strtol(argv[++i], NULL, 16);
        }
        else if (!strcmp(argv[i], "--reg_sf")) {
            reg_sf = (TDA8425_Register)strtol(argv[++i], NULL, 16);
        }
        else if (!strcmp(argv[i], "--reg_tr")) {
            reg_tr = (TDA8425_Register)strtol(argv[++i], NULL, 16);
        }
        else if (!strcmp(argv[i], "--reg_vl")) {
            reg_vl = (TDA8425_Register)strtol(argv[++i], NULL, 16);
        }
        else if (!strcmp(argv[i], "--reg_vr")) {
            reg_vr = (TDA8425_Register)strtol(argv[++i], NULL, 16);
        }
        else if (!strcmp(argv[i], "-s") ||
                 !strcmp(argv[i], "--selector")) {
            char const* label = argv[++i];
            int j;
            for (j = 0; SELECTOR_TABLE[j].label; ++j) {
                if (!strcmp(label, SELECTOR_TABLE[j].label)) {
                    reg_sf &= (TDA8425_Register)~TDA8425_Selector_Mask;
                    reg_sf |= (TDA8425_Register)SELECTOR_TABLE[j].value;
                    break;
                }
            }
            if (!SELECTOR_TABLE[j].label) {
                fprintf(stderr, "Unknown selector: %s", label);
                return 1;
            }
        }
        else if (!strcmp(argv[i], "-t") ||
                 !strcmp(argv[i], "--treble")) {
            long db = strtol(argv[++i], NULL, 10);
            int j;
            for (j = 0; j < TDA8425_Tone_Data_Count; ++j) {
                if (TDA8425_TrebleDecibel_Table[j] == db) {
                    reg_tr = (TDA8425_Register)j;
                    break;
                }
            }
            if (j >= TDA8425_Tone_Data_Count) {
                fprintf(stderr, "Unsupported treble decibel gain: %s", argv[i]);
                return 1;
            }
        }
        else if (!strcmp(argv[i], "-v") ||
                 !strcmp(argv[i], "--volume")) {
            long db = strtol(argv[++i], NULL, 10);
            int j;
            for (j = 0; j < TDA8425_Volume_Data_Count; ++j) {
                if (TDA8425_VolumeDecibel_Table[j] == db) {
                    reg_vl = (TDA8425_Register)j;
                    reg_vr = (TDA8425_Register)j;
                    break;
                }
            }
            if (j >= TDA8425_Volume_Data_Count) {
                fprintf(stderr, "Unsupported volume decibel gain: %s", argv[i]);
                return 1;
            }
        }
        else if (!strcmp(argv[i], "--volume-left")) {
            long db = strtol(argv[++i], NULL, 10);
            int j;
            for (j = 0; j < TDA8425_Volume_Data_Count; ++j) {
                if (TDA8425_VolumeDecibel_Table[j] == db) {
                    reg_vl = (TDA8425_Register)j;
                    break;
                }
            }
            if (j >= TDA8425_Volume_Data_Count) {
                fprintf(stderr, "Unsupported volume decibel gain: %s", argv[i]);
                return 1;
            }
        }
        else if (!strcmp(argv[i], "--volume-right")) {
            long db = strtol(argv[++i], NULL, 10);
            int j;
            for (j = 0; j < TDA8425_Volume_Data_Count; ++j) {
                if (TDA8425_VolumeDecibel_Table[j] == db) {
                    reg_vr = (TDA8425_Register)j;
                    break;
                }
            }
            if (j >= TDA8425_Volume_Data_Count) {
                fprintf(stderr, "Unsupported volume decibel gain: %s", argv[i]);
                return 1;
            }
        }
        else {
            fprintf(stderr, "Unknown switch: %s", argv[i]);
            return 1;
        }

        if (errno) {
            fprintf(stderr, "arg %d", i);
            perror("");
            return 1;
        }
    }

    _setmode(_fileno(stdin), O_BINARY);
    if (errno) {
        perror("_setmode(stdin)");
        return 1;
    }

    _setmode(_fileno(stdout), O_BINARY);
    if (errno) {
        perror("_setmode(stdout)");
        return 1;
    }

    TDA8425_ChipFloat chip;
    TDA8425_ChipFloat_Ctor(&chip);
    TDA8425_ChipFloat_Setup(&chip, rate, pseudo_c1, pseudo_c2);
    TDA8425_ChipFloat_Reset(&chip);
    TDA8425_ChipFloat_Write(&chip, TDA8425_Reg_VL, reg_vl);
    TDA8425_ChipFloat_Write(&chip, TDA8425_Reg_VR, reg_vr);
    TDA8425_ChipFloat_Write(&chip, TDA8425_Reg_BA, reg_ba);
    TDA8425_ChipFloat_Write(&chip, TDA8425_Reg_TR, reg_tr);
    TDA8425_ChipFloat_Write(&chip, TDA8425_Reg_SF, reg_sf);
    TDA8425_ChipFloat_Start(&chip);

    while (!feof(stdin)) {
        TDA8425_ChipFloat_Process_Data data;
        long channel;

        TDA8425_Float* inputs = &data.inputs[0][0];  // overflows
        for (channel = 0; channel < channels; ++channel) {
            if (!stream_reader(&inputs[channel])) {
                if (ferror(stdin)) {
                    perror("stream_reader()");
                    goto on_error_;
                }
                else {
                    goto on_eof_;
                }
            }
        }
        for (; channel < MAX_INPUTS; ++channel) {
            inputs[channel] = 0;
        }

        TDA8425_ChipFloat_Process(&chip, &data);

        for (channel = 0; channel < MAX_OUTPUTS; ++channel) {
            if (!stream_writer(data.outputs[channel])) {
                if (ferror(stdout)) {
                    perror("stream_writer()");
                    goto on_error_;
                }
                else {
                    goto on_eof_;
                }
            }
        }
    }

on_eof_:
    TDA8425_ChipFloat_Stop(&chip);
    TDA8425_ChipFloat_Dtor(&chip);
    return 0;

on_error_:
    TDA8425_ChipFloat_Stop(&chip);
    TDA8425_ChipFloat_Dtor(&chip);
    return 1;
}
