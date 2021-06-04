

#include <assert.h>

#include "fuzzr.h"


//GL_NEAREST
//GL_LINEAR
//GL_NEAREST_MIPMAP_NEAREST
//GL_LINEAR_MIPMAP_NEAREST
//GL_NEAREST_MIPMAP_LINEAR
//GL_LINEAR_MIPMAP_LINEAR

GLuint GLTextureFilters[] = {
	GL_NEAREST,
	GL_LINEAR,
	GL_NEAREST_MIPMAP_NEAREST,
	GL_LINEAR_MIPMAP_NEAREST,
	GL_NEAREST_MIPMAP_LINEAR,
	GL_LINEAR_MIPMAP_LINEAR
};

GLenum ShaderTypeToGLenum(ShaderType Type) {
	if (Type == ShaderType::Vertex) {
		return GL_VERTEX_SHADER;
	}
	else if (Type == ShaderType::Pixel) {
		return GL_FRAGMENT_SHADER;
	}
	else {
		assert(false && "unreachable");
		return 0;
	}
}


void DeserializeGLCmdsFromBytecode(std::vector<GLCmd>* OutCommands, uint32* BytecodePtr, int32 NumBytecodeOps) {
	
}


void ExecuteGLCmds(GLCtxState* Ctx, const std::vector<GLCmd>& Commands) {
	for (const auto& Cmd : Commands) {
		if (auto* MakeTex = Cmd.MaybeAsGLCmdMakeTexture()) {
			GLuint NewTexture;
			glGenTextures(1, &NewTexture);

			Ctx->Textures.SetByIndex(MakeTex->Index, NewTexture);
			Ctx->Textures.MarkAsAllocated(NewTexture);
		}
		else if (auto* DestroyTex = Cmd.MaybeAsGLCmdDestroyTexture()) {
			auto TexID = Ctx->Textures.GetByIndex(DestroyTex->Index);
			Ctx->Textures.MarkAsDestroyed(TexID);
			glDeleteTextures(1, &TexID);
		}
		else if (auto* ActiveTex = Cmd.MaybeAsGLCmdActiveTextureSlot()) {
			Ctx->glActiveTexture_func(GL_TEXTURE0 + ActiveTex->Index);
		}
		else if (auto* BindTex = Cmd.MaybeAsGLCmdBindTexture()) {
			auto TexID = Ctx->Textures.GetByIndex(BindTex->Index);
			glBindTexture(GL_TEXTURE_2D, TexID);
		}
		else if (auto* SetMinFilter = Cmd.MaybeAsGLCmdSetTextureMinFilter()) {
			auto GLFilter = GLTextureFilters[(int32)SetMinFilter->Filter];
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLFilter);
		}
		else if (auto* SetMagFilter = Cmd.MaybeAsGLCmdSetTextureMagFilter()) {
			auto GLFilter = GLTextureFilters[(int32)SetMagFilter->Filter];
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLFilter);
		}
		else if (auto* MakeShader = Cmd.MaybeAsGLCmdMakeShader()) {
			GLuint NewShader = Ctx->glCreateShader_func(ShaderTypeToGLenum(MakeShader->Type));
			Ctx->Shaders.SetByIndex(MakeShader->Index, NewShader);
			Ctx->Shaders.MarkAsAllocated(NewShader);
		}
		else if (auto* DestroyShader = Cmd.MaybeAsGLCmdDestroyShader()) {
			auto ShaderID = Ctx->Shaders.GetByIndex(DestroyShader->Index);
			Ctx->Shaders.MarkAsDestroyed(ShaderID);
			Ctx->glDeleteShader_func(ShaderID);
		}
		else if (auto* SetShaderSrc = Cmd.MaybeAsGLCmdSetShaderSource()) {
			const GLchar* progTexts[1];
			progTexts[0] = SetShaderSrc->SourceCode;

			auto ShaderID = Ctx->Shaders.GetByIndex(SetShaderSrc->Index);
			Ctx->glShaderSource_func(ShaderID, 1, progTexts, nullptr);
		}
		else if (auto* CompileShader = Cmd.MaybeAsGLCmdCompileShader()) {
			auto ShaderID = Ctx->Shaders.GetByIndex(CompileShader->Index);
			Ctx->glCompileShader_func(ShaderID);
		}
		else if (auto* MakeProgram = Cmd.MaybeAsGLCmdMakeProgram()) {
			GLuint NewProgram = Ctx->glCreateProgram_func();
			Ctx->Programs.SetByIndex(MakeProgram->Index, NewProgram);
			Ctx->Programs.MarkAsAllocated(NewProgram);
		}
		else if (auto* DestroyProgram = Cmd.MaybeAsGLCmdDestroyProgram()) {
			auto ProgramID = Ctx->Programs.GetByIndex(DestroyProgram->Index);
			Ctx->Programs.MarkAsDestroyed(ProgramID);
			Ctx->glDeleteProgram_func(ProgramID);
		}
		else if (auto* AttachShader = Cmd.MaybeAsGLCmdAttachShader()) {
			auto ProgramID = Ctx->Programs.GetByIndex(AttachShader->ProgramIndex);
			auto ShaderID = Ctx->Shaders.GetByIndex(AttachShader->ShaderIndex);
			Ctx->glAttachShader_func(ProgramID, ShaderID);
		}
		else if (auto* LinkProgram = Cmd.MaybeAsGLCmdLinkProgram()) {
			auto ProgramID = Ctx->Programs.GetByIndex(LinkProgram->Index);
			Ctx->glLinkProgram_func(ProgramID);

			GLint success;
			Ctx->glGetProgramiv_func(ProgramID, GL_LINK_STATUS, &success);

			if (success == 0) {
				GLchar errorLog[1024];
				Ctx->glGetProgramInfoLog_func(ProgramID, sizeof(errorLog), NULL, errorLog);
				printf("Error linking shader program: '%s'\n", errorLog);
				//assert(false);
			}

		}
		else if (auto* ValidateProgram = Cmd.MaybeAsGLCmdValidateProgram()) {
			auto ProgramID = Ctx->Programs.GetByIndex(ValidateProgram->Index);
			Ctx->glValidateProgram_func(ProgramID);
		}
		else if (auto* UseProgram = Cmd.MaybeAsGLCmdUseProgram()) {
			auto ProgramID = Ctx->Programs.GetByIndex(UseProgram->Index);
			Ctx->glUseProgram_func(ProgramID);
		}
		else if (auto* MakeBuffer = Cmd.MaybeAsGLCmdMakeBuffer()) {
			GLuint NewBuffer;
			Ctx->glGenBuffers_func(1, &NewBuffer);

			Ctx->Buffers.SetByIndex(MakeBuffer->Index, NewBuffer);
			Ctx->Buffers.MarkAsAllocated(NewBuffer);
		}
		else if (auto* DestroyBuffer = Cmd.MaybeAsGLCmdDestroyBuffer()) {
			auto BufferID = Ctx->Buffers.GetByIndex(DestroyBuffer->Index);
			Ctx->Buffers.MarkAsDestroyed(BufferID);
			Ctx->glDeleteBuffers_func(1, &BufferID);
		}
		else if (auto* BindBuffer = Cmd.MaybeAsGLCmdBindBuffer()) {
			auto BufferID = Ctx->Buffers.GetByIndex(BindBuffer->Index);
			Ctx->glBindBuffer_func(GL_ARRAY_BUFFER, BufferID);
		}
		else if (auto* BufferData = Cmd.MaybeAsGLCmdBufferData()) {
			Ctx->glBufferData_func(GL_ARRAY_BUFFER, BufferData->NumBytes, BufferData->DataPtr, BufferData->bIsDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		}
		else if (auto* GetUniformLocation = Cmd.MaybeAsGLCmdGetUniformLocation()) {
			auto ProgramID = Ctx->Programs.GetByIndex(GetUniformLocation->ProgramIndex);
			GLint UniformLoc = Ctx->glGetUniformLocation_func(ProgramID, GetUniformLocation->UniformName);
			Ctx->UniformLocations.SetByIndex(GetUniformLocation->UniformLocationIndex, UniformLoc);
		}
		else if (auto* SetUniform = Cmd.MaybeAsGLCmdSetUniform()) {
			auto UniformLoc = Ctx->UniformLocations.GetByIndex(SetUniform->UniformLocationIndex);
			if (SetUniform->Type == UniformType::Int) {
				Ctx->glUniform1iv_func(UniformLoc, 1, (GLint*)SetUniform->UniformData);
			}
			else if (SetUniform->Type == UniformType::Float) {
				Ctx->glUniform1fv_func(UniformLoc, 1, (GLfloat*)SetUniform->UniformData);
			}
			else if (SetUniform->Type == UniformType::Vec2) {
				assert(false && "unimplemented");
			}
			else if (SetUniform->Type == UniformType::Vec3) {
				assert(false && "unimplemented");
			}
			else if (SetUniform->Type == UniformType::Vec4) {
				assert(false && "unimplemented");
			}
			else if (SetUniform->Type == UniformType::Mat4) {
				// NOTE: Assuming row-major order
				Ctx->glUniformMatrix4fv_func(UniformLoc, 1, true, (GLfloat*)SetUniform->UniformData);
			}
			else {
				assert(false && "unreachable");
			}
		}
		else if (auto* GetVertexAttribLocation = Cmd.MaybeAsGLCmdGetVertexAttribLocation()) {
			auto ProgramID = Ctx->Programs.GetByIndex(GetVertexAttribLocation->ProgramIndex);
			auto AttribLocation = Ctx->glGetAttribLocation_func(ProgramID, GetVertexAttribLocation->AttribName);
			Ctx->AttribLocations.SetByIndex(GetVertexAttribLocation->AttribLocationIndex, AttribLocation);
		}
		else if (auto* EnableVertexAttrib = Cmd.MaybeAsGLCmdEnableVertexAttrib()) {
			auto AttribLoc = Ctx->AttribLocations.GetByIndex(EnableVertexAttrib->AttribLocationIndex);
			Ctx->glEnableVertexAttribArray_func(AttribLoc);
		}
		else if (auto* DisableVertexAttrib = Cmd.MaybeAsGLCmdDisableVertexAttrib()) {
			auto AttribLoc = Ctx->AttribLocations.GetByIndex(DisableVertexAttrib->AttribLocationIndex);
			Ctx->glDisableVertexAttribArray_func(AttribLoc);
		}
		else if (auto* VertexAttribPtr = Cmd.MaybeAsGLCmdVertexAttribPointer()) {
			auto AttribLoc = Ctx->AttribLocations.GetByIndex(VertexAttribPtr->AttribLocationIndex);
			Ctx->glVertexAttribPointer_func(AttribLoc, VertexAttribPtr->NumComponents, GL_FLOAT, GL_FALSE, 0, 0);
		}
		else if (auto* DrawArrays = Cmd.MaybeAsGLCmdDrawArrays()) {
			glDrawArrays(GL_TRIANGLES, 0, DrawArrays->NumTriangles);
			//Ctx->glDrawArrays_func(GL_TRIANGLES, 0, DrawArrays->NumTriangles);
		}
		else {
			assert(false && "unreachable");
		}

		//GLenum Err = glGetError();
		//assert(Err == GL_NO_ERROR);
	}
}


void GLCtxState::Reset() {
	Textures.ForAllOutstandingAllocs([](GLuint TexID) {
		glDeleteTextures(1, &TexID);
	});

	Shaders.ForAllOutstandingAllocs([this](GLuint ShaderID) {
		this->glDeleteShader_func(ShaderID);
	});

	Programs.ForAllOutstandingAllocs([this](GLuint ProgramID) {
		this->glDeleteProgram_func(ProgramID);
	});

	Buffers.ForAllOutstandingAllocs([this](GLuint BufferID) {
		this->glDeleteBuffers_func(1, &BufferID);
	});


	Textures.Reset();
	Shaders.Reset();
	Programs.Reset();
	Buffers.Reset();

	// These don't need to be destroyed, just the list cleared out
	UniformLocations.Reset();
	AttribLocations.Reset();
}

void GLCtxState::InitFuncPtrs() {
	glActiveTexture_func = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
	glDeleteShader_func = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	glCreateProgram_func = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	glCreateShader_func = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	glShaderSource_func = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	glCompileShader_func = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	glDeleteProgram_func = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	glAttachShader_func = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	glLinkProgram_func = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	glValidateProgram_func = (PFNGLVALIDATEPROGRAMPROC)wglGetProcAddress("glValidateProgram");
	glUseProgram_func = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	glDeleteBuffers_func = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	glGenBuffers_func = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glBindBuffer_func = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glBufferData_func = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glGetUniformLocation_func = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");

	glUniform1iv_func = (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
	glUniform1fv_func = (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv");
	glUniformMatrix4fv_func = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");

	glGetAttribLocation_func = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");

	glEnableVertexAttribArray_func = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	glDisableVertexAttribArray_func = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
	glVertexAttribPointer_func = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");

	glDrawArrays_func = (PFNGLDRAWARRAYSPROC)wglGetProcAddress("glDrawArrays");

	glGetProgramiv_func = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	glGetProgramInfoLog_func = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");


	glGenVertexArrays_func = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
	glBindVertexArray_func = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
	glDeleteVertexArrays_func = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
}

