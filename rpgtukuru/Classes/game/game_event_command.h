#pragma once

// �C�x���g�R�[�h(�c�N�[�����Ŏg�p)
// ����n
#define CODE_OPERATE_SWITCH		0x27e2	///< �X�C�b�`
#define CODE_OPERATE_VAR		0x27ec	///< �ϐ�
#define CODE_OPERATE_ITEM		0x2850	///< �A�C�e��
#define CODE_OPERATE_KEY		0x2d5a	///< �L�[
#define CODE_OPERATE_INPUT		0x27A6	///< ���l����
#define CODE_OPERATE_TIMER		0x27F6	///< �^�C�}�[
#define CODE_OPERATE_MONEY		0x2846	///< ������
// �p�[�e�B
#define CODE_PARTY_CHANGE		0x285a	///< �p�[�e�B�ύX
#define CODE_PARTY_HP			0x28dc	///< HP
#define CODE_PARTY_MP			0x28E6	///< MP
#define CODE_PARTY_STATE		0x28f0	///< ��ԕύX
#define CODE_PARTY_SKILL		0x28c8	///< ���Z
#define CODE_PARTY_REFRESH		0x28fa	///< ��
#define CODE_PARTY_EXP			0x28AA	///< �o���l
#define CODE_PARTY_LV			0x28B4	///< ���x��
#define CODE_PARTY_POWER		0x28BE	///< �\�͒l
#define CODE_PARTY_SOUBI		0x28D2	///< ����
#define CODE_PARTY_DAMAGE		0x2904	///< �_���[�W����
#define CODE_PARTY_NAME			0x2972	///< ���O
#define CODE_PARTY_TITLE		0x297C	///< ������
#define CODE_PARTY_WALK			0x2986	///< ���s�G
#define CODE_PARTY_FACE			0x2990	///< ��G
// �V�X�e��
#define CODE_SYSTEM_VEHICLE		0x299A	///< ��蕨�G�ύX
#define CODE_SYSTEM_BGM			0x29a4	///< �V�X�e��BGM
#define CODE_SYSTEM_SOUND		0x29AE	///< �V�X�e�����ʉ�
#define CODE_SYSTEM_GRAPHIC		0x29B8	///< �V�X�e���O���t�B�b�N
#define CODE_SYSTEM_SCREEN		0x29C2	///< ��ʐ؂�ւ�����
// ��蕨
#define CODE_VEH_RIDE			0x2A58	///< ��~
#define CODE_VEH_LOCATE			0x2A62	///< �ʒu�ݒ�
// �\��
#define CODE_EVENT_END			0x0000	///< Event terminater
#define CODE_BLOCK_END			0x000a	///< Block
#define CODE_IF_START			0x2eea	///< if
#define CODE_IF_ELSE			0x55fa	///< else
#define CODE_IF_END				0x55fb	///< end if
#define CODE_LOOP_START			0x2fb2	///< loop
#define CODE_LOOP_BREAK			0x2fbc	///< break
#define CODE_LOOP_END			0x56c2	///< loop end
#define CODE_GOTO_MOVE			0x2f58	///< goto
#define CODE_GOTO_LABEL			0x2f4e	///< goto label
#define CODE_SELECT_START		0x279c	///< select case
#define CODE_SELECT_CASE		0x4eac	///< case n
#define CODE_SELECT_END			0x4ead	///< end select
#define CODE_SHOP				0x29E0	///< ���X
#define CODE_SHOP_IF_START		0x50F0	///< ���X(��������)
#define CODE_SHOP_IF_ELSE		0x50F1	///< ���X(�������Ȃ�)
#define CODE_SHOP_IF_END		0x50F2	///< ���X(End If)
#define CODE_INN				0x29EA	///< �h��
#define CODE_INN_IF_START		0x50FA	///< �h��(�h������)
#define CODE_INN_IF_ELSE		0x50FB	///< �h��(�h�����Ȃ�)
#define CODE_INN_IF_END			0x50FC	///< �h��(End IF)
// �C�x���g
#define CODE_EVENT_BREAK		0x3016	///< �C�x���g���f
#define CODE_EVENT_CLEAR		0x3020	///< �ꎞ����
#define CODE_EVENT_GOSUB		0x302a	///< �T�u���[�`��(�C�x���g�̂�т���)
// �}���`���f�B�A(MM -> MultiMedia)
#define CODE_MM_SOUND			0x2d1e	///< ���ʉ�
#define CODE_MM_BGM_PLAY		0x2cf6	///< BGM�Đ�
#define CODE_MM_BGM_FADEOUT		0x2d00	///< BGM�t�F�[�h�A�E�g
#define CODE_MM_BGM_SAVE		0x2D0A	///< BGM�L��
#define CODE_MM_BGM_LOAD		0x2D14	///< �L������BGM���Đ�
#define CODE_MM_MOVIE			0x2D28	///< ���[�r�[
// ���
#define CODE_SCREEN_CLEAR		0x2b02	///< ����
#define CODE_SCREEN_SHOW		0x2b0c	///< �\��
#define CODE_SCREEN_COLOR		0x2b16	///< �F���ύX
#define CODE_SCREEN_SHAKE		0x2b2a	///< �V�F�C�N
#define CODE_SCREEN_FLASH		0x2b20	///< �t���b�V��
#define CODE_SCREEN_SCROLL		0x2B34	///< �X�N���[��
#define CODE_SCREEN_WEATHER		0x2B3E	///< �V�C
// �s�N�`��
#define CODE_PICT_SHOW			0x2b66	///< �\��
#define CODE_PICT_MOVE			0x2b70	///< �ړ�
#define CODE_PICT_CLEAR			0x2b7a	///< ����
// �L����
#define CODE_CHARA_TRANS		0x2c2e	///< ������ԕύX
#define CODE_CHARA_MOVE			0x2c42	///< �ړ�
#define CODE_CHARA_FLASH		0x2C38	///< �t���b�V��
#define CODE_MOVEALL_START		0x2C4C	///< �w�蓮��̑S���s
#define CODE_MOVEALL_CANCEL		0x2C56	///< �w�蓮��̑S���s���L�����Z��
// �ʒu
#define CODE_LOCATE_MOVE		0x2a3a	///< �ꏊ�ړ�(�Œ�l)
#define CODE_LOCATE_SAVE		0x2a44	///< ���ݒn�ۑ�
#define CODE_LOCATE_LOAD		0x2a4e	///< �L�������ꏊ�Ɉړ�(�ϐ�)
// �e�L�X�g�E����
#define CODE_TXT_REM			0x307a	///< ����
#define CODE_TXT_REM_ADD		0x578a	///< ���ߒǉ�
#define CODE_TXT_SHOW			0x277e	///< �{��
#define CODE_TXT_SHOW_ADD		0x4e8e	///< �{���ǉ�
#define CODE_TXT_OPTION			0x2788	///< ���͕\���I�v�V����
#define CODE_TXT_FACE			0x2792	///< ��CG�ύX
// ���̑�
#define CODE_NAME_INPUT			0x29F4	///< ���O����
#define CODE_EVENT_LOCATE		0x2A6C	///< �C�x���g�ʒu�ݒ�
#define CODE_EVENT_SWAP			0x2A76	///< �C�x���g�ʒu����
#define CODE_LAND_ID			0x2A9E	///< �n�`ID�擾
#define CODE_EVENT_ID			0x2AA8	///< �C�x���gID�擾
#define CODE_WAIT				0x2c92	///< �E�F�C�g
#define CODE_CHIPSET			0x2DBE	///< �`�b�v�Z�b�g
#define CODE_PANORAMA			0x2DC8	///< ���i
#define CODE_ENCOUNT			0x2DDC	///< �G���J�E���g
#define CODE_CHIP_SWAP		0x2DE6	///< �`�b�v�u��
#define CODE_TELEPORT			0x2E22	///< �e���|�[�g����
#define CODE_TELEPORT_PERM		0x2E2C	///< �e���|�[�g�֎~
#define CODE_ESCAPE				0x2E36	///< �G�X�P�[�v�ʒu
#define CODE_ESCAPE_PERM		0x2E40	///< �G�X�P�[�v�֎~
#define CODE_SAVE_SHOW			0x2E86	///< �Z�[�u���
#define CODE_SAVE_PERM			0x2E9A	///< �Z�[�u�֎~
#define CODE_MENU_SHOW			0x2eae	///< ���j���[�\��
#define CODE_MENU_PERM			0x2EB8	///< ���j���[�֎~
#define CODE_LABEL				0x2F4E	///< ���x���̐ݒ�
#define CODE_GAMEOVER			0x3084	///< �Q�[���I�[�o�[
#define CODE_TITLE				0x30DE	///< �^�C�g���ɖ߂�
#define CODE_BTLANIME			0x2BCA	///< �퓬�A�j��(��퓬)
// �퓬�n
#define CODE_BTL_GO_START		0x29d6	///< �퓬����
#define CODE_BTL_GO_WIN			0x50e6	///< ��������
#define CODE_BTL_GO_ESCAPE		0x50e7	///< ��������
#define CODE_BTL_GO_LOSE		0x50e8	///< ��������
#define CODE_BTL_GO_END			0x50e9	///< ����I��
#define CODE_BTL_ANIME			0x33cc	///< �퓬�A�j��
// �c�N�[��2003�ǉ���
#define CODE_2003_JOB			0x03F0	///< �E�ƕύX
#define CODE_2003_BTL_CMD		0x03F1	///< �퓬�R�}���h
#define CODE_2003_ATK_REPEAT	0x03EF	///< �A���U��
#define CODE_2003_ESCAPE100		0x03EE	///< 100%�E�o
#define CODE_2003_BTL_COMMON	0x03ED	///< �o�g���C�x���g����R������т���

////////////////////////////////////////// �o�g���C�x���g
#define CODE_BTL_BACKGROUND		0x339a	///< �w�i�ύX
#define CODE_BTL_STOP			0x3462	///< �퓬���f
// �G
#define CODE_BTL_ENEMY_HP		0x3336	///< HP����
#define CODE_BTL_ENEMY_MP		0x3340	///< MP����
#define CODE_BTL_ENEMY_STATE	0x334a	///< ��ԕύX
#define CODE_BTL_ENEMY_APPEAR	0x335e	///< �o��
// �퓬�\��
#define CODE_BTL_IF_START		0x33fe	///< if
#define CODE_BTL_IF_ELSE		0x5b0e	///< ���ꏏ
#define CODE_BTL_IF_END			0x5b0f	///< ���ꏏ


#define PP_protoType(code) \
	template< > void GameEventManager::command<code>(const rpg2k::structure::Instruction& com)

PP_protoType(CODE_OPERATE_SWITCH);
PP_protoType(CODE_OPERATE_VAR);
PP_protoType(CODE_OPERATE_ITEM);
PP_protoType(CODE_OPERATE_KEY);
PP_protoType(CODE_OPERATE_INPUT);
PP_protoType(CODE_OPERATE_TIMER);
PP_protoType(CODE_OPERATE_MONEY);

PP_protoType(CODE_PARTY_CHANGE);
PP_protoType(CODE_PARTY_HP);
PP_protoType(CODE_PARTY_MP);
PP_protoType(CODE_PARTY_STATE);
PP_protoType(CODE_PARTY_SKILL);
PP_protoType(CODE_PARTY_REFRESH);
PP_protoType(CODE_PARTY_EXP);
PP_protoType(CODE_PARTY_LV);
PP_protoType(CODE_PARTY_POWER);
PP_protoType(CODE_PARTY_SOUBI);
PP_protoType(CODE_PARTY_DAMAGE);
PP_protoType(CODE_PARTY_NAME);
PP_protoType(CODE_PARTY_TITLE);
PP_protoType(CODE_PARTY_WALK);
PP_protoType(CODE_PARTY_FACE);

PP_protoType(CODE_SYSTEM_VEHICLE);
PP_protoType(CODE_SYSTEM_BGM);
PP_protoType(CODE_SYSTEM_SOUND);
PP_protoType(CODE_SYSTEM_GRAPHIC);
PP_protoType(CODE_SYSTEM_SCREEN);

PP_protoType(CODE_VEH_RIDE);
PP_protoType(CODE_VEH_LOCATE);

PP_protoType(CODE_EVENT_END);
PP_protoType(CODE_BLOCK_END);
PP_protoType(CODE_IF_START);
PP_protoType(CODE_IF_ELSE);
PP_protoType(CODE_IF_END);
PP_protoType(CODE_LOOP_START);
PP_protoType(CODE_LOOP_BREAK);
PP_protoType(CODE_LOOP_END);
PP_protoType(CODE_GOTO_MOVE);
PP_protoType(CODE_GOTO_LABEL);
PP_protoType(CODE_SELECT_START);
PP_protoType(CODE_SELECT_CASE);
PP_protoType(CODE_SELECT_END);
PP_protoType(CODE_SHOP);
PP_protoType(CODE_SHOP_IF_START);
PP_protoType(CODE_SHOP_IF_ELSE);
PP_protoType(CODE_SHOP_IF_END);
PP_protoType(CODE_INN);
PP_protoType(CODE_INN_IF_START);
PP_protoType(CODE_INN_IF_ELSE);
PP_protoType(CODE_INN_IF_END);

PP_protoType(CODE_EVENT_BREAK);
PP_protoType(CODE_EVENT_CLEAR);
PP_protoType(CODE_EVENT_GOSUB);

PP_protoType(CODE_MM_SOUND);
PP_protoType(CODE_MM_BGM_PLAY);
PP_protoType(CODE_MM_BGM_FADEOUT);
PP_protoType(CODE_MM_BGM_SAVE);
PP_protoType(CODE_MM_BGM_LOAD);
PP_protoType(CODE_MM_MOVIE);

PP_protoType(CODE_SCREEN_CLEAR);
PP_protoType(CODE_SCREEN_SHOW);
PP_protoType(CODE_SCREEN_COLOR);
PP_protoType(CODE_SCREEN_SHAKE);
PP_protoType(CODE_SCREEN_FLASH);
PP_protoType(CODE_SCREEN_SCROLL);
PP_protoType(CODE_SCREEN_WEATHER);

PP_protoType(CODE_PICT_SHOW);
PP_protoType(CODE_PICT_MOVE);
PP_protoType(CODE_PICT_CLEAR);

PP_protoType(CODE_CHARA_TRANS);
PP_protoType(CODE_CHARA_MOVE);
PP_protoType(CODE_CHARA_FLASH);
PP_protoType(CODE_MOVEALL_START);
PP_protoType(CODE_MOVEALL_CANCEL);

PP_protoType(CODE_LOCATE_MOVE);
PP_protoType(CODE_LOCATE_SAVE);
PP_protoType(CODE_LOCATE_LOAD);

PP_protoType(CODE_TXT_REM);
PP_protoType(CODE_TXT_REM_ADD);
PP_protoType(CODE_TXT_SHOW);
PP_protoType(CODE_TXT_SHOW_ADD);
PP_protoType(CODE_TXT_OPTION);
PP_protoType(CODE_TXT_FACE);

PP_protoType(CODE_NAME_INPUT);
PP_protoType(CODE_EVENT_LOCATE);
PP_protoType(CODE_EVENT_SWAP);
PP_protoType(CODE_LAND_ID);
PP_protoType(CODE_EVENT_ID);
PP_protoType(CODE_WAIT);
PP_protoType(CODE_CHIPSET);
PP_protoType(CODE_PANORAMA);
PP_protoType(CODE_ENCOUNT);
PP_protoType(CODE_CHIP_SWAP);
PP_protoType(CODE_TELEPORT);
PP_protoType(CODE_TELEPORT_PERM);
PP_protoType(CODE_ESCAPE);
PP_protoType(CODE_ESCAPE_PERM);
PP_protoType(CODE_SAVE_SHOW);
PP_protoType(CODE_SAVE_PERM);
PP_protoType(CODE_MENU_SHOW);
PP_protoType(CODE_MENU_PERM);
PP_protoType(CODE_LABEL);
PP_protoType(CODE_GAMEOVER);
PP_protoType(CODE_TITLE);
PP_protoType(CODE_BTLANIME);

PP_protoType(CODE_BTL_GO_START);
PP_protoType(CODE_BTL_GO_WIN);
PP_protoType(CODE_BTL_GO_ESCAPE);
PP_protoType(CODE_BTL_GO_LOSE);
PP_protoType(CODE_BTL_GO_END);
PP_protoType(CODE_BTL_ANIME);

PP_protoType(CODE_2003_JOB);
PP_protoType(CODE_2003_BTL_CMD);
PP_protoType(CODE_2003_ATK_REPEAT);
PP_protoType(CODE_2003_ESCAPE100);
PP_protoType(CODE_2003_BTL_COMMON);

#define PP_protoTypeWait(code) \
	template< > void GameEventManager::commandWait<code>(const rpg2k::structure::Instruction& com)
PP_protoTypeWait(CODE_LOCATE_MOVE);
PP_protoTypeWait(CODE_TXT_SHOW);
PP_protoTypeWait(CODE_BTL_GO_START);
PP_protoTypeWait(CODE_SELECT_START);
PP_protoTypeWait(CODE_WAIT);
PP_protoTypeWait(CODE_PICT_MOVE);
PP_protoTypeWait(CODE_SCREEN_SCROLL);
PP_protoTypeWait(CODE_NAME_INPUT);
PP_protoTypeWait(CODE_OPERATE_KEY);
PP_protoTypeWait(CODE_INN);
PP_protoTypeWait(CODE_SHOP);
PP_protoTypeWait(CODE_SCREEN_COLOR);
PP_protoTypeWait(CODE_BTLANIME);