# Entity Lump Manager 

> :warning: This is (currently) not intended for production use.  While it works pretty well in
> my tests, the intent is hopefully to upstream this to SourceMod, per the following issues:
> 
> - [alliedmodders/sourcemod#1534][issue_1534]
> - [alliedmodders/sourcemod#1547][issue_1547]
> 
> If that plan doesn't work out, I'd be more than happy to maintain this extension myself (incl.
> providing builds), but since there may be breakages in the hypothetical merge into SourceMod
> itself, builds are currently do-it-yourself so I know you're capable of handling any changes
> that may happen in the future.

[issue_1534]: https://github.com/alliedmodders/sourcemod/issues/1534
[issue_1547]: https://github.com/alliedmodders/sourcemod/issues/1547

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
