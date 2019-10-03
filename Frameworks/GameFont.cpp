
#include "GameFont.h"

bool GameFont::Load(GameContext & ctx, const wchar_t * file_name)
{
	auto device = ctx.GetDR().GetD3DDevice();
	auto context = ctx.GetDR().GetD3DDeviceContext();

	m_state = std::make_unique<DirectX::CommonStates>(device);
	if (!m_state.get())return false;
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
	if (!m_spriteBatch.get())return false;
	m_spriteFont = std::make_unique<DirectX::SpriteFont>(device, file_name);
	if (!m_spriteFont.get())return false;

	return true;
}

void GameFont::Draw(DirectX::SimpleMath::Vector2 pos, std::string fmt_str, ...)
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

	float scale = 0.75f;

	m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, m_state->NonPremultiplied());
	m_spriteFont->DrawString(m_spriteBatch.get(), result.c_str(),
		pos,DirectX::Colors::Black, 
		0, DirectX::SimpleMath::Vector3::Zero, scale);
	m_spriteBatch->End();
}
