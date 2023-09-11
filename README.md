This package extends the Adafruit SSD1306 library to print Japanese fonts with UTF8 character encoding.

[README in English](#readme-in-english)

このパッケージはAdafruit SSD1306ライブラリを拡張しUTF8文字コードで日本語フォントをprint可能にします。

[日本語README](#日本語readme)

# README in English

## Integration

After adding this library with Arduino IDE's library manager,
Change `#include <Adafruit_SSD1306.h>` in *.ino to `#include <UTF8_Adafruit_SSD1306.h>`.

This allows you to output `'STR'` which describes Japanese characters (UTF8) with `print('STR')`/`println('STR')`.

## Extended functions

Misaki font's half-width characters are 4x8 dots and are difficult to read, so we added a function to convert half-width alphanumeric symbols and kana to full-width characters.

Set with `void setHalf2Full(bool enable)`. (Half-width full-width conversion is disabled in the standard state.)

## UTF8 font

The included fontconvert/font_converter.py converts the Misaki font (TrueType format) into a bitmap and incorporates it.

(Misaki font is an 8x8 dot Japanese font, which is suitable for microcontrollers with small ROM capacity such as Arduino.)

If you want to reduce ROM capacity by omitting unused characters, or add unused characters,
Please change fontconvert/chara_list.txt and run fontconvert/font_converter.py.

Reference "Misaki font": https://littlelimit.net/misaki.htm

## Apply to another device
`UTF8_Adafruit_SSD1306.h` is very small.

Override `virtual size_t write(uint8_t c)` of the parent `class Print` of the inherited `class Adafruit_SSD1306` parent `class Adfruit_GFX` with `virtual size_t write(uint8_t c)` of `UTF8_Core/UTF8_InjectCore.h` This will replace the write function with one that is UTF8 compatible.

(Rather than creating a new class that inherits `class Print` or `class Adfruit_GFX` and overriding it, It override it with `class UTF8_Adafruit_SSD1306`, which is the last child of inheritance, to minimum code changes.)

Since the amount of code changes is small, it is easy to adapt to other devices. .

1. Copy `UTF8_Adafruit_SSD1306.h` to another name
1. Change `#include <Adafruit_SSD1306.h>` to the header file of the device you want to apply
1. `class UTF8_Adafruit_SSD1306 : public Adafruit_SSD1306, private UTF8_Write {change the class name `UTF8_Adafruit_SSD1306` and `Adafruit_SSD1306` to the class name of the device you want to apply
1. Define a constructor with the same arguments as the constructor of the device you want to apply to the constructor, and call `UTF8_ExtensionCore_Constructor_Initializer` with the constructor of the device you want to apply.

# 日本語README

## 組み込み
Arduino IDEのライブラリマネージャで本ライブラリを追加した後、
*.inoの`#include <Adafruit_SSD1306.h>`を`#include <UTF8_Adafruit_SSD1306.h>`に変更します。

これにより日本語文字(UTF8)を記述した`'STR'`を`print('STR')`/`println('STR')`で出力できます。

## 拡張機能
美咲フォントの半角文字は4x8ドットで判読が難しいので、半角英数記号カナを全角に変換する機能を追加しました。

`void setHalf2Full(bool enable)`で設定します。（標準状態では半角全角変換は無効です。）

## UTF8フォント
同梱のfontconvert/font_converter.pyで美咲フォント(TrueType 形式)をビットマップ化して組み込んでいます。

（美咲フォントは8x8ドットの日本語フォントで、ArduinoのようなROM容量の少ないマイコンに好都合です。）

使用しない文字を省いてROM容量を抑えたり、未収蔵の文字を追加したい場合は、
fontconvert/chara_list.txt を変更して、fontconvert/font_converter.pyを実行してください。

参照「美咲フォント」: https://littlelimit.net/misaki.htm

## 別デバイスへの適用
`UTF8_Adafruit_SSD1306.h`は非常にシンプルです。

継承元`class Adafruit_SSD1306`の親`class Adfruit_GFX`の親`class Print`の`virtual size_t write(uint8_t c)`を、`UTF8_Core/UTF8_InjectCore.h`の`virtual size_t write(uint8_t c)`でオーバーライドすることで、write 関数を UTF8 互換のものに置き換えます。

（`class Print`または`class Adfruit_GFX`を継承したクラスを新設してオーバーライドするのではなく、継承の末子となる`class UTF8_Adafruit_SSD1306`でオーバーライドを行うことで、コード変更量を少なく抑えています。）

コード変更量が少ないので、他のデバイスへの対応も容易です。。

1. `UTF8_Adafruit_SSD1306.h`を別名コピー
1. `#include <Adafruit_SSD1306.h>`を適用したいデバイスのヘッダファイルに変更
1. `class UTF8_Adafruit_SSD1306 : public Adafruit_SSD1306, private UTF8_Write {`のクラス名`UTF8_Adafruit_SSD1306`と`Adafruit_SSD1306`を適用したいデバイスのクラス名に変更
1. コンストラクタに適用したいデバイスのコンストラクタ同引数のコンストラクタを定義し、適用したいデバイスのコンストラクトと共に`UTF8_ExtensionCore_Constructor_Initializer`を呼出し

