# Changes to the original Befaco Oneiroi source

Unofficial personal build, not affiliated with Befaco or Rebel Technology.
Source: https://github.com/Befaco/Oneiroi (release 1.2.2, commit e048098), GPL-3.0.

## Source files changed
None yet. This is the stock 1.2.2 build (step 2). Any later change will be
listed here, file by file, and marked in the code with `// ONEIROI-PERSONAL`.

## Files added (not part of the original)
- `ONEIROI-CHANGES.md` (this file)
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
Verified: the source compiles and links (local test, patch.bin 102,596 bytes).
Not verified: that this build is byte-identical to Befaco's official patch.syx
(theirs shows size 101620 on the module; a different compiler version is the
likely reason). Not yet tested on the module.
