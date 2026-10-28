# Touch FX for XDJ-AZ

By mrdjjag. **Current review version: 0.8.2, adapted to AZ-MODS 1.2.7.**

[Download the complete 0.8.2 source archive](Touch-FX-0.8.2-public-source-draft.zip). It contains the panel, custom core/audio source, build and restore instructions, licences and verification notes. Earlier archives remain historical versions.

## Features

- Touch-controlled STEMS and Drum Roll effects, LATCH and per-deck panel memory.
- Automatic channel/panel following with mixer faders and crossfader.
- Adaptive 2, 2/4 and 4-deck layouts; colour, no colour or transparent backgrounds.
- STEMS beat choices: 4, 2, 1, 3/4, 1/2, 1/4, 1/8, 1/16, following live BPM.
- EQ/pad-linked dimming, optional touch control of Kyle's native STEMS boxes and sampler bank shortcuts.
- Pointer style and colours remain selectable. Opening button is now always small beside A.CUE; trail size is fixed to Full. The module registers 16 settings.
- Upstream 1.2.7 STEMS pad FX and Drum Roll HOLD code retained; empty-deck master beat sync also works through the sampler Touch FX render path.

## Requirements and status

**STEMS Touch FX requires tracks separated using Kyle's AZ-MODS Mod Manager, with STEMS enabled on the selected deck.** Drum Roll uses its own samples.

**Matching customised azmod.so AND azstem.so libraries are required. This is not a standalone one-click Community Mod.**

The author reports this version working on XDJ-AZ firmware 2.01. Local panel/transparency tests, 96 sampler checks, 40 upstream STEMS FX checks and the upstream HOLD suite passed. Live combined-effect testing is limited; other firmware versions and a fresh USB installation have not been verified. This is an independent custom build, not an official or signed AZ-MODS release.

No custom core binaries, firmware images, database key bytes or private settings are included. See BUILD.md, INSTALL.md, RESTORE.md, VALIDATION.md and VERIFICATION.json inside the archive. The source is shared for review and feedback; upstream acceptance is not confirmed.

## Credits

Built on Kyle Hosman's AZ-MODS and SDK. GPL-3.0-only; see LICENSE and NOTICE. Not affiliated with AlphaTheta or Pioneer DJ.
