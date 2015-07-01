_Tutorial of Stupa in English_



# Introduction #

Stupa is an associative search engine. You can search related documents with high performance and high precision. Since document data and inverted indexes are kept in memory, stupa reflects updates of documents in search results in real time.

A server implementation of Stupa is possible by using [Thrift](http://incubator.apache.org/thrift/).

# What's new #

  * 2010/06/30: stupa-0.1.2 Released
    * modified the install path of header files

  * 2010/04/19: stupa-0.1.1, stupa-thrift-0.1.1 Released

# License #

GPL2 (Gnu General Public License Version 2)

# Download #

Please download [the latest version](http://code.google.com/p/stupa/downloads/list).

# Installation #

## C++ library of Stupa ##

Download the latest source package, and type the following.

I recommend that you should install [google-sparsehash](http://code.google.com/p/google-sparsehash/) in advance to run stupa faster.

```
% tar xvzf stupa-*.*.*.tar.gz
% cd stupa-*.*.*
% ./configure
% make
% make check
% sudo make install
```

## Server implementation using Thrift ##

Download the latest source package, and type the following. You must install C++ library of Stupa and [Thrift](http://incubator.apache.org/thrift/) in advance.

```
% tar xvzf stupa-thrift-*.*.*.tar.gz
% cd stupa-thrift-*.*.*
% make
```

# Format of input data #

Stupa can read two formats of input data, tab-separated text format files and binary format files. In this section I show the details about text format, and binary format is described in command-line tools section.

## List of input documents ##

The file of the list of input documents needs to be in a tab-separated text format as bellow. Each line must contain the information about only one document, the identifier of a document followed by the keywords in the document. Blank lines must not be inserted.

```
document_id1 \t key1-1 \t key1-2 \t key1-3 \t ...\n
document_id2 \t key2-1 \t key2-2 \t key2-3 \t ...\n
...
```

  * _document\_id_: This is the identifier of a document, and must be the unique string which contains no tab characters.
  * _key_: This is the keyword in a document, and must be the string which contains no tab characters.

_key_ needs to reflect the degree of the feature appropriately. So, you should apply weighting scheme (e.g. [tf-idf](http://en.wikipedia.org/wiki/TF_IDF)) to input data, and pick up the characteristic keywords in each document. And the number of the keywords of each document should be nearly equal.

## Examples of input data ##

```
Alex   Pop      R&B      Rock
Bob    Jazz     Reggae
Dave   Classic  World    
Ted    Jazz     Metal    Reggae
Fred   Pop      Rock     Hip-hop
Sam    Classic  Rock
```

In the above example, 'Alex', 'Bob', 'Ted' are the identifiers of the documents, and 'Alex' has the features as the genres of music (Pop, R&B, Rock) which he often listens.

# Usage of command line tools #

You can search related documents interactively, and convert tab-separated text format files to binary format files.

```
stpctl: Stupa Search utility

Usage:
 % stpctl search [-b][-f] file [invsize]
 % stpctl save [-b] infile outfile [invsize]
    -b        read binary format file
    -f        search by feature strings
              (default: search by document identifier strings)
    invsize   maximum size of inverted indexes (default:100)
```

## Search related documents interactively ##

You can search related documents interactively when you specify **search** option. By default Stupa reads input data as a text format file. When you specify **-b** option, Stupa reads input data as a binary format file.

```
% stpctl search data/test1.tsv 
Reading input documents (Text, invsize:100) ... 6 documents
Query> 
```

You can specify the maximum size of each posting list of inverted indexes. If you set a small value, the search time might be shortened, but old documents might rarely be in the search results.

```
% stpctl search data/test1.tsv 3
Reading input documents (Text, invsize:3) ... 6 documents
Query> 
```

### Maximum size of posting lists ###

I show the maximum size of posting lists of inverted indexes in detail. Stupa keeps inverted indexes to search related documents quickly. For each keyword, inverted indexes keep a posing list which contains the list of document identifiers whose document contains the keyword.

```
key1  =>  document_id1  document_id2  document_id3 ...
key2  =>  document_id4  document_id5  document_id5 ...
...
```

You can specify the maximum size of posting lists. When documents are added beyond the maximum size of posing lists, Stupa removes the oldest documents.

### Search by document identifiers ###

By default Stupa searches related documents by documents identifiers.

When you specify a document identifier after "Query>" prompt, pairs of document identifiers which are related to query documents, and points are displayed. The maximum number of search results is 20. And you can specify multiple document identifiers using tab delimiters.

```
% cat data/test1.tsv
Alex   Pop      R&B     Rock
Bob    Jazz     Reggae
Dave   Classic  World
Ted    Jazz     Metal   Reggae
Fred   Pop      Rock    Hip-hop
Sam    Classic  Rock

% stpctl search data/test1.tsv
Read input documents ...
Query> Alex
Alex    2.995732
Fred    1.609438
Sam     0.693147
(search time: 0.03ms)

Query> Dave
Dave    2.302585
Sam     0.916291
(search time: 0.02ms)
```

If you specify the document identifiers which are not included in input data, no search results are displayed.

### Search by feature identifiers ###

With **-f** option, you can search related documents by keywords in input documents.

```
% stpctl -f search data/test1.tsv 
Reading input documents (Text, invsize:100) ... 6 documents
Query> Rock
Sam     0.693147
Fred    0.693147
Alex    0.693147
(search time: 0.06ms)

Query> Classic  Rock
Sam     1.609438
Dave    0.916291
Fred    0.693147
Alex    0.693147
(search time: 0.05ms)
```

When you search related documents by document identifiers, query documents must be added to Stupa preliminarily. If you want to search using documents which aren't added to Stupa yet, you can search related documents using the keywords in input documents as a query.

## Save input documents to a binary format file ##

Reading text format files needs some processes such as parsing text format file, adding identifiers to inverted indexes, and so on. So you should use binary format files if you often use same large scale files as input. To use binary format files, convert text format files into binary format files using **save** option. The arguments of **save** option are the path of input file, the path of output file, the maximum size of posting lists of inverted indexes.

```
% stpctl save input.tsv output_binary 100
```

A server implementation of Stupa reads only binary format files.

# Server implementation using Thrift #

A server implementation of Stupa using Thrift contains **ThreadPoolServer** using a thread pool schema, and NonblockingServer using non-blocking I/O schema. **NonblockingServer** may be more high performance.

## Start up server ##

### Start up ThreadPoolServer ###

```
% ./stupa_thread
```

### Start up NonblockingServer ###

```
% ./stupa_nonblock
```

## Start-up options ##

You can specify the following options on both ThreadPoolServer and NonblockingServer.

```
-p port     port number (default:9090)
-d num      maximum number of keeping documents (default: no limit)
-i size     maximum size of inverted indexes (default:100)
-w nworker  number of worker threads (default:4)
-f file     load a file (binary format file only)
-h          show help message
```

## Save status ##

Stupa keeps document data and inverted indexes in memory. Therefore, their data will be cleared when Stupa server is stopped. Calling 'save' method from clients, the status is saved to a binary format file. Saved data can be used as input of Stupa server start-up, the command-line search tool, and 'load' method.

## Perl client ##

### ThreadPoolServer ###

```
% ./client_sample.pl --buffered
Add: Alex       Pop, R&B, Rock
Add: Bob        Jazz, Reggae
Add: Dave       Classic, World
Add: Ted        Jazz, Metal, Reggae
Add: Fred       Pop, Rock, Hip-hop
Add: Sam        Classic, Rock
Size: 6
Search Result (by document ids):
 1      Fred    2.995732
 2      Alex    1.609438
 3      Sam     0.693147
Search Result (by feature ids):
 1      Fred    1.609438
 2      Alex    1.609438
 3      Sam     0.693147
```

### NonblockingServer ###

```
% ./client_sample.pl --framed
Add: Alex       Pop, R&B, Rock
Add: Bob        Jazz, Reggae
Add: Dave       Classic, World
Add: Ted        Jazz, Metal, Reggae
Add: Fred       Pop, Rock, Hip-hop
Add: Sam        Classic, Rock
Size: 6
Search Result (by document ids):
 1      Fred    2.995732
 2      Alex    1.609438
 3      Sam     0.693147
Search Result (by feature ids):
 1      Fred    1.609438
 2      Alex    1.9438
 3      Sam     0.693147
```

## Make client libraries in other programming languages ##

The definition of Thrift interfaces of Stupa is included in "[stupa.thrift](http://code.google.com/p/stupa/source/browse/trunk/stupa-thrift/stupa.thrift)" file. You can make client libraries in other programming languages using this definition file.

For details, see [Thrift](http://incubator.apache.org/thrift/) documents.