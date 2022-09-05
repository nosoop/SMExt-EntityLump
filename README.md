# Entity Lump Manager 

> &#8505;&#65039; This repository is no longer active.  This extension has been merged into
> SourceMod as [alliedmodders/sourcemod#1673][pr_1673].  It should also be available in
> SourceMod 1.11 in the future.
> 
> If you were a developer using this extension, you must recompile plugins to use the version
> in SourceMod; there is no other migration path, and the extension cannot be used alongside
> the implementation in SourceMod.

[pr_1673]: https://github.com/alliedmodders/sourcemod/pull/1673

A SourceMod extension that parses out the level entity string into a format that can be easily
manipulated by plugins.

## Overview

The API has two methodmaps; `EntityLump` and `EntityLumpEntry`.

An `EntityLumpEntry` is an ordered list of key / value pairs, behaving similar to an
`ArrayList`.  Duplicate keys are allowed.

`EntityLump` is similar to a global `ArrayList` that holds `EntityLumpEntry` instances.

## Standalone binary

Compiling the extension also produces the `entlump_parser` executable, which is a small
command-line tool intended to test the parsing logic without needing a map or Source dedicated
server running.

The tool can be used as follows:

    ./entlump_parser ${file}

The tool parses the entity lump from the input file, then outputs a reconstructed one on
standard output.  If parsing is correct, then the strings should look identical aside from
whitespace outside of keys / values.

A few test cases are provided in `standalone-parser/test_files`.
