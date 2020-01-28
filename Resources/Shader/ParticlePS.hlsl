#include "Particle.hlsli"


Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
SamplerState samLinear : register(s0);

float random(float2 uv)
{
	return frac(sin(dot(uv, float2(12.9898f, 78.233f))) * 43758.5453f);
}

float noise(float2 uv)
{
	float2 p = floor(uv);
	return random(p);
}

float valueNoise(float2 st)
{
	float2 p = floor(st);
	float2 f = frac(st);

	float v00 = random(p + float2(0, 0));
	float v10 = random(p + float2(1, 0));
	float v01 = random(p + float2(0, 1));
	float v11 = random(p + float2(1, 1));

	float2 u = f * f * (3.0 - 2.0 * f);

	float v0010 = lerp(v00, v10, u.x);
	float v0111 = lerp(v01, v11, u.x);
	return lerp(v0010, v0111, u.y);
}

float2 random2(float2 uv)
{
	float2 r = float2(dot(uv, float2(127.1f, 311.7f)),
		dot(uv, float2(269.5f, 183.3f)));
	return -1.0 + 2.0f * frac(sin(r)*43758.5453123f);
}

float perlinNoise(float2 uv)
{
	float2 p = floor(uv);
	float2 f = frac(uv);
	float2 u = f * f * (3.0f - 2.0f * f);

	float v00 = random2(p + float2(0, 0));
	float v10 = random2(p + float2(1, 0));
	float v01 = random2(p + float2(0, 1));
	float v11 = random2(p + float2(1, 1));

	return lerp(lerp(dot(v00, f - float2(0, 0)), dot(v10, f - float2(1, 0)), u.x),
		lerp(dot(v01, f - float2(0, 1)), dot(v11, f - float2(1, 1)), u.x),
		u.y) + 0.5f;
}

float fBm(float2 uv)
{
	float f = 0;
	float2 q = uv;

	//正式な式は3行目も実行されるが、処理負荷が高くなるのでコメントアウト 
	f += 0.5000f*perlinNoise(q); q = q * 2.01f;
	f += 0.2500f*perlinNoise(q); q = q * 2.02f;
	//f += 0.1250f*perlinNoise(q); q = q * 2.03f; 
	f += 0.0625f*perlinNoise(q); q = q * 2.01f;

	return f;
}

float4 blur(float2 uv)
{
	float b = 0.06f;
	float c = 1.0f - b * 8;

	float4 dest = tex.Sample(samLinear, uv + float2(0.0f, 0.0f)) *c
		+ tex.Sample(samLinear, uv + float2(-0.01f, -0.01f))* b
		+ tex.Sample(samLinear, uv + float2(-0.01f, 0.0f))* b
		+ tex.Sample(samLinear, uv + float2(-0.01f, 0.01f))* b
		+ tex.Sample(samLinear, uv + float2(0.0f, -0.01f))* b
		+ tex.Sample(samLinear, uv + float2(0.0f, 0.01f))* b
		+ tex.Sample(samLinear, uv + float2(0.01f, -0.01f))* b
		+ tex.Sample(samLinear, uv + float2(0.01f, 0.0f))* b
		+ tex.Sample(samLinear, uv + float2(0.01f, 0.01f))* b;

	return dest;
}

float2 mosaic(float2 baseUV, float mag)
{
	return floor(baseUV * mag) / mag;
}

float2 split(float num, float x, float y)
{
	float2 uv = (float2)0;
	uv.x += frac(num / x);
	uv.y += floor(num / x) / y;

	return uv;
}

float2 splitUV(float2 uv, float num, float x, float y)
{
	uv.x /= x;
	uv.y /= y;
	uv += split(num, x, y);
	return uv;
}

float4 movingTexture(Texture2D tex, float2 uv)
{
	float2 _uv = uv;
	float ran = perlinNoise(_uv * 20);
	_uv.x += (ran - 0.5f) * 0.1f * (2 - Time.z);
	float4 retUV = tex.Sample(samLinear, _uv);

	return retUV;
}
float4 movingTexture2(Texture2D tex, float2 uv)
{

	float2 _uv = uv;
	float ran = perlinNoise(_uv * 20);
	_uv.x += (ran - 0.5f) * 0.1f * (3 - Time.x);
	float4 retUV = tex.Sample(samLinear, _uv);

	return retUV;
}
float4 movingTexture3(Texture2D tex, float2 uv)
{

	float2 _uv = uv;
	float ran = perlinNoise(_uv * 20);
	_uv.x += (ran - 0.5f) * 0.1f * (5 - Time.x);
	float4 retUV = tex.Sample(samLinear, _uv);

	return retUV;
}

//歪みの偏差を求める処理 
float distortion(float2 center, float r, float d_r, float2 uv)
{
	//今のピクセルと中心座標の距離 
	float dist = distance(center, uv);

	//半径ｒの歪みを作る、幅はd_r 
	return 1 - smoothstep(r - d_r, r, dist);
}

//ポータルっぽい穴を開ける処理 
float4 portal(float2 center, float r, float d_r, float2 uv)
{
	//穴のゆがみ 
	float d = distortion(center, r, d_r, uv);

	//画像のuv座標 
	float2 _uv = lerp(uv, center, d);


	//サンプリング画像( 歪ませる画像（穴なし）） 
	float4 base = tex.Sample(samLinear, _uv);

	//画像に穴を開けた色情報 
	//base(穴を開けた画像) 
	//float4(0,0,0,0)(穴をあけた先の色) 
	return lerp(base, float4(0, 0, 0, 0), step(1, d));
}

//ブラウン管テレビ風演出
float4 oldTV(float2 inUV) {
	//画面の歪みを作るために操作用変数に代入
	float2 uv = inUV;

	//上下左右を少し丸くするためにuvの中心点を移動（元のfloat2(0.5f,0.5f)が中心(0,0)となる）
	uv = uv - 0.5f; // uv - float2(0.5f,0.5f)
					//中心からの距離を計測
	float vignette = length(uv);//distance(float2(0, 0), uv);
								//中心からの距離に応じて歪みを作る
	uv /= 1 - vignette * 0.2f;

	//中心以外（画面外）は黒く塗りつぶす
	//if(texUV.x < 0 || texUV.x > 1 || texUV.y < 0 || texUV.y > 1)
	if (max(abs(uv.y) - 0.5f, abs(uv.x) - 0.5f) > 0)
	{
		return float4(0, 0, 0, 1);
	}


	//画像読み込み用のUVを作成する(歪み用UV座標から中心点を左上に戻す）
	float2 texUV = uv + 0.5f;
	//読み込みの時点で少しだけズレを入れておく
	texUV.x += sin(texUV.y * 100)*0.002f;
	texUV += 0.001f;
	//画像のゆらぎを作成する
	texUV.x += (random(floor(texUV.y * 100) + Time.z) - 0.5f) * 0.01f;
	//1以上の部分は小数点にしておく（念のため）
	texUV = frac(texUV);
	//一旦画像を読み込む
	float4 base = tex.Sample(samLinear, texUV);

	//画面の色ノイズを作成する
	float3 col;
	//取得するrgbを少しずつずらして壊れかけ感を出す
	col.r = tex.Sample(samLinear, texUV).r;
	col.g = tex.Sample(samLinear, texUV + float2(0.002f, 0)).g;
	col.b = tex.Sample(samLinear, texUV + float2(0.004f, 0)).b;

	//ランダムな列にラインノイズを追加する
	if (random(floor(texUV.y * 500) + Time.z) < 0.001f)
	{
		//表示されるノイズは砂嵐
		col.r = random(uv + float2(123 + Time.z, 0));
		col.g = random(uv + float2(123 + Time.z, 1));
		col.b = random(uv + float2(123 + Time.z, 2));
	}
	//画面の隅を暗くする(vignette)
	col *= 1 - vignette * 1.3f;
	base = float4(col, base.a);

	return base;

}

float4 oldTVportal(float2 inUV) {
	//画面の歪みを作るために操作用変数に代入
	float2 uv = inUV;

	//上下左右を少し丸くするためにuvの中心点を移動（元のfloat2(0.5f,0.5f)が中心(0,0)となる）
	uv = uv - 0.5f; // uv - float2(0.5f,0.5f)
					//中心からの距離を計測
	float vignette = length(uv);//distance(float2(0, 0), uv);
								//中心からの距離に応じて歪みを作る
	uv /= 1 - vignette * 0.2f;

	//中心以外（画面外）は黒く塗りつぶす
	//if(texUV.x < 0 || texUV.x > 1 || texUV.y < 0 || texUV.y > 1)
	if (max(abs(uv.y) - 0.5f, abs(uv.x) - 0.5f) > 0)
	{
		return float4(0, 0, 0, 1);
	}


	//画像読み込み用のUVを作成する(歪み用UV座標から中心点を左上に戻す）
	float2 texUV = uv + 0.5f;
	float2 pUV = uv + 0.5f;

	float size = (1 + Time.z) / 4.0f;
	//読み込みの時点で少しだけズレを入れておく
	texUV.x += sin(texUV.y * 100)*0.002f;
	texUV += 0.001f;
	//画像のゆらぎを作成する
	texUV.x += (random(floor(texUV.y * 100) + Time.z) - 0.5f) * 0.01f;
	//1以上の部分は小数点にしておく（念のため）
	//texUV = frac(texUV); 
	//一旦画像を読み込む
	float4 base = portal(Mouse, size, size / 2.0f, pUV);//tex.Sample(samLinear, texUV);

														//画面の色ノイズを作成する
	float3 col;
	//取得するrgbを少しずつずらして壊れかけ感を出す
	col.r = portal(Mouse, size, size / 2.0f, pUV).r;
	col.g =	portal(Mouse, size, size / 2.0f, pUV + float2(0.002f, 0)).g;
	col.b = portal(Mouse, size, size / 2.0f, pUV + float2(0.004f, 0)).b;

	//ランダムな列にラインノイズを追加する
	if (random(floor(texUV.y * 500) + Time.z) < 0.001f)
	{
		//表示されるノイズは砂嵐
		col.r = random(uv + float2(123 + Time.z, 0));
		col.g = random(uv + float2(123 + Time.z, 1));
		col.b = random(uv + float2(123 + Time.z, 2));
	}
	//画面の隅を暗くする(vignette)
	col *= 1 - vignette * 1.3f;
	base = float4(col, base.a);

	return base;

}

float4 main(PS_INPUT input) : SV_TARGET
{
	float2 uv = input.Tex;
	float4 output = tex.Sample(samLinear, uv);

	return output;
}