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

	//�����Ȏ���3�s�ڂ����s����邪�A�������ׂ������Ȃ�̂ŃR�����g�A�E�g 
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

//�c�݂̕΍������߂鏈�� 
float distortion(float2 center, float r, float d_r, float2 uv)
{
	//���̃s�N�Z���ƒ��S���W�̋��� 
	float dist = distance(center, uv);

	//���a���̘c�݂����A����d_r 
	return 1 - smoothstep(r - d_r, r, dist);
}

//�|�[�^�����ۂ������J���鏈�� 
float4 portal(float2 center, float r, float d_r, float2 uv)
{
	//���̂䂪�� 
	float d = distortion(center, r, d_r, uv);

	//�摜��uv���W 
	float2 _uv = lerp(uv, center, d);


	//�T���v�����O�摜( �c�܂���摜�i���Ȃ��j�j 
	float4 base = tex.Sample(samLinear, _uv);

	//�摜�Ɍ����J�����F��� 
	//base(�����J�����摜) 
	//float4(0,0,0,0)(������������̐F) 
	return lerp(base, float4(0, 0, 0, 0), step(1, d));
}

//�u���E���ǃe���r�����o
float4 oldTV(float2 inUV) {
	//��ʂ̘c�݂���邽�߂ɑ���p�ϐ��ɑ��
	float2 uv = inUV;

	//�㉺���E�������ۂ����邽�߂�uv�̒��S�_���ړ��i����float2(0.5f,0.5f)�����S(0,0)�ƂȂ�j
	uv = uv - 0.5f; // uv - float2(0.5f,0.5f)
					//���S����̋������v��
	float vignette = length(uv);//distance(float2(0, 0), uv);
								//���S����̋����ɉ����Ęc�݂����
	uv /= 1 - vignette * 0.2f;

	//���S�ȊO�i��ʊO�j�͍����h��Ԃ�
	//if(texUV.x < 0 || texUV.x > 1 || texUV.y < 0 || texUV.y > 1)
	if (max(abs(uv.y) - 0.5f, abs(uv.x) - 0.5f) > 0)
	{
		return float4(0, 0, 0, 1);
	}


	//�摜�ǂݍ��ݗp��UV���쐬����(�c�ݗpUV���W���璆�S�_������ɖ߂��j
	float2 texUV = uv + 0.5f;
	//�ǂݍ��݂̎��_�ŏ��������Y�������Ă���
	texUV.x += sin(texUV.y * 100)*0.002f;
	texUV += 0.001f;
	//�摜�̂�炬���쐬����
	texUV.x += (random(floor(texUV.y * 100) + Time.z) - 0.5f) * 0.01f;
	//1�ȏ�̕����͏����_�ɂ��Ă����i�O�̂��߁j
	texUV = frac(texUV);
	//��U�摜��ǂݍ���
	float4 base = tex.Sample(samLinear, texUV);

	//��ʂ̐F�m�C�Y���쐬����
	float3 col;
	//�擾����rgb�����������炵�ĉ�ꂩ�������o��
	col.r = tex.Sample(samLinear, texUV).r;
	col.g = tex.Sample(samLinear, texUV + float2(0.002f, 0)).g;
	col.b = tex.Sample(samLinear, texUV + float2(0.004f, 0)).b;

	//�����_���ȗ�Ƀ��C���m�C�Y��ǉ�����
	if (random(floor(texUV.y * 500) + Time.z) < 0.001f)
	{
		//�\�������m�C�Y�͍���
		col.r = random(uv + float2(123 + Time.z, 0));
		col.g = random(uv + float2(123 + Time.z, 1));
		col.b = random(uv + float2(123 + Time.z, 2));
	}
	//��ʂ̋����Â�����(vignette)
	col *= 1 - vignette * 1.3f;
	base = float4(col, base.a);

	return base;

}

float4 oldTVportal(float2 inUV) {
	//��ʂ̘c�݂���邽�߂ɑ���p�ϐ��ɑ��
	float2 uv = inUV;

	//�㉺���E�������ۂ����邽�߂�uv�̒��S�_���ړ��i����float2(0.5f,0.5f)�����S(0,0)�ƂȂ�j
	uv = uv - 0.5f; // uv - float2(0.5f,0.5f)
					//���S����̋������v��
	float vignette = length(uv);//distance(float2(0, 0), uv);
								//���S����̋����ɉ����Ęc�݂����
	uv /= 1 - vignette * 0.2f;

	//���S�ȊO�i��ʊO�j�͍����h��Ԃ�
	//if(texUV.x < 0 || texUV.x > 1 || texUV.y < 0 || texUV.y > 1)
	if (max(abs(uv.y) - 0.5f, abs(uv.x) - 0.5f) > 0)
	{
		return float4(0, 0, 0, 1);
	}


	//�摜�ǂݍ��ݗp��UV���쐬����(�c�ݗpUV���W���璆�S�_������ɖ߂��j
	float2 texUV = uv + 0.5f;
	float2 pUV = uv + 0.5f;

	float size = (1 + Time.z) / 4.0f;
	//�ǂݍ��݂̎��_�ŏ��������Y�������Ă���
	texUV.x += sin(texUV.y * 100)*0.002f;
	texUV += 0.001f;
	//�摜�̂�炬���쐬����
	texUV.x += (random(floor(texUV.y * 100) + Time.z) - 0.5f) * 0.01f;
	//1�ȏ�̕����͏����_�ɂ��Ă����i�O�̂��߁j
	//texUV = frac(texUV); 
	//��U�摜��ǂݍ���
	float4 base = portal(Mouse, size, size / 2.0f, pUV);//tex.Sample(samLinear, texUV);

														//��ʂ̐F�m�C�Y���쐬����
	float3 col;
	//�擾����rgb�����������炵�ĉ�ꂩ�������o��
	col.r = portal(Mouse, size, size / 2.0f, pUV).r;
	col.g =	portal(Mouse, size, size / 2.0f, pUV + float2(0.002f, 0)).g;
	col.b = portal(Mouse, size, size / 2.0f, pUV + float2(0.004f, 0)).b;

	//�����_���ȗ�Ƀ��C���m�C�Y��ǉ�����
	if (random(floor(texUV.y * 500) + Time.z) < 0.001f)
	{
		//�\�������m�C�Y�͍���
		col.r = random(uv + float2(123 + Time.z, 0));
		col.g = random(uv + float2(123 + Time.z, 1));
		col.b = random(uv + float2(123 + Time.z, 2));
	}
	//��ʂ̋����Â�����(vignette)
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