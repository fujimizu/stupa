[Tutorial in Japanese](Tutorial (Japanese))

[Tutorial in English](Tutorial (English))

## Overview ##

**Stupa** is an associative search engine. You can search related documents with high performance and high precision. Since document data and inverted indexes are kept in memory, stupa reflects updates of documents in search results in real time.

A server implementation of Stupa is possible by using Thrift.

## Install ##

```
% ./configure
% make
% make check  (googletest required)
% sudo make install
```

## Usage ##

### Search related documents interactively ###
```
% stpctl search [-b][-f] file [invsize]
```

### Convert an input tsv file to a binary format file ###
```
% stpctl save [-b] infile outfile [invsize]
```

### Options ###
```
 -b        read a binary format file
 -f        search by feature strings
           (default: search by document identifier strings)
 invsize   maximum size of inverted indexes (default:100)
```

## Format of Input Data ##

### List of input documents ###
```
document_id1 \t key1-1 \t key1-2 \t key1-3 \t ...\n
document_id2 \t key2-1 \t key2-2 \t key2-3 \t ...\n
...
```
  * document\_id : string
  * key         : string

## Requirement ##
  * C++ compiler with STL (Standard Template Library)

### Recommended ###
  * [google-sparsehash](http://code.google.com/p/google-sparsehash/)
    * If google-sparsehash not installed, this clustering tool uses "gnu\_cxx::hash\_map" or "std::map"

## License ##
GPL2 (Gnu General Public License Version 2)

## Author ##
Mizuki Fujisawa <[fujisawa@bayon.cc](mailto:fujisawa@bayon.cc)>
