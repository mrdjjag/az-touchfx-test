# Touch FX for XDJ-AZ

By mrdjjag. The current review version is **0.8.0**.

[Download the complete 0.8.0 source archive](Touch-FX-0.8.0-public-source-draft.zip).
The archive contains the panel, custom core and audio source, build instructions, installation/restore guides, licences and verification results. Extract it to see the full directory structure. The previous 0.1.0 display prototype is superseded.

## Features

- Touch-controlled STEMS and Drum Roll effects.
- Adaptive 2, 2/4 and 4-deck layouts, with transparent/colour/no-colour options.
- Separate panel memory for each deck and automatic mixer following.
- STEMS timing: 4, 2, 1, 3/4, 1/2, 1/4, 1/8, 1/16 beats, following live BPM.
- EQ/pad-linked STEMS dimming and optional native STEMS touchscreen controls.
- Customisable pointer colours/trails and optional sampler bank shortcuts.

## Requirements and status

**STEMS Touch FX requires tracks separated using Kyle's AZ-MODS Mod Manager, with STEMS enabled on the selected deck.** Drum Roll uses its own samples.

**This version requires matching customised azmod.so and azstem.so libraries.**
It is not yet a standalone one-click Community Mod. The source archive explains the manual developer installation and its requirements. No custom core binary, firmware images or database key bytes are included in this repository.

Tested on the author's XDJ-AZ with firmware tag ep147-9e9454e357389ece. All three binaries were reproduced byte-for-byte from the cleaned source using the tested local toolchain. Other firmware/core versions and installation on a fresh USB have not been validated. See VERIFICATION.json inside the archive. The source has been submitted for review; upstream acceptance/signing is not confirmed.

## Credits

Built on Kyle Hosman's AZ-MODS and SDK. GPL-3.0-only; see LICENSE and NOTICE. Third-party notices and source licences are included in the archive. Independent project, not affiliated with AlphaTheta or Pioneer DJ.
