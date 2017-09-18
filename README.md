# Python C Extension の練習
[Python インタプリタの拡張と埋め込み](https://docs.python.jp/3/extending/index.html) や
[Python/C API リファレンスマニュアル](https://docs.python.jp/3/c-api/index.html)
を読んだものの。いざつかおうとすると何がどこにあるか探しなおすことに。

なので複数回世話になった部分についてリンクをはったり動きが確かめられるコードをかいてみたりすることにした。

* empty
  * 空のモジュールの作成方法。一段階初期化。
  * [PyModule_Create](https://docs.python.jp/3/c-api/module.html#c.PyModule_Create)
  * [PyModuleDef](https://docs.python.jp/3/c-api/module.html#c.PyModuleDef)
* pymethodef
  * モジュール関数の作成方法。
  * [PyMethodDef](https://docs.python.jp/3/c-api/structures.html#c.PyMethodDef)
  * ml_flags で使える呼び出し規約 (calling convention)
    * METH_VARARGS, METH_KEYWORDS, METH_NOARGS, METH_O
  * PyMethodDef はクラスメソッド、スタティックメソッドの作成にも使う。 binding convention 。
    * METH_CLASS, METH_STATIC
* parseargs
  * [関数、メソッドの引数の解釈](https://docs.python.jp/3/c-api/arg.html#parsing-arguments)
    * PyArgs_ParseTuple
    * PyArgs_ParseTupleAndKeywords 
  * [値の構築](https://docs.python.jp/3/c-api/arg.html#building-values)
    * Py_BuildValue
