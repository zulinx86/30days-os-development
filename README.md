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
- [NASM](https://www.nasm.us/): nask の代わりに使用するアセンブラ
- [mtools](https://www.gnu.org/software/mtools/): edimg の代わりに使用するフロッピーディスク用のイメージ作成ツール
- [i386-elf-toolchain (i386-elf-gcc, i386-elf-binutils)](https://github.com/nativeos/homebrew-i386-elf-toolchain): cc1 の代わりに使用する C 言語のコンパイラ (Mac の gcc は clang のため、`-T` でリンカスクリプトを渡せない)

インストール方法は以下の通りです。
```
brew install qemu make nasm mtools
brew tap nativeos/i386-elf-toolchain
brew install i386-elf-binutils i386-elf-gcc
```

バージョンが表示されていれば、インストールできています。
```
qemu-system-i386 --version
make --version
nasm --version
mtools --version
i386-elf-gcc --version
```

### 動作確認環境
- macOS Big Sur 11.3.1
- QEMU emulator version 6.0.0
- GNU Make 3.81
- NASM version 2.15.05
- mtools (GNU mtools) 4.0.31
- i386-elf-gcc (GCC) 11.1.0


## 変更点一覧
|変更前 (nask)|変更後 (nasm)|コメント・説明|
|:--|:--|:--|
|`RESB 18`|`TIMES 18 DB 0`|`RESB` を使うと警告が出ます。|
|`RESB 0x7dfe-$`|`TIMES 0x1fe-($-$$) DB 0`||
|`JMP entry`|`JMP SHORT entry`|特にエラーや警告は出ませんでしたが、参考にした文献では意図しないコードになりえるとのこと。|
|`[INSTRSET “i486p”]`|削除|エラーが発生します。必要ないらしいです。|
|`[FORMAT "WCOFF"]`|削除|エラーが発生します。必要ないらしいです。|
|`[FILE "naskfunc.nas"]`|削除|エラーが発生します。必要ないらしいです。|
|`_io_hlt`|`io_hlt`|アンダーバーがあるとエラーが発生します。他の関数も同様。|


## リンカスクリプト
リンカスクリプトは、[『30日でできる！OS自作入門』のメモ](https://vanya.jp.net/os/haribote.html#hrb) に紹介されていたものを、ほぼそのまま使用しています。

微修正した点は、.data セクションの位置です。

## フォントファイル
[『30日でできる！OS自作入門』のメモ](https://vanya.jp.net/os/haribote.html) より変換ずみの hankaku.c を使用しました。


## sprintf
標準ライブラリを使用しないようにするため、[sprintfを実装する | OS自作入門 5日目-2 【Linux】 | サラリーマンがハッカーを真剣に目指す](http://bttb.s1.valueserver.jp/wordpress/blog/2017/12/17/makeos-5-2/) より sprintf 関数の実装をベースに使用しました。

上記の実装に加えて、以下の機能を追加しています。
- `%X`(16 進数表示の際のアルファベットの大文字表記) への対応
- 文字数指定
- ゼロパディング


## 性能測定
性能測定をする際に、以下のようなスクリプトを実行します。
```c
struct FIFO32 fifo;
int count = 0;

// snipped

for (;;) {
	count++;

	io_cli();
	if (fifo32_status(&fifo) == 0) {
		io_sti();
	}
	else {
		// snipped
	}
}
```

QEMU でこのスクリプトを実行すると、割り込み処理が実行されない事象が発生しました。

この事象は、複数のブログにて報告されていて、[OS自作入門してみた＆やりきった - ハラミTech](https://blog.haramishio.xyz/entry/hariboteos) に記載があるように、`-enable-kvm` を QEMU のオプションをつけると解決するようです。

しかし、手元の環境では以下のようにエラーが出て利用できませんでした。
```
qemu-system-i386: invalid accelerator kvm
qemu-system-x86_64: invalid accelerator kvm
```

[macos - How to enable KVM on a Mac for Qemu? - Stack Overflow](https://stackoverflow.com/questions/53778106/how-to-enable-kvm-on-a-mac-for-qemu) によると、macOS 環境では代わりに `-accel hvf` を利用できるということでした。

手元の環境で試してみたところ、`qemu-system-i386` では以下のエラーが出て利用できませんでした。
```
qemu-system-i386: -accel hvf: invalid accelerator hvf
```
一方で、`qemu-system-x86_64` の場合は、以下の警告が出ますが、とりあえずはプログラムが動いてくれました。
```
qemu-system-x86_64: warning: host doesn't support requested feature: CPUID.80000001H:ECX.svm [bit 2]
```
しかし、結局のところ、測定結果が実行ごとに大きく異なってしまい、性能測定の目的は果たすことができませんでした。

`count++` と `io_cli()` の間に文字列表示の処理を入れることで、割り込み処理が正常に行われるようになったため、本リポジトリのコードでは、こちらの方法を使用していますが、実際のところ性能評価としては、あまり使い物になりませんでした。
```
		count++;
		putfonts8_asc_sht(sht_back, 0, 112, COL8_FFFFFF, COL8_008484, " ", 1);
		io_cli();
```

本にも記載されていますが、性能評価をする時は、やはり実機で試すのがベストだと思います。


## ファイル圧縮
Chapter 29 の harib26b にあるファイル圧縮には、対応していません。

理由は `bim2bin` による tek2 / tek5 の圧縮方法が、一般的な圧縮形式ではないためです。

`bim2bin` のソースコードは、[こちら](https://github.com/HariboteOS/tolsrc/blob/master/bim2bi4w/bim2bin.c) で確認できますので、どうしてもファイルを圧縮したい方は、参考にできるかと思います。


## 参考文献
- [tools/nask - hrb-wiki](http://hrb.osask.jp/wiki/?tools/nask)
- [『30日でできる！OS自作入門』のメモ](https://vanya.jp.net/os/haribote.html#hrb)
- [『30日でできる！OS自作入門』を macOS Catalina で実行する - Qiita](https://qiita.com/noanoa07/items/8828c37c2e286522c7ee)
- [noanoa07/myHariboteOS: 『30日でできる！OS自作入門』 for macOS Catalina](https://github.com/noanoa07/myHariboteOS)
- [sprintfを実装する | OS自作入門 5日目-2 【Linux】 | サラリーマンがハッカーを真剣に目指す](http://bttb.s1.valueserver.jp/wordpress/blog/2017/12/17/makeos-5-2/)
