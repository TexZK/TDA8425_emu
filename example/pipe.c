#include "TDA8425_emu.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "endian.h"


#if __BYTE_ORDER == __LITTLE_ENDIAN
    #define READ_LE (dst_, size_)  (fread ((dst_), (size_), 1, stdin ) == (size_))
    #define WRITE_LE(src_, size_)  (fwrite((src_), (size_), 1, stdout) == (size_))
    #define READ_BE (dst_, size_)  (fread ((dst_), 1, (size_), stdin ) == (size_))
    #define WRITE_BE(src_, size_)  (fwrite((src_), 1, (size_), stdout) == (size_))
#elif __BYTE_ORDER == __BIG_ENDIAN
    #define READ_LE (dst_, size_)  (fread ((dst_), 1, (size_), stdin ) == (size_))
    #define WRITE_LE(src_, size_)  (fwrite((src_), 1, (size_), stdout) == (size_))
    #define READ_BE (dst_, size_)  (fread ((dst_), (size_), 1, stdin ) == (size_))
    #define WRITE_BE(src_, size_)  (fwrite((src_), (size_), 1, stdout) == (size_))
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
    float32_t src;
    if (READ_LE(&src, sizeof(src))) {
        *dst = (TDA8425_Float)src;
        return 1;
    }
    return 0;
}

int ReadF32B(TDA8425_Float* dst) {
    float32_t src;
    if (READ_BE(&src, sizeof(src))) {
        *dst = (TDA8425_Float)src;
        return 1;
    }
    return 0;
}

int ReadF64L(TDA8425_Float* dst) {
    float64_t src;
    if (READ_LE(&src, sizeof(src))) {
        *dst = (TDA8425_Float)src;
        return 1;
    }
    return 0;
}

int ReadF64B(TDA8425_Float* dst) {
    float64_t src;
    if (READ_BE(&src, sizeof(src))) {
        *dst = (TDA8425_Float)src;
        return 1;
    }
    return 0;
}


TDA8425_Float const* WriteU8(TDA8425_Float src) {
    int8_t dst = (int8_t)(src * -(TDA8425_Float)INT8_MIN);
    return WRITE_LE(&dst, sizeof(dst));
}

TDA8425_Float const* WriteS8(TDA8425_Float src) {
    int8_t dst = (int8_t)(src * -(TDA8425_Float)INT8_MIN);
    dst -= INT8_MIN;
    return WRITE_LE(&dst, sizeof(dst));
}

TDA8425_Float const* WriteU16L(TDA8425_Float src) {
    int16_t dst = (int16_t)(src * -(TDA8425_Float)INT16_MIN);
    dst -= INT16_MIN;
    return WRITE_LE(&dst, sizeof(dst));
}

TDA8425_Float const* WriteU16B(TDA8425_Float src) {
    int16_t dst = (int16_t)(src * -(TDA8425_Float)INT16_MIN);
    dst -= INT16_MIN;
    return WRITE_BE(&dst, sizeof(dst));
}

TDA8425_Float const* WriteS16L(TDA8425_Float src) {
    int16_t dst = (int16_t)(src * -(TDA8425_Float)INT16_MIN);
    return WRITE_LE(&dst, sizeof(dst));
}

TDA8425_Float const* WriteS16B(TDA8425_Float src) {
    int16_t dst = (int16_t)(src * -(TDA8425_Float)INT16_MIN);
    return WRITE_BE(&dst, sizeof(dst));
}

TDA8425_Float const* WriteU32L(TDA8425_Float src) {
    int32_t dst = (int32_t)(src * -(TDA8425_Float)INT32_MIN);
    dst -= INT32_MIN;
    return WRITE_LE(&dst, sizeof(dst));
}

TDA8425_Float const* WriteU32B(TDA8425_Float src) {
    int32_t dst = (int32_t)(src * -(TDA8425_Float)INT32_MIN);
    dst -= INT32_MIN;
    return WRITE_BE(&dst, sizeof(dst));
}

TDA8425_Float const* WriteS32L(TDA8425_Float src) {
    int32_t dst = (int32_t)(src * -(TDA8425_Float)INT32_MIN);
    return WRITE_LE(&dst, sizeof(dst));
}

TDA8425_Float const* WriteS32B(TDA8425_Float src) {
    int32_t dst = (int32_t)(src * -(TDA8425_Float)INT32_MIN);
    return WRITE_BE(&dst, sizeof(dst));
}

TDA8425_Float const* WriteU64L(TDA8425_Float src) {
    int64_t dst = (int64_t)(src * -(TDA8425_Float)INT64_MIN);
    dst -= INT64_MIN;
    return WRITE_LE(&dst, sizeof(dst));
}

TDA8425_Float const* WriteU64B(TDA8425_Float src) {
    int64_t dst = (int64_t)(src * -(TDA8425_Float)INT64_MIN);
    dst -= INT64_MIN;
    return WRITE_BE(&dst, sizeof(dst));
}

TDA8425_Float const* WriteS64L(TDA8425_Float src) {
    int64_t dst = (int64_t)(src * -(TDA8425_Float)INT64_MIN);
    return WRITE_LE(&dst, sizeof(dst));
}

TDA8425_Float const* WriteS64B(TDA8425_Float src) {
    int64_t dst = (int64_t)(src * -(TDA8425_Float)INT64_MIN);
    return WRITE_BE(&dst, sizeof(dst));
}

TDA8425_Float const* WriteF32L(TDA8425_Float src) {
    float32_t dst = (float32_t)src;
    return WRITE_LE(&dst, sizeof(dst));
}

TDA8425_Float const* WriteF32B(TDA8425_Float src) {
    float32_t dst = (float32_t)src;
    return WRITE_BE(&dst, sizeof(dst));
}

TDA8425_Float const* WriteF64L(TDA8425_Float src) {
    float64_t dst = (float64_t)src;
    return WRITE_LE(&dst, sizeof(dst));
}

TDA8425_Float const* WriteF64B(TDA8425_Float src) {
    float64_t dst = (float64_t)src;
    return WRITE_BE(&dst, sizeof(dst));
}


typedef int (*STREAM_READER)(TDA8425_Float* dst);
typedef int (*STREAM_WRITER)(TDA8425_Float src);

struct {
    char const *label;
    STREAM_READER reader;
    STREAM_WRITER writer;
} FORMAT_METHOD_TABLE[] =
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


static char const* USAGE = (
"usage:\n"
"  pipe [-h|--help]\n"
);


int main(int argc, char const* argv[])
{
    long channels = 1;
    double duration = 0;
    STREAM_READER stream_reader = ReadU8;
    STREAM_WRITER stream_writer = WriteU8;
    TDA8425_Float rate = 44100;
    TDA8425_Float pseudo_c1 = TDA8425_Pseudo_C1_Table[0];
    TDA8425_Float pseudo_c2 = TDA8425_Pseudo_C2_Table[0];
    TDA8425_Register reg_vl = (TDA8425_Register)TDA8425_Volume_Data_Mask;
    TDA8425_Register reg_vr = (TDA8425_Register)TDA8425_Volume_Data_Mask;
    TDA8425_Register reg_ba = (TDA8425_Register)TDA8425_Tone_Data_Unity;
    TDA8425_Register reg_tr = (TDA8425_Register)TDA8425_Tone_Data_Unity;
    TDA8425_Register reg_sf = (TDA8425_Register)(
        TDA8425_Selector_Stereo_1 |
        (TDA8425_Mode_LinearStereo << TDA8425_Reg_SF_STL)
    );

    for (int i = 1; i < argc; ++i) {
        // unary arguments
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
        else if (!strcmp(argv[i], "-r") ||
                 !strcmp(argv[i], "--rate")) {
            rate = (TDA8425_Float)atof(argv[++i]);
            if (rate < 1) {
                fprintf(stderr, "Invalid rate: %s", argv[i]);
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
        else if (!strcmp(argv[i], "-d") ||
                 !strcmp(argv[i], "--duration")) {
            duration = atof(argv[++i]);
            if (duration < 0) {
                fprintf(stderr, "Invalid duration: %s", argv[i]);
                return 1;
            }
        }
        else if (!strcmp(argv[i], "-f") ||
                 !strcmp(argv[i], "--format")) {
            char const* fmt = argv[++i];
            int j;
            for (j = 0; FORMAT_METHOD_TABLE[j].label; ++j) {
                if (!strcmp(fmt, FORMAT_METHOD_TABLE[j].label)) {
                    stream_reader = FORMAT_METHOD_TABLE[j].reader;
                    stream_writer = FORMAT_METHOD_TABLE[j].writer;
                    break;
                }
            }
            if (!FORMAT_METHOD_TABLE[j].label) {
                fprintf(stderr, "Unknown format: %s", fmt);
                return 1;
            }
        }
        else if (!strcmp(argv[i], "--pseudo-preset")) {
            long preset = strtol(argv[++i], NULL, 10);
            if (preset < 1 || preset > (long)TDA8425_Pseudo_Preset_Count) {
                fprintf(stderr, "Invalid pseudo selection: %s", argv[i]);
                return 1;
            }
            --preset;
            pseudo_c1 = TDA8425_Pseudo_C1_Table[preset];
            pseudo_c2 = TDA8425_Pseudo_C2_Table[preset];
        }
        else if (!strcmp(argv[i], "--reg_vl")) {
            reg_vl = (TDA8425_Register)strtol(argv[++i], NULL, 16);
        }
        else if (!strcmp(argv[i], "--reg_vr")) {
            reg_vr = (TDA8425_Register)strtol(argv[++i], NULL, 16);
        }
        else if (!strcmp(argv[i], "--reg_ba")) {
            reg_ba = (TDA8425_Register)strtol(argv[++i], NULL, 16);
        }
        else if (!strcmp(argv[i], "--reg_tr")) {
            reg_tr = (TDA8425_Register)strtol(argv[++i], NULL, 16);
        }
        else if (!strcmp(argv[i], "--reg_sf")) {
            reg_sf = (TDA8425_Register)strtol(argv[++i], NULL, 16);
        }
        else {
            fprintf(stderr, "unknown switch: %s", argv[i]);
            return 1;
        }

        if (errno) {
            fprintf(stderr, "arg %d", i);
            perror("");
            return 1;
        }
    }

    TDA8425_ChipFloat chip;
    TDA8425_ChipFloat_Ctor(&chip);
    TDA8425_ChipFloat_Setup(&chip, rate, pseudo_c1, pseudo_c2);
    TDA8425_ChipFloat_Reset(&chip);
    TDA8425_ChipFloat_Start(&chip);

    while (!feof(stdin)) {
        TDA8425_ChipFloat_Process_Data data;
        long channel;

        for (channel = 0; channel < channels; ++channel) {
            if (!stream_reader(&data.inputs[0][channel])) {  // allow overflow
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
            data.inputs[0][channel] = 0;  // allow overflow
        }

        TDA8425_ChipFloat_ProcessSample(&chip, &data);

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
