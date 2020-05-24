#include "TDA8425_emu.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char const* argv[])
{
    TDA8425_Float fs = 44100;
    TDA8425_Register reg_vl = (TDA8425_Register)TDA8425_Volume_Data_Mask;
    TDA8425_Register reg_vr = (TDA8425_Register)TDA8425_Volume_Data_Mask;
    TDA8425_Register reg_ba = (TDA8425_Register)TDA8425_Tone_Data_Unity;
    TDA8425_Register reg_tr = (TDA8425_Register)TDA8425_Tone_Data_Unity;
    TDA8425_Register reg_sf = (TDA8425_Register)(
        TDA8425_Selector_Stereo_1 |
        (TDA8425_Mode_LinearStereo << TDA8425_Reg_SF_STL)
    );

    for (int i = 1; i < argc - 1; ++i) {
        if (!strcmp(argv[i], "--fs")) {
            fs = atof(argv[++i]);
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
            printf("argv: unknown option: %s", argv[i]);
            return 1;
        }
        if (errno) {
            perror("argv");
            return 1;
        }
    }

    TDA8425_ChipFloat chip;
    TDA8425_ChipFloat_Ctor(&chip);
    TDA8425_ChipFloat_Setup(&chip, fs,
                            TDA8425_Pseudo_C1_Table[0],
                            TDA8425_Pseudo_C2_Table[0]);
    TDA8425_ChipFloat_Reset(&chip);
    TDA8425_ChipFloat_Start(&chip);

    while (!feof(stdin)) {
        TDA8425_ChipFloat_Process_Data data;

        fread(&data.inputs, sizeof(TDA8425_Float), TDA8425_Stereo_Count, stdin);
        if (feof(stdin)) {
            break;
        }
        if (ferror(stdin)) {
            perror("fread()");
            return 1;
        }
        data.inputs[TDA8425_Source_2][TDA8425_Stereo_L] = 0;
        data.inputs[TDA8425_Source_2][TDA8425_Stereo_R] = 0;

        TDA8425_ChipFloat_ProcessSample(&chip, &data);

        fwrite(&data.outputs, sizeof(TDA8425_Float), TDA8425_Stereo_Count, stdout);
        if (ferror(stdout)) {
            perror("fwrite()");
            return 1;
        }
    }

    TDA8425_ChipFloat_Stop(&chip);
    TDA8425_ChipFloat_Dtor(&chip);

    return 0;
}
