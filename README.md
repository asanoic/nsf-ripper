nsf-ripper
==========

NES game sound file (.nsf) music extractor to FLAC format

demo NSF files from [Zophar's music](https://www.zophar.net/music/nintendo-nes-nsf)

some files from [gme project](https://github.com/mcfiredrill/libgme)

build
=====

run
1. `cmake [PATH] -DWITH_OGG=OFF -DCMAKE_BUILD_TYPE=Release`
2. `cmake --build .`

how to use
==========

```
NsfRipper [NSF file]
```

for example

```
NsfRipper "Chip 'n Dale Rescue Rangers.nsf"
```
