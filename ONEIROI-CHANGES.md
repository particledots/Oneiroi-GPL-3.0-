# Changes to the original Befaco Oneiroi source

Unofficial personal build, not affiliated with Befaco or Rebel Technology.
Source: https://github.com/Befaco/Oneiroi (release 1.2.2, commit e048098), GPL-3.0.

## Source files changed
Every change is marked in the code with `// ONEIROI-PERSONAL`.
- `ParamController.h`: new method `KnobController::Snap()` (after `Read()`). It
  snaps the smoothed knob value to a grid, only for the knob's main function
  (not its Shift, Mod or CV function).
- `Ui.h`: includes `Snap.h`; two new flags (`snapOn_`, `snapRandomWasPressed_`);
  in `Poll()`, after the knobs are read, four knobs are snapped when snap is on
  (Oscillator Pitch, Looper Speed, Resonator Tune, Filter Cutoff); in
  `HandleLedButtons()`, MOD/CV mode + a tap of RANDOM switches snap on or off,
  and RANDOM blinks twice when snap turns on. Snap is always off at power-up.

## Files added (not part of the original)
- `Snap.h` : the snap grids, ported from `src/engine/AetherSnap.h` of
  https://github.com/particledots/AetherUmbra (same author, GPL-3.0), text
  read-outs removed. Checked on a PC against AetherSnap.h at 100,001 knob
  positions (no differences).
- `ONEIROI-CHANGES.md` (this file)
- `README.md` and `CHANGELOG`: a marked "ONEIROI-PERSONAL" section appended at
  the end of each (test results only; the original text is untouched)
- `.github/workflows/build.yml` : builds patch.syx on GitHub Actions
- `build-support/OpenWareMidiControl.h` : copy of the header from
  https://github.com/pingdynasty/OpenWare, branch `mk/oneiroi`, commit
  8a6d691fefe40cc38e98226a37650806ac465c15 (GPL-3.0). The Oneiroi code needs
  BUTTON_9 to BUTTON_11, which the current OwlProgram header does not define.
  The build copies it into OwlProgram's LibSource folder.

## How the build is made
- OwlProgram (https://github.com/pingdynasty/OwlProgram) commit
  9706d5d9c7bb802ae52d3b6b92a4f0d795aa5c9f, with the header above swapped in.
- Arm GNU toolchain 13.3.Rel1, `PLATFORM=OWL3`, `CONFIG=Release`,
  patch name `Oneiroi_1_2_2`.
- FirmwareSender (https://github.com/pingdynasty/FirmwareSender) commit
  12300e4ef02370395d00e2de2b95f70b2ba3fe0f makes patch.syx.

## What is verified and what is not
Verified on the module (step 2, 12 Nov 2026, RAM test with Run only, nothing
stored to slot 1): the stock build loads (Preset 0 Oneiroi_1_2_2 [102668]),
CPU 84% (official 83%), memory unchanged. Looper recording from the Crave and
from the oscillators with the resample switch flipped both work; loop length
is correct and the sound is clear.
Not verified: that this build is byte-identical to Befaco's official
patch.syx (official: 121,008 bytes, module shows size 101620; this build:
122,255 bytes, shows 102668; a different compiler version is the likely
reason). Not stored to slot 1.

## Step 3 (pitch snap), 12 Nov 2026
Verified on the module (RAM test with Run only, nothing stored to slot 1):
the four knobs snap as described above, Shift functions and the other knobs
are unchanged, the switch gesture works, and after Random at maximum amount
the knobs take over when turned and the arrow LEDs do not blink continuously.
The RANDOM LED blinks twice both when snap turns on and when it turns off
(the code was written to blink twice on and show the stock single flash off;
this difference is cosmetic and not changed).
Also verified (owner, same session): undo / redo with snap on works as normal;
with snap off it sounds identical to stock (by ear, not bit for bit); the module
page shows Preset 0 Oneiroi_1_2_2 [102612], CPU 83%, memory 7319080 (unchanged
from stock).
Not verified: that snap off is bit-for-bit identical to stock. The built patch.bin is 102,612 bytes, 56 bytes smaller than the stock
build (102,668); the reason is not known.
