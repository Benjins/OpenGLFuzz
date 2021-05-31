

#include <assert.h>

#include "fuzzr.h"

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
		else {
			assert(false && "unreachable");
		}
	}
}


void GLCtxState::Reset() {
	Textures.ForAllOutstandingAllocs([](GLuint TexID) {
		glDeleteTextures(1, &TexID);
	});

	Textures.Reset();
}

