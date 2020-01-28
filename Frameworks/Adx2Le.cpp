
#include "ADX2Le.h"
#include <vector>


// 同時再生数関連のパラメータ
const int Adx2Le::MAX_VOICES         = 24;
const int Adx2Le::MAX_VIRTUAL_VOICES = 64;    // 通常ボイス＋HCA-MXボイス＋α
const int Adx2Le::MAX_LOADERS        = 64;    // 通常ボイス＋HCA-MXボイス＋α

// サンプリングレート関連
const int Adx2Le::MAX_SAMPLING_RATE   = 48000 * 2;
const int Adx2Le::SAMPLING_RATE_HCAMX = 32000;



// コンストラクタ
Adx2Le::Adx2Le()
	: m_player(nullptr)
	, m_standardVoicePool(nullptr)
	, m_hcamxVoicePool(nullptr)
	, m_acb(CRI_NULL)
	, m_dbas(CRIATOMDBAS_ILLEGAL_ID)
{

}



// デストラクタ
Adx2Le::~Adx2Le()
{
	Finalize();
}



// システムに対する操作 =======================================================
// 初期化処理
void Adx2Le::Initialize(const wchar_t* atomConfigFile)
{
	// エラーコールバック関数の登録
	criErr_SetCallback(OnError);


	// メモリアロケータの登録
	criAtomEx_SetUserAllocator(Allocate, Deallocate, NULL);


	// ライブラリ初期化
	CriFsConfig fileSystemConfig;    // ファイルシステムの初期化パラメータ
	criFs_SetDefaultConfig(&fileSystemConfig);
	fileSystemConfig.num_loaders = MAX_LOADERS;

	CriAtomExConfig_WASAPI libraryConfig;    // ライブラリ初期化パラメータ
	criAtomEx_SetDefaultConfig_WASAPI(&libraryConfig);
	libraryConfig.atom_ex.max_virtual_voices  = MAX_VIRTUAL_VOICES;
	libraryConfig.hca_mx.output_sampling_rate = SAMPLING_RATE_HCAMX;
	libraryConfig.atom_ex.fs_config           = &fileSystemConfig;
	criAtomEx_Initialize_WASAPI(&libraryConfig, nullptr, 0);


	// D-Basの作成
	m_dbas = criAtomDbas_Create(nullptr, nullptr, 0);


	// ACFファイル(全体設定)の登録
	std::string acfPath = WStringToString(atomConfigFile);
	criAtomEx_RegisterAcfFile(nullptr, acfPath.c_str(), nullptr, 0);


	// DSPバス設定のアタッチ
	criAtomEx_AttachDspBusSetting("DspBusSetting_0", nullptr, 0);


	// ボイスプールの作成
	// ADX/HCAコーデック用ボイスプール
	CriAtomExStandardVoicePoolConfig standardVoicePoolConfig;
	criAtomExVoicePool_SetDefaultConfigForStandardVoicePool(&standardVoicePoolConfig);
	standardVoicePoolConfig.num_voices = MAX_VOICES;
	standardVoicePoolConfig.player_config.max_sampling_rate = MAX_SAMPLING_RATE;
	standardVoicePoolConfig.player_config.streaming_flag = CRI_TRUE;
	m_standardVoicePool = criAtomExVoicePool_AllocateStandardVoicePool(&standardVoicePoolConfig, nullptr, 0);

	// HCA-MXコーデック用ボイスプール
	CriAtomExHcaMxVoicePoolConfig hcamxVoicePoolConfig;
	criAtomExVoicePool_SetDefaultConfigForHcaMxVoicePool(&hcamxVoicePoolConfig);
	hcamxVoicePoolConfig.num_voices = MAX_VOICES;
	hcamxVoicePoolConfig.player_config.max_sampling_rate = MAX_SAMPLING_RATE;
	hcamxVoicePoolConfig.player_config.streaming_flag = CRI_TRUE;
	m_hcamxVoicePool = criAtomExVoicePool_AllocateHcaMxVoicePool(&hcamxVoicePoolConfig, nullptr, 0);


	// 音声再生用プレーヤの作成
	m_player = criAtomExPlayer_Create(nullptr, nullptr, 0);
}



// 終了処理
void Adx2Le::Finalize()
{
	// DSPバス設定のデタッチ
	criAtomEx_DetachDspBusSetting();


	// 音声再生用のプレーヤの破棄
	criAtomExPlayer_Destroy(m_player);


	// ボイスプールの破棄
	criAtomExVoicePool_FreeAll();


	// キューシート情報の解放
	criAtomExAcb_ReleaseAll();


	// ACFファイル(全体設定)の登録解除
	criAtomEx_UnregisterAcf();


	// D-BASの破棄
	criAtomDbas_Destroy(m_dbas);


	// ライブラリの終了
	criAtomEx_Finalize_WASAPI();
}



// 更新処理
void Adx2Le::Update()
{
	// CRI Atomライブラリの内部状態を更新
	criAtomEx_ExecuteMain();
}



// ACBファイル(キューシート情報)の読み込み
void Adx2Le::LoadAcbFile(const wchar_t* atomCueSheetBinaryFile, const wchar_t* atomWaveBankFile)
{
	// 既存のキューシート情報が存在している場合
	if (m_acb != nullptr)
	{
		criAtomExAcb_Release(m_acb);
	}


	// ACBファイル(キューシート情報)の読み込み
	std::string acbPath = WStringToString(atomCueSheetBinaryFile);
	std::string awbPath = WStringToString(atomWaveBankFile);
	m_acb = criAtomExAcb_LoadAcbFile(nullptr, acbPath.c_str(), nullptr, awbPath.c_str(), nullptr, 0);
}



// プレーヤに対する操作 =======================================================
// 指定キューの再生 
CriAtomExPlaybackId Adx2Le::Play(CriAtomExCueId cueId, float volume)
{
	// 音量の設定
	criAtomExPlayer_SetVolume(m_player, volume);


	// キューIDの指定
	criAtomExPlayer_SetCueId(m_player, m_acb, cueId);


	// 再生の開始
	CriAtomExPlaybackId playbackId = criAtomExPlayer_Start(m_player);


	// 再生した音声のID
	return playbackId;
}



// プレーヤの一時停止
void Adx2Le::Pause()
{
	criAtomExPlayer_Pause(m_player, CRI_TRUE);
}



// プレーヤの一時停止状態の解除
void Adx2Le::Resume()
{
	criAtomExPlayer_Resume(m_player, CRIATOMEX_RESUME_ALL_PLAYBACK);
}



// プレーヤの停止
void Adx2Le::Stop()
{
	criAtomExPlayer_Stop(m_player);
}



// プレーヤの音量の設定
void Adx2Le::SetVolume(float volume)
{
	criAtomExPlayer_SetVolume(m_player, volume);
	criAtomExPlayer_UpdateAll(m_player);
}



// プレーヤに対する状態の問い合わせ ===========================================
// プレーヤが一時停止状態かどうか
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



// 音声に対する操作 ===========================================================
// ID指定した音声の一時停止状態の解除
void Adx2Le::PauseByID(CriAtomExPlaybackId playbackId)
{
	criAtomExPlayback_Pause(playbackId, CRI_TRUE);
}



// ID指定した音声を停止
void Adx2Le::ResumeByID(CriAtomExPlaybackId playbackId)
{
	criAtomExPlayback_Resume(playbackId, CRIATOMEX_RESUME_ALL_PLAYBACK);
}



// ID指定した音声を停止
void Adx2Le::StopByID(CriAtomExPlaybackId playbackId)
{
	criAtomExPlayback_Stop(playbackId);
}



// ID指定した音声の音量の設定
void Adx2Le::SetVolumeByID(CriAtomExPlaybackId playbackId, float volume)
{
	criAtomExPlayer_SetVolume(m_player, volume);
	criAtomExPlayer_Update(m_player, playbackId);
}



// 音声に対する状態の問い合わせ ===============================================
// ID指定した音声が再生中(再生準備中を含む)かどうか
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



// ID指定した音声が一時停止状態かどうか
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



// ID指定した音声が再生終了(停止を含む)状態かどうか
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



// コールバック関数 ===========================================================
// エラーコールバック
void Adx2Le::OnError(const CriChar8* errorId, CriUint32 p1, CriUint32 p2, CriUint32* parray)
{
	UNREFERENCED_PARAMETER(parray);


	// エラーコードからエラー文字列に変換してコンソール出力する
	const CriChar8* errorMessage = criErr_ConvertIdToMessage(errorId, p1, p2);

	// コンソール出力する
	std::wstring outputString = StringToWString(errorMessage);
	OutputDebugString(outputString.c_str());
	OutputDebugString(L"\n");
}



// メモリアロケータ ===========================================================
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



// 文字列の変換 ===============================================================
// wstring からstringへの変換
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



// string からwstringへの変換
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
