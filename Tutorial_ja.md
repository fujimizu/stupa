_Tutorial of Stupa in Japanese_



# はじめに #

Stupaはある文書に類似する文書群を高速に検索できる、関連文書検索エンジンです。転置インデックスも付属しており、高速に検索を実行することができます。文書データ・転置インデックスはメモリ上で展開して動作しているため、主にリアルタイム性の高いアプリケーションでの利用を想定しています。

また[Thrift](http://incubator.apache.org/thrift/)を使用した簡単なサーバ実装も配布していますので、関連文書検索サーバとして実際に使用することも可能です。

※初期バージョンのリリースでは、検索のアルゴリズムに[Bayesian Sets](http://www.gatsby.ucl.ac.uk/~heller/bsets.pdf)を使用していたのですが、Bayesian Setsは[特許](http://www.wipo.int/pctdb/en/wo.jsp?WO=2007063328)が取得されており、特許の権利者よりBayesian Setsの利用停止の連絡が来たため、現在はIDFベースの検索アルゴリズムに変更しています。

# 更新情報 #

  * 2010/06/39: stupa-0.1.2リリース
    * headerファイルのインストール先を変更しました

  * 2010/04/19: stupa-0.1.1, stupa-thrirft-0.1.1 リリース
    * 検索アルゴリズムの変更、一部の環境でビルドできない問題を解消しました

  * 2010/03/04: 一時的にダウンロードパッケージを削除しています。少々お待ち下さい

  * 2010/02/26: stupa-0.0.3, stupa-thrift-0.0.3 リリース
    * ライセンスの項目に特許について追記しました
    * bug fix

  * 2010/02/09: stupa-0.0.2, stupa-thrift-0.0.2 リリース
    * コマンドラインツールのファイル名を変更しました(bsmgr → stpctl, bsrand → stprand)
    * コマンドラインツールstpctlにオプションを追加
    * C++ライブラリ、ThriftサーバのAPIの一部変更

  * 2010/02/04: stupa-0.0.1, stupa-thrift-0.0.1 リリース

# ライセンス #

GPL2 (Gnu General Public License Version 2)

# ダウンロード #

[最新版のソースコード](http://code.googlecom/p/stupa/downloads/list)をダウンロードしてください。

# インストール #

## Stupa C++ライブラリ ##

最新のソースコードをダウンロードし、以下のように実行してください。make checkを実行する場合は[googletest](http://code.google.com/p/googletest/)が必要となるため、事前にgoogletestをインストールしておいてください。

また[google-sparsehash](http://code.googlecom/p/google-sparsehash/)がインストールされていると、さらに高速に実行できるようになるため、事前にインストールしておくことをお勧めします。

```
% tar xvzf stupa-*.*.*.tar.gz
% cd stupa-*.*.*
% ./configure
% make
% make check (googletestをインストールしておく必要があります)
% sudo make install
```

BSD系のOSを使用している場合は、make の代わりに gmake を使わないとビルドできないかもしれないため、ご注意ください。

## 関連文書検索サーバ ##

最新のソースコードをダウンロードして以下のように実行してください。ただし事前に上記のstupa C++ライブラリと[Thrift](http://incubator.apache.org/thrift/)をインストールしておく必要があります。

```
% tar xvzf stupa-thrift-*.*.*.tar.gz
% cd stupa-thrift-*.*.*
% make
```

# 入力データのフォーマット #

入力データには以下のフォーマットのテキストファイルと、stupaから生成されるバイナリファイルを使用することができます。ここではテキストファイルのフォーマットについてご説明します。バイナリ形式のファイルの作成については、コマンドラインツールの項目でご説明します。

## ドキュメントのリスト ##

関連文書検索の対象となるドキュメントリストのデータは、以下のようにタブ区切りフォーマットのテキストファイルにしてください。1行に1つのドキュメントの情報を記入し、空行は含めないようにしてください。各行は先頭にそのドキュメントのIDを記入し、IDの後はそのドキュメントの特徴を表す特徴語のリストを記入してください。

```
document_id1 \t key1-1 \t key1-2 \t key1-3 \t ...\n
document_id2 \t key2-1 \t key2-2 \t key2-3 \t ...\n
...
```

各フィールドは以下を表しています。

  * _document\_id_: 各ドキュメントのIDで、ドキュメント毎にユニークである必要があります。タブを含まない任意の文字列を使用できます。
  * _key_: ドキュメントの特徴を表す文字列です。タブを含まない任意の文字列が使用できます。

言語処理ではしばしば[TFIDF](http://ja.wikipedia.org/wiki/Tf-idf)などを使用して各特徴のポイントづけを行ったデータを使用しまが、Stupaでは特徴を持つか持たないかの2値しか使用しないため、入力データにも各特徴語に対するポイントは必要なく、特徴語のリストのみを指定します。

ただしより高精度に動作させるためには、入力データ作成の際は事前に特徴的な語を特定しておき、それらのみを使用するようにしてください。また各文書で特徴語の数にあまり差が出ないようにしておくことをお勧めします。

## 入力データの例 ##

特徴としてよく聞く音楽のジャンルを使った入力例を示します。

```
Alex   Pop      R&B     Rock
Bob    Jazz     Reggae
Dave   Classic  World
Ted    Jazz     Metal   Reggae
Fred   Pop      Rock    Hip-hop
Sam    Classic  Rock
```

上記の例では"Alex", "Bob", "Dave"が各ドキュメントのID(人名)を表し、"Alex"の特徴を表すフレーズ(音楽のジャンル)として"POP"、"R&B"、"Rock"が挙げられています。

# コマンドラインツールの使い方 #

stupaに同梱されているコマンドラインツールを使って関連文書検索を実行できます。

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

## インタラクティブに検索 ##

コマンドラインツールstpctlのオプションに **search** を指定すると、インタラクティブに関連文書検索を実行できます。試しにどんな結果が得られるのかを確認するのに使用します。デフォルトでは指定されたファイルをテキスト形式のファイルとして読み込みますが、 **-b** オプションを指定するとバイナリ形式のファイルとして読み込まれます。

```
% stpctl search data/test1.tsv 
Reading input documents (Text, invsize:100) ... 6 documents
Query> 
```

また転置インデックス中の各posting listの最大サイズを指定して実行することもできます。このサイズを小さくすると、検索時間を短縮することができますが、古い文書が検索にヒットしなくなる可能性が高くなります。ただしバイナリ形式の入力ファイルを指定した場合は、サイズ指定は無視されます。

```
% stpctl search data/test1.tsv 3
Reading input documents (Text, invsize:3) ... 6 documents
Query> 
```

### posting listの最大サイズ ###

posting listの最大サイズについてもう少し詳しく説明します。stupaでは検索を高速に行うために、転置インデックスを持っています。

```
key1  =>  document_id1  document_id2  document_id3 ...
key2  =>  document_id4  document_id5  document_id5 ...
...
```

転置インデックスは上記のように、各特徴語に対しその特徴を持つドキュメントIDのリスト(posting list)を保持しています。このposting listの最大数はあらかじめて指定しておくことができます。stupaではposting listの最大数を越えた場合は、最も古く登録された文書をposting listから削除していきます。

### 文書IDで検索 ###

まず指定された入力ファイルからドキュメントデータを読み込み、その後"query>"というプロンプトが出ます。そこにクエリとして適当な文書IDを入力すると、その文書に関連のある文書のIDと関連度が表示されます。 **入力文書集合に含まれない文書IDを指定した場合は、何も結果は返ってこないのでご注意ください。** またクエリはタブ区切りで複数入力することもできます。

同梱されているテストデータで実行してみますと、以下のような結果が得られます。

```
% cat data/test1.tsv
Alex   Pop      R&B     Rock
Bob    Jazz     Reggae
Dave   Classic  World
Ted    Jazz     Metal   Reggae
Fred   Pop      Rock    Hip-hop
Sam    Classic  Rock

% bsmgr search data/test1.tsv
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

### 特徴語で検索 ###

**-f** オプションを指定すると、検索クエリとして各文書が持つ特徴語を指定することができます。

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

通常は文書IDを指定する検索で問題ないのですが、文書IDでの検索ではクエリの文書が事前に登録されている必要があるため、未知の文書をクエリとして使用することができません。そこで未知の文書をクエリとする場合は、その文書の特徴語のリストをクエリとして使用することで検索を実行することができます。

また後述するThriftサーバを使用する際に、文書IDの剰余等でサーバを分散させるときなどにも特徴語での検索が役に立つかもしれません。サーバを分散することによって各サーバで登録されている文書が異なるため、文書IDをクエリにして検索するとその文書IDが登録されていないサーバからは正しい結果を得ることができません。そこで上記と同様に特徴語のリストをクエリとすることで、各サーバから正しい結果を得ることが可能となります。

## TSVファイルをバイナリ形式に変換して保存 ##

テキストファイルの読み込みはテキストのparseや、特徴語の採番など多くの処理が実行されるため時間がかかります。大規模のテキストファイルをコマンドラインツールから頻繁に入力として使用する場合は、下記のように **save** オプションを使用してバイナリ形式に保存して、バイナリのデータを入力として使用することをお勧めします。また後述するThriftを使用したサーバで起動時にデータを渡す場合は、バイナリ形式しか現在はサポートしていないため、テキスト形式の場合は以下のようにしてデータを変換して使用してください。

```
% stpctl save input.tsv output_binary 100
```

**save** オプション以降の引数は、入力テキストファイル名、出力バイナリファイル名、転置インデックスが持つ各posting listの最大サイズです。

# Thriftを使った関連文書検索サーバ #

## サーバの起動 ##

スレッドプールを使用したThreadPoolServerと、libeventを使用してノンブロッキングに動作するNonblockingServerの2つが含まれています。性能評価はまだ行えていないのではっきりとしたことは言えないのですが、NonblockingServerの方が性能がよいように感じます。

### ThreadPoolServerの起動 ###

```
% ./stupa_thread
```

### NonblockingServerの起動 ###

```
% ./stupa_nonblock
```

## 起動オプション ##

ThreadPoolServer、NonblockingServer ともに以下のオプションを指定することができます。

```
 -p port     サーバのポート番号 (デフォルトは9090)
 -d num      ドキュメントの最大登録数 (デフォルトは制限なし)
 -i size     転置インデックスの各posting listの最大サイズ (デフォルトは100)
 -w nworker  workerスレッドの数 (デフォルトは4)
 -f file     起動時に読み込むデータファイル (バイナリフォーマットのみ)
 -h          ヘルプメッセージを表示
```

**-d** オプションを指定して起動した場合、最大登録数を超えて文書が追加されると、最も古く登録された文書から順に削除されていきます。

## 登録済みデータの保存 ##

Stupaは登録された文書データ・転置インデックス等をメモリ上で保持しているため、サーバを停止すると登録データはすべてクリアされてしまいます。動作中の状態を保存したい場合は、クライアントからsaveメソッドを呼び出して、ファイルに保存するようにしてください。

保存されたデータは、サーバの起動時に読み込ませたり、動作中にクライアントからloadメソッドで読み込むことができます（ただしloadメソッドを使用すると、それ以前の状態は全てクリアされますのでご注意ください）。またコマンドラインツールで使用することも可能です。

## Perlクライアント ##

stupa-thrift/perl 以下がPerlクライアントのコードです。lib以下はThriftによって自動生成されたものです。client\_sample.plがサンプルコードになりますので、Perlクライアントを作成する際はこちらを参考にしてください。

### ThreadPoolServerを起動している場合 ###
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

### NonblockingServerを起動している場合 ###
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

## 他言語のクライアントを作成する場合 ##

Thriftの定義ファイルが"stupa.thrift"という名前で同梱されていますので、こちらを使って生成してください。詳しくは[Thrift](http://incubator.apache.org/thrift/)のドキュメントをご参照下さい。

```
# pythonのクライアントを生成する場合

% thrift --gen py stupa.thrift
% cd gen-py
% ...
```