//#version 300 es
//#extension GL_EXT_separate_shader_objects : enable
//#extension GL_ARB_separate_shader_objects : enable

#ifdef IMAGE
uniform mediump sampler2D tex_Diffuse;
#endif // IMAGE

#ifdef TEXT
uniform mediump sampler2D tex_Diffuse;
#endif // TEXT

//uniform MEDIUMP float c_fAlphaValue;
//uniform MEDIUMP vec3 c_outline_color;
//uniform MEDIUMP float c_fLuminance;
//uniform MEDIUMP float c_fMipLevel;	
//uniform MEDIUMP vec2 c_TwoBlurSize;

//layout(location = 1) in highp vec4 oDiffuse;
//layout(location = 2) in mediump vec2 oTexCoord;
in highp vec4 oDiffuse;
in mediump vec2 oTexCoord;

layout(location = 0) out mediump vec4 o_FragColor;

void main()
{
	mediump vec4 output_color = vec4(1.0, 1.0, 1.0, 1.0);

#ifdef SHAPE
	output_color = oDiffuse;
#endif // SHAPE

#ifdef IMAGE
	mediump vec4 tex_color = texture(tex_Diffuse, vec2(oTexCoord.x, oTexCoord.y));

	output_color = tex_color * oDiffuse;
#endif // IMAGE

#ifdef TEXT
	mediump vec4 tex_color = texture(tex_Diffuse, vec2(oTexCoord.x, oTexCoord.y));

	tex_color.xyz = vec3(1.0, 1.0, 1.0);
	output_color = tex_color * oDiffuse;
#endif // TEXT
	
	o_FragColor = output_color;
}
