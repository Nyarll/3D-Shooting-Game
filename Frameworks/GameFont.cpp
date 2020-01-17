
#include "GameFont.h"

bool GameFont::Load(GameContext & ctx, const wchar_t * file_name, float scale)
{
	auto device = ctx.GetDR().GetD3DDevice();
	auto context = ctx.GetDR().GetD3DDeviceContext();

	bool result = true;

	m_state = std::make_unique<DirectX::CommonStates>(device);
	assert(m_state.get());
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
	assert(m_spriteBatch.get());
	m_spriteFont = std::make_unique<DirectX::SpriteFont>(device, file_name);
	assert(m_spriteFont.get());

	m_scale = scale;

	return true;
}

void GameFont::Draw(DirectX::SimpleMath::Vector2 pos, DirectX::FXMVECTOR color, std::string fmt_str, ...)
{
	int final_n, n = ((int)fmt_str.size()) * 2;
	std::unique_ptr<char[]> formatted;
	va_list ap;
	while (1)
	{
		formatted.reset(new char[n]);
		strcpy(&formatted[0], fmt_str.c_str());
		va_start(ap, fmt_str);
		final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}
	std::string result = std::string(formatted.get());

	m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, m_state->NonPremultiplied());
	m_spriteFont->DrawString(m_spriteBatch.get(), result.c_str(),
		pos, color,
		0, DirectX::SimpleMath::Vector3::Zero, m_scale);
	m_spriteBatch->End();
}
