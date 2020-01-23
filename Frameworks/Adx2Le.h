/// ADX2LEを扱うライブラリ
#pragma once

#include <string>
#include <cri_adx2le.h>



// ADX2LEクラス
class Adx2Le
{
	// コンストラクタ
	public:
		Adx2Le();

	// デストラクタ
	public:
		~Adx2Le();


	// システムに対する操作
	public:
		// 初期化処理
		void Initialize(const wchar_t* atomConfigFile);

		// 終了処理
		void Finalize();

		// 更新処理
		void Update();

		// ACBファイル(キューシート情報)の読み込み
		void LoadAcbFile(const wchar_t* atomCueSheetBinaryFile, const wchar_t* atomWaveBankFile = L"");


	// プレーヤに対する操作
	public:
		// 指定キューの再生 
		CriAtomExPlaybackId Play(CriAtomExCueId cueId, float volume = 1.0f);

		// プレーヤの一時停止
		void Pause();

		// プレーヤの一時停止状態の解除
		void Resume();

		// プレーヤの停止
		void Stop();

		// プレーヤの音量の設定
		void SetVolume(float volume);


	// プレーヤに対する状態の問い合わせ
	public:
		// プレーヤが一時停止状態かどうか
		bool IsPaused();


	// 音声に対する操作
	public:
		// ID指定した音声を一時停止
		void PauseByID(CriAtomExPlaybackId playbackId);

		// ID指定した音声の一時停止状態の解除
		void ResumeByID(CriAtomExPlaybackId playbackId);

		// ID指定した音声を停止
		void StopByID(CriAtomExPlaybackId playbackId);

		// ID指定した音声の音量の設定
		void SetVolumeByID(CriAtomExPlaybackId playbackId, float volume);


	// 音声に対する状態の問い合わせ
	public:
		// ID指定した音声が再生中(再生準備中を含む)かどうか
		bool IsPlayingByID(CriAtomExPlaybackId playbackId);

		// ID指定した音声が一時停止状態かどうか
		bool IsPausedByID(CriAtomExPlaybackId playbackId);

		// ID指定した音声が再生終了(停止を含む)状態かどうか
		bool IsPlayEndedByID(CriAtomExPlaybackId playbackId);



	// コールバック関数
	private:
		// エラーコールバック
		static void  OnError(const CriChar8* errorId, CriUint32 p1, CriUint32 p2, CriUint32* parray);
		
		// メモリアロケータ
		static void* Allocate(void* obj, CriUint32 size);
		static void  Deallocate(void* obj, void* ptr);


	// 文字列の変換
	private:
		// wstring からstringへの変換
		static std::string WStringToString(const std::wstring& s);

		// string からwstringへの変換
		static std::wstring StringToWString(const std::string& s);



	// クラス定数
	private:
		// 同時再生数関連
		static const int MAX_VOICES;            // プールする音声の数
		static const int MAX_VIRTUAL_VOICES;    // アプリケーションで同時に発音制御を行う音声の数
		static const int MAX_LOADERS;           // アプリケーション中で使用するローダの数

		// サンプリングレート関連
		static const int MAX_SAMPLING_RATE;      // 再生する音声のサンプリングレート
		static const int SAMPLING_RATE_HCAMX;    // 再生するHCA-MXデータのサンプリングレート



	// メンバ変数
	private:
		CriAtomExPlayerHn    m_player;              // 音声再生用プレーヤ
		CriAtomExVoicePoolHn m_standardVoicePool;   // ADX/HCAコーデック用ボイスプール
		CriAtomExVoicePoolHn m_hcamxVoicePool;		// HCA-MXコーデック用ボイスプール
		CriAtomExAcbHn       m_acb;                 // キューシート情報
		CriAtomDbasId        m_dbas;                // D-BAS
};
