# Touch FX Display Test 0.1.0

Författare: mrdjjag. Fristående, begränsad SDK-prototyp, inte fullständiga v51.

## Detta fungerar i prototypen

- Egen displaypanel som öppnas och stängs med `Panel test` i deckvyn.
- Manuellt kanalval, sparat kanalval och visuellt padkontextminne per kanal.
- Byte av låt eller padsida på en annan kanal byter inte vald kanal.
- Valfri följning av mixerreglar/crossfader, också när panelen är stängd.
- Nedtoning av Vocals/Instrument-indikatorerna är på som standard och kan stängas av.
- Inga extraljud och inga Sweep-/Crash-boxar, i något deckläge.

## Begränsningar

Detta är ett DISPLAYTEST: inga Touch FX-ljudeffekter, ingen sampler, inga
muteknappar och ingen styrning av enskilda STEMS-delar. Vocals/Instrument
är enbart indikatorer och båda tonas ned när vald kanals STEMS-motor är av.
Det är inte samma återkoppling som v51:s individuella STEMS-nedtoning.
Ingen officiell vågform eller informationsruta patchas. Panelens placering är
provisorisk för XDJ-AZ:s 1280x800-skärm och behöver granskas på riktig spelare
i både 2- och 4-deckläge. Nuvarande SDK-tester ersätter inte fysisk testning.

Mixerföljning är AV från början. För att prova: ställ de fyra `CH n crossfader`
valen till exakt samma A/THRU/B-tilldelningar som i spelarens Utility och slå
på `Follow mixer (test)`. Unknown på något val blockerar automatiken. Den
följer en enda tillåten, öppen kanal efter minst 80 ms; vid flera öppna kanaler
behålls senaste valet. Det är inte detektering av hörbart ljud eller PLAY-status.

## Säker lokal installation när paketet är färdigt

Behåll en backup av v51. Installera officiell AZ-MODS och starta om först.
Kopiera det färdigvaliderade .azmod-paketet till USB enligt SDK-guiden och
godkänn det under Community Mods. Skriv inte över azmod.so eller azstem.so.
Det här projektet har INTE installerats på spelaren av byggprocessen.

## Bygga och paketera

SDK-commit: 3dcaf8f7331ea94135d7161f653edfaf906d5ea6.

1. Publicera denna källkod i ditt eget repo.
2. Sätt `source` i azmod.json till repots riktiga HTTPS-adress.
3. Kör SDK:ts `tools/azmod-pack build .`.
4. Kör `tools/azmod-pack validate .`.
5. Kör `tools/azmod-pack package .` för ett lokalt testpaket.

Ingen publik release eller Mod Index-ansökan är gjord. `source` lämnas tomt
tills ditt repo finns. Då går metadata inte igenom validatorn ännu, avsiktligt:
vi anger inte någon annans repo som källa för vår kod.

Källkoden är GPL-3.0-only, se LICENSE. SDK från Kyle-Hosman. Mixeravkodning
och följarregler bygger på GPL-koden i Kyle-Hosman/xdj-az-mods och våra
tidigare anpassningar. Inga firmwarebilder, fonter eller hemliga nycklar medföljer.
