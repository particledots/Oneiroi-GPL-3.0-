// ONEIROI-PERSONAL: added file, not part of Befaco's Oneiroi.
// Pitch snap grids for four knobs. Ported from AetherSnap.h of particledots/AetherUmbra
// (same author, GPL-3.0), with the text read-outs removed and float-only math.
// Each function takes a knob value 0..1 and returns the knob value of the nearest step.
// The second argument is only used where noted; the others ignore it.
#pragma once

#include <math.h>

namespace snap
{
constexpr float kSpeedTop = 0.99f;  // Commons.h CenterMap: the top end of the Looper Speed knob
constexpr float kLn2200 = 7.696213f; // ln (22000 / 10), Filter.h cutoff range

inline float Clamp01(float v) { return fminf(1.f, fmaxf(0.f, v)); }

// Oscillator Pitch: the firmware uses 12 * (value - 0.5) semitones, so whole
// semitones are 13 positions from -6 to +6. (arg unused)
inline float PitchKnob(float v, float)
{
    return Clamp01(0.5f + roundf((v - 0.5f) * 12.f) / 12.f);
}

// Looper Speed: CenterMap turns the knob into -2..+2 with 0 at speedZero (the
// calibrated centre, passed as arg). Looper.h then ADDS a small amount to that
// value (Modulate(): CV amount * CV value; with nothing plugged in the CV value
// is forced to kCvMinThreshold). speedOffset is that amount, set by Ui.h before
// the call, so the speed the looper really plays lands on the grid.
// Grid (forward and reversed): semitones 2^(n/12) from 1x to 2x; below 1x only
// 1x, then a fifth (n = -7), an octave (-12), octave + fifth (-19), 2 octaves
// (-24), ... down to 1/8 (-36). The zones the firmware locks at 0x (under 0.1)
// and 1x / -1x (0.95 to 1.05) are left alone.
static float speedOffset = 0.f;
inline float SpeedKnob(float knob, float zero)
{
    const float v0 = knob < zero ? -2.f + 2.f * knob / zero
                                 : 2.f * (knob - zero) / (kSpeedTop - zero);
    const float v = v0 + speedOffset; // the speed Looper.h will see
    const float a = fabsf(v);
    if (a <= 0.1f || (a >= 0.95f && a <= 1.05f))
        return knob;
    int n;
    if (a > 1.f)
    {
        n = (int)lroundf(12.f * log2f(a));
        if (n > 12)
            n = 12;
    }
    else
    {
        static const int kDown[7] = {0, 7, 12, 19, 24, 31, 36};
        const float x = -12.f * log2f(a); // semitones below 1x, > 0
        int best = 0;
        for (int i = 1; i < 7; i++)
            if (fabsf(x - (float)kDown[i]) < fabsf(x - (float)kDown[best]))
                best = i;
        n = -kDown[best];
    }
    const float s = copysignf(exp2f((float)n / 12.f), v) - speedOffset; // wanted knob speed
    const float k = s < 0.f ? (s + 2.f) * zero / 2.f
                            : zero + s * (kSpeedTop - zero) / 2.f;
    return Clamp01(k);
}

// Resonator Tune: the main pole moves from -24 semitones to +range (24 at
// Dissonance 0, 16 at full, an integer as in Resonator.h). Whole semitones.
// arg = the Dissonance value (0..1).
inline int TuneRange(float dissonance) { return (int)(24.f - 8.f * Clamp01(dissonance)); }
inline float TuneKnob(float tune, float dissonance)
{
    const int range = TuneRange(dissonance);
    int semis = (int)lroundf(-24.f + tune * (float)(range + 24));
    if (semis > range)
        semis = range;
    if (semis < -24)
        semis = -24;
    return (float)(semis + 24) / (float)(range + 24);
}

// Filter Cutoff: 10 Hz * 2200^value (10 Hz to 22 kHz, Filter.h). Notes of the
// equal-tempered scale, A = 440 Hz. The two ends stay where they are. (arg unused)
inline float CutoffKnob(float v, float)
{
    if (v <= 0.005f || v >= 0.995f)
        return v;
    const float hz = 10.f * expf(v * kLn2200);
    const int note = (int)lroundf(12.f * log2f(hz / 440.f));
    const float snapped = 440.f * exp2f((float)note / 12.f);
    return Clamp01(logf(snapped / 10.f) / kLn2200);
}
} // namespace snap
