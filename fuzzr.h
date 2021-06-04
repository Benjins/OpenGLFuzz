
#pragma once

#include <Windows.h>

#include <gl/GL.h>

//-----------------------------------------------------------------------------
// Shims
	
#ifndef APIENTRY
#define APIENTRY
#endif

#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

typedef char GLchar;
typedef size_t GLsizeiptr;

#define GL_TEXTURE0                       0x84C0

#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_LINK_STATUS                    0x8B82

#define GL_ARRAY_BUFFER                   0x8892

#define GL_STATIC_DRAW                    0x88E4
#define GL_DYNAMIC_DRAW                   0x88E8

typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC) (GLenum texture);
typedef void (APIENTRYP PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef GLuint(APIENTRYP PFNGLCREATEPROGRAMPROC) (void);
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
typedef GLuint(APIENTRYP PFNGLCREATEPROGRAMPROC) (void);
typedef GLuint(APIENTRYP PFNGLCREATESHADERPROC) (GLenum type);
typedef void (APIENTRYP PFNGLDELETESHADERPROC) (GLuint shader);
typedef void (APIENTRYP PFNGLDELETEPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRYP PFNGLLINKPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC) (GLuint program);
typedef void (APIENTRYP PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint* buffers);
typedef void (APIENTRYP PFNGLGENBUFFERSPROC) (GLsizei n, GLuint* buffers);
typedef void (APIENTRYP PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRYP PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const void* data, GLenum usage);
typedef GLint(APIENTRYP PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar* name);
typedef void (APIENTRYP PFNGLUNIFORM1IVPROC) (GLint location, GLsizei count, const GLint* value);
typedef void (APIENTRYP PFNGLUNIFORM1FVPROC) (GLint location, GLsizei count, const GLfloat* value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef GLint(APIENTRYP PFNGLGETATTRIBLOCATIONPROC) (GLuint program, const GLchar* name);
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRYP PFNGLDISABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
typedef void (APIENTRYP PFNGLDRAWARRAYSPROC) (GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint* params);
typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint* arrays);
typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint* arrays);

//-----------------------------------------------------------------------------

#include <stdint.h>

#include <vector>
#include <unordered_set>

#include "disc_enum.h"

using int64 = int64_t;
using uint64 = uint64_t;
using int32 = int32_t;
using uint32 = uint32_t;
using int16 = int16_t;
using uint16 = uint16_t;

enum struct TexFilter {
	NEAREST,
	LINEAR,
	NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR,
	Count
};

enum struct ShaderType {
	Vertex,
	Pixel
};

enum struct UniformType {
	Int,
	Float,
	Vec2,
	Vec3,
	Vec4,
	Mat4,
	Count
};

// Idk about this one
struct GLCmdSetRNGSeed {

};

struct GLCmdMakeTexture {
	int32 Index = 0;
	GLCmdMakeTexture() { }
	GLCmdMakeTexture(int32 InIndex) : Index(InIndex) { }
};

struct GLCmdActiveTextureSlot {
	int32 Index = 0;
	GLCmdActiveTextureSlot() { }
	GLCmdActiveTextureSlot(int32 InIndex) : Index(InIndex) { }
};

struct GLCmdBindTexture {
	int32 Index = 0;
	GLCmdBindTexture() { }
	GLCmdBindTexture(int32 InIndex) : Index(InIndex) { }
};

struct GLCmdSetTextureMinFilter {
	TexFilter Filter = TexFilter::LINEAR;
	GLCmdSetTextureMinFilter() { }
	GLCmdSetTextureMinFilter(TexFilter InFilter) : Filter(InFilter) { }
};

struct GLCmdSetTextureMagFilter {
	TexFilter Filter = TexFilter::LINEAR;
	GLCmdSetTextureMagFilter() { }
	GLCmdSetTextureMagFilter(TexFilter InFilter) : Filter(InFilter) { }
};

struct GLCmdDestroyTexture {
	int32 Index = 0;
	GLCmdDestroyTexture() { }
	GLCmdDestroyTexture(int32 InIndex) : Index(InIndex) { }
};

struct GLCmdMakeShader {
	int32 Index = 0;
	ShaderType Type = ShaderType::Vertex;
	GLCmdMakeShader() { }
	GLCmdMakeShader(int32 InIndex, ShaderType InType) : Index(InIndex), Type(InType) { }
};

struct GLCmdSetShaderSource {
	int32 Index = 0;
	// Not fuzzing all possible shaders, so we're picking from an existing array
	const char* SourceCode = "";
	GLCmdSetShaderSource() { }
	GLCmdSetShaderSource(int32 InIndex, const char* InSourceCode) : Index(InIndex), SourceCode(InSourceCode){ }
};

struct GLCmdCompileShader {
	int32 Index = 0;
	GLCmdCompileShader() { }
	GLCmdCompileShader(int32 InIndex) : Index(InIndex) { }
};

struct GLCmdDestroyShader {
	int32 Index = 0;
	GLCmdDestroyShader() { }
	GLCmdDestroyShader(int32 InIndex) : Index(InIndex) { }
};

struct GLCmdMakeProgram {
	int32 Index = 0;
	GLCmdMakeProgram() { }
	GLCmdMakeProgram(int32 InIndex) : Index(InIndex) { }
};

struct GLCmdAttachShader {
	int32 ProgramIndex = 0;
	int32 ShaderIndex = 0;

	GLCmdAttachShader() { }
	GLCmdAttachShader(int32 InProgramIndex, int32 InShaderIndex) : ProgramIndex(InProgramIndex), ShaderIndex(InShaderIndex) { }
};

struct GLCmdLinkProgram {
	int32 Index = 0;
	GLCmdLinkProgram() { }
	GLCmdLinkProgram(int32 InIndex) : Index(InIndex) { }
};

struct GLCmdValidateProgram {
	int32 Index = 0;
	GLCmdValidateProgram() { }
	GLCmdValidateProgram(int32 InIndex) : Index(InIndex) { }
};

struct GLCmdUseProgram {
	int32 Index = 0;
	GLCmdUseProgram() { }
	GLCmdUseProgram(int32 InIndex) : Index(InIndex) { }
};

struct GLCmdDestroyProgram {
	int32 Index = 0;
	GLCmdDestroyProgram() { }
	GLCmdDestroyProgram(int32 InIndex) : Index(InIndex) { }
};

// For now just GL_ARRAY_BUFFER, idk tho
struct GLCmdMakeBuffer {
	int32 Index = 0;
	GLCmdMakeBuffer() { }
	GLCmdMakeBuffer(int32 InIndex) : Index(InIndex) { }
};

struct GLCmdBindBuffer {
	int32 Index = 0;
	GLCmdBindBuffer() { }
	GLCmdBindBuffer(int32 InIndex) : Index(InIndex) { }
};

struct GLCmdBufferData {
	int32 NumBytes = 0;
	void* DataPtr = nullptr;
	bool bIsDynamic = false;
	GLCmdBufferData() { }
	GLCmdBufferData(int32 InNumBytes, void* InDataPtr, bool bInIsDynamic)
		: NumBytes(InNumBytes), DataPtr(InDataPtr), bIsDynamic(bInIsDynamic) { }
};

struct GLCmdDestroyBuffer {
	int32 Index = 0;
	GLCmdDestroyBuffer() { }
	GLCmdDestroyBuffer(int32 InIndex) : Index(InIndex) { }
};

struct GLCmdGetUniformLocation {
	int32 UniformLocationIndex = 0;
	int32 ProgramIndex = 0;
	const char* UniformName = "";
	GLCmdGetUniformLocation() { }
	GLCmdGetUniformLocation(int32 InUniformLocationIndex, int32 InProgramIndex, const char* InUniformName)
		: UniformLocationIndex(InUniformLocationIndex), ProgramIndex(InProgramIndex), UniformName(InUniformName) { }
};

struct GLCmdSetUniform {
	int32 UniformLocationIndex = 0;
	UniformType Type = UniformType::Int;
	void* UniformData = nullptr;
	GLCmdSetUniform() { }
	GLCmdSetUniform(int32 InUniformLocationIndex, UniformType InType, void* InUniformData)
		: UniformLocationIndex(InUniformLocationIndex), Type(InType), UniformData(InUniformData) { }
};

struct GLCmdGetVertexAttribLocation {
	int32 AttribLocationIndex = 0;
	int32 ProgramIndex = 0;
	const char* AttribName = "";
	GLCmdGetVertexAttribLocation() { }
	GLCmdGetVertexAttribLocation(int32 InAttribLocationIndex, int32 InProgramIndex, const char* InAttribName)
		: AttribLocationIndex(InAttribLocationIndex), ProgramIndex(InProgramIndex), AttribName(InAttribName) { }
};

struct GLCmdEnableVertexAttrib {
	int32 AttribLocationIndex = 0;
	GLCmdEnableVertexAttrib() { }
	GLCmdEnableVertexAttrib(int32 InAttribLocationIndex) : AttribLocationIndex(InAttribLocationIndex) { }
};

struct GLCmdDisableVertexAttrib {
	int32 AttribLocationIndex = 0;
	GLCmdDisableVertexAttrib() { }
	GLCmdDisableVertexAttrib(int32 InAttribLocationIndex) : AttribLocationIndex(InAttribLocationIndex) { }
};

struct GLCmdVertexAttribPointer {
	int32 AttribLocationIndex = 0;
	int32 NumComponents = 0;
	GLCmdVertexAttribPointer() { }
	GLCmdVertexAttribPointer(int32 InAttribLocationIndex, int32 InNumComponents)
		: AttribLocationIndex(InAttribLocationIndex), NumComponents(InNumComponents) { }
};

struct GLCmdDrawArrays {
	int32 NumTriangles = 0;
	GLCmdDrawArrays() { }
	GLCmdDrawArrays(int32 InNumTriangles) : NumTriangles(InNumTriangles) { }
};

#define DISC_MAC(mac)                   \
	mac(GLCmdMakeTexture)               \
	mac(GLCmdActiveTextureSlot)         \
	mac(GLCmdBindTexture)               \
	mac(GLCmdSetTextureMinFilter)       \
	mac(GLCmdSetTextureMagFilter)       \
	mac(GLCmdDestroyTexture)            \
	mac(GLCmdMakeShader)                \
	mac(GLCmdSetShaderSource)           \
	mac(GLCmdCompileShader)             \
	mac(GLCmdDestroyShader)             \
	mac(GLCmdMakeProgram)               \
	mac(GLCmdAttachShader)              \
	mac(GLCmdLinkProgram)               \
	mac(GLCmdValidateProgram)           \
	mac(GLCmdUseProgram)                \
	mac(GLCmdDestroyProgram)            \
	mac(GLCmdMakeBuffer)                \
	mac(GLCmdBindBuffer)                \
	mac(GLCmdBufferData)                \
	mac(GLCmdDestroyBuffer)             \
	mac(GLCmdGetUniformLocation)        \
	mac(GLCmdSetUniform)                \
	mac(GLCmdGetVertexAttribLocation)   \
	mac(GLCmdEnableVertexAttrib)        \
	mac(GLCmdDisableVertexAttrib)       \
	mac(GLCmdVertexAttribPointer)       \
	mac(GLCmdDrawArrays)                \



DEFINE_DISCRIMINATED_UNION(GLCmd, DISC_MAC)

#undef DISC_MAC

template<typename T>
struct GLResourceManager {
	std::vector<T> ByIndex;
	std::unordered_set<T> AllAllocated;
	std::unordered_set<T> AllDestroyed;

	void SetByIndex(int32 Index, T Val) {
		if (Index >= ByIndex.size()) {
			ByIndex.resize(Index + 1, 0);
		}

		ByIndex[Index] = Val;
	}

	T GetByIndex(int32 Index) {
		if (Index >= ByIndex.size()) {
			ByIndex.resize(Index + 1, 0);
		}

		return ByIndex[Index];
	}

	void MarkAsAllocated(T Val) {
		AllAllocated.insert(Val);
	}

	void MarkAsDestroyed(T Val) {
		AllDestroyed.insert(Val);
	}

	template<typename LAMBDA>
	void ForAllOutstandingAllocs(LAMBDA Func) {
		for (auto Allocated : AllAllocated) {
			if (AllDestroyed.count(Allocated) == 0) {
				Func(Allocated);
			}
		}
	}

	void Reset() {
		ByIndex.clear();
		AllAllocated.clear();
		AllDestroyed.clear();
	}
};

struct GLCtxState {
	GLResourceManager<GLuint> Textures;
	GLResourceManager<GLuint> Shaders;
	GLResourceManager<GLuint> Programs;
	GLResourceManager<GLuint> Buffers;
	GLResourceManager<GLint> UniformLocations;
	GLResourceManager<GLint> AttribLocations;

	PFNGLACTIVETEXTUREPROC glActiveTexture_func;
	PFNGLDELETESHADERPROC glDeleteShader_func;
	PFNGLCREATEPROGRAMPROC glCreateProgram_func;
	PFNGLCREATESHADERPROC glCreateShader_func;
	PFNGLSHADERSOURCEPROC glShaderSource_func;
	PFNGLCOMPILESHADERPROC glCompileShader_func;
	PFNGLDELETEPROGRAMPROC glDeleteProgram_func;
	PFNGLATTACHSHADERPROC glAttachShader_func;
	PFNGLLINKPROGRAMPROC glLinkProgram_func;
	PFNGLVALIDATEPROGRAMPROC glValidateProgram_func;
	PFNGLUSEPROGRAMPROC glUseProgram_func;
	PFNGLDELETEBUFFERSPROC glDeleteBuffers_func;
	PFNGLGENBUFFERSPROC glGenBuffers_func;
	PFNGLBINDBUFFERPROC glBindBuffer_func;
	PFNGLBUFFERDATAPROC glBufferData_func;
	PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation_func;

	PFNGLUNIFORM1IVPROC glUniform1iv_func;
	PFNGLUNIFORM1FVPROC glUniform1fv_func;
	PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv_func;

	PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation_func;

	PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray_func;
	PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray_func;

	PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer_func;

	PFNGLDRAWARRAYSPROC glDrawArrays_func;

	PFNGLGETPROGRAMIVPROC glGetProgramiv_func;
	PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog_func;

	PFNGLGENVERTEXARRAYSPROC glGenVertexArrays_func;
	PFNGLBINDVERTEXARRAYPROC glBindVertexArray_func;
	PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays_func;

	void InitFuncPtrs();

	void Reset();
};

void ExecuteGLCmds(GLCtxState* Ctx, const std::vector<GLCmd>& Commands);

void DeserializeGLCmdsFromBytecode(std::vector<GLCmd>* OutCommands, uint32* BytecodePtr, int32 NumBytecodeOps);

