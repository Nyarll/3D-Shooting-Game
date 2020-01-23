/// ADX2LE���������C�u����
#pragma once

#include <string>
#include <cri_adx2le.h>



// ADX2LE�N���X
class Adx2Le
{
	// �R���X�g���N�^
	public:
		Adx2Le();

	// �f�X�g���N�^
	public:
		~Adx2Le();


	// �V�X�e���ɑ΂��鑀��
	public:
		// ����������
		void Initialize(const wchar_t* atomConfigFile);

		// �I������
		void Finalize();

		// �X�V����
		void Update();

		// ACB�t�@�C��(�L���[�V�[�g���)�̓ǂݍ���
		void LoadAcbFile(const wchar_t* atomCueSheetBinaryFile, const wchar_t* atomWaveBankFile = L"");


	// �v���[���ɑ΂��鑀��
	public:
		// �w��L���[�̍Đ� 
		CriAtomExPlaybackId Play(CriAtomExCueId cueId, float volume = 1.0f);

		// �v���[���̈ꎞ��~
		void Pause();

		// �v���[���̈ꎞ��~��Ԃ̉���
		void Resume();

		// �v���[���̒�~
		void Stop();

		// �v���[���̉��ʂ̐ݒ�
		void SetVolume(float volume);


	// �v���[���ɑ΂����Ԃ̖₢���킹
	public:
		// �v���[�����ꎞ��~��Ԃ��ǂ���
		bool IsPaused();


	// �����ɑ΂��鑀��
	public:
		// ID�w�肵���������ꎞ��~
		void PauseByID(CriAtomExPlaybackId playbackId);

		// ID�w�肵�������̈ꎞ��~��Ԃ̉���
		void ResumeByID(CriAtomExPlaybackId playbackId);

		// ID�w�肵���������~
		void StopByID(CriAtomExPlaybackId playbackId);

		// ID�w�肵�������̉��ʂ̐ݒ�
		void SetVolumeByID(CriAtomExPlaybackId playbackId, float volume);


	// �����ɑ΂����Ԃ̖₢���킹
	public:
		// ID�w�肵���������Đ���(�Đ����������܂�)���ǂ���
		bool IsPlayingByID(CriAtomExPlaybackId playbackId);

		// ID�w�肵���������ꎞ��~��Ԃ��ǂ���
		bool IsPausedByID(CriAtomExPlaybackId playbackId);

		// ID�w�肵���������Đ��I��(��~���܂�)��Ԃ��ǂ���
		bool IsPlayEndedByID(CriAtomExPlaybackId playbackId);



	// �R�[���o�b�N�֐�
	private:
		// �G���[�R�[���o�b�N
		static void  OnError(const CriChar8* errorId, CriUint32 p1, CriUint32 p2, CriUint32* parray);
		
		// �������A���P�[�^
		static void* Allocate(void* obj, CriUint32 size);
		static void  Deallocate(void* obj, void* ptr);


	// ������̕ϊ�
	private:
		// wstring ����string�ւ̕ϊ�
		static std::string WStringToString(const std::wstring& s);

		// string ����wstring�ւ̕ϊ�
		static std::wstring StringToWString(const std::string& s);



	// �N���X�萔
	private:
		// �����Đ����֘A
		static const int MAX_VOICES;            // �v�[�����鉹���̐�
		static const int MAX_VIRTUAL_VOICES;    // �A�v���P�[�V�����œ����ɔ���������s�������̐�
		static const int MAX_LOADERS;           // �A�v���P�[�V�������Ŏg�p���郍�[�_�̐�

		// �T���v�����O���[�g�֘A
		static const int MAX_SAMPLING_RATE;      // �Đ����鉹���̃T���v�����O���[�g
		static const int SAMPLING_RATE_HCAMX;    // �Đ�����HCA-MX�f�[�^�̃T���v�����O���[�g



	// �����o�ϐ�
	private:
		CriAtomExPlayerHn    m_player;              // �����Đ��p�v���[��
		CriAtomExVoicePoolHn m_standardVoicePool;   // ADX/HCA�R�[�f�b�N�p�{�C�X�v�[��
		CriAtomExVoicePoolHn m_hcamxVoicePool;		// HCA-MX�R�[�f�b�N�p�{�C�X�v�[��
		CriAtomExAcbHn       m_acb;                 // �L���[�V�[�g���
		CriAtomDbasId        m_dbas;                // D-BAS
};
