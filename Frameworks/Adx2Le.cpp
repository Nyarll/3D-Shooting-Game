
#include "ADX2Le.h"
#include <vector>


// �����Đ����֘A�̃p�����[�^
const int Adx2Le::MAX_VOICES         = 24;
const int Adx2Le::MAX_VIRTUAL_VOICES = 64;    // �ʏ�{�C�X�{HCA-MX�{�C�X�{��
const int Adx2Le::MAX_LOADERS        = 64;    // �ʏ�{�C�X�{HCA-MX�{�C�X�{��

// �T���v�����O���[�g�֘A
const int Adx2Le::MAX_SAMPLING_RATE   = 48000 * 2;
const int Adx2Le::SAMPLING_RATE_HCAMX = 32000;



// �R���X�g���N�^
Adx2Le::Adx2Le()
	: m_player(nullptr)
	, m_standardVoicePool(nullptr)
	, m_hcamxVoicePool(nullptr)
	, m_acb(CRI_NULL)
	, m_dbas(CRIATOMDBAS_ILLEGAL_ID)
{

}



// �f�X�g���N�^
Adx2Le::~Adx2Le()
{
	Finalize();
}



// �V�X�e���ɑ΂��鑀�� =======================================================
// ����������
void Adx2Le::Initialize(const wchar_t* atomConfigFile)
{
	// �G���[�R�[���o�b�N�֐��̓o�^
	criErr_SetCallback(OnError);


	// �������A���P�[�^�̓o�^
	criAtomEx_SetUserAllocator(Allocate, Deallocate, NULL);


	// ���C�u����������
	CriFsConfig fileSystemConfig;    // �t�@�C���V�X�e���̏������p�����[�^
	criFs_SetDefaultConfig(&fileSystemConfig);
	fileSystemConfig.num_loaders = MAX_LOADERS;

	CriAtomExConfig_WASAPI libraryConfig;    // ���C�u�����������p�����[�^
	criAtomEx_SetDefaultConfig_WASAPI(&libraryConfig);
	libraryConfig.atom_ex.max_virtual_voices  = MAX_VIRTUAL_VOICES;
	libraryConfig.hca_mx.output_sampling_rate = SAMPLING_RATE_HCAMX;
	libraryConfig.atom_ex.fs_config           = &fileSystemConfig;
	criAtomEx_Initialize_WASAPI(&libraryConfig, nullptr, 0);


	// D-Bas�̍쐬
	m_dbas = criAtomDbas_Create(nullptr, nullptr, 0);


	// ACF�t�@�C��(�S�̐ݒ�)�̓o�^
	std::string acfPath = WStringToString(atomConfigFile);
	criAtomEx_RegisterAcfFile(nullptr, acfPath.c_str(), nullptr, 0);


	// DSP�o�X�ݒ�̃A�^�b�`
	criAtomEx_AttachDspBusSetting("DspBusSetting_0", nullptr, 0);


	// �{�C�X�v�[���̍쐬
	// ADX/HCA�R�[�f�b�N�p�{�C�X�v�[��
	CriAtomExStandardVoicePoolConfig standardVoicePoolConfig;
	criAtomExVoicePool_SetDefaultConfigForStandardVoicePool(&standardVoicePoolConfig);
	standardVoicePoolConfig.num_voices = MAX_VOICES;
	standardVoicePoolConfig.player_config.max_sampling_rate = MAX_SAMPLING_RATE;
	standardVoicePoolConfig.player_config.streaming_flag = CRI_TRUE;
	m_standardVoicePool = criAtomExVoicePool_AllocateStandardVoicePool(&standardVoicePoolConfig, nullptr, 0);

	// HCA-MX�R�[�f�b�N�p�{�C�X�v�[��
	CriAtomExHcaMxVoicePoolConfig hcamxVoicePoolConfig;
	criAtomExVoicePool_SetDefaultConfigForHcaMxVoicePool(&hcamxVoicePoolConfig);
	hcamxVoicePoolConfig.num_voices = MAX_VOICES;
	hcamxVoicePoolConfig.player_config.max_sampling_rate = MAX_SAMPLING_RATE;
	hcamxVoicePoolConfig.player_config.streaming_flag = CRI_TRUE;
	m_hcamxVoicePool = criAtomExVoicePool_AllocateHcaMxVoicePool(&hcamxVoicePoolConfig, nullptr, 0);


	// �����Đ��p�v���[���̍쐬
	m_player = criAtomExPlayer_Create(nullptr, nullptr, 0);
}



// �I������
void Adx2Le::Finalize()
{
	// DSP�o�X�ݒ�̃f�^�b�`
	criAtomEx_DetachDspBusSetting();


	// �����Đ��p�̃v���[���̔j��
	criAtomExPlayer_Destroy(m_player);


	// �{�C�X�v�[���̔j��
	criAtomExVoicePool_FreeAll();


	// �L���[�V�[�g���̉��
	criAtomExAcb_ReleaseAll();


	// ACF�t�@�C��(�S�̐ݒ�)�̓o�^����
	criAtomEx_UnregisterAcf();


	// D-BAS�̔j��
	criAtomDbas_Destroy(m_dbas);


	// ���C�u�����̏I��
	criAtomEx_Finalize_WASAPI();
}



// �X�V����
void Adx2Le::Update()
{
	// CRI Atom���C�u�����̓�����Ԃ��X�V
	criAtomEx_ExecuteMain();
}



// ACB�t�@�C��(�L���[�V�[�g���)�̓ǂݍ���
void Adx2Le::LoadAcbFile(const wchar_t* atomCueSheetBinaryFile, const wchar_t* atomWaveBankFile)
{
	// �����̃L���[�V�[�g��񂪑��݂��Ă���ꍇ
	if (m_acb != nullptr)
	{
		criAtomExAcb_Release(m_acb);
	}


	// ACB�t�@�C��(�L���[�V�[�g���)�̓ǂݍ���
	std::string acbPath = WStringToString(atomCueSheetBinaryFile);
	std::string awbPath = WStringToString(atomWaveBankFile);
	m_acb = criAtomExAcb_LoadAcbFile(nullptr, acbPath.c_str(), nullptr, awbPath.c_str(), nullptr, 0);
}



// �v���[���ɑ΂��鑀�� =======================================================
// �w��L���[�̍Đ� 
CriAtomExPlaybackId Adx2Le::Play(CriAtomExCueId cueId, float volume)
{
	// ���ʂ̐ݒ�
	criAtomExPlayer_SetVolume(m_player, volume);


	// �L���[ID�̎w��
	criAtomExPlayer_SetCueId(m_player, m_acb, cueId);


	// �Đ��̊J�n
	CriAtomExPlaybackId playbackId = criAtomExPlayer_Start(m_player);


	// �Đ�����������ID
	return playbackId;
}



// �v���[���̈ꎞ��~
void Adx2Le::Pause()
{
	criAtomExPlayer_Pause(m_player, CRI_TRUE);
}



// �v���[���̈ꎞ��~��Ԃ̉���
void Adx2Le::Resume()
{
	criAtomExPlayer_Resume(m_player, CRIATOMEX_RESUME_ALL_PLAYBACK);
}



// �v���[���̒�~
void Adx2Le::Stop()
{
	criAtomExPlayer_Stop(m_player);
}



// �v���[���̉��ʂ̐ݒ�
void Adx2Le::SetVolume(float volume)
{
	criAtomExPlayer_SetVolume(m_player, volume);
	criAtomExPlayer_UpdateAll(m_player);
}



// �v���[���ɑ΂����Ԃ̖₢���킹 ===========================================
// �v���[�����ꎞ��~��Ԃ��ǂ���
bool Adx2Le::IsPaused()
{
	CriBool isPaused = criAtomExPlayer_IsPaused(m_player);

	if (isPaused == CRI_TRUE)
	{
		return true;
	}
	else
	{
		return false;
	}
}



// �����ɑ΂��鑀�� ===========================================================
// ID�w�肵�������̈ꎞ��~��Ԃ̉���
void Adx2Le::PauseByID(CriAtomExPlaybackId playbackId)
{
	criAtomExPlayback_Pause(playbackId, CRI_TRUE);
}



// ID�w�肵���������~
void Adx2Le::ResumeByID(CriAtomExPlaybackId playbackId)
{
	criAtomExPlayback_Resume(playbackId, CRIATOMEX_RESUME_ALL_PLAYBACK);
}



// ID�w�肵���������~
void Adx2Le::StopByID(CriAtomExPlaybackId playbackId)
{
	criAtomExPlayback_Stop(playbackId);
}



// ID�w�肵�������̉��ʂ̐ݒ�
void Adx2Le::SetVolumeByID(CriAtomExPlaybackId playbackId, float volume)
{
	criAtomExPlayer_SetVolume(m_player, volume);
	criAtomExPlayer_Update(m_player, playbackId);
}



// �����ɑ΂����Ԃ̖₢���킹 ===============================================
// ID�w�肵���������Đ���(�Đ����������܂�)���ǂ���
bool Adx2Le::IsPlayingByID(CriAtomExPlaybackId playbackId)
{
	CriAtomExPlaybackStatus status = criAtomExPlayback_GetStatus(playbackId);

	if ((status == CRIATOMEXPLAYBACK_STATUS_PREP) ||
		(status == CRIATOMEXPLAYBACK_STATUS_PLAYING))
	{
		return true;
	}
	else
	{
		return false;
	}
}



// ID�w�肵���������ꎞ��~��Ԃ��ǂ���
bool Adx2Le::IsPausedByID(CriAtomExPlaybackId playbackId)
{
	if (criAtomExPlayback_IsPaused(playbackId) == CRI_TRUE)
	{
		return true;
	}
	else
	{
		return false;
	}
}



// ID�w�肵���������Đ��I��(��~���܂�)��Ԃ��ǂ���
bool Adx2Le::IsPlayEndedByID(CriAtomExPlaybackId playbackId)
{
	CriAtomExPlaybackStatus status = criAtomExPlayback_GetStatus(playbackId);

	if (status == CRIATOMEXPLAYBACK_STATUS_REMOVED)
	{
		return true;
	}
	else
	{
		return false;
	}
}



// �R�[���o�b�N�֐� ===========================================================
// �G���[�R�[���o�b�N
void Adx2Le::OnError(const CriChar8* errorId, CriUint32 p1, CriUint32 p2, CriUint32* parray)
{
	UNREFERENCED_PARAMETER(parray);


	// �G���[�R�[�h����G���[������ɕϊ����ăR���\�[���o�͂���
	const CriChar8* errorMessage = criErr_ConvertIdToMessage(errorId, p1, p2);

	// �R���\�[���o�͂���
	std::wstring outputString = StringToWString(errorMessage);
	OutputDebugString(outputString.c_str());
	OutputDebugString(L"\n");
}



// �������A���P�[�^ ===========================================================
void *Adx2Le::Allocate(void* obj, CriUint32 size)
{
	UNREFERENCED_PARAMETER(obj);

	void* ptr = operator new(size);
	return ptr;
}



void Adx2Le::Deallocate(void* obj, void* ptr)
{
	UNREFERENCED_PARAMETER(obj);

	operator delete(ptr);
}



// ������̕ϊ� ===============================================================
// wstring ����string�ւ̕ϊ�
std::string Adx2Le::WStringToString(const std::wstring& s)
{
	std::string result;

	int requiredBufferSize = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (requiredBufferSize > 0)
	{
		std::vector<char> buffer(requiredBufferSize);
		WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, &buffer[0], buffer.capacity(), nullptr, nullptr);
		result = std::string(buffer.begin(), buffer.end());
	}

	return result;
}



// string ����wstring�ւ̕ϊ�
std::wstring Adx2Le::StringToWString(const std::string& s)
{
	std::wstring result;

	int requiredBufferSize = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
	if (requiredBufferSize > 0)
	{
		std::vector<wchar_t> buffer(requiredBufferSize);
		MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &buffer[0], buffer.capacity());
		result = std::wstring(buffer.begin(), buffer.end());
	}

	return result;
}
