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

#include <math.h>

// ============================================================================

signed char const TDA8425_VolumeDecibel_Table[TDA8425_Volume_Data_Count] =
{
    -128,  //  0
    -128,  //  1
    -128,  //  2
    -128,  //  3
    -128,  //  4
    -128,  //  5
    -128,  //  6
    -128,  //  7
    -128,  //  8
    -128,  //  9
    -128,  // 10
    -128,  // 11
    -128,  // 12
    -128,  // 13
    -128,  // 14
    -128,  // 15
    -128,  // 16
    -128,  // 17
    -128,  // 18
    -128,  // 19
    -128,  // 20
    -128,  // 21
    -128,  // 22
    -128,  // 23
    -128,  // 24
    -128,  // 25
    -128,  // 26
    -128,  // 27
    - 64,  // 28
    - 62,  // 29
    - 60,  // 30
    - 58,  // 31
    - 56,  // 32
    - 54,  // 33
    - 52,  // 34
    - 50,  // 35
    - 48,  // 36
    - 46,  // 37
    - 44,  // 38
    - 42,  // 39
    - 40,  // 40
    - 38,  // 41
    - 36,  // 42
    - 34,  // 43
    - 32,  // 44
    - 30,  // 45
    - 28,  // 46
    - 26,  // 47
    - 24,  // 48
    - 22,  // 49
    - 20,  // 50
    - 18,  // 51
    - 16,  // 52
    - 14,  // 53
    - 12,  // 54
    - 10,  // 55
    -  8,  // 56
    -  6,  // 57
    -  4,  // 58
    -  2,  // 59
       0,  // 60
    +  2,  // 61
    +  4,  // 62
    +  6   // 63
};

TDA8425_Float const TDA8425_VolumeLinear_Table[TDA8425_Volume_Data_Count] =
{
    (TDA8425_Float)0.000000000000000000,  //  0
    (TDA8425_Float)0.000000000000000000,  //  1
    (TDA8425_Float)0.000000000000000000,  //  2
    (TDA8425_Float)0.000000000000000000,  //  3
    (TDA8425_Float)0.000000000000000000,  //  4
    (TDA8425_Float)0.000000000000000000,  //  5
    (TDA8425_Float)0.000000000000000000,  //  6
    (TDA8425_Float)0.000000000000000000,  //  7
    (TDA8425_Float)0.000000000000000000,  //  8
    (TDA8425_Float)0.000000000000000000,  //  9
    (TDA8425_Float)0.000000000000000000,  // 10
    (TDA8425_Float)0.000000000000000000,  // 11
    (TDA8425_Float)0.000000000000000000,  // 12
    (TDA8425_Float)0.000000000000000000,  // 13
    (TDA8425_Float)0.000000000000000000,  // 14
    (TDA8425_Float)0.000000000000000000,  // 15
    (TDA8425_Float)0.000000000000000000,  // 16
    (TDA8425_Float)0.000000000000000000,  // 17
    (TDA8425_Float)0.000000000000000000,  // 18
    (TDA8425_Float)0.000000000000000000,  // 19
    (TDA8425_Float)0.000000000000000000,  // 20
    (TDA8425_Float)0.000000000000000000,  // 21
    (TDA8425_Float)0.000000000000000000,  // 22
    (TDA8425_Float)0.000000000000000000,  // 23
    (TDA8425_Float)0.000000000000000000,  // 24
    (TDA8425_Float)0.000000000000000000,  // 25
    (TDA8425_Float)0.000000000000000000,  // 26
    (TDA8425_Float)0.000000000000000000,  // 27
    (TDA8425_Float)0.000630957344480193,  // 28
    (TDA8425_Float)0.000794328234724281,  // 29
    (TDA8425_Float)0.001000000000000000,  // 30
    (TDA8425_Float)0.001258925411794167,  // 31
    (TDA8425_Float)0.001584893192461114,  // 32
    (TDA8425_Float)0.001995262314968879,  // 33
    (TDA8425_Float)0.002511886431509579,  // 34
    (TDA8425_Float)0.003162277660168379,  // 35
    (TDA8425_Float)0.003981071705534973,  // 36
    (TDA8425_Float)0.005011872336272725,  // 37
    (TDA8425_Float)0.006309573444801930,  // 38
    (TDA8425_Float)0.007943282347242814,  // 39
    (TDA8425_Float)0.010000000000000000,  // 40
    (TDA8425_Float)0.012589254117941675,  // 41
    (TDA8425_Float)0.015848931924611134,  // 42
    (TDA8425_Float)0.019952623149688799,  // 43
    (TDA8425_Float)0.025118864315095794,  // 44
    (TDA8425_Float)0.031622776601683791,  // 45
    (TDA8425_Float)0.039810717055349734,  // 46
    (TDA8425_Float)0.050118723362727220,  // 47
    (TDA8425_Float)0.063095734448019331,  // 48
    (TDA8425_Float)0.079432823472428138,  // 49
    (TDA8425_Float)0.100000000000000006,  // 50
    (TDA8425_Float)0.125892541179416728,  // 51
    (TDA8425_Float)0.158489319246111343,  // 52
    (TDA8425_Float)0.199526231496887974,  // 53
    (TDA8425_Float)0.251188643150958013,  // 54
    (TDA8425_Float)0.316227766016837941,  // 55
    (TDA8425_Float)0.398107170553497203,  // 56
    (TDA8425_Float)0.501187233627272244,  // 57
    (TDA8425_Float)0.630957344480193250,  // 58
    (TDA8425_Float)0.794328234724281490,  // 59
    (TDA8425_Float)1.000000000000000000,  // 60
    (TDA8425_Float)1.258925411794167282,  // 61
    (TDA8425_Float)1.584893192461113598,  // 62
    (TDA8425_Float)1.995262314968879513   // 63
};

// ----------------------------------------------------------------------------

signed char const TDA8425_BassDecibel_Table[TDA8425_Tone_Data_Count] =
{
    -12,  //  0
    -12,  //  1
    -12,  //  2
    - 9,  //  3
    - 6,  //  4
    - 3,  //  5
      0,  //  6
    + 3,  //  7
    + 6,  //  8
    + 9,  //  9
    +12,  // 10
    +15,  // 11
    +15,  // 12
    +15,  // 13
    +15,  // 14
    +15   // 15
};

TDA8425_Float const TDA8425_BassLinear_Table[TDA8425_Tone_Data_Count] =
{
    (TDA8425_Float)0.251188643150958013,  //  0
    (TDA8425_Float)0.251188643150958013,  //  1
    (TDA8425_Float)0.251188643150958013,  //  2
    (TDA8425_Float)0.354813389233575471,  //  3
    (TDA8425_Float)0.501187233627272244,  //  4
    (TDA8425_Float)0.707945784384137911,  //  5
    (TDA8425_Float)1.000000000000000000,  //  6
    (TDA8425_Float)1.412537544622754382,  //  7
    (TDA8425_Float)1.995262314968879513,  //  8
    (TDA8425_Float)2.818382931264453717,  //  9
    (TDA8425_Float)3.981071705534972249,  // 10
    (TDA8425_Float)5.623413251903491172,  // 11
    (TDA8425_Float)5.623413251903491172,  // 12
    (TDA8425_Float)5.623413251903491172,  // 13
    (TDA8425_Float)5.623413251903491172,  // 14
    (TDA8425_Float)5.623413251903491172   // 15
};

// ----------------------------------------------------------------------------

signed char const TDA8425_TrebleDecibel_Table[TDA8425_Tone_Data_Count] =
{
    -12,  //  0
    -12,  //  1
    -12,  //  2
    - 9,  //  3
    - 6,  //  4
    - 3,  //  5
      0,  //  6
    + 3,  //  7
    + 6,  //  8
    + 9,  //  9
    +12,  // 10
    +12,  // 11
    +12,  // 12
    +12,  // 13
    +12,  // 14
    +12   // 15
};

TDA8425_Float const TDA8425_TrebleLinear_Table[TDA8425_Tone_Data_Count] =
{
    (TDA8425_Float)0.251188643150958013,  //  0
    (TDA8425_Float)0.251188643150958013,  //  1
    (TDA8425_Float)0.251188643150958013,  //  2
    (TDA8425_Float)0.354813389233575471,  //  3
    (TDA8425_Float)0.501187233627272244,  //  4
    (TDA8425_Float)0.707945784384137911,  //  5
    (TDA8425_Float)1.000000000000000000,  //  6
    (TDA8425_Float)1.412537544622754382,  //  7
    (TDA8425_Float)1.995262314968879513,  //  8
    (TDA8425_Float)2.818382931264453717,  //  9
    (TDA8425_Float)3.981071705534972249,  // 10
    (TDA8425_Float)3.981071705534972249,  // 11
    (TDA8425_Float)3.981071705534972249,  // 12
    (TDA8425_Float)3.981071705534972249,  // 13
    (TDA8425_Float)3.981071705534972249,  // 14
    (TDA8425_Float)3.981071705534972249   // 15
};

// ----------------------------------------------------------------------------

TDA8425_Float const TDA8425_Pseudo_C1_Table[TDA8425_Pseudo_Preset_Count] =
{
    (TDA8425_Float)15.e-9,
    (TDA8425_Float)5.6e-9,
    (TDA8425_Float)5.6e-9
};

TDA8425_Float const TDA8425_Pseudo_C2_Table[TDA8425_Pseudo_Preset_Count] =
{
    (TDA8425_Float)15.e-9,
    (TDA8425_Float)47.e-9,
    (TDA8425_Float)68.e-9
};

// ============================================================================

void TDA8425_BiQuadModelFloat_SetupPseudo(
    TDA8425_BiQuadModelFloat* model,
    TDA8425_Float sample_rate,
    TDA8425_Float pseudo_c1,
    TDA8425_Float pseudo_c2
)
{
    double fs = sample_rate;

    double c1 = pseudo_c1;
    double c2 = pseudo_c2;

    double r1 = (double)TDA8425_Pseudo_R1;
    double r2 = (double)TDA8425_Pseudo_R2;

    double w1 = c1 * r1;
    double w2 = c2 * r2;

    double k1 = (w1 * w2) * 4;
    double k2 = (w1 + w2) * (2 / fs);
    double tt = 1 / (fs * fs);

    double a0 = k1 + k2 + tt;
    double a1 = (tt - k1) * 2;
    double a2 = k1 - k2 + tt;

    double b0 = k1 - k2 + tt;
    double b1 = (tt - k1) * 2;
    double b2 = k1 + k2 + tt;

    double ra0 = 1 / a0;

    model->b0 = (TDA8425_Float)(b0 *  ra0);
    model->b1 = (TDA8425_Float)(b1 *  ra0);
    model->b2 = (TDA8425_Float)(b2 *  ra0);

    model->a1 = (TDA8425_Float)(a1 * -ra0);
    model->a2 = (TDA8425_Float)(a2 * -ra0);
}

// ----------------------------------------------------------------------------

void TDA8425_BiQuadModelFloat_SetupBass(
    TDA8425_BiQuadModelFloat* model,
    TDA8425_Float sample_rate,
    TDA8425_Float bass_gain
)
{
    double fs = sample_rate;
    double w = (2 * M_PI) * (double)TDA8425_Bass_Frequency;
    double s = .5;
    double g = sqrt(bass_gain);

    double p = w / fs;
    double cw = cos(p);
    double sw = sin(p);

    double gm1 = g - 1;
    double gp1 = g + 1;

    double gm1_cw = gm1 * cw;
    double gp1_cw = gp1 * cw;

    double h = sw * sqrt((g * g + 1) * (1 / (g * s) - 1) + g * 2);

    double a0 = (gp1 + gm1_cw + h)         ;
    double a1 = (gm1 + gp1_cw    )     * -2;
    double a2 = (gp1 + gm1_cw - h)         ;

    double b0 = (gp1 - gm1_cw + h) * g     ;
    double b1 = (gm1 - gp1_cw    ) * g *  2;
    double b2 = (gp1 - gm1_cw - h) * g     ;

    double ra0 = 1 / a0;

    model->b0 = (TDA8425_Float)(b0 *  ra0);
    model->b1 = (TDA8425_Float)(b1 *  ra0);
    model->b2 = (TDA8425_Float)(b2 *  ra0);

    model->a1 = (TDA8425_Float)(a1 * -ra0);
    model->a2 = (TDA8425_Float)(a2 * -ra0);
}

// ----------------------------------------------------------------------------

void TDA8425_BiQuadModelFloat_SetupTreble(
    TDA8425_BiQuadModelFloat* model,
    TDA8425_Float sample_rate,
    TDA8425_Float treble_gain
)
{
    double fs = sample_rate;
    double w = (2 * M_PI) * (double)TDA8425_Treble_Frequency;
    double s = .5;
    double g = sqrt(treble_gain);

    double p = w / fs;
    double cw = cos(p);
    double sw = sin(p);

    double gm1 = g - 1;
    double gp1 = g + 1;

    double gm1_cw = gm1 * cw;
    double gp1_cw = gp1 * cw;

    double h = sw * sqrt((g * g + 1) * (1 / s - 1) + g * 2);

    double a0 = (gp1 - gm1_cw + h)         ;
    double a1 = (gm1 - gp1_cw    )     *  2;
    double a2 = (gp1 - gm1_cw - h)         ;

    double b0 = (gp1 + gm1_cw + h) * g     ;
    double b1 = (gm1 + gp1_cw    ) * g * -2;
    double b2 = (gp1 + gm1_cw - h) * g     ;

    double ra0 = 1 / a0;

    model->b0 = (TDA8425_Float)(b0 *  ra0);
    model->b1 = (TDA8425_Float)(b1 *  ra0);
    model->b2 = (TDA8425_Float)(b2 *  ra0);

    model->a1 = (TDA8425_Float)(a1 * -ra0);
    model->a2 = (TDA8425_Float)(a2 * -ra0);
}

// ----------------------------------------------------------------------------

void TDA8425_BiQuadStateFloat_Clear(
    TDA8425_BiQuadStateFloat* state,
    TDA8425_Float output
)
{
    state->x0 = 0;
    state->x1 = 0;
    state->x2 = 0;

    state->y0 = output;
    state->y1 = output;
    state->y2 = output;
}

// ----------------------------------------------------------------------------

TDA8425_Float TDA8425_BiQuadFloat_ProcessSample(
    TDA8425_BiQuadModelFloat* model,
    TDA8425_BiQuadStateFloat* state,
    TDA8425_Float input
)
{
    state->x2 = state->x1;
    state->x1 = state->x0;
    state->x0 = input;

    state->y2 = state->y1;
    state->y1 = state->y0;

    state->y0 = (state->x0 * model->b0 +
                 state->x1 * model->b1 +
                 state->x2 * model->b2
                 +
                 state->y1 * model->a1 +
                 state->y2 * model->a2);

    return state->y0;
}

// ----------------------------------------------------------------------------

void TDA8425_BiQuadFloat_ProcessVector(
    TDA8425_BiQuadModelFloat* model,
    TDA8425_BiQuadStateFloat* state,
    TDA8425_Index count,
    TDA8425_Float const inputs[],
    TDA8425_Float outputs[],
)
{
    TDA8425_Float x0 = state->x0;
    TDA8425_Float x1 = state->x1;
    TDA8425_Float x2 = state->x2;

    TDA8425_Float y0 = state->y0;
    TDA8425_Float y1 = state->y1;
    TDA8425_Float y2 = state->y2;

    TDA8425_Float b0 = model->b0;
    TDA8425_Float b1 = model->b1;
    TDA8425_Float b2 = model->b2;

    TDA8425_Float a1 = model->a1;
    TDA8425_Float a2 = model->a2;

    while (count--) {
        x2 = x1;
        x1 = x0;
        x0 = *inputs++;

        y2 = y1;
        y1 = y0;

        y0 = (x0 * b0 +
              x1 * b1 +
              x2 * b2
              +
              y1 * a1 +
              y2 * a2);

        *outputs++ = y0;
    }

    state->x0 = x0;
    state->x1 = x1;
    state->x2 = x2;

    state->y0 = y0;
    state->y1 = y1;
    state->y2 = y2;
}

// ============================================================================

void TDA8425_ChipFloat_Ctor(TDA8425_ChipFloat* self)
{
    (void)self;
}

// ----------------------------------------------------------------------------

void TDA8425_ChipFloat_Dtor(TDA8425_ChipFloat* self)
{
    (void)self;
}

// ----------------------------------------------------------------------------

void TDA8425_ChipFloat_Setup(
    TDA8425_ChipFloat* self,
    TDA8425_Float sample_rate,
    TDA8425_Float pseudo_c1,
    TDA8425_Float pseudo_c2
)
{
    self->sample_rate_ = sample_rate;

    TDA8425_BiQuadModelFloat_SetupPseudo(
        &self->pseudo_model_,
        sample_rate,
        pseudo_c1,
        pseudo_c2
    );

    TDA8425_ChipFloat_Write(self, TDA8425_Reg_BA, self->reg_ba_);
    TDA8425_ChipFloat_Write(self, TDA8425_Reg_TR, self->reg_tr_);
}

// ----------------------------------------------------------------------------

void TDA8425_ChipFloat_Reset(TDA8425_ChipFloat* self)
{
    TDA8425_ChipFloat_Write(self, TDA8425_Reg_VL, 0);
    TDA8425_ChipFloat_Write(self, TDA8425_Reg_VR, 0);
    TDA8425_ChipFloat_Write(self, TDA8425_Reg_BA, 0);
    TDA8425_ChipFloat_Write(self, TDA8425_Reg_TR, 0);
    TDA8425_ChipFloat_Write(self, TDA8425_Reg_SF, 0);
}

// ----------------------------------------------------------------------------

void TDA8425_ChipFloat_Start(TDA8425_ChipFloat* self)
{
    TDA8425_BiQuadStateFloat_Clear(&self->pseudo_state_, 0);

    for (unsigned i = 0; i < TDA8425_Stereo_Count; ++i) {
        TDA8425_BiQuadStateFloat_Clear(&self->bass_state_[i], 0);
        TDA8425_BiQuadStateFloat_Clear(&self->treble_state_[i], 0);
    }
}

// ----------------------------------------------------------------------------

void TDA8425_ChipFloat_Stop(TDA8425_ChipFloat* self)
{
    (void)self;
}

// ----------------------------------------------------------------------------

void TDA8425_ChipFloat_ProcessSample(
    TDA8425_ChipFloat* self,
    TDA8425_ChipFloat_Process_Data* data
)
{
    TDA8425_ChipFloat_ProcessVector(
        self,
        1,
        data.inputs,
        data.outputs
    );
}

// ----------------------------------------------------------------------------

void TDA8425_ChipFloat_ProcessVector(
    TDA8425_ChipFloat* self,
    TDA8425_Index count,
    TDA8425_Float const **inputs,
    TDA8425_Float **outputs
)
{
    TDA8425_Float const *input[TDA8425_Stereo_Count];
    TDA8425_Float *output[TDA8425_Stereo_Count];

    switch ((TDA8425_Selector)(self->reg_sf_ & 7))
    {
    case TDA8425_Selector_Sound_A_1:
        input[TDA8425_Stereo_L] = inputs[TDA8425_Source_1][TDA8425_Stereo_L];
        input[TDA8425_Stereo_R] = inputs[TDA8425_Source_1][TDA8425_Stereo_L];
        break;

    case TDA8425_Selector_Sound_A_2:
        input[TDA8425_Stereo_L] = inputs[TDA8425_Source_2][TDA8425_Stereo_L];
        input[TDA8425_Stereo_R] = inputs[TDA8425_Source_2][TDA8425_Stereo_L];
        break;

    case TDA8425_Selector_Sound_B_1:
        input[TDA8425_Stereo_L] = inputs[TDA8425_Source_1][TDA8425_Stereo_R];
        input[TDA8425_Stereo_R] = inputs[TDA8425_Source_1][TDA8425_Stereo_R];
        break;

    case TDA8425_Selector_Sound_B_2:
        input[TDA8425_Stereo_L] = inputs[TDA8425_Source_2][TDA8425_Stereo_L];
        input[TDA8425_Stereo_R] = inputs[TDA8425_Source_2][TDA8425_Stereo_R];
        break;

    case TDA8425_Selector_Stereo_1:
        input[TDA8425_Stereo_L] = inputs[TDA8425_Source_1][TDA8425_Stereo_L];
        input[TDA8425_Stereo_R] = inputs[TDA8425_Source_1][TDA8425_Stereo_R];
        break;

    case TDA8425_Selector_Stereo_2:
        input[TDA8425_Stereo_L] = inputs[TDA8425_Source_2][TDA8425_Stereo_L];
        input[TDA8425_Stereo_R] = inputs[TDA8425_Source_2][TDA8425_Stereo_R];
        break;

    default:
        return;
    }
    output[TDA8425_Stereo_L] = outputs[TDA8425_Stereo_L];
    output[TDA8425_Stereo_R] = outputs[TDA8425_Stereo_R];

    switch ((TDA8425_Mode)((self->reg_sf_ >> TDA8425_Reg_SF_STL) & 3))
    {
    case TDA8425_Mode_ForcedMono:
        input[TDA8425_Stereo_R] = input[TDA8425_Stereo_L];
        break;

    case TDA8425_Mode_LinearStereo:
        break;

    case TDA8425_Mode_PseudoStereo:
        TDA8425_BiQuadFloat_ProcessVector(
            &self->pseudo_model_,
            &self->pseudo_state_,
            count,
            input[TDA8425_Stereo_L],
            output[TDA8425_Stereo_L],
        );
        input[TDA8425_Stereo_L] = output[TDA8425_Stereo_L];
        break;

    case TDA8425_Mode_SpatialStereo:
        for (TDA8425_Index n = 0; n < count; ++n) {
            TDA8425_Float l1 = input[TDA8425_Stereo_L][n];
            TDA8425_Float r1 = input[TDA8425_Stereo_R][n];
            TDA8425_Float c1 = l1 + r1;
            TDA8425_Float l2 = l1 - c1;
            TDA8425_Float r2 = r1 - c1;
            TDA8425_Float k = ((TDA8425_Float)TDA8425_Spatial_Antiphase / 100);
            TDA8425_Float c2 = c1 * k;
            TDA8425_Float l3 = l2 + c2;
            TDA8425_Float r3 = r2 + c2;
            output[TDA8425_Stereo_L][n] = l3;
            output[TDA8425_Stereo_R][n] = r3;
        }
        input[TDA8425_Stereo_L] = output[TDA8425_Stereo_L];
        input[TDA8425_Stereo_R] = output[TDA8425_Stereo_R];
        break;

    default:
        return;
    }

    for (unsigned channel = 0; channel < TDA8425_Stereo_Count; ++channel) {
        TDA8425_Float* incoming = input[channel];
        TDA8425_Float* outgoing = output[channel];
        TDA8425_Float volume = self->volume_[channel];

        for (TDA8425_Index n = 0; n < count; ++n) {
            TDA8425_Float amplified = volume * *incoming++;

            TDA8425_Float bass = TDA8425_BiQuadFloat_ProcessSample(
                &self->bass_model_,
                &self->bass_state_[channel],
                amplified
            );

            TDA8425_Float treble = TDA8425_BiQuadFloat_ProcessSample(
                &self->treble_model_,
                &self->treble_state_[channel],
                bass
            );

            *outgoing++ = treble;
        }
    }
}

// ----------------------------------------------------------------------------

TDA8425_Register TDA8425_ChipFloat_Read(
    TDA8425_ChipFloat const* self,
    TDA8425_Address address
)
{
    switch (address)
    {
    case TDA8425_Reg_VL:
        return self->reg_vl_ | (TDA8425_Register)~TDA8425_Volume_Data_Mask;

    case TDA8425_Reg_VR:
        return self->reg_vr_ | (TDA8425_Register)~TDA8425_Volume_Data_Mask;

    case TDA8425_Reg_BA:
        return self->reg_ba_ | (TDA8425_Register)~TDA8425_Tone_Data_Mask;

    case TDA8425_Reg_TR:
        return self->reg_tr_ | (TDA8425_Register)~TDA8425_Tone_Data_Mask;

    case TDA8425_Reg_SF:
        return self->reg_sf_ | (TDA8425_Register)~TDA8425_Switch_Data_Mask;

    default:
        return (TDA8425_Register)~0;
    }
}

// ----------------------------------------------------------------------------

void TDA8425_ChipFloat_Write(
    TDA8425_ChipFloat* self,
    TDA8425_Address address,
    TDA8425_Register data
)
{
    switch (address)
    {
    case TDA8425_Reg_VL: {
        data |= (TDA8425_Register)~TDA8425_Volume_Data_Mask;
        self->reg_vl_ = data;

        TDA8425_Register index = data & TDA8425_Volume_Data_Mask;
        TDA8425_Float gain = TDA8425_VolumeLinear_Table[index];
        self->volume_[TDA8425_Stereo_L] = gain;
        break;
    }
    case TDA8425_Reg_VR: {
        data |= (TDA8425_Register)~TDA8425_Volume_Data_Mask;
        self->reg_vr_ = data;

        TDA8425_Register index = data & TDA8425_Volume_Data_Mask;
        TDA8425_Float gain = TDA8425_VolumeLinear_Table[index];
        self->volume_[TDA8425_Stereo_R] = gain;
        break;
    }
    case TDA8425_Reg_BA: {
        data |= (TDA8425_Register)~TDA8425_Tone_Data_Mask;
        self->reg_ba_ = data;

        TDA8425_Register index = data & TDA8425_Volume_Data_Mask;
        TDA8425_Float gain = TDA8425_BassLinear_Table[index];

        TDA8425_BiQuadModelFloat_SetupBass(
            &self->bass_model_,
            self->sample_rate_,
            gain
        );
        break;
    }
    case TDA8425_Reg_TR: {
        data |= (TDA8425_Register)~TDA8425_Tone_Data_Mask;
        self->reg_tr_ = data;

        TDA8425_Register index = data & TDA8425_Volume_Data_Mask;
        TDA8425_Float gain = TDA8425_TrebleLinear_Table[index];

        TDA8425_BiQuadModelFloat_SetupTreble(
            &self->treble_model_,
            self->sample_rate_,
            gain
        );
        break;
    }
    case TDA8425_Reg_SF: {
        data |= (TDA8425_Register)~TDA8425_Switch_Data_Mask;
        self->reg_sf_ = data;
        break;
    }
    default:
        break;
    }
}
