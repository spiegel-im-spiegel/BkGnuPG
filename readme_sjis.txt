
                GNU Privacy Guard Plug-in for Becky! 2

                              Copyright (C) 2000-2004 Yasuhiro ARAKAWA
                                                       Version 1.0.5b2
                                                        2004年11月12日


☆ソフトウェア概要

本ソフトウェア (BkGnuPG) は Becky! Internet Mail Ver.2 (B2) 上で動作す
るプラグイン・ソフトウェアです。
Win32版 GNU Privacy Guard (GnuPG) を使ってメールメッセージの署名・暗号
化を行います。また Pretty Good Privacy (PGP) あるいは GnuPG により署名・
暗号化されたメールを検証・復号化することもできます。

メールの暗号化を行うプラグインとしては，B2に標準で同梱されている「Becky!
PGP プラグイン (BkPGP)」がありますが，このソフトウェアとほぼ同等の機能
を GnuPG を使って実現します。


☆動作環境

B2 および Win32版 GnuPG が動作する環境であれば大丈夫と思いますが，開発
環境の問題で以下のプラットフォームでのみ動作確認を行っています。

    ・ Windows 2000 Professional Service Pack 3 (日本語版)


☆インストール/アンインストール方法

ここでは，フルパッケージ版をダウンロードされた方のために解説します。イ
ンストーラ付のパッケージをダウンロードされた方はダウンロードしたファイ
ルを実行し，画面の指示に従っていけば完了します。またアンインストールは
コントロールパネルの「アプリケーションの追加と削除」からアンインストー
ルを行って下さい。

最初に，B2 および GnuPG のインストールが完了していることを確認して下さ
い。特に GnuPG については，

 ・ 実行モジュール (gpg.exe) のあるフォルダにパスを通す
 ・ GnuPG ホームディレクトリ (レジストリで指定) を設定する

ことをお薦めします。これらの設定方法については，GnuPG に同梱されている
README.W32 ファイルをご覧ください。

ダウンロードしたファイルを解凍すると以下のファイルが取り出せます。

 ・ BkGnuPG ロードモジュール (DLLファイル)
 ・ BkGnuPG ソースファイル (zip圧縮)
 ・ マニュアル (manual\ フォルダ以下のHTMLファイル)
 ・ 簡易マニュアル (このファイル)

BkGnuPG をインストールするには，上記ファイルのうち BkGnuPG ロードモジュ
ール (BkGnuPG.dll) を B2 インストールフォルダ下の「PlugIns」フォルダ，
またはデータフォルダ下の「PlugIns」フォルダに置きます。
例えば B2 を「C:\Program Files\RimArts\」にインストールし，データフォル
ダを「C:\Becky!\Spiegel\」に設定している場合，

    C:\Program Files\RimArts\B2\PlugIns\

または

    C:\Becky!\Spiegel\PlugIns\

のいずれかのフォルダに置きます。

アンインストールはコピーしたDLLファイルを削除すれば完了です。BkGnuPG.ini
があればこれも削除します。BkGnuPG 自身はレジストリを汚しません。


☆開発履歴

2004年11月13日 Verion 1.0.5b2
  ・復号メッセージを移動またはコピーすると元のメッセージの PGP/MIME パ
    ートが失われる障害に対応した（つもり）。

2003年05月03日 Verion 1.0.5b
  ・署名用のメッセージダイジェストアルゴリズムを指定できるようにした。
    詳しくは manual\bkgnupg_ini_sj.txt を参照。

2003年05月02日 Verion 1.0.4
  ・ロシア語のリソースを修正した。
  ・英語のリソースについてフォントを「MS Shell Dlg」に統一した。

2003年05月01日 Verion 1.0.4b (βバージョン)
  ・ロシア語のリソースを追加した。(自信なし)
  ・GnuPG のレジストリ設定から gpg.exe のパスを取得するようにした。ただ
    し INI ファイルの隠し設定の方が優先されます。
  ・--use-agent オプションに対応した。 WinPT に同梱されている gpg-agent.exe
    と組み合わせて使います。

2002年12月31日 Verion 1.0.3
  ・署名または信用されない鍵などで暗号化する際，gpg.exe がハングアップ
    する不具合を修正した。

2002年03月21日 Verion 1.0.2
  ・復号時に条件によってヘッダ要素 (Content-Type など) が増殖する不具合
    を修正した。

2001年06月17日 Verion 1.0.1
  ・gpg.exe へのパスが空白文字を含む名前の場合に，うまく起動しないこと
    がある不具合を修正した。
  ・テキストモードでPGP/MIME署名できるようにした。(Sylpheed対策) 詳しく
    は manual\bkgnupg_ini_sj.txt を参照。
  ・パスフレーズを入力する際，入力内容が表示できるようにした。
  ・処理結果のアウトプットでUTF-8コードに対応した。

2000年11月08日 Verion 1.0.0
  ・正式版。バージョン管理方法を変える。機能的な部分の変更はなし。
  ・インストーラ付きのパッケージの配布を始める。

2000年10月31日 Verion 0.40
  ・暗号化の際，受信者に From ユーザやデフォルトユーザを含めることがで
    きるようにした。
  ・マニュアルを大幅改訂。英語版にも対応した。
  ・SDK 0.33 に対応した。

2000年10月26日 Verion 0.32
  ・一部のドキュメントを英語化してみる。翻訳ソフトを使ってるので変な表
    現になってるかもしれないが，自分じゃ分からん (^^;)
  ・コンパイルオプションを一部変更。
  ・ファイル削除処理を変えてみた。これで例外は発生しないと思うんだけど
    ...
  ・登録したメニューのIDを内部で保持するようにした。
  ・リストビューのメニュー項目を「メールのプロパティ」より上側にセット
    するようにした。

2000年10月19日 Verion 0.31
  ・なんと，英語リソースが付く。これで英語環境でもちゃんと使える(筈)。
  ・自公開鍵を添付する際，「PGP互換」等のオプションを有効にするようにし
    た。でもあんまり意味ないかも。公開鍵のインポート/エクスポートについ
    てはもう少し分かりやすいユーザ・インタフェースが必要かもしれない。
  ・サポートページを Vector に引っ越す。ここが安住の地となるだろう。

2000年10月08日 Verion 0.30
  ・Version 0.21 の不具合にちゃんと対処した。また添付ファイル付きメール
    の署名と検証がうまくいかないことがある不具合に対処。(根は同じ)
  ・受信者選択ダイアログおよびパスフレーズ入力ダイアログから設定ダイア
    ログを呼び出すボタンを追加する。これで設定を一時的に変更できる。(た
    だしINIファイルには反映されない)
  ・設定ダイアログでメッセージ要約アルゴリズムに MD5 を指定できるように
    した。(デフォルトは SHA-1)
  ・リストビューのコンテキストメニューから署名・検証を行う項目を追加し
    た。
  ・コールバック関数単位で例外ハンドラを実装した。これで致命的な例外が
    発生しても，最悪の場合でも，B2 には影響を与えない(筈)。例外ハンドラ
    の実装については将来のバージョンで変更するかも。
  ・INIファイルの扱いをまた変更した。デフォルトは起動ディレクトリ。もし
    データフォルダ(のプラグインフォルダ)や GnuPG のホームディレクトリに
    INIファイルがあればそちらを使う。

2000年10月04日 Verion 0.21
  ・添付ファイル付きのメールをPGP/MIME暗号化した場合，そのメールを複合
    化しようとすると例外処理で B2 が以上終了してしまう不具合に応急対処。
  ・マニュアルを同梱した。

2000年10月03日 Verion 0.20
  ・日本語で作成された gnupg.mo を使って，処理結果のアウトプットを日本
    語表示できるようにした。現在 JIS (iso-2022-jp) および EUC-JP のみ対
    応。(デフォルトは Shift-JIS)
  ・メモリリーク監視用のデバッグルーチンを埋めこむ。まぁリリース版のDLL
    モジュールには関係ないけど。
  ・SDK に付属する CMIMEItem クラスをSTLベースで書き直した。他の細かい
    関数もSTLベースで書き直す。これで SDK へのコード依存度は減ったが，
    ロードモジュールサイズがまた膨れる...
  ・上記の作業に併せ，メモリリーク等の細かい不具合を修正。
  ・PGP署名 (クリア署名) する際，PGP互換の指定がある場合にはメッセージ
    に改行(LF+CR)を付加するようにした。
  ・--rfc1991 オプションに対応した。

2000年10月29日 Verion 0.11 (未公開バージョン)
  ・JISコードで書かれた gnupg.mo を使って，処理結果のアウトプットを日本
    語表示できるようにしてみた。

2000年9月27日 Verion 0.10
  ・gpg.exe の起動で COMSPEC を経由しないで直接起動するようにした。
  ・INIファイルを GnuPG のホームディレクトリ(レジストリで指定したディレ
    クトリ)に置くようにした。 GnuPG のホームディレクトリがない場合は，
    従来通り BkGnuPG.dll のあるディレクトリに置く。
  ・暗号化時に受信者の一覧を表示し，手動で変更できるようにした。
  ・--openpgp および --throw-keyid オプションに対応した。Setting ダイア
    ログで「OpenPGP に準拠した署名・暗号を行う」，「受信者を匿名にする」
    をチェックすると有効になる。
  ・Setting ダイアログで，gpg.exe へのパスを指定する項目を廃止。ただし，
    INIファイルに直接記述することで変更できる。
  ・内部のクラス構成を変えた。将来への布石，のつもり。
  ・SDK Beta29 に対応した。(メモリリークする問題に対応)
  ・その他細かい部分を変更・修正(したけど忘れた (^^;))。

2000年9月14日 Verion 0.01
  ・Win98でうまく出力をリダイレクトできないらしいので，方法を変えてみた。
  ・PGP/MIME署名時にテキストを Quoted-Printable に変更しないようにした。
  ・SDK Beta28 に対応した。(処理自体には変更なし)

2000年9月12日 Verion 0.00
    取り敢えず動作するものを作ってみた。

