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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "endian.h"

#ifdef __WINDOWS__
#include <io.h>
#endif


static char const* USAGE = ("\
TDA8425_pipe (c) 2020, Andrea Zoppi. All rights reserved.\n\
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
--reg-BA [0x]HEX\n\
    Value of BA (bass) register; hexadecimal string.\n\
\n\
--reg-SF [0x]HEX\n\
    Value of SF (switch function) register; hexadecimal string.\n\
\n\
--reg-TR [0x]HEX\n\
    Value of TR (treble) register; hexadecimal string.\n\
\n\
--reg-VL [0x]HEX\n\
    Value of VL (volume left) register; hexadecimal string.\n\
\n\
--reg-VR [0x]HEX\n\
    Value of VR (volume right) register; hexadecimal string.\n\
\n\
-s, --selector SELECTOR\n\
    Input source selector; default: S1.\n\
    See SELECTOR table.\n\
\n\
--t-filter\n\
    Enables T-filter.\n\
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
- mono:    forced mono (left).\n\
- pseudo:  pseudo stereo (left).\n\
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
    double scaled = src * -(double)INT8_MIN;
    int8_t dst = (int8_t)fmin(fmax(scaled, INT8_MIN), INT8_MAX);
    dst -= INT8_MIN;
    return WRITE_LE(&dst, sizeof(dst));
}

int WriteS8(TDA8425_Float src) {
    double scaled = src * -(double)INT8_MIN;
    int8_t dst = (int8_t)fmin(fmax(scaled, INT8_MIN), INT8_MAX);
    return WRITE_LE(&dst, sizeof(dst));
}

int WriteU16L(TDA8425_Float src) {
    double scaled = src * -(double)INT16_MIN;
    int16_t dst = (int16_t)fmin(fmax(scaled, INT16_MIN), INT16_MAX);
    dst -= INT16_MIN;
    return WRITE_LE(&dst, sizeof(dst));
}

int WriteU16B(TDA8425_Float src) {
    double scaled = src * -(double)INT16_MIN;
    int16_t dst = (int16_t)fmin(fmax(scaled, INT16_MIN), INT16_MAX);
    dst -= INT16_MIN;
    return WRITE_BE(&dst, sizeof(dst));
}

int WriteS16L(TDA8425_Float src) {
    double scaled = src * -(double)INT16_MIN;
    int16_t dst = (int16_t)fmin(fmax(scaled, INT16_MIN), INT16_MAX);
    return WRITE_LE(&dst, sizeof(dst));
}

int WriteS16B(TDA8425_Float src) {
    double scaled = src * -(double)INT16_MIN;
    int16_t dst = (int16_t)fmin(fmax(scaled, INT16_MIN), INT16_MAX);
    return WRITE_BE(&dst, sizeof(dst));
}

int WriteU32L(TDA8425_Float src) {
    double scaled = src * -(double)INT32_MIN;
    int32_t dst = (int32_t)fmin(fmax(scaled, INT32_MIN), INT32_MAX);
    dst -= INT32_MIN;
    return WRITE_LE(&dst, sizeof(dst));
}

int WriteU32B(TDA8425_Float src) {
    double scaled = src * -(double)INT32_MIN;
    int32_t dst = (int32_t)fmin(fmax(scaled, INT32_MIN), INT32_MAX);
    dst -= INT32_MIN;
    return WRITE_BE(&dst, sizeof(dst));
}

int WriteS32L(TDA8425_Float src) {
    double scaled = src * -(double)INT32_MIN;
    int32_t dst = (int32_t)fmin(fmax(scaled, INT32_MIN), INT32_MAX);
    return WRITE_LE(&dst, sizeof(dst));
}

int WriteS32B(TDA8425_Float src) {
    double scaled = src * -(double)INT32_MIN;
    int32_t dst = (int32_t)fmin(fmax(scaled, INT32_MIN), INT32_MAX);
    return WRITE_BE(&dst, sizeof(dst));
}

int WriteU64L(TDA8425_Float src) {
    double scaled = src * -(double)INT64_MIN;
    int64_t dst = (int64_t)fmin(fmax(scaled, (double)INT64_MIN), (double)INT64_MAX);
    dst -= INT64_MIN;
    return WRITE_LE(&dst, sizeof(dst));
}

int WriteU64B(TDA8425_Float src) {
    double scaled = src * -(double)INT64_MIN;
    int64_t dst = (int64_t)fmin(fmax(scaled, (double)INT64_MIN), (double)INT64_MAX);
    dst -= INT64_MIN;
    return WRITE_BE(&dst, sizeof(dst));
}

int WriteS64L(TDA8425_Float src) {
    double scaled = src * -(double)INT64_MIN;
    int64_t dst = (int64_t)fmin(fmax(scaled, (double)INT64_MIN), (double)INT64_MAX);
    return WRITE_LE(&dst, sizeof(dst));
}

int WriteS64B(TDA8425_Float src) {
    double scaled = src * -(double)INT64_MIN;
    int64_t dst = (int64_t)fmin(fmax(scaled, (double)INT64_MIN), (double)INT64_MAX);
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


struct RegisterTable {
    char const* label;
    TDA8425_Reg value;
} const REGISTER_TABLE[(int)TDA8425_RegOrder_Count + 1] =
{
    { "VL", TDA8425_Reg_VL },
    { "VR", TDA8425_Reg_VR },
    { "BA", TDA8425_Reg_BA },
    { "TR", TDA8425_Reg_TR },
    { "SF", TDA8425_Reg_SF },
    { NULL, (TDA8425_Reg)0 }
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


typedef struct Args {
    long channels;
    STREAM_READER stream_reader;
    STREAM_WRITER stream_writer;
    TDA8425_Float rate;
    TDA8425_Float pseudo_c1;
    TDA8425_Float pseudo_c2;
    TDA8425_Tfilter_Mode tfilter_mode;
    TDA8425_Register regs[TDA8425_RegOrder_Count];
} Args;


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


static int Run(Args const* args);


int main(int argc, char const* argv[])
{
    Args args;
    args.channels = 1;
    args.stream_reader = ReadU8;
    args.stream_writer = WriteU8;
    args.rate = 44100;
    args.pseudo_c1 = TDA8425_Pseudo_C1_Table[0];
    args.pseudo_c2 = TDA8425_Pseudo_C2_Table[0];
    args.tfilter_mode = TDA8425_Tfilter_Mode_Disabled;
    args.regs[TDA8425_RegOrder_VL] = (TDA8425_Register)TDA8425_Volume_Data_Unity;
    args.regs[TDA8425_RegOrder_VR] = (TDA8425_Register)TDA8425_Volume_Data_Unity;
    args.regs[TDA8425_RegOrder_BA] = (TDA8425_Register)TDA8425_Tone_Data_Unity;
    args.regs[TDA8425_RegOrder_TR] = (TDA8425_Register)TDA8425_Tone_Data_Unity;
    args.regs[TDA8425_RegOrder_SF] = (
        (TDA8425_Register)TDA8425_Selector_Stereo_1 |
        ((TDA8425_Register)TDA8425_Mode_LinearStereo << TDA8425_Reg_SF_STL)
    );

    for (int i = 1; i < argc; ++i) {
        // Unary arguments
        if (!strcmp(argv[i], "-h") ||
            !strcmp(argv[i], "--help")) {
            puts(USAGE);
            return 0;
        }
        else if (!strcmp(argv[i], "--t-filter")) {
            args.tfilter_mode = TDA8425_Tfilter_Mode_Enabled;
            continue;
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
                    args.regs[TDA8425_RegOrder_BA] = (TDA8425_Register)j;
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
            args.channels = strtol(argv[++i], NULL, 10);
            if (args.channels < 1) {
                fprintf(stderr, "Invalid channels: %s", argv[i]);
                return 1;
            }
            else if (args.channels > MAX_INPUTS) {
                args.channels = MAX_INPUTS;
            }
        }
        else if (!strcmp(argv[i], "-f") ||
            !strcmp(argv[i], "--format")) {
            char const* label = argv[++i];
            int j;
            for (j = 0; FORMAT_TABLE[j].label; ++j) {
                if (!strcmp(label, FORMAT_TABLE[j].label)) {
                    args.stream_reader = FORMAT_TABLE[j].reader;
                    args.stream_writer = FORMAT_TABLE[j].writer;
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
                    args.regs[TDA8425_RegOrder_SF] &= (TDA8425_Register)~(TDA8425_Mode_Mask << TDA8425_Reg_SF_STL);
                    args.regs[TDA8425_RegOrder_SF] |= (TDA8425_Register)MODE_TABLE[j].value << TDA8425_Reg_SF_STL;
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
            args.pseudo_c1 = (TDA8425_Float)value;
        }
        else if (!strcmp(argv[i], "--pseudo-c2")) {
            double value = atof(argv[++i]);
            if (value <= 0) {
                fprintf(stderr, "Invalid capacitance: %s", argv[i]);
                return 1;
            }
            args.pseudo_c2 = (TDA8425_Float)value;
        }
        else if (!strcmp(argv[i], "--pseudo-preset")) {
            long preset = strtol(argv[++i], NULL, 10);
            if (preset < 1 || preset >(long)TDA8425_Pseudo_Preset_Count) {
                fprintf(stderr, "Invalid pseudo selection: %s", argv[i]);
                return 1;
            }
            --preset;
            args.pseudo_c1 = TDA8425_Pseudo_C1_Table[preset];
            args.pseudo_c2 = TDA8425_Pseudo_C2_Table[preset];
        }
        else if (!strcmp(argv[i], "-r") ||
            !strcmp(argv[i], "--rate")) {
            args.rate = (TDA8425_Float)atof(argv[++i]);
            if (args.rate < 1) {
                fprintf(stderr, "Invalid rate: %s", argv[i]);
                return 1;
            }
        }
        else if (!strncmp(argv[i], "--reg-", 6)) {
            char const* label = &argv[i][6];
            int r;
            for (r = 0; REGISTER_TABLE[r].label; ++r) {
                if (!strcmp(label, REGISTER_TABLE[r].label)) {
                    break;
                }
            }
            if (!REGISTER_TABLE[r].label) {
                fprintf(stderr, "Unknown register: %s", label);
                return 1;
            }
            long value = strtol(argv[++i], NULL, 16);
            if (errno || value < 0x00 || value > 0xFF) {
                fprintf(stderr, "Invalid register value: %s", argv[i]);
                return 1;
            }
            args.regs[r] = (TDA8425_Register)value;
        }
        else if (!strcmp(argv[i], "-s") ||
            !strcmp(argv[i], "--selector")) {
            char const* label = argv[++i];
            int j;
            for (j = 0; SELECTOR_TABLE[j].label; ++j) {
                if (!strcmp(label, SELECTOR_TABLE[j].label)) {
                    args.regs[TDA8425_RegOrder_SF] &= (TDA8425_Register)~TDA8425_Selector_Mask;
                    args.regs[TDA8425_RegOrder_SF] |= (TDA8425_Register)SELECTOR_TABLE[j].value;
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
                    args.regs[TDA8425_RegOrder_TR] = (TDA8425_Register)j;
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
                    args.regs[TDA8425_RegOrder_VL] = (TDA8425_Register)j;
                    args.regs[TDA8425_RegOrder_VR] = (TDA8425_Register)j;
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
                    args.regs[TDA8425_RegOrder_VL] = (TDA8425_Register)j;
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
                    args.regs[TDA8425_RegOrder_VR] = (TDA8425_Register)j;
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

#ifdef __WINDOWS__
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
#endif  // __WINDOWS__

    return Run(&args);
}


static int Run(Args const* args)
{
    TDA8425_Chip* chip;
    chip = (TDA8425_Chip*)malloc(sizeof(TDA8425_Chip));
    if (!chip) {
        return 1;
    }
    TDA8425_Chip_Ctor(chip);
    TDA8425_Chip_Setup(chip, args->rate, args->pseudo_c1, args->pseudo_c2, args->tfilter_mode);
    TDA8425_Chip_Reset(chip);
    TDA8425_Chip_Write(chip, (TDA8425_Address)TDA8425_Reg_VL, args->regs[TDA8425_RegOrder_VL]);
    TDA8425_Chip_Write(chip, (TDA8425_Address)TDA8425_Reg_VR, args->regs[TDA8425_RegOrder_VR]);
    TDA8425_Chip_Write(chip, (TDA8425_Address)TDA8425_Reg_BA, args->regs[TDA8425_RegOrder_BA]);
    TDA8425_Chip_Write(chip, (TDA8425_Address)TDA8425_Reg_TR, args->regs[TDA8425_RegOrder_TR]);
    TDA8425_Chip_Write(chip, (TDA8425_Address)TDA8425_Reg_SF, args->regs[TDA8425_RegOrder_SF]);
    TDA8425_Chip_Start(chip);
    int error = 0;

    while (!feof(stdin)) {
        TDA8425_Chip_Process_Data data;
        long channel;

        TDA8425_Float* inputs = &data.inputs[0][0];  // overflows
        for (channel = 0; channel < args->channels; ++channel) {
            if (!args->stream_reader(&inputs[channel])) {
                if (ferror(stdin)) {
                    perror("stream_reader()");
                    error = 1;
                }
                goto end;
            }
        }
        for (; channel < MAX_INPUTS; ++channel) {
            inputs[channel] = 0;
        }

        TDA8425_Chip_Process(chip, &data);

        for (channel = 0; channel < MAX_OUTPUTS; ++channel) {
            if (!args->stream_writer(data.outputs[channel])) {
                perror("stream_writer()");
                error = 1;
                goto end;
            }
        }
    }

end:
    TDA8425_Chip_Stop(chip);
    TDA8425_Chip_Dtor(chip);
    free(chip);
    return error;
}
