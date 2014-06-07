
// 以下の ifdef ブロックは DLL から簡単にエクスポートさせるマクロを作成する標準的な方法です。
// この DLL 内のすべてのファイルはコマンドラインで定義された BKGNUPG_EXPORTS シンボル
// でコンパイルされます。このシンボルはこの DLL が使用するどのプロジェクト上でも未定義でなけ
// ればなりません。この方法ではソースファイルにこのファイルを含むすべてのプロジェクトが DLL
// からインポートされたものとして BKGNUPG_API 関数を参照し、そのためこの DLL はこのマク
// ロで定義されたシンボルをエクスポートされたものとして参照します。
#ifdef BKGNUPG_EXPORTS
#define BKGNUPG_API __declspec(dllexport)
#else
#define BKGNUPG_API __declspec(dllimport)
#endif

// このクラスは BkGnuPG.dll からエクスポートされます
class BKGNUPG_API CBkGnuPG {
public:
  CBkGnuPG(void);
  // TODO: この位置にメソッドを追加してください。
};

extern BKGNUPG_API int nBkGnuPG;

BKGNUPG_API int fnBkGnuPG(void);

