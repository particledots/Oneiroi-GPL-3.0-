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
// calibrated centre, passed as arg). Grid (forward and reversed): semitones
// 2^(n/12) from 1x to 2x; below 1x only 1x, then a fifth (n = -7), an octave
// (-12), octave + fifth (-19), 2 octaves (-24), ... down to 1/8 (-36). The zones
// the firmware locks at 0x (under 0.1) and 1x / -1x (0.95 to 1.05) are left alone.
// Hysteresis: the step chosen last is kept until the knob is clearly closer to
// another one (kSpeedHyst semitones past the middle), so a trembling knob or
// noise cannot flip between neighbouring steps.
constexpr float kSpeedHyst = 0.4f;
inline float SpeedKnob(float knob, float zero)
{
    static int lastM = 0;
    static bool lastNeg = false;
    static bool lastValid = false;
    static const int kSteps[19] = {-36, -31, -24, -19, -12, -7, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

    const float v = knob < zero ? -2.f + 2.f * knob / zero
                                : 2.f * (knob - zero) / (kSpeedTop - zero);
    const float a = fabsf(v);
    if (a <= 0.1f || (a >= 0.95f && a <= 1.05f))
    {
        lastValid = false;
        return knob;
    }
    const bool neg = v < 0.f;
    const float x = 12.f * log2f(a); // semitones from 1x
    int best = 0;
    for (int i = 1; i < 19; i++)
        if (fabsf(x - (float)kSteps[i]) < fabsf(x - (float)kSteps[best]))
            best = i;
    int m = kSteps[best];
    if (lastValid && lastNeg == neg && fabsf(x - (float)lastM) <= fabsf(x - (float)m) + kSpeedHyst)
        m = lastM;
    lastM = m;
    lastNeg = neg;
    lastValid = true;
    const float s = copysignf(exp2f((float)m / 12.f), v);
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
