//#version 300 es
//#extension GL_EXT_separate_shader_objects : enable
//#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in mediump vec4 iPosition;
layout(location = 1) in mediump vec4 iDiffuse;
layout(location = 2) in mediump vec2 iTexCoord;

//layout(location = 1) out highp vec4 oDiffuse;
//layout(location = 2) out mediump vec2 oTexCoord;
out highp vec4 oDiffuse;
out mediump vec2 oTexCoord;

//uniform mediump mat4 c_mtxOrient;
//uniform mediump vec4 c_TwoPixelSize;
//uniform mediump mat4 c_mtxRotate;

void main()
{ 
	//oDiffuse = iDiffuse.zyxw;
	oDiffuse = iDiffuse;
	oTexCoord = iTexCoord;
	gl_Position = iPosition;
}
