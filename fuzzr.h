
#pragma once

#include <Windows.h>

#include <gl/GL.h>

#include <stdint.h>

#include <vector>
#include <unordered_set>

#include "disc_enum.h"

using int32 = int32_t;
using uint32 = uint32_t;
using int16 = int16_t;
using uint16 = uint16_t;


// Idk about this one
struct GLCmdSetRNGSeed {

};

struct GLCmdMakeTexture {
	int32 Index = 0;
	GLCmdMakeTexture(int32 InIndex = 0) : Index(InIndex) { }
};

struct GLCmdDestroyTexture {
	int32 Index = 0;
	GLCmdDestroyTexture(int32 InIndex = 0) : Index(InIndex) { }
};


#define DISC_MAC(mac)              \
	mac(GLCmdMakeTexture)          \
	mac(GLCmdDestroyTexture)       \


DEFINE_DISCRIMINATED_UNION(GLCmd, DISC_MAC)

#undef DISC_MAC

template<typename T>
struct GLResourceManager {
	std::vector<GLuint> ByIndex;
	std::unordered_set<GLuint> AllAllocated;
	std::unordered_set<GLuint> AllDestroyed;

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

	void Reset();
};

void ExecuteGLCmds(GLCtxState* Ctx, const std::vector<GLCmd>& Commands);

void DeserializeGLCmdsFromBytecode(std::vector<GLCmd>* OutCommands, uint32* BytecodePtr, int32 NumBytecodeOps);

