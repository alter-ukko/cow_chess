#pragma once
/*
    #version:1# (machine generated, don't edit!)

    Generated by sokol-shdc (https://github.com/floooh/sokol-tools)

    Cmdline: sokol-shdc --input game.glsl --output game.glsl.h --slang glsl330:hlsl5:metal_macos

    Overview:

        Shader program 'texcube':
            Get shader desc: texcube_shader_desc(sg_query_backend());
            Vertex shader: vs
                Attribute slots:
                    ATTR_vs_pos = 0
                    ATTR_vs_color0 = 1
                    ATTR_vs_texcoord0 = 2
                Uniform block 'vs_params':
                    C struct: vs_params_t
                    Bind slot: SLOT_vs_params = 0
            Fragment shader: fs
                Image 'tex':
                    Type: SG_IMAGETYPE_2D
                    Sample Type: SG_IMAGESAMPLETYPE_FLOAT
                    Bind slot: SLOT_tex = 0
                Sampler 'smp':
                    Type: SG_SAMPLERTYPE_FILTERING
                    Bind slot: SLOT_smp = 0
                Image Sampler Pair 'tex_smp':
                    Image: tex
                    Sampler: smp


    Shader descriptor structs:

        sg_shader texcube = sg_make_shader(texcube_shader_desc(sg_query_backend()));

    Vertex attribute locations for vertex shader 'vs':

        sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc){
            .layout = {
                .attrs = {
                    [ATTR_vs_pos] = { ... },
                    [ATTR_vs_color0] = { ... },
                    [ATTR_vs_texcoord0] = { ... },
                },
            },
            ...});


    Image bind slots, use as index in sg_bindings.vs.images[] or .fs.images[]

        SLOT_tex = 0;

    Sampler bind slots, use as index in sg_bindings.vs.sampler[] or .fs.samplers[]

        SLOT_smp = 0;

    Bind slot and C-struct for uniform block 'vs_params':

        vs_params_t vs_params = {
            .mvp = ...;
        };
        sg_apply_uniforms(SG_SHADERSTAGE_[VS|FS], SLOT_vs_params, &SG_RANGE(vs_params));

*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#if !defined(SOKOL_SHDC_ALIGN)
  #if defined(_MSC_VER)
    #define SOKOL_SHDC_ALIGN(a) __declspec(align(a))
  #else
    #define SOKOL_SHDC_ALIGN(a) __attribute__((aligned(a)))
  #endif
#endif
#define ATTR_vs_pos (0)
#define ATTR_vs_color0 (1)
#define ATTR_vs_texcoord0 (2)
#define SLOT_tex (0)
#define SLOT_smp (0)
#define SLOT_vs_params (0)
#pragma pack(push,1)
SOKOL_SHDC_ALIGN(16) typedef struct vs_params_t {
    hmm_mat4 mvp;
} vs_params_t;
#pragma pack(pop)
/*
    #version 330
    
    uniform vec4 vs_params[4];
    layout(location = 0) in vec4 pos;
    out vec4 color;
    layout(location = 1) in vec4 color0;
    out vec2 uv;
    layout(location = 2) in vec2 texcoord0;
    
    void main()
    {
        gl_Position = mat4(vs_params[0], vs_params[1], vs_params[2], vs_params[3]) * pos;
        color = color0;
        uv = texcoord0;
    }
    
*/
static const char vs_source_glsl330[326] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x33,0x33,0x30,0x0a,0x0a,0x75,0x6e,
    0x69,0x66,0x6f,0x72,0x6d,0x20,0x76,0x65,0x63,0x34,0x20,0x76,0x73,0x5f,0x70,0x61,
    0x72,0x61,0x6d,0x73,0x5b,0x34,0x5d,0x3b,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,0x28,
    0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x30,0x29,0x20,0x69,0x6e,
    0x20,0x76,0x65,0x63,0x34,0x20,0x70,0x6f,0x73,0x3b,0x0a,0x6f,0x75,0x74,0x20,0x76,
    0x65,0x63,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x6c,0x61,0x79,0x6f,0x75,
    0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x31,0x29,0x20,
    0x69,0x6e,0x20,0x76,0x65,0x63,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x30,0x3b,0x0a,
    0x6f,0x75,0x74,0x20,0x76,0x65,0x63,0x32,0x20,0x75,0x76,0x3b,0x0a,0x6c,0x61,0x79,
    0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x32,
    0x29,0x20,0x69,0x6e,0x20,0x76,0x65,0x63,0x32,0x20,0x74,0x65,0x78,0x63,0x6f,0x6f,
    0x72,0x64,0x30,0x3b,0x0a,0x0a,0x76,0x6f,0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,0x28,
    0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,
    0x69,0x6f,0x6e,0x20,0x3d,0x20,0x6d,0x61,0x74,0x34,0x28,0x76,0x73,0x5f,0x70,0x61,
    0x72,0x61,0x6d,0x73,0x5b,0x30,0x5d,0x2c,0x20,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,
    0x6d,0x73,0x5b,0x31,0x5d,0x2c,0x20,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,
    0x5b,0x32,0x5d,0x2c,0x20,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,0x33,
    0x5d,0x29,0x20,0x2a,0x20,0x70,0x6f,0x73,0x3b,0x0a,0x20,0x20,0x20,0x20,0x63,0x6f,
    0x6c,0x6f,0x72,0x20,0x3d,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x30,0x3b,0x0a,0x20,0x20,
    0x20,0x20,0x75,0x76,0x20,0x3d,0x20,0x74,0x65,0x78,0x63,0x6f,0x6f,0x72,0x64,0x30,
    0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
/*
    #version 330
    
    uniform sampler2D tex_smp;
    
    layout(location = 0) out vec4 frag_color;
    in vec2 uv;
    in vec4 color;
    
    void main()
    {
        frag_color = texture(tex_smp, uv);
    }
    
*/
static const char fs_source_glsl330[169] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x33,0x33,0x30,0x0a,0x0a,0x75,0x6e,
    0x69,0x66,0x6f,0x72,0x6d,0x20,0x73,0x61,0x6d,0x70,0x6c,0x65,0x72,0x32,0x44,0x20,
    0x74,0x65,0x78,0x5f,0x73,0x6d,0x70,0x3b,0x0a,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,
    0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x30,0x29,0x20,0x6f,
    0x75,0x74,0x20,0x76,0x65,0x63,0x34,0x20,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,
    0x6f,0x72,0x3b,0x0a,0x69,0x6e,0x20,0x76,0x65,0x63,0x32,0x20,0x75,0x76,0x3b,0x0a,
    0x69,0x6e,0x20,0x76,0x65,0x63,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x0a,
    0x76,0x6f,0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,
    0x20,0x20,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x74,
    0x65,0x78,0x74,0x75,0x72,0x65,0x28,0x74,0x65,0x78,0x5f,0x73,0x6d,0x70,0x2c,0x20,
    0x75,0x76,0x29,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
/*
    cbuffer vs_params : register(b0)
    {
        row_major float4x4 _19_mvp : packoffset(c0);
    };
    
    
    static float4 gl_Position;
    static float4 pos;
    static float4 color;
    static float4 color0;
    static float2 uv;
    static float2 texcoord0;
    
    struct SPIRV_Cross_Input
    {
        float4 pos : TEXCOORD0;
        float4 color0 : TEXCOORD1;
        float2 texcoord0 : TEXCOORD2;
    };
    
    struct SPIRV_Cross_Output
    {
        float4 color : TEXCOORD0;
        float2 uv : TEXCOORD1;
        float4 gl_Position : SV_Position;
    };
    
    void vert_main()
    {
        gl_Position = mul(pos, _19_mvp);
        color = color0;
        uv = texcoord0;
    }
    
    SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
    {
        pos = stage_input.pos;
        color0 = stage_input.color0;
        texcoord0 = stage_input.texcoord0;
        vert_main();
        SPIRV_Cross_Output stage_output;
        stage_output.gl_Position = gl_Position;
        stage_output.color = color;
        stage_output.uv = uv;
        return stage_output;
    }
*/
static const char vs_source_hlsl5[912] = {
    0x63,0x62,0x75,0x66,0x66,0x65,0x72,0x20,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,
    0x73,0x20,0x3a,0x20,0x72,0x65,0x67,0x69,0x73,0x74,0x65,0x72,0x28,0x62,0x30,0x29,
    0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x72,0x6f,0x77,0x5f,0x6d,0x61,0x6a,0x6f,0x72,
    0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x78,0x34,0x20,0x5f,0x31,0x39,0x5f,0x6d,0x76,
    0x70,0x20,0x3a,0x20,0x70,0x61,0x63,0x6b,0x6f,0x66,0x66,0x73,0x65,0x74,0x28,0x63,
    0x30,0x29,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x0a,0x73,0x74,0x61,0x74,0x69,0x63,0x20,
    0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,
    0x6f,0x6e,0x3b,0x0a,0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,
    0x34,0x20,0x70,0x6f,0x73,0x3b,0x0a,0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,0x6c,
    0x6f,0x61,0x74,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x73,0x74,0x61,0x74,
    0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x30,
    0x3b,0x0a,0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,
    0x75,0x76,0x3b,0x0a,0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,
    0x32,0x20,0x74,0x65,0x78,0x63,0x6f,0x6f,0x72,0x64,0x30,0x3b,0x0a,0x0a,0x73,0x74,
    0x72,0x75,0x63,0x74,0x20,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,
    0x5f,0x49,0x6e,0x70,0x75,0x74,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,
    0x61,0x74,0x34,0x20,0x70,0x6f,0x73,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,
    0x52,0x44,0x30,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,
    0x63,0x6f,0x6c,0x6f,0x72,0x30,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,0x52,
    0x44,0x31,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,0x74,
    0x65,0x78,0x63,0x6f,0x6f,0x72,0x64,0x30,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,
    0x4f,0x52,0x44,0x32,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,
    0x20,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,0x74,
    0x70,0x75,0x74,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,
    0x20,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,0x52,
    0x44,0x30,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,0x75,
    0x76,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,0x52,0x44,0x31,0x3b,0x0a,0x20,
    0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,
    0x69,0x74,0x69,0x6f,0x6e,0x20,0x3a,0x20,0x53,0x56,0x5f,0x50,0x6f,0x73,0x69,0x74,
    0x69,0x6f,0x6e,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x76,0x6f,0x69,0x64,0x20,0x76,0x65,
    0x72,0x74,0x5f,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,
    0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x6d,0x75,
    0x6c,0x28,0x70,0x6f,0x73,0x2c,0x20,0x5f,0x31,0x39,0x5f,0x6d,0x76,0x70,0x29,0x3b,
    0x0a,0x20,0x20,0x20,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x63,0x6f,0x6c,
    0x6f,0x72,0x30,0x3b,0x0a,0x20,0x20,0x20,0x20,0x75,0x76,0x20,0x3d,0x20,0x74,0x65,
    0x78,0x63,0x6f,0x6f,0x72,0x64,0x30,0x3b,0x0a,0x7d,0x0a,0x0a,0x53,0x50,0x49,0x52,
    0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,0x74,0x70,0x75,0x74,0x20,0x6d,
    0x61,0x69,0x6e,0x28,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,
    0x49,0x6e,0x70,0x75,0x74,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,
    0x74,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x70,0x6f,0x73,0x20,0x3d,0x20,0x73,
    0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,0x74,0x2e,0x70,0x6f,0x73,0x3b,0x0a,
    0x20,0x20,0x20,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x30,0x20,0x3d,0x20,0x73,0x74,0x61,
    0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,0x74,0x2e,0x63,0x6f,0x6c,0x6f,0x72,0x30,0x3b,
    0x0a,0x20,0x20,0x20,0x20,0x74,0x65,0x78,0x63,0x6f,0x6f,0x72,0x64,0x30,0x20,0x3d,
    0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,0x74,0x2e,0x74,0x65,0x78,
    0x63,0x6f,0x6f,0x72,0x64,0x30,0x3b,0x0a,0x20,0x20,0x20,0x20,0x76,0x65,0x72,0x74,
    0x5f,0x6d,0x61,0x69,0x6e,0x28,0x29,0x3b,0x0a,0x20,0x20,0x20,0x20,0x53,0x50,0x49,
    0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,0x74,0x70,0x75,0x74,0x20,
    0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,0x75,0x74,0x70,0x75,0x74,0x3b,0x0a,0x20,0x20,
    0x20,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,0x75,0x74,0x70,0x75,0x74,0x2e,0x67,
    0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x67,0x6c,0x5f,
    0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x3b,0x0a,0x20,0x20,0x20,0x20,0x73,0x74,
    0x61,0x67,0x65,0x5f,0x6f,0x75,0x74,0x70,0x75,0x74,0x2e,0x63,0x6f,0x6c,0x6f,0x72,
    0x20,0x3d,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x20,0x20,0x20,0x20,0x73,0x74,
    0x61,0x67,0x65,0x5f,0x6f,0x75,0x74,0x70,0x75,0x74,0x2e,0x75,0x76,0x20,0x3d,0x20,
    0x75,0x76,0x3b,0x0a,0x20,0x20,0x20,0x20,0x72,0x65,0x74,0x75,0x72,0x6e,0x20,0x73,
    0x74,0x61,0x67,0x65,0x5f,0x6f,0x75,0x74,0x70,0x75,0x74,0x3b,0x0a,0x7d,0x0a,0x00,

};
/*
    Texture2D<float4> tex : register(t0);
    SamplerState smp : register(s0);
    
    static float4 frag_color;
    static float2 uv;
    static float4 color;
    
    struct SPIRV_Cross_Input
    {
        float4 color : TEXCOORD0;
        float2 uv : TEXCOORD1;
    };
    
    struct SPIRV_Cross_Output
    {
        float4 frag_color : SV_Target0;
    };
    
    void frag_main()
    {
        frag_color = tex.Sample(smp, uv);
    }
    
    SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
    {
        uv = stage_input.uv;
        color = stage_input.color;
        frag_main();
        SPIRV_Cross_Output stage_output;
        stage_output.frag_color = frag_color;
        return stage_output;
    }
*/
static const char fs_source_hlsl5[591] = {
    0x54,0x65,0x78,0x74,0x75,0x72,0x65,0x32,0x44,0x3c,0x66,0x6c,0x6f,0x61,0x74,0x34,
    0x3e,0x20,0x74,0x65,0x78,0x20,0x3a,0x20,0x72,0x65,0x67,0x69,0x73,0x74,0x65,0x72,
    0x28,0x74,0x30,0x29,0x3b,0x0a,0x53,0x61,0x6d,0x70,0x6c,0x65,0x72,0x53,0x74,0x61,
    0x74,0x65,0x20,0x73,0x6d,0x70,0x20,0x3a,0x20,0x72,0x65,0x67,0x69,0x73,0x74,0x65,
    0x72,0x28,0x73,0x30,0x29,0x3b,0x0a,0x0a,0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,
    0x6c,0x6f,0x61,0x74,0x34,0x20,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,
    0x3b,0x0a,0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,
    0x75,0x76,0x3b,0x0a,0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,
    0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,
    0x20,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x49,0x6e,0x70,
    0x75,0x74,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,
    0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,0x52,0x44,
    0x30,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,0x75,0x76,
    0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,0x52,0x44,0x31,0x3b,0x0a,0x7d,0x3b,
    0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,
    0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,0x74,0x70,0x75,0x74,0x0a,0x7b,0x0a,0x20,0x20,
    0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,
    0x6c,0x6f,0x72,0x20,0x3a,0x20,0x53,0x56,0x5f,0x54,0x61,0x72,0x67,0x65,0x74,0x30,
    0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x76,0x6f,0x69,0x64,0x20,0x66,0x72,0x61,0x67,0x5f,
    0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x72,0x61,
    0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x74,0x65,0x78,0x2e,0x53,0x61,
    0x6d,0x70,0x6c,0x65,0x28,0x73,0x6d,0x70,0x2c,0x20,0x75,0x76,0x29,0x3b,0x0a,0x7d,
    0x0a,0x0a,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,
    0x74,0x70,0x75,0x74,0x20,0x6d,0x61,0x69,0x6e,0x28,0x53,0x50,0x49,0x52,0x56,0x5f,
    0x43,0x72,0x6f,0x73,0x73,0x5f,0x49,0x6e,0x70,0x75,0x74,0x20,0x73,0x74,0x61,0x67,
    0x65,0x5f,0x69,0x6e,0x70,0x75,0x74,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x75,
    0x76,0x20,0x3d,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,0x74,0x2e,
    0x75,0x76,0x3b,0x0a,0x20,0x20,0x20,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,
    0x73,0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,0x74,0x2e,0x63,0x6f,0x6c,0x6f,
    0x72,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x72,0x61,0x67,0x5f,0x6d,0x61,0x69,0x6e,
    0x28,0x29,0x3b,0x0a,0x20,0x20,0x20,0x20,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,
    0x6f,0x73,0x73,0x5f,0x4f,0x75,0x74,0x70,0x75,0x74,0x20,0x73,0x74,0x61,0x67,0x65,
    0x5f,0x6f,0x75,0x74,0x70,0x75,0x74,0x3b,0x0a,0x20,0x20,0x20,0x20,0x73,0x74,0x61,
    0x67,0x65,0x5f,0x6f,0x75,0x74,0x70,0x75,0x74,0x2e,0x66,0x72,0x61,0x67,0x5f,0x63,
    0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,
    0x72,0x3b,0x0a,0x20,0x20,0x20,0x20,0x72,0x65,0x74,0x75,0x72,0x6e,0x20,0x73,0x74,
    0x61,0x67,0x65,0x5f,0x6f,0x75,0x74,0x70,0x75,0x74,0x3b,0x0a,0x7d,0x0a,0x00,
};
/*
    #include <metal_stdlib>
    #include <simd/simd.h>
    
    using namespace metal;
    
    struct vs_params
    {
        float4x4 mvp;
    };
    
    struct main0_out
    {
        float4 color [[user(locn0)]];
        float2 uv [[user(locn1)]];
        float4 gl_Position [[position]];
    };
    
    struct main0_in
    {
        float4 pos [[attribute(0)]];
        float4 color0 [[attribute(1)]];
        float2 texcoord0 [[attribute(2)]];
    };
    
    vertex main0_out main0(main0_in in [[stage_in]], constant vs_params& _19 [[buffer(0)]])
    {
        main0_out out = {};
        out.gl_Position = _19.mvp * in.pos;
        out.color = in.color0;
        out.uv = in.texcoord0;
        return out;
    }
    
*/
static const char vs_source_metal_macos[596] = {
    0x23,0x69,0x6e,0x63,0x6c,0x75,0x64,0x65,0x20,0x3c,0x6d,0x65,0x74,0x61,0x6c,0x5f,
    0x73,0x74,0x64,0x6c,0x69,0x62,0x3e,0x0a,0x23,0x69,0x6e,0x63,0x6c,0x75,0x64,0x65,
    0x20,0x3c,0x73,0x69,0x6d,0x64,0x2f,0x73,0x69,0x6d,0x64,0x2e,0x68,0x3e,0x0a,0x0a,
    0x75,0x73,0x69,0x6e,0x67,0x20,0x6e,0x61,0x6d,0x65,0x73,0x70,0x61,0x63,0x65,0x20,
    0x6d,0x65,0x74,0x61,0x6c,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x76,
    0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,
    0x6c,0x6f,0x61,0x74,0x34,0x78,0x34,0x20,0x6d,0x76,0x70,0x3b,0x0a,0x7d,0x3b,0x0a,
    0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x6f,0x75,
    0x74,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x63,
    0x6f,0x6c,0x6f,0x72,0x20,0x5b,0x5b,0x75,0x73,0x65,0x72,0x28,0x6c,0x6f,0x63,0x6e,
    0x30,0x29,0x5d,0x5d,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,
    0x20,0x75,0x76,0x20,0x5b,0x5b,0x75,0x73,0x65,0x72,0x28,0x6c,0x6f,0x63,0x6e,0x31,
    0x29,0x5d,0x5d,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,
    0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x5b,0x5b,0x70,0x6f,
    0x73,0x69,0x74,0x69,0x6f,0x6e,0x5d,0x5d,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x73,0x74,
    0x72,0x75,0x63,0x74,0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x69,0x6e,0x0a,0x7b,0x0a,
    0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x70,0x6f,0x73,0x20,0x5b,
    0x5b,0x61,0x74,0x74,0x72,0x69,0x62,0x75,0x74,0x65,0x28,0x30,0x29,0x5d,0x5d,0x3b,
    0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x63,0x6f,0x6c,0x6f,
    0x72,0x30,0x20,0x5b,0x5b,0x61,0x74,0x74,0x72,0x69,0x62,0x75,0x74,0x65,0x28,0x31,
    0x29,0x5d,0x5d,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,
    0x74,0x65,0x78,0x63,0x6f,0x6f,0x72,0x64,0x30,0x20,0x5b,0x5b,0x61,0x74,0x74,0x72,
    0x69,0x62,0x75,0x74,0x65,0x28,0x32,0x29,0x5d,0x5d,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,
    0x76,0x65,0x72,0x74,0x65,0x78,0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x6f,0x75,0x74,
    0x20,0x6d,0x61,0x69,0x6e,0x30,0x28,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x69,0x6e,0x20,
    0x69,0x6e,0x20,0x5b,0x5b,0x73,0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x5d,0x5d,0x2c,
    0x20,0x63,0x6f,0x6e,0x73,0x74,0x61,0x6e,0x74,0x20,0x76,0x73,0x5f,0x70,0x61,0x72,
    0x61,0x6d,0x73,0x26,0x20,0x5f,0x31,0x39,0x20,0x5b,0x5b,0x62,0x75,0x66,0x66,0x65,
    0x72,0x28,0x30,0x29,0x5d,0x5d,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x6d,0x61,
    0x69,0x6e,0x30,0x5f,0x6f,0x75,0x74,0x20,0x6f,0x75,0x74,0x20,0x3d,0x20,0x7b,0x7d,
    0x3b,0x0a,0x20,0x20,0x20,0x20,0x6f,0x75,0x74,0x2e,0x67,0x6c,0x5f,0x50,0x6f,0x73,
    0x69,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x5f,0x31,0x39,0x2e,0x6d,0x76,0x70,0x20,
    0x2a,0x20,0x69,0x6e,0x2e,0x70,0x6f,0x73,0x3b,0x0a,0x20,0x20,0x20,0x20,0x6f,0x75,
    0x74,0x2e,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x69,0x6e,0x2e,0x63,0x6f,0x6c,
    0x6f,0x72,0x30,0x3b,0x0a,0x20,0x20,0x20,0x20,0x6f,0x75,0x74,0x2e,0x75,0x76,0x20,
    0x3d,0x20,0x69,0x6e,0x2e,0x74,0x65,0x78,0x63,0x6f,0x6f,0x72,0x64,0x30,0x3b,0x0a,
    0x20,0x20,0x20,0x20,0x72,0x65,0x74,0x75,0x72,0x6e,0x20,0x6f,0x75,0x74,0x3b,0x0a,
    0x7d,0x0a,0x0a,0x00,
};
/*
    #include <metal_stdlib>
    #include <simd/simd.h>
    
    using namespace metal;
    
    struct main0_out
    {
        float4 frag_color [[color(0)]];
    };
    
    struct main0_in
    {
        float2 uv [[user(locn1)]];
    };
    
    fragment main0_out main0(main0_in in [[stage_in]], texture2d<float> tex [[texture(0)]], sampler smp [[sampler(0)]])
    {
        main0_out out = {};
        out.frag_color = tex.sample(smp, in.uv);
        return out;
    }
    
*/
static const char fs_source_metal_macos[391] = {
    0x23,0x69,0x6e,0x63,0x6c,0x75,0x64,0x65,0x20,0x3c,0x6d,0x65,0x74,0x61,0x6c,0x5f,
    0x73,0x74,0x64,0x6c,0x69,0x62,0x3e,0x0a,0x23,0x69,0x6e,0x63,0x6c,0x75,0x64,0x65,
    0x20,0x3c,0x73,0x69,0x6d,0x64,0x2f,0x73,0x69,0x6d,0x64,0x2e,0x68,0x3e,0x0a,0x0a,
    0x75,0x73,0x69,0x6e,0x67,0x20,0x6e,0x61,0x6d,0x65,0x73,0x70,0x61,0x63,0x65,0x20,
    0x6d,0x65,0x74,0x61,0x6c,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x6d,
    0x61,0x69,0x6e,0x30,0x5f,0x6f,0x75,0x74,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,
    0x6c,0x6f,0x61,0x74,0x34,0x20,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,
    0x20,0x5b,0x5b,0x63,0x6f,0x6c,0x6f,0x72,0x28,0x30,0x29,0x5d,0x5d,0x3b,0x0a,0x7d,
    0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,
    0x69,0x6e,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,
    0x75,0x76,0x20,0x5b,0x5b,0x75,0x73,0x65,0x72,0x28,0x6c,0x6f,0x63,0x6e,0x31,0x29,
    0x5d,0x5d,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x66,0x72,0x61,0x67,0x6d,0x65,0x6e,0x74,
    0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x6f,0x75,0x74,0x20,0x6d,0x61,0x69,0x6e,0x30,
    0x28,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x69,0x6e,0x20,0x69,0x6e,0x20,0x5b,0x5b,0x73,
    0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x5d,0x5d,0x2c,0x20,0x74,0x65,0x78,0x74,0x75,
    0x72,0x65,0x32,0x64,0x3c,0x66,0x6c,0x6f,0x61,0x74,0x3e,0x20,0x74,0x65,0x78,0x20,
    0x5b,0x5b,0x74,0x65,0x78,0x74,0x75,0x72,0x65,0x28,0x30,0x29,0x5d,0x5d,0x2c,0x20,
    0x73,0x61,0x6d,0x70,0x6c,0x65,0x72,0x20,0x73,0x6d,0x70,0x20,0x5b,0x5b,0x73,0x61,
    0x6d,0x70,0x6c,0x65,0x72,0x28,0x30,0x29,0x5d,0x5d,0x29,0x0a,0x7b,0x0a,0x20,0x20,
    0x20,0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x6f,0x75,0x74,0x20,0x6f,0x75,0x74,0x20,
    0x3d,0x20,0x7b,0x7d,0x3b,0x0a,0x20,0x20,0x20,0x20,0x6f,0x75,0x74,0x2e,0x66,0x72,
    0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x74,0x65,0x78,0x2e,0x73,
    0x61,0x6d,0x70,0x6c,0x65,0x28,0x73,0x6d,0x70,0x2c,0x20,0x69,0x6e,0x2e,0x75,0x76,
    0x29,0x3b,0x0a,0x20,0x20,0x20,0x20,0x72,0x65,0x74,0x75,0x72,0x6e,0x20,0x6f,0x75,
    0x74,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
#if !defined(SOKOL_GFX_INCLUDED)
  #error "Please include sokol_gfx.h before game.glsl.h"
#endif
static inline const sg_shader_desc* texcube_shader_desc(sg_backend backend) {
  if (backend == SG_BACKEND_GLCORE33) {
    static sg_shader_desc desc;
    static bool valid;
    if (!valid) {
      valid = true;
      desc.attrs[0].name = "pos";
      desc.attrs[1].name = "color0";
      desc.attrs[2].name = "texcoord0";
      desc.vs.source = vs_source_glsl330;
      desc.vs.entry = "main";
      desc.vs.uniform_blocks[0].size = 64;
      desc.vs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
      desc.vs.uniform_blocks[0].uniforms[0].name = "vs_params";
      desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
      desc.vs.uniform_blocks[0].uniforms[0].array_count = 4;
      desc.fs.source = fs_source_glsl330;
      desc.fs.entry = "main";
      desc.fs.images[0].used = true;
      desc.fs.images[0].multisampled = false;
      desc.fs.images[0].image_type = SG_IMAGETYPE_2D;
      desc.fs.images[0].sample_type = SG_IMAGESAMPLETYPE_FLOAT;
      desc.fs.samplers[0].used = true;
      desc.fs.samplers[0].sampler_type = SG_SAMPLERTYPE_FILTERING;
      desc.fs.image_sampler_pairs[0].used = true;
      desc.fs.image_sampler_pairs[0].image_slot = 0;
      desc.fs.image_sampler_pairs[0].sampler_slot = 0;
      desc.fs.image_sampler_pairs[0].glsl_name = "tex_smp";
      desc.label = "texcube_shader";
    }
    return &desc;
  }
  if (backend == SG_BACKEND_D3D11) {
    static sg_shader_desc desc;
    static bool valid;
    if (!valid) {
      valid = true;
      desc.attrs[0].sem_name = "TEXCOORD";
      desc.attrs[0].sem_index = 0;
      desc.attrs[1].sem_name = "TEXCOORD";
      desc.attrs[1].sem_index = 1;
      desc.attrs[2].sem_name = "TEXCOORD";
      desc.attrs[2].sem_index = 2;
      desc.vs.source = vs_source_hlsl5;
      desc.vs.d3d11_target = "vs_5_0";
      desc.vs.entry = "main";
      desc.vs.uniform_blocks[0].size = 64;
      desc.vs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
      desc.fs.source = fs_source_hlsl5;
      desc.fs.d3d11_target = "ps_5_0";
      desc.fs.entry = "main";
      desc.fs.images[0].used = true;
      desc.fs.images[0].multisampled = false;
      desc.fs.images[0].image_type = SG_IMAGETYPE_2D;
      desc.fs.images[0].sample_type = SG_IMAGESAMPLETYPE_FLOAT;
      desc.fs.samplers[0].used = true;
      desc.fs.samplers[0].sampler_type = SG_SAMPLERTYPE_FILTERING;
      desc.fs.image_sampler_pairs[0].used = true;
      desc.fs.image_sampler_pairs[0].image_slot = 0;
      desc.fs.image_sampler_pairs[0].sampler_slot = 0;
      desc.label = "texcube_shader";
    }
    return &desc;
  }
  if (backend == SG_BACKEND_METAL_MACOS) {
    static sg_shader_desc desc;
    static bool valid;
    if (!valid) {
      valid = true;
      desc.vs.source = vs_source_metal_macos;
      desc.vs.entry = "main0";
      desc.vs.uniform_blocks[0].size = 64;
      desc.vs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
      desc.fs.source = fs_source_metal_macos;
      desc.fs.entry = "main0";
      desc.fs.images[0].used = true;
      desc.fs.images[0].multisampled = false;
      desc.fs.images[0].image_type = SG_IMAGETYPE_2D;
      desc.fs.images[0].sample_type = SG_IMAGESAMPLETYPE_FLOAT;
      desc.fs.samplers[0].used = true;
      desc.fs.samplers[0].sampler_type = SG_SAMPLERTYPE_FILTERING;
      desc.fs.image_sampler_pairs[0].used = true;
      desc.fs.image_sampler_pairs[0].image_slot = 0;
      desc.fs.image_sampler_pairs[0].sampler_slot = 0;
      desc.label = "texcube_shader";
    }
    return &desc;
  }
  return 0;
}
