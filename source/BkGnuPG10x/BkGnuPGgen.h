
// �ȉ��� ifdef �u���b�N�� DLL ����ȒP�ɃG�N�X�|�[�g������}�N�����쐬����W���I�ȕ��@�ł��B
// ���� DLL ���̂��ׂẴt�@�C���̓R�}���h���C���Œ�`���ꂽ BKGNUPG_EXPORTS �V���{��
// �ŃR���p�C������܂��B���̃V���{���͂��� DLL ���g�p����ǂ̃v���W�F�N�g��ł�����`�łȂ�
// ��΂Ȃ�܂���B���̕��@�ł̓\�[�X�t�@�C���ɂ��̃t�@�C�����܂ނ��ׂẴv���W�F�N�g�� DLL
// ����C���|�[�g���ꂽ���̂Ƃ��� BKGNUPG_API �֐����Q�Ƃ��A���̂��߂��� DLL �͂��̃}�N
// ���Œ�`���ꂽ�V���{�����G�N�X�|�[�g���ꂽ���̂Ƃ��ĎQ�Ƃ��܂��B
#ifdef BKGNUPG_EXPORTS
#define BKGNUPG_API __declspec(dllexport)
#else
#define BKGNUPG_API __declspec(dllimport)
#endif

// ���̃N���X�� BkGnuPG.dll ����G�N�X�|�[�g����܂�
class BKGNUPG_API CBkGnuPG {
public:
  CBkGnuPG(void);
  // TODO: ���̈ʒu�Ƀ��\�b�h��ǉ����Ă��������B
};

extern BKGNUPG_API int nBkGnuPG;

BKGNUPG_API int fnBkGnuPG(void);

