# macOS で作る「30日でできる！OS自作入門」
## モチベーション
これまで以下のようなリポジトリにおいて、「30日でできる！OS自作入門」を macOS 環境で扱う方法が紹介されていました。
- [tatsumack/30nichideosjisaku: 『30日でできる！ OS自作入門』のmacOS開発環境構築](https://github.com/tatsumack/30nichideosjisaku)

ただし、アセンブラの nask が 32bit アプリであるために、macOS Catalina 以降では利用することができなくなってしまいました。

実行すると、以下のようなエラーが出ます。
```
Bad CPU type in executable
```

nask 自体は、nasm の文法の多くを真似て、自動最適化能力を高めたアセンブラということですので、最新の macOS でも利用できる nasm を使用する方法に置き換えようと考えました。

とはいえ、そこまで難しくはなく、Linux 上で nasm を使って実現されている方々がブログを残されており、また C 言語になれば修正が必要な部分はほとんどありません。
このリポジトリでは、これらのブログを参考に、macOS で「30日でできる！OS自作入門」を勉強するための環境構築やソースコードを、記録として残しておくことを目的としています。

## 必要なツール類
- [Hex Friend](https://apps.apple.com/jp/app/hex-fiend/id1342896380): バイナリエディタ
- [QEMU](https://www.qemu.org/): 作成した OS の起動実験用のエミュレータ
- [GNU Make](https://www.gnu.org/software/make/): コンパイル作業を自動化するツール

```
brew install qemu make
```

### 動作確認環境
- macOS Big Sur 11.3.1
- QEMU emulator version 6.0.0
- GNU Make 3.81
