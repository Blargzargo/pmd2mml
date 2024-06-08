# PMD2MML

This is a fork from an old build of libopenpmd by OPNA2608. The purpose of this software is to decompile PMD files directly to MML, with minimal post-processing needed.

This program was tested using MC.EXE version 4.8s, and will generate MML code compatible with that version. All commands are supported, and should generate code that is byte-perfect for 99% of songs.

I started programming less than a month ago, so apologies for the messy code!

## Features

99% byte-perfect functionality with OPN

Works with nearly every OPN song, even ones created using earlier versions of MC.EXE

Automatically adds length commands for better readability

Automatically adds octave commands and octave signs (> and <)

Prints #DT2Flag On if DT2 is used.

Prints #Option flags (/v and /c are automatically added, /n is omitted)

Can bulk-convert songs, so feel free to drag-and-drop an entire OST.

## Bugs

Might work well with OPM (X68000)

Instruments won't work with OPL

Will NOT work with FM-TOWNS

Ties get regrouped when re-compiled (not much can be done about that, it's a compiler quirk)

Extra Zenlen command might get spit out on the G channel when re-compiling


## Credits

- [@OPNA2608], for writing the code for libopenpmd
- [@kajaponn], for writing the original PMD driver and all of the original tools.
- [@ValleyBell], for their [documentation of the PMD format].
- [@AnhanLi], for inspiring me with their [format analysis earlier this year] to pursue this project.
- [@NoyemiK], for her .M(2) file that I may include as an example.
- [@Blargzargo], for his independent [documentation of PMD and MML].
- [@Ravancloak], for her example P86 bank.

[@OPNA2608]: https://github.com/OPNA2608
[@kajaponn]: https://twitter.com/kajaponn
[@ValleyBell]: https://github.com/ValleyBell
[@AnhanLi]: https://twitter.com/AnhanLi
[@NoyemiK]: https://github.com/NoyemiK
[@Blargzargo]: https://www.youtube.com/channel/UCDZR3q3anQ9boE6IAvorz8Q
[@Ravancloak]: https://ravancloak.bandcamp.com/

[documentation of the PMD format]: https://raw.githubusercontent.com/ValleyBell/MidiConverters/master/pmd_SeqFormat.txt
[format analysis earlier this year]: https://lithcore.cn/2318
[documentation of PMD and MML]: https://pastebin.com/raw/FP5q8zgC
