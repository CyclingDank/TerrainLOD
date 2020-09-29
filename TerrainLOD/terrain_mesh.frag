#version 330 core

struct LightSource
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
};
in vec2 uv;
in vec2 uvForest;

out vec4 color;
uniform sampler2D heightmap;
uniform sampler2D forestTex;
uniform sampler2D cityTex;

uniform LightSource lightObj;

vec3 get_normal(vec2 iuv)
{

	vec2 uv_L = vec2(iuv.x - 1.0f/1600.0f, iuv.y);
	vec2 uv_R = vec2(iuv.x + 1.0f/1600.0f, iuv.y);

	vec2 uv_U = vec2(iuv.x, iuv.y + 1.0f/1600.0f);
	vec2 uv_D = vec2(iuv.x, iuv.y - 1.0f/1600.0f);

	float UL = texture(heightmap, uv_L).r;
	float UR = texture(heightmap, uv_R).r;

	float UU = texture(heightmap, uv_U).r;
	float UD = texture(heightmap, uv_D).r;

	vec3 normal;
	normal.x = UL - UR;
	normal.y = UD - UU;
	normal.z = 0.1;
	normal = normalize(normal);

	return normal;
}

vec3 cubic_interpolation(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float x)
{
	return p1 + x * 0.5f * (- p0 + p2) + x * x * 0.5f * (2.0f * p0 - 5.0f * p1 + 4 * p2 - p3) + x * x * x * 0.5f * (3.0f * p1 - 3.0f * p2 - p0 + p3);
}

vec3 blur_normals()
{
	vec2 uv_UL = vec2(uv.x - 1.0f/1600.0f, uv.y - 1.0f/1600.0f);
	vec2 uv_UR = vec2(uv.x + 1.0f/1600.0f, uv.y - 1.0f/1600.0f);

	vec2 uv_LL = vec2(uv.x - 1.0f/1600.0f, uv.y + 1.0f/1600.0f);
	vec2 uv_LR = vec2(uv.x + 1.0f/1600.0f, uv.y + 1.0f/1600.0f);

	
	vec2 uv_L = vec2(uv.x - 1.0f/1600.0f, uv.y);
	vec2 uv_R = vec2(uv.x + 1.0f/1600.0f, uv.y);

	vec2 uv_U = vec2(uv.x, uv.y + 1.0f/1600.0f);
	vec2 uv_D = vec2(uv.x, uv.y - 1.0f/1600.0f);

	vec3 normal_UL = get_normal(uv_UL);
	vec3 normal_UR = get_normal(uv_UR);

	vec3 normal_LL = get_normal(uv_LL);
	vec3 normal_LR = get_normal(uv_LR);

	vec3 normal_L = get_normal(uv_L) * 2.0f;
	vec3 normal_R = get_normal(uv_R) * 2.0f;

	vec3 normal_U = get_normal(uv_U) * 2.0f;
	vec3 normal_D = get_normal(uv_D) * 2.0f;

	vec3 normal = (normal_UL + normal_UR + normal_LL + normal_LR + normal_L + normal_R + normal_U + normal_D + get_normal(uv) * 4.0f) / 16.0f;

	return normal;
}

vec3 bicubic_normal()
{

	vec3 normals_1[4];
	normals_1[0] = get_normal(vec2(uv.x - 1.5f/1600.0f, uv.y + 1.5f/1600.0f));
	normals_1[1] = get_normal(vec2(uv.x - 0.5f/1600.0f, uv.y + 1.5f/1600.0f));
	normals_1[2] = get_normal(vec2(uv.x + 0.5f/1600.0f, uv.y + 1.5f/1600.0f));
	normals_1[3] = get_normal(vec2(uv.x + 1.5f/1600.0f, uv.y + 1.5f/1600.0f));
	vec3 normals_2[4];
	normals_2[0] = get_normal(vec2(uv.x - 1.5f/1600.0f, uv.y + 0.5f/1600.0f));
	normals_2[1] = get_normal(vec2(uv.x - 0.5f/1600.0f, uv.y + 0.5f/1600.0f));
	normals_2[2] = get_normal(vec2(uv.x + 0.5f/1600.0f, uv.y + 0.5f/1600.0f));
	normals_2[3] = get_normal(vec2(uv.x + 1.5f/1600.0f, uv.y + 0.5f/1600.0f));
	vec3 normals_3[4];
	normals_3[0] = get_normal(vec2(uv.x - 1.5f/1600.0f, uv.y - 0.5f/1600.0f));
	normals_3[1] = get_normal(vec2(uv.x - 0.5f/1600.0f, uv.y - 0.5f/1600.0f));
	normals_3[2] = get_normal(vec2(uv.x + 0.5f/1600.0f, uv.y - 0.5f/1600.0f));
	normals_3[3] = get_normal(vec2(uv.x + 1.5f/1600.0f, uv.y - 0.5f/1600.0f));
	vec3 normals_4[4];
	normals_4[0] = get_normal(vec2(uv.x - 1.5f/1600.0f, uv.y - 1.5f/1600.0f));
	normals_4[1] = get_normal(vec2(uv.x - 0.5f/1600.0f, uv.y - 1.5f/1600.0f));
	normals_4[2] = get_normal(vec2(uv.x + 0.5f/1600.0f, uv.y - 1.5f/1600.0f));
	normals_4[3] = get_normal(vec2(uv.x + 1.5f/1600.0f, uv.y - 1.5f/1600.0f));

	vec3 a = cubic_interpolation(normals_1[0],normals_1[1],normals_1[2],normals_1[3],uv.y);
	vec3 b = cubic_interpolation(normals_2[0],normals_2[1],normals_2[2],normals_2[3],uv.y);
	vec3 c = cubic_interpolation(normals_3[0],normals_3[1],normals_3[2],normals_3[3],uv.y);
	vec3 d = cubic_interpolation(normals_4[0],normals_4[1],normals_4[2],normals_4[3],uv.y);
	return normalize(cubic_interpolation(a,b,c,d,uv.x));
}

void main( )
{
	vec3 normal = bicubic_normal();
	//vec3 normal = blur_normals();
	//vec3 normal = get_normal(uv);

	vec4 darkblueColor = vec4(0.2f, 0.26f, 0.34f, 0.75f);
	vec4 lightblueColor = vec4(0.2f, 0.55f, 0.6f, 0.80f);
	vec4 cityColor = vec4(0.7f, 0.85f, 0.85f, 1.0f);
	vec4 beachColor = vec4(0.45f, 0.45f, 0.4f, 1.0f);
	vec4 greenColor = vec4(0.10f, 0.25f, 0.2f, 1.0f);
	vec4 brownColor = vec4(0.4f, 0.4f, 0.25f, 1.0f);
	vec4 snowColor = vec4(0.85f, 0.9f, 0.9f, 1.0f);


	float Height = texture(heightmap, uv).r;

	vec4 tex;

	if(Height <= 0.3)
	{
		float border = smoothstep(0.0f, 0.3f, Height);
		tex = darkblueColor;
		normal = vec3(0.0f,0.0f,0.0f);
	}
	else if(Height >  0.3f && Height < 0.69f)
	{
		float border = smoothstep(0.3f, 0.69f, Height);
		tex = darkblueColor * (1.0 - border) + lightblueColor * border;
		normal = (normal * border)*border;
	}
	else if (Height >= 0.69f && Height <= 0.7f)
	{
		float border = smoothstep(0.69f, 0.7f, Height);
		tex = lightblueColor * (1.0 - border) + beachColor * border;
	}
	else if (Height > 0.7f && Height < 0.74f)
	{
		float border = smoothstep(0.7f, 0.71f, Height);
		vec4 citTex = texture(cityTex, uvForest/ 32.0f);
		tex = beachColor * (1.0 - border) +  border* greenColor;
	}
	else if (Height >= 0.74f && Height <= 0.9f)
	{
		tex = vec4(0.0f, 0.45f, 0.4f, 1.0f);
		vec4 forrTex0 = texture(forestTex, uvForest / 64.0f);
		vec4 forrTex1 = texture(forestTex, uvForest / 128.0f);

		float border = smoothstep(0.74f, 0.9f, Height);
		tex = (greenColor * (1.0 - border)) + tex * texture(forestTex, uv) * texture(forestTex, uv)* texture(forestTex, uv) * forrTex0 * forrTex1* forrTex1 * border;
	}
	else if (Height > 0.9f && Height < 0.95f)
	{
		float border = smoothstep(0.9f, 0.95f, Height);
		vec4 forrTex0 = texture(forestTex, uvForest / 64.0f);
		vec4 forrTex1 = texture(forestTex, uvForest / 128.0f);
		tex = vec4(0.0f, 0.45f, 0.4f, 1.0f)* forrTex0 * forrTex1* forrTex1 * texture(forestTex, uv) * texture(forestTex, uv)* (1.0 - border) + brownColor * border;
	}
	else
	{
		float border = smoothstep(0.95f, 1.0f, Height);
		vec4 forrTex0 = texture(forestTex, uvForest / 256.0f);
		tex = brownColor * (1.0 - border) + ( forrTex0) * border ;
	}
	vec3 lightDir = normalize(-lightObj.direction);
	float diff = max( dot( normal, lightDir ), 0.0 );
    vec3 ambient = lightObj.ambient;
    vec3 diffuse = lightObj.diffuse * diff;
    vec3 result = ambient + diffuse;

	color = tex * vec4(result, 1.0);
	//color =  vec4(normalize(normal), 1.0);

}