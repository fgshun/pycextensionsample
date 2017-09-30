# Python C Extension の練習
[Python インタプリタの拡張と埋め込み](https://docs.python.jp/3/extending/index.html) や
[Python/C API リファレンスマニュアル](https://docs.python.jp/3/c-api/index.html)
は良く出来た文章だけれども。いざ使おうとすると自分の未熟さゆえ何がどこにあるか探しなおすことに。

なので複数回世話になった部分についてリンクをはったり動きが確かめられるコードをかいてみたりすることにした。

Python 3.6.2 にて動作させている。たぶん 3.5 でも動く。

## empty
ほぼ空のモジュールの作成方法。一段階初期化。

まず [PyModuleDef](https://docs.python.jp/3/c-api/module.html#c.PyModuleDef) がいる。これを [PyModule_Create](https://docs.python.jp/3/c-api/module.html#c.PyModule_Create) に渡すことでモジュールオブジェクトができる。モジュールオブジェクトに何かオブジェクトを追加するには [PyModule_AddObject](https://docs.python.jp/3/c-api/module.html#c.PyModule_AddObject) 。


## multiphasemoduleinit
モジュールの作成方法。多段階初期化。

Python 3.5 より [PEP 489](https://www.python.org/dev/peps/pep-0451/) にある [PyModuleDef_Init](https://docs.python.jp/3/c-api/module.html#c.PyModuleDef_Init) によるモジュール作成が可能になった。一段階初期化では PyInit_ で直接行っていたモジュールオブジェクトの操作は Py_mod_exec に登録した関数で行う。 `__init__` を書く感覚と変わらない書き心地。多段階初期化の手間が軽減されている気がする、むしろ一段階初期化と同程度の手間で済んでいるのではないだろうか。

モジュールオブジェクトの作り方にまで手を出すときは Py_mod_create に関数を登録。こちらは `__new__` みたいな感じ。必要となるケースは少なそう。

各モジュールオブジェクトごとに固有のメモリを割り当てることができる。 m_size にて大きさを指定。不要なら 0　。 -1 は特殊で再初期化不可であることをインタプリタに伝える、静的な領域を使っているときなどに -1 設定する。 [PEP 3121](https://www.python.org/dev/peps/pep-3121/) に 1 以上を設定し [PyModule_GetState](https://docs.python.jp/3/c-api/module.html#c.PyModule_GetState) を使う例がある。

[xxmodule.c](https://github.com/python/cpython/blob/3.6/Modules/xxmodule.c) というテンプレートが CPython のソースには含まれている。ある程度わかっていれば、これから作ろうとしているものに対しどこが不要か選べるならば便利なのだろうと思う。新しい型の作り方から始まっている。

## pymethodef
モジュール関数の作成方法。

[PyCFunction](https://docs.python.jp/3/c-api/structures.html?highlight=pycfunction#c.PyCFunction) 、つまりひとつ目の引数に モジュールオブジェクト、ふたつ目の引数に引数を表す tuple オブジェクトを受け取る関数を作ればよい。作ったメソッドをモジュールやクラスに紐付けるためには [PyMethodDef](https://docs.python.jp/3/c-api/structures.html#c.PyMethodDef) を作る。呼び出し規約 (calling convention) ml_flags を設定することでキーワード引数を使えるようにしたり、引数を受け取らない、引数をひとつだけ受け取ることを示しオーバーヘッドを減らしたりできる。

[呼び出し規約 METH_FASTCALL が 3.6 で追加](http://dsas.blog.klab.org/archives/2017-01/python-dev-201701.html) とのこと。タプルや辞書への詰め直しが不要で早い。 [たしかにあった](https://github.com/python/cpython/blob/6969eaf4682beb01bc95eeb14f5ce6c01312e297/Include/methodobject.h#L91-L114) がドキュメント化されてはいないのでサードパーティーが使うものではないというところだろうか。

## parseargs
モジュール関数の引数は通常 Python の tuple と dict オブジェクトとして見える。これを C の変数に変換するのはよくある作業。なのでそれ用の関数が用意されている。また、戻り値は Python オブジェクトと決まっているので C の変数から変換するのもよくある作業。これも。

* [関数、メソッドの引数の解釈](https://docs.python.jp/3/c-api/arg.html#parsing-arguments)
  * PyArgs_ParseTuple
  * PyArgs_ParseTupleAndKeywords
* [値の構築](https://docs.python.jp/3/c-api/arg.html#building-values)
  * Py_BuildValue

## useabstractprotocol
python だと `a.b` とか `a()` とか `a[0]` とか `for b in a:` などと書くいつもの操作、 C API ではどうやるの？ それにこたえてくれる文章が[抽象オブジェクトレイヤ (abstract objects layer)](https://docs.python.jp/3/c-api/abstract.html) 。ついでに `import a` のやりかたも。 [PyImport_ImportModule](https://docs.python.jp/3/c-api/import.html#c.PyImport_ImportModule) を使う。抽象オブジェクトレイヤの関数、メソッド呼び出しと組み合わせれば標準ライブラリが使える。
