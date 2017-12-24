# Python C Extension の練習
[Python インタプリタの拡張と埋め込み](https://docs.python.jp/3/extending/index.html) や [Python/C API リファレンスマニュアル](https://docs.python.jp/3/c-api/index.html) は良く出来た文章だけれども。いざ使おうとすると自分の未熟さゆえ何がどこにあるか探しなおすことに。

なので複数回世話になった部分についてリンクをはったり動きが確かめられるコードをかいてみたりすることにした。

Python 3.6.3 にて動作させている。たぶん 3.5 でも動く。

## [empty](empty/spam.c)
空のモジュールの作成方法。一段階初期化。

まず [PyModuleDef](https://docs.python.jp/3/c-api/module.html#c.PyModuleDef) がいる。これを [PyModule_Create](https://docs.python.jp/3/c-api/module.html#c.PyModule_Create) に渡すことでモジュールオブジェクトができる。モジュールオブジェクトに何かオブジェクトを追加するには [PyModule_AddObject](https://docs.python.jp/3/c-api/module.html#c.PyModule_AddObject) 。


## [multiphasemoduleinit](multiphasemoduleinit/spam.c)
モジュールの作成方法。多段階初期化。

Python 3.5 より [PEP 489](https://www.python.org/dev/peps/pep-0451/) にある [PyModuleDef_Init](https://docs.python.jp/3/c-api/module.html#c.PyModuleDef_Init) によるモジュール作成が可能になった。一段階初期化では PyInit_ で直接行っていたモジュールオブジェクトの操作は Py_mod_exec に登録した関数で行う。 `__init__` を書く感覚と変わらない書き心地。  3.4 のころより多段階初期化の手間が軽減されている気がする。

モジュールオブジェクトの作り方にまで手を出すときは Py_mod_create に関数を登録。こちらは `__new__` みたいな感じ。

各モジュールオブジェクトごとに固有のメモリを割り当てることができる。 m_size にて大きさを指定。不要なら 0　。 -1 は特殊で再初期化不可であることをインタプリタに伝える、静的な領域を使っているときなどに -1 設定する。 [PEP 3121](https://www.python.org/dev/peps/pep-3121/) に 1 以上を設定し [PyModule_GetState](https://docs.python.jp/3/c-api/module.html#c.PyModule_GetState) を使う例がある。

[xxmodule.c](https://github.com/python/cpython/blob/v3.6.3/Modules/xxmodule.c) というテンプレートが CPython のソースには含まれている。ある程度わかっていれば、これから作ろうとしているものに対しどこが不要か選べるならば便利なのだろうと思う。はやくそうなりたいものだ。新しい型の作り方から始まっている。

## [pymethoddef](pymethoddef/spam.c)
モジュール関数の作成方法。

[PyCFunction](https://docs.python.jp/3/c-api/structures.html?highlight=pycfunction#c.PyCFunction) 、つまりひとつ目の引数に モジュールオブジェクト、ふたつ目の引数に引数を表す tuple オブジェクトを受け取る関数を作ればよい。作ったメソッドをモジュールやクラスに紐付けるためには [PyMethodDef](https://docs.python.jp/3/c-api/structures.html#c.PyMethodDef) を作る。呼び出し規約 (calling convention) ml_flags を設定することでキーワード引数を使えるようにしたり、引数を受け取らない、引数をひとつだけ受け取ることを示す。オーバーヘッドを減らすためにあるのか？ いや [PyCFunction_Call](https://github.com/python/cpython/blob/v3.6.3/Objects/methodobject.c#L81) などを見るに受け取り側の手間を省くのが目的で処理量は変わらない、か。

[呼び出し規約 METH_FASTCALL が 3.6 で追加](http://dsas.blog.klab.org/archives/2017-01/python-dev-201701.html) とのこと。タプルや辞書への詰め直しが不要で早い。 [たしかにあった](https://github.com/python/cpython/blob/v3.6.3/Include/methodobject.h#L89) がドキュメント化されてはいないのでまだサードパーティーが使うものではないというところだろうか。

## [parseargs](parseargs/spam.c)
PyCFunction, PyCFunctionWithKeywords は通常 Python の tuple と dict オブジェクトを渡される。これを C の変数に変換するのはよくある作業。なのでそれ用の関数が用意されている。また、戻り値は Python オブジェクトと決まっているので C の変数から変換するのもよくある作業。これも。

* [関数、メソッドの引数の解釈](https://docs.python.jp/3/c-api/arg.html#parsing-arguments)
  * PyArgs_ParseTuple
  * PyArgs_ParseTupleAndKeywords
* [値の構築](https://docs.python.jp/3/c-api/arg.html#building-values)
  * Py_BuildValue

## [abstractprotocol](abstractprotocol/spam.c)
Python だと `a.b` とか `a()` とか `a[0]` とか `for b in a:` などと書くいつもの操作、 C API ではどうやるの？ それにこたえてくれる文章が[抽象オブジェクトレイヤ (abstract objects layer)](https://docs.python.jp/3/c-api/abstract.html) 。また、ライブラリを import するには [PyImport_ImportModule](https://docs.python.jp/3/c-api/import.html#c.PyImport_ImportModule) を使う。

## [exception](exception/spam.c)
例外の送出、エラーインジケータの操作。

[例外を送出](https://docs.python.jp/3/c-api/exceptions.html#raising-exceptions) するには PyErr_SetString などを使う。

例外が起きたかどうかは戻り値を確認する。ほとんどの関数は例外をセットすると PyObject \* なら NULL 、 int なら -1 を返してくる。戻り値での確認では具合が悪い場合は [エラーインジケータへの問い合わせ](https://docs.python.jp/3/c-api/exceptions.html#querying-the-error-indicator) をおこなう。 PyErr_Occurred でだいたい事足りる。 例外のあるなしではなく何が起こったかまでを知るには PyErr_ExceptionMatches, PyErr_GivinExceptionMatches 。

例外を補足し投げ直すようなコードはを書くには PyErr_Fetch 、 PyErr_Restore か。

[raise 文](https://docs.python.jp/3/reference/simple_stmts.html#the-raise-statement) の説明も読む。 Python で except や finally の中で例外を送出すると新しい例外の `__context__` に古い例外が付与される。 raise from すると同様に `__cause__` に付与される。 with_traceback すればトレースバックオブジェクトの設定がなされる。これを C 拡張で模倣するには [PyException_Set](https://docs.python.jp/3/c-api/exceptions.html#exception-objects) シリーズをつかう、であっているだろうか。

## [datetime](datetime/spam.c)
datetime モジュールを使う。

`PyImport_ImportModule('datetime')` することもできるけれども。オブジェクト生成や値の取得のための [マクロ](https://docs.python.jp/3/c-api/datetime.html) が用意されている。これを使うときは `datetime.h` を include し `PyDateTime_IMPORT` マクロを実行しておく必要がある。

datetime は date の subtype だけれども time の subtype ではない。データの詰め込まれ方を見ればそりゃそうだな、と。なので [対応するマクロ](https://github.com/python/cpython/blob/v3.6.3/Include/datetime.h#L117-L140) も異なる。 datetime には `PyDateTime_DATE_` マクロを、 time には `PyDateTime_TIME_` マクロをつかう。

[PyCapsule](https://docs.python.jp/3/c-api/capsule.html) の使用例を _datetimemodule.cで発見。 [PyDateTime_IMPORT マクロの正体](https://github.com/python/cpython/blob/v3.6.3/Modules/_datetimemodule.c#L5806) は PyCapsule_Import で型オブジェクトや関数へのポインタを用意するというものだった。

## [definingtypes](definingtypes/spam.c)
static にタイプオブジェクトを確保する方法は [チュートリアル](https://docs.python.jp/3/extending/newtypes.html) にある。ヒープに作るには [PyType_FromSpec](https://docs.python.jp/3/c-api/type.html#c.PyType_FromSpec) にておこなう。 new で PyObject_New を用いたら dealloc で PyObject_Del をわすれずに。

PyObject* をメンバーにもったら循環参照対策を検討。 Py_TPFLAGS_HAVE_GC フラグを立てて PyObject_GC_New して PyObject_GC_Track 、 PyObject_GC_Untrack, PyObject_GC_DEL するようにするのと tp_traverse, tp_clear への登録をする。

flags が READONLY ではない PyObject* メンバーを持った場合、これを del される可能性がある。 del された場合 NULL が入るので備えなければならない。

## [buffer](buffer/spam.c)
[Buffer Protocol](https://docs.python.jp/3/c-api/buffer.html) に対応した型を使う場合、 PyObject_GetBuffer もしくは PyArgs_ParseTuple を使う。これにより C から直接型が保持しているメモリにアクセスできる。得られたメモリが書き換え可能か、構造は、連続性はあるかなどは PyObject_GetBuffer でどのように要求したか次第。その要求に Buffer Protocol 対応型が応じてくれるかは型次第。

Buffer Protocol に対応した型を作る場合、 [PyBufferProcs](https://docs.python.jp/3/c-api/typeobj.html#buffer-object-structures) を定義し getbuffer, releasebuffer を書かなくてはならない。 getbuffer でメモリを参照するポインタを返してやる。単なるバイト列を公開する程度の用途であれば [PyBuffer_FillInfo](https://docs.python.jp/3/c-api/buffer.html#c.Py_buffer) を使うのが楽。書き換えの可不可に応じて FillInfo の readonly フラグを使い分ける。

getbuffer には view-\>obj の参照カウントを増やしてメモリ公開元のオブジェクト自身が GC に不意に回収されないようにする義務があるが、 PyBuffer_FillInfo を使う場合これが [おこなってくれる](https://github.com/python/cpython/blob/v3.6.3/Objects/abstract.c#L636) ので改めて Py_INCREF する必要はない。そして PyBuffer_Release は Py_DECREF を[おこなう](https://github.com/python/cpython/blob/v3.6.3/Objects/abstract.c#L667) 。数え間違いに注意。

## [pymem](pymem/spam.c)
Python ヒープより[メモリを借り受け、使い、開放する](https://docs.python.jp/3/c-api/memory.html#memory-interface)には PyMem_Malloc, PyMemFree などをつかう。 C の malloc, free らにそっくり。これらをつかうメリットの説明は[ここ](https://docs.python.jp/3/c-api/memory.html#memory-interface)にあるように少量短寿命の用途のためのメモリの使い回し。なのでこれにそぐわないような、たとえば最初から最後までメモリ借りっぱなしとかであれば PyMem_RawMalloc らの使用を考慮する。確保しようとするサイズが大きければ勝手に Raw が使用される。

ANSI C の malloc, free までをも直接使わず [PyMem_RawMalloc](https://github.com/python/cpython/blob/v3.6.3/Objects/obmalloc.c#L65) , PyMem_RawFree としてラップしている理由は理解しきれてはいないけれども、要り用だったのだろう。ポータビリティの向上か。少なくとも malloc(0) の挙動の処理系依存を回避しようとしているのは確か。

メモリ確保、開放例として二分探索木を用意し再帰をつかってみたついでに。 python 側に[再帰の監視](https://docs.python.jp/3/c-api/exceptions.html#recursion-control)をまかせることができる。 Py_EnterRecursiveCall と Py_LeaveRecursiveCall で再帰しようとしているところを挟む。これでバグって暴走したり、そも対象データが大きすぎたりしたときに RecursionError をセットしつつ止まってくれる。

## [zstd](zstd/myzstd.c)
[Zstandard](http://facebook.github.io/zstd/) をつかってみた。 CPython の [バッファプロトコル](https://docs.python.jp/3/c-api/buffer.html) 対応型 bytes, bytesarray などは保持しているメモリをエクスポートできる。あとは使用するライブラリのマニュアル、サンプルコードらを見つつ C でのプログラミング。

## [use_iter](use_iter/spam.c)
イテレータの使用。 [PyObject_GetIter](https://docs.python.jp/3/c-api/object.html#c.PyObject_GetIter) が Python での `iter()` に相当する。 [PyIter_Next](https://docs.python.jp/3/c-api/iter.html#c.PyIter_Next) が `next` 相当、 while ループで繰り返し呼ぶことで要素を取り出す。 NULL が返ったとき要素がなくなったのかエラーなのか判別するために `PyErr_Occurred` がいる。

`PyIter_Check` マクロの実装が `(obj)->ob_type->tp_iternext != NULL` を含むせいで LIMITED API 環境での使用不可、かつ `PyIter_Next` に非イテレータをわたすとセグフォである。とはいえ今回の `spam_use_iterator` のような引数がイテレータそのものであることを期待した関数を作らなければ問題ない。もしくは Python 側に公開することを避けて、イテレータそのもの以外に使わないようにすればなんとか。公式 API にもチェックを省略してあって間違った使い方をすれば落ちる関数、マクロはあることだし。

## [iterator](iterator/spam.c)
イテレータの作成。 iter があればイテレート可能に、自身を返す iter と [iternext](https://docs.python.jp/3/c-api/typeobj.html#c.PyTypeObject.tp_iternext) があればイテレータになる。 iternext は次の要素を、要素がないときは NULL を返すようにつくる。ないときに StopIteration 例外は設定してもしなくてもよいそうな。

## [number_protocol](number_protocol/spam.c)
`+` とか `*` などの演算子が使える数値風クラスの作成方法。 Python では `__add__` とか `__mul__` といったメソッドを作っていた、 C ではどうするのか。 [PyNumberMethods](https://docs.python.jp/3/c-api/typeobj.html#number-structs) を用意して `tp_as_number` にいれる。ヒープに [PyType_FrinSpec](https://docs.python.jp/3/c-api/type.html#c.PyType_FromSpec) で作るクラスの場合は `PyType_slot` に　`nb_` から始まる名前で関数を登録する。 LIMITED API 環境でも使えるスロットの一覧は [PEP 384](https://www.python.org/dev/peps/pep-0384/#type-objects) にある。
